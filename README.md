# ht16k33_bargraph

ESPHome component for the Adafruit 24-bar bi-color (red/green) bargraph with HT16K33
I2C backpack. Compatible with [Adafruit product 1721](https://www.adafruit.com/product/1721).

## Configuration

```yaml
ht16k33_bargraph:
  id: bargraph
  address: 0x70   # default; set via A0-A2 solder jumpers on backpack (0x70-0x77)
  brightness: 15  # optional, 0 (dim) to 15 (full), default 15
```

Multiple bargraphs on the same bus are supported — give each a unique `id` and `address`.

## Actions

All buffer-mutating actions (`clear`, `fill`, `set_bar`, `fill_range`) only update the
local buffer. Call `write_display` once after all changes to push everything to the
hardware in a single I2C transaction.

### `ht16k33_bargraph.clear`
Turn off all 24 bars.
```yaml
- ht16k33_bargraph.clear: bargraph
```

### `ht16k33_bargraph.fill`
Set all 24 bars to one color.
```yaml
- ht16k33_bargraph.fill:
    id: bargraph
    color: red   # red | green | yellow | off
```

### `ht16k33_bargraph.set_bar`
Set one bar's color. `bar` (0–23) and `color` are templatable.
```yaml
- ht16k33_bargraph.set_bar:
    id: bargraph
    bar: 5
    color: green
```

### `ht16k33_bargraph.fill_range`
Fill a contiguous range of bars `[from_bar, to_bar]` with one color.
All three parameters are templatable, making this suitable for VU-meter and
level-indicator patterns driven by `!lambda` expressions.
```yaml
- ht16k33_bargraph.fill_range:
    id: bargraph
    from_bar: !lambda "return 24 - (id(fader_pos) * 24 + 127) / 255;"
    to_bar: 23
    color: red
```

### `ht16k33_bargraph.write_display`
Push the buffer to the HT16K33 in one I2C transaction.
```yaml
- ht16k33_bargraph.write_display: bargraph
```

### `ht16k33_bargraph.set_brightness`
Set display brightness immediately (0–15). Templatable.
```yaml
- ht16k33_bargraph.set_brightness:
    id: bargraph
    brightness: 8
```

## Colors

| YAML value | Description |
|---|---|
| `off` | Both LEDs off |
| `red` | Red LED on |
| `green` | Green LED on |
| `yellow` | Both LEDs on (red + green → yellow/amber) |

## Hardware notes

- The HT16K33 bit layout matches the `Adafruit_24bargraph` Arduino library.
  Bar 0 is one end of the strip; bar 23 is the other. Physical orientation
  depends on how the bargraph is mounted.
- The display RAM is cleared and the oscillator is started automatically during
  `setup()`. No `on_boot` initialization is needed in YAML.
