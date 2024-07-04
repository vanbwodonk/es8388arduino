#include "ES8388.h"

#include <Arduino.h>
#include <Wire.h>

#define ES8388_ADDR 0x10
/* ES8388 register */
#define ES8388_CONTROL1 0x00
#define ES8388_CONTROL2 0x01
#define ES8388_CHIPPOWER 0x02
#define ES8388_ADCPOWER 0x03
#define ES8388_DACPOWER 0x04
#define ES8388_CHIPLOPOW1 0x05
#define ES8388_CHIPLOPOW2 0x06
#define ES8388_ANAVOLMANAG 0x07
#define ES8388_MASTERMODE 0x08
/* ADC */
#define ES8388_ADCCONTROL1 0x09
#define ES8388_ADCCONTROL2 0x0a
#define ES8388_ADCCONTROL3 0x0b
#define ES8388_ADCCONTROL4 0x0c
#define ES8388_ADCCONTROL5 0x0d
#define ES8388_ADCCONTROL6 0x0e
#define ES8388_ADCCONTROL7 0x0f
#define ES8388_ADCCONTROL8 0x10
#define ES8388_ADCCONTROL9 0x11
#define ES8388_ADCCONTROL10 0x12
#define ES8388_ADCCONTROL11 0x13
#define ES8388_ADCCONTROL12 0x14
#define ES8388_ADCCONTROL13 0x15
#define ES8388_ADCCONTROL14 0x16
/* DAC */
#define ES8388_DACCONTROL1 0x17
#define ES8388_DACCONTROL2 0x18
#define ES8388_DACCONTROL3 0x19
#define ES8388_DACCONTROL4 0x1a
#define ES8388_DACCONTROL5 0x1b
#define ES8388_DACCONTROL6 0x1c
#define ES8388_DACCONTROL7 0x1d
#define ES8388_DACCONTROL8 0x1e
#define ES8388_DACCONTROL9 0x1f
#define ES8388_DACCONTROL10 0x20
#define ES8388_DACCONTROL11 0x21
#define ES8388_DACCONTROL12 0x22
#define ES8388_DACCONTROL13 0x23
#define ES8388_DACCONTROL14 0x24
#define ES8388_DACCONTROL15 0x25
#define ES8388_DACCONTROL16 0x26
#define ES8388_DACCONTROL17 0x27
#define ES8388_DACCONTROL18 0x28
#define ES8388_DACCONTROL19 0x29
#define ES8388_DACCONTROL20 0x2a
#define ES8388_DACCONTROL21 0x2b
#define ES8388_DACCONTROL22 0x2c
#define ES8388_DACCONTROL23 0x2d
#define ES8388_DACCONTROL24 0x2e
#define ES8388_DACCONTROL25 0x2f
#define ES8388_DACCONTROL26 0x30
#define ES8388_DACCONTROL27 0x31
#define ES8388_DACCONTROL28 0x32
#define ES8388_DACCONTROL29 0x33
#define ES8388_DACCONTROL30 0x34

ES8388::ES8388(uint8_t _sda, uint8_t _scl, uint32_t _speed) {
  _pinsda = _sda;
  _pinscl = _scl;
  _i2cspeed = _speed;
  i2c.begin(_sda, _scl, _speed);
}

ES8388::~ES8388() { i2c.~TwoWire(); }

bool ES8388::write_reg(uint8_t reg_add, uint8_t data) {
  i2c.beginTransmission(ES8388_ADDR);
  i2c.write(reg_add);
  i2c.write(data);
  return i2c.endTransmission() == 0;
}

bool ES8388::read_reg(uint8_t reg_add, uint8_t& data) {
  bool retval = false;
  i2c.beginTransmission(ES8388_ADDR);
  i2c.write(reg_add);
  i2c.endTransmission(false);
  i2c.requestFrom((uint16_t)ES8388_ADDR, (uint8_t)1, true);
  if (i2c.available() >= 1) {
    data = i2c.read();
    retval = true;
  }
  return retval;
}

bool ES8388::identify(int sda, int scl, uint32_t frequency) {
  i2c.begin(sda, scl, frequency);
  i2c.beginTransmission(ES8388_ADDR);
  return i2c.endTransmission() == 0;
}

