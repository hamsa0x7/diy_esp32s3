# ESP32-S3 CAM + INMP441 + MAX98357A + OLED

Raw-I2S Xiaozhi board profile for:

- ESP32-S3 CAM dev board family camera header
- INMP441 microphone on I2S RX
- MAX98357A amplifier on I2S TX
- Shared BCLK/LRCLK full-duplex bus
- SSD1315/SSD1306-compatible 128x64 OLED on I2C
- 4 optional external keys

This profile reuses the repo's ESP32S3CAM-style camera pin map and moves the
external audio/OLED/buttons onto separate GPIOs.

Recommended external wiring:

- OLED I2C: `SCL=GPIO1`, `SDA=GPIO3`
- I2S clocks: `BCLK=GPIO14`, `LRCLK=GPIO21`
- I2S mic data: `GPIO46`
- I2S speaker data: `GPIO47`

Notes:

- Optional external keys are disabled in the board profile for now.
- `GPIO45` is left free for future expansion.
- `GPIO48` is left free and is a good candidate for future WS2812 strip output.
- If you want an external strip to mirror the onboard RGB LED, you can also use
  the same `GPIO48` data line.

Build with:

```bash
python scripts/release.py esp32s3-inmp441-max98357a
```

Or select `ESP32-S3 + INMP441 + MAX98357A` in `idf.py menuconfig`.
