#include <driver/i2s.h>
#include <Arduino.h>
#include <microphone.h>

#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT
// either wire your microphone to the same pins or change these to match your wiring
#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_26
#define I2S_MIC_LEFT_RIGHT_CLOCK GPIO_NUM_22
#define I2S_MIC_SERIAL_DATA GPIO_NUM_21

int i2s_init(uint32_t sampling_rate) {
  // Start listening for audio: MONO @ 8/16KHz
  // don't mess around with this
  i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX),
        .sample_rate = sampling_rate,
        .bits_per_sample = (i2s_bits_per_sample_t)16,
        .channel_format = I2S_MIC_CHANNEL,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = -1,
    };

  // and don't mess around with this
  i2s_pin_config_t pin_config = {
      .bck_io_num = I2S_MIC_SERIAL_CLOCK,
      .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
      .data_out_num = I2S_PIN_NO_CHANGE,
      .data_in_num = I2S_MIC_SERIAL_DATA
  };
  esp_err_t ret = 0;

  ret = i2s_driver_install((i2s_port_t)1, &i2s_config, 0, NULL);
  if (ret != ESP_OK) {
    Serial.println("Error in i2s_driver_install");
  }

  ret = i2s_set_pin((i2s_port_t)1, &pin_config);
  if (ret != ESP_OK) {
    Serial.println("Error in i2s_driver_install");
  }

  ret = i2s_zero_dma_buffer((i2s_port_t)1);
  if (ret != ESP_OK) {
    Serial.println("Error in i2s_driver_install");
  }

  return int(ret);
}