uint8_t* ES8388::readAllReg() {
  static uint8_t reg[53];
  for (uint8_t i = 0; i < 53; i++) {
    read_reg(i, reg[i]);
  }
  return reg;
}

bool ES8388::init() {
  bool res = true;
  /* INITIALIZATION (BASED ON ES8388 USER GUIDE EXAMPLE) */
  // Set Chip to Slave
  res &= write_reg(ES8388_MASTERMODE, 0x00);
  // Power down DEM and STM
  res &= write_reg(ES8388_CHIPPOWER, 0xFF);
  // Set same LRCK	Set same LRCK
  res &= write_reg(ES8388_DACCONTROL21, 0x80);
  // Set Chip to Play&Record Mode
  res &= write_reg(ES8388_CONTROL1, 0x05);
  // Power Up Analog and Ibias
  res &= write_reg(ES8388_CONTROL2, 0x40);

  /* ADC setting */
  // Micbias for Record
  res &= write_reg(ES8388_ADCPOWER, 0x00);
  // Enable Lin1/Rin1 (0x00 0x00) for Lin2/Rin2 (0x50 0x80)
  res &= write_reg(ES8388_ADCCONTROL2, 0x50);
  res &= write_reg(ES8388_ADCCONTROL3, 0x80);
  // PGA gain (0x88 - 24db) (0x77 - 21db)
  res &= write_reg(ES8388_ADCCONTROL1, 0x77);
  // SFI setting (i2s mode/16 bit)
  res &= write_reg(ES8388_ADCCONTROL4, 0x0C);
  // ADC MCLK/LCRK ratio (256)
  res &= write_reg(ES8388_ADCCONTROL5, 0x02);
  // set ADC digital volume
  res &= write_reg(ES8388_ADCCONTROL8, 0x00);
  res &= write_reg(ES8388_ADCCONTROL9, 0x00);
  // recommended ALC setting for VOICE refer to ES8388 MANUAL
  res &= write_reg(ES8388_ADCCONTROL10, 0xEA);
  res &= write_reg(ES8388_ADCCONTROL11, 0xC0);
  res &= write_reg(ES8388_ADCCONTROL12, 0x12);
  res &= write_reg(ES8388_ADCCONTROL13, 0x06);
  res &= write_reg(ES8388_ADCCONTROL14, 0xC3);

  /* DAC setting */
  // Power Up DAC& enable Lout/Rout
  res &= write_reg(ES8388_DACPOWER, 0x3C);
  // SFI setting (i2s mode/16 bit)
  res &= write_reg(ES8388_DACCONTROL1, 0x18);
  // DAC MCLK/LCRK ratio (256)
  res &= write_reg(ES8388_DACCONTROL2, 0x02);
  // unmute codec
  res &= write_reg(ES8388_DACCONTROL3, 0x00);
  // set DAC digital volume
  res &= write_reg(ES8388_DACCONTROL4, 0x00);
  res &= write_reg(ES8388_DACCONTROL5, 0x00);
  // Setup Mixer
  // (reg[16] 1B mic Amp, 0x09 direct;[reg 17-20] 0x90 DAC, 0x50 Mic Amp)
  res &= write_reg(ES8388_DACCONTROL16, 0x09);
  res &= write_reg(ES8388_DACCONTROL17, 0x50);
  res &= write_reg(ES8388_DACCONTROL18, 0x38);  //??
  res &= write_reg(ES8388_DACCONTROL19, 0x38);  //??
  res &= write_reg(ES8388_DACCONTROL20, 0x50);
  // set Lout/Rout Volume -45db
  res &= write_reg(ES8388_DACCONTROL24, 0x00);
  res &= write_reg(ES8388_DACCONTROL25, 0x00);
  res &= write_reg(ES8388_DACCONTROL26, 0x00);
  res &= write_reg(ES8388_DACCONTROL27, 0x00);

  /* Power up DEM and STM */
  res &= write_reg(ES8388_CHIPPOWER, 0x00);
  /* set up MCLK) */
  return res;
}

