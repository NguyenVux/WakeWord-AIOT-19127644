#include <Arduino.h>
#include <driver/i2s.h>
#include "microphone.h"

// don't mess around with this

void setup()
{
  // we need serial output for the plotter
  Serial.begin(115200);
  // start up the I2S peripheral
  i2s_init(16000);
}

int32_t raw_samples[512];
void loop()
{
  // read from the I2S device
  size_t bytes_read = 0;
  i2s_read((i2s_port_t)1, raw_samples, sizeof(int32_t) * 512, &bytes_read, portMAX_DELAY);
  int samples_read = bytes_read / sizeof(int32_t);
  // dump the samples out to the serial channel.
  for (int i = 0; i < samples_read; i++)
  {
    Serial.printf("%ld\n", raw_samples[i]);
  }
}