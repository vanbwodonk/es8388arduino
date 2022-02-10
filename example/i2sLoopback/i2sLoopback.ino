#include <Arduino.h>

#include "ES8388.h"
#include "driver/i2s.h"

// ES8388 es8388(18, 23, 400000);
ES8388 es8388(33, 32, 400000);

uint32_t timeLapsed, ledTick;
uint8_t volume = 12;
uint16_t rxbuf[256], txbuf[256];

i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 256,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0};

// 18/23 version
// i2s_pin_config_t pin_config = {
//    .bck_io_num = 5, .ws_io_num = 25, .data_out_num = 26, .data_in_num = 35};

// 33/32 version
i2s_pin_config_t pin_config = {
    .bck_io_num = 27, .ws_io_num = 25, .data_out_num = 26, .data_in_num = 35};

size_t readsize = 0;

void setup() {
  Serial.begin(115200);
  pinMode(39, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(19, OUTPUT);
  pinMode(22, OUTPUT);
  Serial.println("Read Reg ES8388 : ");
  if (!es8388.init()) Serial.println("Init Fail");
  es8388.inputSelect(IN2);
  es8388.setInputGain(8);
  es8388.outputSelect(OUT2);
  es8388.setOutputVolume(volume);
  es8388.mixerSourceSelect(MIXADC, MIXADC);
  es8388.mixerSourceControl(DACOUT);
  uint8_t *reg;
  for (uint8_t i = 0; i < 53; i++) {
    reg = es8388.readAllReg();
    Serial.printf("Reg-%02d = 0x%02x\r\n", i, reg[i]);
  }

  // i2s
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
  WRITE_PERI_REG(PIN_CTRL, 0xFFF0);
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
}

void loop() {
  // read 256 samples (128 stereo samples)
  i2s_read(I2S_NUM_0, &rxbuf[0], 256 * 2, &readsize, 1000);
  for (int i = 0; i < 256; i++) {
    // direct transfer too txbuff
    txbuf[i] = rxbuf[i];
    // txbuf[i] = 0; //mute
  }
  // play received buffer
  i2s_write(I2S_NUM_0, &txbuf[0], 256 * 2, &readsize, 1000);
}