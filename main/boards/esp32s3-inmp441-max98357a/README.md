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

Build with:

```bash
python scripts/release.py esp32s3-inmp441-max98357a
```

Or select `ESP32-S3 + INMP441 + MAX98357A` in `idf.py menuconfig`.
