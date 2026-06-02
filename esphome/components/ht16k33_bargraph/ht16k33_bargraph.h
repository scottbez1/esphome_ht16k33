#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace ht16k33_bargraph {

/**
 * LED color values for use with set_bar(), fill(), and fill_range().
 * In YAML automations these are referenced by name (red, green, yellow, off);
 * in C++ lambdas use the integer values directly (0–3).
 *
 * The bi-color bargraph has independent red and green LEDs per bar.
 * YELLOW lights both simultaneously, producing yellow/amber output.
 */
static const uint8_t BAR_OFF    = 0;
static const uint8_t BAR_RED    = 1;
static const uint8_t BAR_YELLOW = 2;  // red + green on simultaneously
static const uint8_t BAR_GREEN  = 3;

/**
 * HT16K33-backed 24-bar bi-color bargraph (Adafruit product 1721).
 *
 * Bit layout (matches Adafruit_24bargraph library):
 *   word index c = bar/4 (bars 0–11) or (bar–12)/4 (bars 12–23)
 *   bit offset a = bar%4 (bars 0–11) or bar%4 + 4  (bars 12–23)
 *   RED  bit: bit a   of buffer_[c]  (low byte)
 *   GREEN bit: bit a+8 of buffer_[c]  (high byte)
 *
 * See README.md for YAML configuration and action reference.
 */
class HT16K33Bargraph : public Component, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  // ----- Buffer manipulation (local only, no I2C) -----

  /// Set the color of one bar. bar: 0–23. color: BAR_OFF/BAR_RED/BAR_YELLOW/BAR_GREEN.
  void set_bar(uint8_t bar, uint8_t color);

  /// Fill all 24 bars with one color.
  void fill(uint8_t color);

  /// Fill a contiguous range of bars [from_bar, to_bar] with one color.
  /// No-op if from_bar > to_bar or either is out of range.
  void fill_range(uint8_t from_bar, uint8_t to_bar, uint8_t color);

  /// Turn off all 24 bars.
  void clear();

  // ----- Hardware writes (I2C) -----

  /// Push the local buffer to the HT16K33 in one I2C transaction.
  void write_display();

  /// Set display brightness (0 = dim, 15 = full). Takes effect immediately.
  void set_brightness(uint8_t level);

  /// Turn the display on or off without changing the buffer or brightness.
  void set_display_on(bool on);

  // Called by codegen only
  void set_initial_brightness(uint8_t level) { initial_brightness_ = level; }

 protected:
  uint16_t buffer_[8]{};
  uint8_t initial_brightness_{15};
};

// ---------------------------------------------------------------------------
// YAML action classes
// ---------------------------------------------------------------------------

template<typename... Ts> class ClearAction : public Action<Ts...> {
 public:
  explicit ClearAction(HT16K33Bargraph *parent) : parent_(parent) {}
  void play(const Ts &...x) override { this->parent_->clear(); }
 protected:
  HT16K33Bargraph *parent_;
};

template<typename... Ts> class WriteDisplayAction : public Action<Ts...> {
 public:
  explicit WriteDisplayAction(HT16K33Bargraph *parent) : parent_(parent) {}
  void play(const Ts &...x) override { this->parent_->write_display(); }
 protected:
  HT16K33Bargraph *parent_;
};

template<typename... Ts> class FillAction : public Action<Ts...> {
 public:
  explicit FillAction(HT16K33Bargraph *parent) : parent_(parent) {}
  TEMPLATABLE_VALUE(uint8_t, color)
  void play(const Ts &...x) override {
    this->parent_->fill(this->color_.value(x...));
  }
 protected:
  HT16K33Bargraph *parent_;
};

template<typename... Ts> class SetBarAction : public Action<Ts...> {
 public:
  explicit SetBarAction(HT16K33Bargraph *parent) : parent_(parent) {}
  TEMPLATABLE_VALUE(uint8_t, bar)
  TEMPLATABLE_VALUE(uint8_t, color)
  void play(const Ts &...x) override {
    this->parent_->set_bar(this->bar_.value(x...), this->color_.value(x...));
  }
 protected:
  HT16K33Bargraph *parent_;
};

template<typename... Ts> class FillRangeAction : public Action<Ts...> {
 public:
  explicit FillRangeAction(HT16K33Bargraph *parent) : parent_(parent) {}
  TEMPLATABLE_VALUE(uint8_t, from_bar)
  TEMPLATABLE_VALUE(uint8_t, to_bar)
  TEMPLATABLE_VALUE(uint8_t, color)
  void play(const Ts &...x) override {
    this->parent_->fill_range(this->from_bar_.value(x...), this->to_bar_.value(x...), this->color_.value(x...));
  }
 protected:
  HT16K33Bargraph *parent_;
};

template<typename... Ts> class SetBrightnessAction : public Action<Ts...> {
 public:
  explicit SetBrightnessAction(HT16K33Bargraph *parent) : parent_(parent) {}
  TEMPLATABLE_VALUE(uint8_t, brightness)
  void play(const Ts &...x) override {
    this->parent_->set_brightness(this->brightness_.value(x...));
  }
 protected:
  HT16K33Bargraph *parent_;
};

}  // namespace ht16k33_bargraph
}  // namespace esphome
