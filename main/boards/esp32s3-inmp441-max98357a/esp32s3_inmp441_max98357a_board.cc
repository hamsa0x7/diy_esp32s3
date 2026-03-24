#include "wifi_board.h"
#include "codecs/no_audio_codec.h"
#include "display/oled_display.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "assets/lang_config.h"
#include "esp32_camera.h"

#include <driver/i2c_master.h>
#include <driver/sdmmc_host.h>
#include <algorithm>
#include <esp_err.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_vendor.h>
#include <esp_log.h>
#include <esp_vfs_fat.h>
#include <sdmmc_cmd.h>

#define TAG "ESP32S3_INMP441_MAX98357A"

class Esp32S3Inmp441Max98357aBoard : public WifiBoard {
private:
    i2c_master_bus_handle_t display_i2c_bus_ = nullptr;
    esp_lcd_panel_io_handle_t panel_io_ = nullptr;
    esp_lcd_panel_handle_t panel_ = nullptr;
    Display* display_ = nullptr;
    Esp32Camera* camera_ = nullptr;
    Button boot_button_;
    Button key1_button_;
    Button key2_button_;
    Button key3_button_;
    Button key4_button_;

    bool InitializeDisplayI2c() {
        i2c_master_bus_config_t bus_config = {
            .i2c_port = I2C_NUM_0,
            .sda_io_num = DISPLAY_SDA_PIN,
            .scl_io_num = DISPLAY_SCL_PIN,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .intr_priority = 0,
            .trans_queue_depth = 0,
            .flags = {
                .enable_internal_pullup = 1,
            },
        };
        auto err = i2c_new_master_bus(&bus_config, &display_i2c_bus_);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "SSD1306 I2C bus init failed: %s", esp_err_to_name(err));
            return false;
        }
        return true;
    }

    bool InitializeSsd1306Display() {
        esp_lcd_panel_io_i2c_config_t io_config = {
            .dev_addr = 0x3C,
            .on_color_trans_done = nullptr,
            .user_ctx = nullptr,
            .control_phase_bytes = 1,
            .dc_bit_offset = 6,
            .lcd_cmd_bits = 8,
            .lcd_param_bits = 8,
            .flags = {
                .dc_low_on_data = 0,
                .disable_control_phase = 0,
            },
            .scl_speed_hz = 400 * 1000,
        };

        auto err = esp_lcd_new_panel_io_i2c_v2(display_i2c_bus_, &io_config, &panel_io_);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "SSD1306 panel IO init failed: %s", esp_err_to_name(err));
            return false;
        }

        esp_lcd_panel_dev_config_t panel_config = {};
        panel_config.reset_gpio_num = -1;
        panel_config.bits_per_pixel = 1;

        esp_lcd_panel_ssd1306_config_t ssd1306_config = {
            .height = static_cast<uint8_t>(DISPLAY_HEIGHT),
        };
        panel_config.vendor_config = &ssd1306_config;

        err = esp_lcd_new_panel_ssd1306(panel_io_, &panel_config, &panel_);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "SSD1306 panel init failed: %s", esp_err_to_name(err));
            return false;
        }

        if (esp_lcd_panel_reset(panel_) != ESP_OK || esp_lcd_panel_init(panel_) != ESP_OK) {
            ESP_LOGW(TAG, "SSD1306 panel reset/init failed");
            return false;
        }

        esp_lcd_panel_invert_color(panel_, false);
        esp_lcd_panel_disp_on_off(panel_, true);
        return true;
    }

    void InitializeDisplay() {
        display_ = new NoDisplay();
        if (!InitializeDisplayI2c()) {
            return;
        }

        if (!InitializeSsd1306Display()) {
            return;
        }

        delete display_;
        display_ = new OledDisplay(panel_io_, panel_, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y);
    }

    void InitializeButtons() {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting) {
                EnterWifiConfigMode();
                return;
            }
            app.ToggleChatState();
        });

        key1_button_.OnPressDown([]() {
            Application::GetInstance().StartListening();
        });
        key1_button_.OnPressUp([]() {
            Application::GetInstance().StopListening();
        });

        key2_button_.OnClick([this]() {
            auto codec = GetAudioCodec();
            auto volume = std::min(codec->output_volume() + 10, 100);
            codec->SetOutputVolume(volume);
            GetDisplay()->ShowNotification(Lang::Strings::VOLUME + std::to_string(volume));
        });

        key3_button_.OnClick([this]() {
            auto codec = GetAudioCodec();
            auto volume = std::max(codec->output_volume() - 10, 0);
            codec->SetOutputVolume(volume);
            GetDisplay()->ShowNotification(Lang::Strings::VOLUME + std::to_string(volume));
        });

        key4_button_.OnClick([]() {
            Application::GetInstance().ToggleChatState();
        });
        key4_button_.OnLongPress([this]() {
            EnterWifiConfigMode();
        });
    }

    void InitializeCamera() {
        camera_config_t config = {};
        config.pin_d0 = CAMERA_PIN_D0;
        config.pin_d1 = CAMERA_PIN_D1;
        config.pin_d2 = CAMERA_PIN_D2;
        config.pin_d3 = CAMERA_PIN_D3;
        config.pin_d4 = CAMERA_PIN_D4;
        config.pin_d5 = CAMERA_PIN_D5;
        config.pin_d6 = CAMERA_PIN_D6;
        config.pin_d7 = CAMERA_PIN_D7;
        config.pin_xclk = CAMERA_PIN_XCLK;
        config.pin_pclk = CAMERA_PIN_PCLK;
        config.pin_vsync = CAMERA_PIN_VSYNC;
        config.pin_href = CAMERA_PIN_HREF;
        config.pin_sccb_sda = CAMERA_PIN_SIOD;
        config.pin_sccb_scl = CAMERA_PIN_SIOC;
        config.sccb_i2c_port = 0;
        config.pin_pwdn = CAMERA_PIN_PWDN;
        config.pin_reset = CAMERA_PIN_RESET;
        config.xclk_freq_hz = XCLK_FREQ_HZ;
        config.pixel_format = PIXFORMAT_RGB565;
        config.frame_size = FRAMESIZE_VGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
        config.fb_location = CAMERA_FB_IN_PSRAM;
        config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
        camera_ = new Esp32Camera(config);
    }

    void InitializeSdCard() {
#if SDCARD_SDMMC_ENABLED
        sdmmc_host_t host = SDMMC_HOST_DEFAULT();
        sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
        slot_config.clk = SDCARD_SDMMC_CLK_PIN;
        slot_config.cmd = SDCARD_SDMMC_CMD_PIN;
        slot_config.d0 = SDCARD_SDMMC_D0_PIN;
        slot_config.width = SDCARD_SDMMC_BUS_WIDTH;
        if (SDCARD_SDMMC_BUS_WIDTH == 4) {
            slot_config.d1 = SDCARD_SDMMC_D1_PIN;
            slot_config.d2 = SDCARD_SDMMC_D2_PIN;
            slot_config.d3 = SDCARD_SDMMC_D3_PIN;
        }

        esp_vfs_fat_sdmmc_mount_config_t mount_config = {
            .format_if_mount_failed = false,
            .max_files = 5,
            .allocation_unit_size = 0,
            .disk_status_check_enable = true,
        };
        sdmmc_card_t* card = nullptr;
        auto err = esp_vfs_fat_sdmmc_mount(
            SDCARD_MOUNT_POINT, &host, &slot_config, &mount_config, &card
        );
        if (err == ESP_OK) {
            sdmmc_card_print_info(stdout, card);
            ESP_LOGI(TAG, "SD card mounted at %s", SDCARD_MOUNT_POINT);
        } else {
            ESP_LOGW(TAG, "Failed to mount SD card: %s", esp_err_to_name(err));
        }
#endif
    }

public:
    Esp32S3Inmp441Max98357aBoard()
        : boot_button_(BOOT_BUTTON_GPIO),
          key1_button_(KEY1_BUTTON_GPIO),
          key2_button_(KEY2_BUTTON_GPIO),
          key3_button_(KEY3_BUTTON_GPIO),
          key4_button_(KEY4_BUTTON_GPIO) {
        InitializeDisplay();
        InitializeButtons();
        InitializeCamera();
        InitializeSdCard();
    }

    virtual AudioCodec* GetAudioCodec() override {
        static NoAudioCodecDuplex audio_codec(
            AUDIO_INPUT_SAMPLE_RATE,
            AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_GPIO_BCLK,
            AUDIO_I2S_GPIO_WS,
            AUDIO_I2S_GPIO_DOUT,
            AUDIO_I2S_GPIO_DIN
        );
        return &audio_codec;
    }

    virtual Display* GetDisplay() override {
        return display_;
    }

    virtual Camera* GetCamera() override {
        return camera_;
    }
};

DECLARE_BOARD(Esp32S3Inmp441Max98357aBoard);
