#include <Arduino.h>
#include <driver/i2s.h>
// #include <Sylo_inferencing.h>
 
void TaskMic( void *pvParameters );
 
 
bool semaphore = false;
 
float features[16000];
//float features_mic[16000];
 
// int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
//   memcpy(out_ptr, features + offset, length * sizeof(float));
//   return 0;
// }
 
 
// void ei_printf(const char *format, ...) {
//   static char print_buf[1024] = { 0 };
 
//   va_list args;
//   va_start(args, format);
//   int r = vsnprintf(print_buf, sizeof(print_buf), format, args);
//   va_end(args);
 
//   if (r > 0) {
//     Serial.write(print_buf);
//   }
// }
 
 
const i2s_port_t I2S_PORT = I2S_NUM_0;
const int BLOCK_SIZE = 1000;
 
void setup() {
  Serial.begin(115200);
 
  xTaskCreatePinnedToCore(
    TaskMic
    ,  "TaskMic"   // A name just for humans
    ,  10024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL
    ,  0);
}
 
 
void loop() {
 
 
  while (semaphore) {
    vTaskDelay(1);
  }
  semaphore = false;
 
  // if (sizeof(features) / sizeof(float) != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
  //   ei_printf("The size of your 'features' array is not correct. Expected %lu items, but had %lu\n",
  //             EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, sizeof(features) / sizeof(float));
  //   delay(1000);
  //   return;
  // }
 
  // ei_impulse_result_t result = { 0 };
 
  // the features are stored into flash, and we don't want to load everything into RAM
  // signal_t features_signal;
  // features_signal.total_length = sizeof(features) / sizeof(features[0]);
  // features_signal.get_data = &raw_feature_get_data;
 
  // invoke the impulse
  // EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug */);
 
 
  int ind = -1;
  float maxx = 0.0;
 
  // for (int in = 0; in < EI_CLASSIFIER_LABEL_COUNT; in++) {
 
  //   if (result.classification[in].value > maxx && result.classification[in].value > 0.80) {
 
  //     maxx = result.classification[in].value;
  //     ind = in;
  //   }
  // }

 
//   if (ind >= 0) {
//     String to_prnt = "";
 
//     to_prnt += result.classification[ind].label;
//     to_prnt += "  ";
//     to_prnt += result.classification[ind].value;
 
//     Serial.println(to_prnt);
//   }
 
//   ei_printf("run_classifier returned: %d\n", res);
 
//   if (res != 0) return;
 
//   // print the predictions
//   ei_printf("Predictions ");
//   ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
//             result.timing.dsp, result.timing.classification, result.timing.anomaly);
//   ei_printf(": \n");
//   ei_printf("[");
//   for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
//     ei_printf("%.5f", result.classification[ix].value);
// #if EI_CLASSIFIER_HAS_ANOMALY == 1
//     ei_printf(", ");
// #else
//     if (ix != EI_CLASSIFIER_LABEL_COUNT - 1) {
//       ei_printf(", ");
//     }
// #endif
//   }
// #if EI_CLASSIFIER_HAS_ANOMALY == 1
//   ei_printf("%.3f", result.anomaly);
// #endif
//   ei_printf("]\n");
 
//   // human-readable predictions
//   for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
//     ei_printf("    %s: %.5f\n", result.classification[ix].label, result.classification[ix].value);
//   }
// #if EI_CLASSIFIER_HAS_ANOMALY == 1
//   ei_printf("    anomaly score: %.3f\n", result.anomaly);
// #endif

// if(result.classification[2].value > 0.8){
//   Serial.println("Sylo Was Heard");
// }
 
 
//   semaphore = false;
 
//   delay(10);
}
 
 
void TaskMic(void *pvParameters) { // This is a task.
 
  Serial.println("Configuring I2S...");
  esp_err_t err;
 
  // The I2S config as per the example
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX), // Receive, not transfer
    .sample_rate = 16000,                         // 16KHz
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT, // could only get it to work with 32bits
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, // although the SEL config should be left, it seems to transmit on right
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,     // Interrupt level 1
    .dma_buf_count = 10,                           // number of buffers
    .dma_buf_len = BLOCK_SIZE                     // samples per buffer
  };
 
  // The pin config as per the setup
  const i2s_pin_config_t pin_config = {
    .bck_io_num = 14,   // BCKL
    .ws_io_num = 15,    // LRCL
    .data_out_num = -1, // not used (only for speakers)
    .data_in_num = 32   // DOUT
  };
 
  // Configuring the I2S driver and pins.
  // This function must be called before any I2S driver read/write operations.
  err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  if (err != ESP_OK) {
    Serial.printf("Failed installing driver: %d\n", err);
    while (true);
  }
  err = i2s_set_pin(I2S_PORT, &pin_config);
  if (err != ESP_OK) {
    Serial.printf("Failed setting pin: %d\n", err);
    while (true);
  }
  Serial.println("I2S driver installed.");

 
  for (;;) {
 
    // Read multiple samples at once and calculate the sound pressure
    int32_t samples[BLOCK_SIZE];
    size_t num_bytes_read= 0; 
    auto err = i2s_read(I2S_PORT,(char *)samples,BLOCK_SIZE * 4, &num_bytes_read,     // the doc says bytes, but its elements.
                                        portMAX_DELAY); // no timeout
 
    int samples_read = num_bytes_read / 8;
 
    while (semaphore) {
      vTaskDelay(1);
    }
    semaphore = true;
 
    for (int i = 0; i < 16000; i++) {
      if (i < 15000) {
        features[i] = features[i + 1000];
 
      } else {
        features[i] = (float)samples[i - 15000];
        Serial.println(features[i]);
      }
 
    }
 
    semaphore = false;

    vTaskDelay(1);
 
  }
}