// Select output sink
// OUT1 -> Select Line OUTL/R1
// OUT2 -> Select Line OUTL/R2
// OUTALL -> Enable ALL
bool ES8388::outputSelect(outsel_t _sel) {
  bool res = true;
  if (_sel == OUTALL)
    res &= write_reg(ES8388_DACPOWER, 0x3C);
  else if (_sel == OUT1)
    res &= write_reg(ES8388_DACPOWER, 0x30);
  else if (_sel == OUT2)
    res &= write_reg(ES8388_DACPOWER, 0x0C);
  _outSel = _sel;
  return res;
}

// Select input source
// IN1     -> Select Line IN L/R 1
// IN2     -> Select Line IN L/R 2
// IN1DIFF -> differential IN L/R 1
// IN2DIFF -> differential IN L/R 2
bool ES8388::inputSelect(insel_t sel) {
  bool res = true;
  if (sel == IN1)
    res &= write_reg(ES8388_ADCCONTROL2, 0x00);
  else if (sel == IN2)
    res &= write_reg(ES8388_ADCCONTROL2, 0x50);
  else if (sel == IN1DIFF) {
    res &= write_reg(ES8388_ADCCONTROL2, 0xF0);
    res &= write_reg(ES8388_ADCCONTROL3, 0x00);
  } else if (sel == IN2DIFF) {
    res &= write_reg(ES8388_ADCCONTROL2, 0xF0);
    res &= write_reg(ES8388_ADCCONTROL3, 0x80);
  }
  _inSel = sel;
  return res;
}

// mute Output
bool ES8388::DACmute(bool mute) {
  uint8_t _reg;
  read_reg(ES8388_ADCCONTROL1, _reg);
  bool res = true;
  if (mute)
    res &= write_reg(ES8388_DACCONTROL3, _reg | 0x04);
  else
    res &= write_reg(ES8388_DACCONTROL3, _reg & ~(0x04));
  return res;
}

// set output volume max is 33
bool ES8388::setOutputVolume(uint8_t vol) {
  if (vol > 33)
    vol = 33;
  bool res = true;
  if (_outSel == OUTALL || _outSel == OUT1) {
    res &= write_reg(ES8388_DACCONTROL24, vol); // LOUT1VOL
    res &= write_reg(ES8388_DACCONTROL25, vol); // ROUT1VOL
  } if (_outSel == OUTALL || _outSel == OUT2) {
    res &= write_reg(ES8388_DACCONTROL26, vol); // LOUT2VOL
    res &= write_reg(ES8388_DACCONTROL27, vol); // ROUT2VOL
  }
  return res;
}

uint8_t ES8388::getOutputVolume() {
  static uint8_t _reg;
  if(_outSel == OUT1)
    read_reg(ES8388_DACCONTROL24, _reg);
  else if(_outSel == OUT2)
    read_reg(ES8388_DACCONTROL26, _reg);
  return _reg;
}

// set input gain max is 8 +24db
bool ES8388::setInputGain(uint8_t gain) {
  if (gain > 8) gain = 8;
  bool res = true;
  gain = (gain << 4) | gain;
  res &= write_reg(ES8388_ADCCONTROL1, gain);
  return res;
}

uint8_t ES8388::getInputGain() {
  static uint8_t _reg;
  read_reg(ES8388_ADCCONTROL1, _reg);
  _reg = _reg & 0x0F;
  return _reg;
}

