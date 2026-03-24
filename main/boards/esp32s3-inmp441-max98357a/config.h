#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

// Duplex raw I2S shares one hardware clock domain, so RX/TX must use
// the same bus sample rate. Xiaozhi resamples mic audio to 16 kHz internally.
#define AUDIO_INPUT_SAMPLE_RATE  24000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

// Keep audio off the known-good OLED bus and fixed camera pins.
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_14
#define AUDIO_I2S_GPIO_WS   GPIO_NUM_21
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_46
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_47

#define BOOT_BUTTON_GPIO GPIO_NUM_0
#define KEY1_BUTTON_GPIO GPIO_NUM_NC
#define KEY2_BUTTON_GPIO GPIO_NUM_NC
#define KEY3_BUTTON_GPIO GPIO_NUM_NC
#define KEY4_BUTTON_GPIO GPIO_NUM_NC

// Match the known-good OLED wiring used by v2.2.4 bread-compact-wifi.
#define DISPLAY_SCL_PIN GPIO_NUM_42
#define DISPLAY_SDA_PIN GPIO_NUM_41
#define DISPLAY_WIDTH   128
#define DISPLAY_HEIGHT  64
#define DISPLAY_MIRROR_X true
#define DISPLAY_MIRROR_Y true

#define SDCARD_SDMMC_ENABLED 0
#define SDCARD_MOUNT_POINT "/sdcard"
#define SDCARD_SDMMC_CLK_PIN GPIO_NUM_39
#define SDCARD_SDMMC_CMD_PIN GPIO_NUM_38
#define SDCARD_SDMMC_D0_PIN  GPIO_NUM_40
#define SDCARD_SDMMC_D1_PIN  GPIO_NUM_41
#define SDCARD_SDMMC_D2_PIN  GPIO_NUM_42
#define SDCARD_SDMMC_D3_PIN  GPIO_NUM_2
#define SDCARD_SDMMC_BUS_WIDTH 4

// Camera header pin map inferred from the repo's ESP32S3CAM-based board
// (bread-compact-wifi-s3cam), which matches this board family best.
#define CAMERA_PIN_D0 GPIO_NUM_11
#define CAMERA_PIN_D1 GPIO_NUM_9
#define CAMERA_PIN_D2 GPIO_NUM_8
#define CAMERA_PIN_D3 GPIO_NUM_10
#define CAMERA_PIN_D4 GPIO_NUM_12
#define CAMERA_PIN_D5 GPIO_NUM_18
#define CAMERA_PIN_D6 GPIO_NUM_17
#define CAMERA_PIN_D7 GPIO_NUM_16
#define CAMERA_PIN_XCLK GPIO_NUM_15
#define CAMERA_PIN_PCLK GPIO_NUM_13
#define CAMERA_PIN_VSYNC GPIO_NUM_6
#define CAMERA_PIN_HREF GPIO_NUM_7
#define CAMERA_PIN_SIOC GPIO_NUM_5
#define CAMERA_PIN_SIOD GPIO_NUM_4
#define CAMERA_PIN_PWDN GPIO_NUM_NC
#define CAMERA_PIN_RESET GPIO_NUM_NC
#define XCLK_FREQ_HZ 20000000

#endif // _BOARD_CONFIG_H_
