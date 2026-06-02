#include "ht16k33_bargraph.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ht16k33_bargraph {

static const char *const TAG = "ht16k33_bargraph";

// HT16K33 command bytes
static const uint8_t CMD_OSCILLATOR_ON = 0x21;  // system setup: oscillator on
static const uint8_t CMD_DISPLAY_BASE  = 0x80;  // display setup base; OR with blink+on bits
static const uint8_t CMD_DISPLAY_ON    = 0x01;  // display-on bit within CMD_DISPLAY_BASE
static const uint8_t CMD_BRIGHTNESS    = 0xE0;  // brightness base; OR with level 0–15
static const uint8_t CMD_RAM_START     = 0x00;  // display RAM start address

void HT16K33Bargraph::setup() {
  uint8_t cmd = CMD_OSCILLATOR_ON;
  if (this->write(&cmd, 1) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "Failed to communicate with HT16K33 — check wiring and address");
    this->mark_failed();
    return;
  }

  cmd = CMD_DISPLAY_BASE | CMD_DISPLAY_ON;
  this->write(&cmd, 1);

  this->set_brightness(this->initial_brightness_);

  this->clear();
  this->write_display();

  ESP_LOGCONFIG(TAG, "HT16K33 bargraph initialized");
}

void HT16K33Bargraph::dump_config() {
  ESP_LOGCONFIG(TAG, "HT16K33 Bargraph:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "  Communication failed");
    return;
  }
  ESP_LOGCONFIG(TAG, "  Brightness: %d", this->initial_brightness_);
}

void HT16K33Bargraph::set_bar(uint8_t bar, uint8_t color) {
  if (bar >= 24)
    return;

  // Determine the word index (c) and bit offset (a) within that word.
  // Bars 0–11 and bars 12–23 share the same word indices 0–2; bars 12–23
  // use bit offsets 4–7 instead of 0–3 to pack into the same uint16_t.
  uint8_t c = (bar < 12) ? bar / 4 : (bar - 12) / 4;
  uint8_t a = bar % 4 + (bar >= 12 ? 4 : 0);

  uint16_t red_bit   = uint16_t(1) << a;
  uint16_t green_bit = uint16_t(1) << (a + 8);

  this->buffer_[c] &= ~(red_bit | green_bit);
  if (color == BAR_RED    || color == BAR_YELLOW) this->buffer_[c] |= red_bit;
  if (color == BAR_GREEN  || color == BAR_YELLOW) this->buffer_[c] |= green_bit;
}

void HT16K33Bargraph::fill(uint8_t color) {
  for (uint8_t bar = 0; bar < 24; bar++)
    this->set_bar(bar, color);
}

void HT16K33Bargraph::fill_range(uint8_t from_bar, uint8_t to_bar, uint8_t color) {
  if (from_bar > to_bar || from_bar >= 24)
    return;
  if (to_bar >= 24)
    to_bar = 23;
  for (uint8_t bar = from_bar; bar <= to_bar; bar++)
    this->set_bar(bar, color);
}

void HT16K33Bargraph::clear() {
  for (auto &w : this->buffer_)
    w = 0;
}

void HT16K33Bargraph::write_display() {
  // One I2C transaction: RAM address byte followed by 8 little-endian 16-bit words.
  uint8_t data[17];
  data[0] = CMD_RAM_START;
  for (uint8_t i = 0; i < 8; i++) {
    data[1 + i * 2] = this->buffer_[i] & 0xFF;
    data[2 + i * 2] = this->buffer_[i] >> 8;
  }
  auto err = this->write(data, sizeof(data));
  if (err != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "write_display failed: %d", err);
  }
}

void HT16K33Bargraph::set_brightness(uint8_t level) {
  if (level > 15)
    level = 15;
  uint8_t cmd = CMD_BRIGHTNESS | level;
  this->write(&cmd, 1);
}

void HT16K33Bargraph::set_display_on(bool on) {
  uint8_t cmd = CMD_DISPLAY_BASE | (on ? CMD_DISPLAY_ON : 0);
  this->write(&cmd, 1);
}


}  // namespace ht16k33_bargraph
}  // namespace esphome
