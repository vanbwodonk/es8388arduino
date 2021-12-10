#include <Arduino.h>

#include "ES8388.h"

// ES8388 es8388(18, 23, 400000);
ES8388 es8388(33, 32, 400000);

uint32_t timeLapsed, ledTick;
uint8_t volume = 12;

void setup() {
  Serial.begin(115200);
  pinMode(39, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(18, INPUT_PULLUP);
  pinMode(19, OUTPUT);
  pinMode(22, OUTPUT);
  Serial.println("Read Reg ES8388 : ");
  if (!es8388.init()) Serial.println("Init Fail");
  es8388.inputSelect(IN2);
  es8388.setInputGain(8);
  es8388.outputSelect(OUT2);
  es8388.setOutputVolume(volume);
  es8388.mixerSourceSelect(MIXADC, MIXADC);
  es8388.mixerSourceControl(SRCSELOUT);
  uint8_t *reg;
  for (uint8_t i = 0; i < 53; i++) {
    reg = es8388.readAllReg();
    Serial.printf("Reg-%02d = 0x%02x\r\n", i, reg[i]);
  }
}

void loop() {
  timeLapsed = millis();
  if (digitalRead(39))
    digitalWrite(19, HIGH);
  else
    digitalWrite(19, LOW);
  if (timeLapsed - ledTick > 2000) {
    ledTick = timeLapsed;
    digitalWrite(22, !digitalRead(22));
  }
  if (digitalRead(5) == LOW) {
    delay(10);
    while (digitalRead(5) == LOW)
      ;
    volume++;
    if (volume > 33) volume = 33;
    es8388.setOutputVolume(volume);
    Serial.printf("Volume ++ : %d\r\n", volume);
  }
  if (digitalRead(18) == LOW) {
    delay(10);
    while (digitalRead(18) == LOW)
      ;
    if (volume != 0) volume--;
    es8388.setOutputVolume(volume);
    Serial.printf("Volume -- : %d\r\n", volume);
  }
}