// Recommended ALC setting from User Guide
// DISABLE -> Disable ALC
// GENERIC -> Generic Mode
// VOICE   -> Voice Mode
// MUSIC   -> Music Mode
bool ES8388::setALCmode(alcmodesel_t alc) {
  bool res = true;

  // generic ALC setting
  uint8_t ALCSEL = 0b11;       // stereo
  uint8_t ALCLVL = 0b0011;     //-12db
  uint8_t MAXGAIN = 0b111;     //+35.5db
  uint8_t MINGAIN = 0b000;     //-12db
  uint8_t ALCHLD = 0b0000;     // 0ms
  uint8_t ALCDCY = 0b0101;     // 13.1ms/step
  uint8_t ALCATK = 0b0111;     // 13.3ms/step
  uint8_t ALCMODE = 0b0;       // ALC
  uint8_t ALCZC = 0b0;         // ZC off
  uint8_t TIME_OUT = 0b0;      // disable
  uint8_t NGAT = 0b1;          // enable
  uint8_t NGTH = 0b10001;      //-51db
  uint8_t NGG = 0b00;          // hold gain
  uint8_t WIN_SIZE = 0b00110;  // default

  if (alc == DISABLE)
    ALCSEL = 0b00;
  else if (alc == MUSIC) {
    ALCDCY = 0b1010;  // 420ms/step
    ALCATK = 0b0110;  // 6.66ms/step
    NGTH = 0b01011;   // -60db
  } else if (alc == VOICE) {
    ALCLVL = 0b1100;  // -4.5db
    MAXGAIN = 0b101;  // +23.5db
    MINGAIN = 0b010;  // 0db
    ALCDCY = 0b0001;  // 820us/step
    ALCATK = 0b0010;  // 416us/step
    NGTH = 0b11000;   // -40.5db
    NGG = 0b01;       // mute ADC
    res &= write_reg(ES8388_ADCCONTROL1, 0x77);
  }
  res &= write_reg(ES8388_ADCCONTROL10, ALCSEL << 6 | MAXGAIN << 3 | MINGAIN);
  res &= write_reg(ES8388_ADCCONTROL11, ALCLVL << 4 | ALCHLD);
  res &= write_reg(ES8388_ADCCONTROL12, ALCDCY << 4 | ALCATK);
  res &= write_reg(ES8388_ADCCONTROL13,
                   ALCMODE << 7 | ALCZC << 6 | TIME_OUT << 5 | WIN_SIZE);
  res &= write_reg(ES8388_ADCCONTROL14, NGTH << 3 | NGG << 2 | NGAT);

  return res;
}

// MIXIN1 – direct IN1 (default)
// MIXIN2 – direct IN2
// MIXRES – reserved es8388
// MIXADC – ADC/ALC input (after mic amplifier)
bool ES8388::mixerSourceSelect(mixsel_t LMIXSEL, mixsel_t RMIXSEL) {
  bool res = true;
  uint8_t _reg;
  _reg = (LMIXSEL << 3) | RMIXSEL;
  res &= write_reg(ES8388_DACCONTROL16, _reg);
  return res;
}

// LD/RD = DAC(i2s), false disable, true enable
// LI2LO/RI2RO from mixerSourceSelect(), false disable, true enable
// LOVOL = gain, 0 -> 6db, 1 -> 3db, 2 -> 0db, higher will attenuate
bool ES8388::mixerSourceControl(bool LD2LO, bool LI2LO, uint8_t LI2LOVOL,
                                bool RD2RO, bool RI2RO, uint8_t RI2LOVOL) {
  bool res = true;
  uint8_t _regL, _regR;
  if (LI2LOVOL > 7) LI2LOVOL = 7;
  if (RI2LOVOL > 7) RI2LOVOL = 7;
  _regL = (LD2LO << 7) | (LI2LO << 6) | (LI2LOVOL << 3);
  _regR = (RD2RO << 7) | (RI2RO << 6) | (RI2LOVOL << 3);
  res &= write_reg(ES8388_DACCONTROL17, _regL);
  res &= write_reg(ES8388_DACCONTROL20, _regR);
  return res;
}

// Mixer source control
// DACOUT -> Select Sink From DAC
// SRCSEL -> Select Sink From SourceSelect()
// MIXALL -> Sink DACOUT + SRCSEL
bool ES8388::mixerSourceControl(mixercontrol_t mix) {
  bool res = true;
  if (mix == DACOUT)
    mixerSourceControl(true, false, 2, true, false, 2);
  else if (mix == SRCSELOUT)
    mixerSourceControl(false, true, 2, false, true, 2);
  else if (mix == MIXALL)
    mixerSourceControl(true, true, 2, true, true, 2);
  return res;
}

// true -> analog out = analog in
// false -> analog out = DAC(i2s)
bool ES8388::analogBypass(bool bypass) {
  bool res = true;
  if (bypass) {
    if (_inSel == IN1)
      mixerSourceSelect(MIXIN1, MIXIN1);
    else if (_inSel == IN2)
      mixerSourceSelect(MIXIN2, MIXIN2);
    mixerSourceControl(false, true, 2, false, true, 2);
  } else {
    mixerSourceControl(true, false, 2, true, false, 2);
  }
  return res;
}
