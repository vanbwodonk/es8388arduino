#ifndef ES8388_H_
#define ES8388_H_

#include <Arduino.h>
#include <Wire.h>
#pragma once
#include <stdint.h>

typedef enum {
  MIXIN1,  // direct line 1
  MIXIN2,  // direct line 2
  MIXRES,  // reserverd es8388
  MIXADC   // Select from ADC/ALC
} mixsel_t;

typedef enum {
  OUT1,    // Select Line OUT L/R 1
  OUT2,    // Select Line OUT L/R 2
  OUTALL,  // Enable ALL
} outsel_t;

typedef enum {
  IN1,      // Select Line IN L/R 1
  IN2,      // Select Line IN L/R 2
  IN1DIFF,  // differential IN L/R 1
  IN2DIFF   // differential IN L/R 2
} insel_t;

typedef enum {
  DACOUT,     // Select Sink From DAC
  SRCSELOUT,  // Select Sink From SourceSelect()
  MIXALL,     // Sink ALL DAC & SourceSelect()
} mixercontrol_t;

typedef enum {
  DISABLE,  // Disable ALC
  GENERIC,  // Generic Mode
  VOICE,    // Voice Mode
  MUSIC     // Music Mode
} alcmodesel_t;

class ES8388 {
 private:
  outsel_t _outSel = OUTALL;
  insel_t _inSel = IN1;
  TwoWire i2c = TwoWire(0);
  uint8_t _pinsda, _pinscl;
  uint32_t _i2cspeed;
  bool write_reg(uint8_t reg_add, uint8_t data);
  bool read_reg(uint8_t reg_add, uint8_t& data);

 public:
  ES8388(uint8_t _sda, uint8_t _scl, uint32_t _speed);
  ~ES8388();
  bool init();
  bool identify(int sda, int scl, uint32_t frequency);
  uint8_t* readAllReg();
  bool outputSelect(outsel_t sel);
  bool inputSelect(insel_t sel);
  bool DACmute(bool mute);
  uint8_t getOutputVolume();
  bool setOutputVolume(uint8_t vol);
  uint8_t getInputGain();
  bool setInputGain(uint8_t gain);
  bool setALCmode(alcmodesel_t alc);
  bool mixerSourceSelect(mixsel_t LMIXSEL, mixsel_t RMIXSEL);
  bool mixerSourceControl(bool LD2LO, bool LI2LO, uint8_t LI2LOVOL, bool RD2RO,
                          bool RI2RO, uint8_t RI2LOVOL);
  bool mixerSourceControl(mixercontrol_t mix);
  bool analogBypass(bool bypass);
};

#endif