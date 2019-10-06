// Fast ILI9163C 128x128 LCD SPI display library
// (c) 2019 by Pawel A. Hernik

#include "Arduino_ILI9163C_Fast.h"
#include <limits.h>
#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h>

// Initialization commands for ILI9163C
// taken from https://github.com/sumotoy/TFT_ILI9163C
static const uint8_t PROGMEM init_ILI9163C[] = {
  18,
  ILI9163C_SWRESET, ST_CMD_DELAY, 100,
  ILI9163C_SLPOUT, ST_CMD_DELAY, 5,
  ILI9163C_PIXFMT, 1+ST_CMD_DELAY, 0x05, 5,
  ILI9163C_GAMMASET, 1+ST_CMD_DELAY, 0x04, 1,
  ILI9163C_GAMRSEL, 1+ST_CMD_DELAY, 0x01, 1,
  ILI9163C_NORML, 0,
  ILI9163C_DFUNCTR, 2,0b11111111,0b00000110,
  
  ILI9163C_FRMCTR1, 2+ST_CMD_DELAY, 0x08,0x02, 1,
  ILI9163C_DINVCTR, 1+ST_CMD_DELAY, 0x07, 1,
  ILI9163C_PWCTR1, 2+ST_CMD_DELAY, 0x0A,0x02, 1,
  ILI9163C_PWCTR2, 1+ST_CMD_DELAY, 0x02, 1,
  ILI9163C_VCOMCTR1, 2+ST_CMD_DELAY, 0x50,99, 1,
  ILI9163C_VCOMOFFS, 1+ST_CMD_DELAY, 0, 1,
  ILI9163C_CLMADRS, 4, 0,0, (_GRAMWIDTH>>8),(_GRAMWIDTH),
  ILI9163C_PGEADRS, 4, 0,0, (_GRAMHEIGH>>8),(_GRAMHEIGH),
  ILI9163C_VSCLLDEF, 6, (__OFFSET>>8),(__OFFSET), (_GRAMHEIGH - __OFFSET)>>8,(_GRAMHEIGH - __OFFSET), 0,0,

  ILI9163C_DISPON, ST_CMD_DELAY, 1,
  ILI9163C_RAMWR, ST_CMD_DELAY, 1
};

#ifdef COMPATIBILITY_MODE
static SPISettings spiSettings;
#define SPI_START  SPI.beginTransaction(spiSettings)
#define SPI_END    SPI.endTransaction()
#else
#define SPI_START
#define SPI_END
#endif

// macros for fast DC and CS state changes
#ifdef COMPATIBILITY_MODE
#define DC_DATA     digitalWrite(dcPin, HIGH)
#define DC_COMMAND  digitalWrite(dcPin, LOW)
#define CS_IDLE     digitalWrite(csPin, HIGH)
#define CS_ACTIVE   digitalWrite(csPin, LOW)
#else
#define DC_DATA    *dcPort |= dcMask
#define DC_COMMAND *dcPort &= ~dcMask
#define CS_IDLE    *csPort |= csMask
#define CS_ACTIVE  *csPort &= ~csMask
#endif

// if CS always connected to the ground then don't do anything for better performance
#ifdef CS_ALWAYS_LOW
#define CS_IDLE
#define CS_ACTIVE
#endif

// ----------------------------------------------------------
// speed test results:
// in AVR best performance mode -> about 6.9 Mbps
// in compatibility mode (SPI.transfer(c)) -> about 4 Mbps
inline void Arduino_ILI9163C::writeSPI(uint8_t c) 
{
#ifdef COMPATIBILITY_MODE
    SPI.transfer(c);
#else
    SPDR = c;
    asm volatile("nop"); // 8 NOPs seem to be enough for 16MHz AVR @ DIV2 to avoid using while loop
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    //while(!(SPSR & _BV(SPIF))) ;
#endif
}

// ----------------------------------------------------------
Arduino_ILI9163C::Arduino_ILI9163C(int8_t dc, int8_t rst, int8_t cs) : Adafruit_GFX(_TFTWIDTH,_TFTHEIGHT) 
{
  csPin = cs;
  dcPin = dc;
  rstPin = rst;
}

// ----------------------------------------------------------
void Arduino_ILI9163C::init() 
{
  commonILI9163CInit(NULL);

  _width  = 128;
  _height = 128;
  displayInit(init_ILI9163C);
  setRotation(0);

  int i;
	writeCmd(ILI9163C_PGAMMAC); //Positive Gamma Correction Setting
	for(i=0;i<15;i++) writeData(pGammaSet[i]);
	writeCmd(ILI9163C_NGAMMAC); //Negative Gamma Correction Setting
	for(i=0;i<15;i++) writeData(nGammaSet[i]);
}

// ----------------------------------------------------------
void Arduino_ILI9163C::writeCmd(uint8_t c) 
{
  DC_COMMAND;
  CS_ACTIVE;
  SPI_START;

  writeSPI(c);

  CS_IDLE;
  SPI_END;
}

// ----------------------------------------------------------
void Arduino_ILI9163C::writeData(uint8_t d) 
{
  DC_DATA;
  CS_ACTIVE;
  SPI_START;
    
  writeSPI(d);

  CS_IDLE;
  SPI_END;
}
// ----------------------------------------------------------
void Arduino_ILI9163C::writeData16(uint16_t d) 
{
  DC_DATA;
  CS_ACTIVE;
  SPI_START;

  writeSPI(d>>8);
  writeSPI(d);

  CS_IDLE;
  SPI_END;
}

// ----------------------------------------------------------
void Arduino_ILI9163C::displayInit(const uint8_t *addr) 
{
  uint8_t numCommands, numArgs;
  uint16_t ms;
  numCommands = pgm_read_byte(addr++);
  while(numCommands--) {
    writeCmd(pgm_read_byte(addr++));
    numArgs  = pgm_read_byte(addr++);
    ms       = numArgs & ST_CMD_DELAY;
    numArgs &= ~ST_CMD_DELAY;
    while(numArgs--) writeData(pgm_read_byte(addr++));

    if(ms) {
      ms = pgm_read_byte(addr++);
      if(ms == 255) ms = 500;
      delay(ms);
    }
  }
}

// ----------------------------------------------------------
// Initialization code - common
void Arduino_ILI9163C::commonILI9163CInit(const uint8_t *cmdList) 
{
  pinMode(dcPin, OUTPUT);
#ifndef CS_ALWAYS_LOW
	pinMode(csPin, OUTPUT);
#endif

#ifndef COMPATIBILITY_MODE
  dcPort = portOutputRegister(digitalPinToPort(dcPin));
  dcMask = digitalPinToBitMask(dcPin);
#ifndef CS_ALWAYS_LOW
	csPort = portOutputRegister(digitalPinToPort(csPin));
	csMask = digitalPinToBitMask(csPin);
#endif
#endif

  SPI.begin();
#ifdef COMPATIBILITY_MODE
  spiSettings = SPISettings(16000000, MSBFIRST, SPI_MODE0);  // 8000000 gives max speed on AVR 16MHz
#else
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setDataMode(SPI_MODE0);
#endif

  CS_ACTIVE;
  if(rstPin != -1) {
    pinMode(rstPin, OUTPUT);
    digitalWrite(rstPin, HIGH);
    delay(50);
    digitalWrite(rstPin, LOW);
    delay(50);
    digitalWrite(rstPin, HIGH);
    delay(50);
  }

  if(cmdList) displayInit(cmdList);

	setRotation(0);
}

// ----------------------------------------------------------
void Arduino_ILI9163C::setRotation(uint8_t m) 
{
  uint8_t madctl;
  rotation = m & 3;
	switch (rotation) {
	case 0:
		madctl =  __COLORSPC;
		_width  = _TFTWIDTH;
		_height = _TFTHEIGHT;
		break;
	case 1:
		madctl = ILI9163C_MADCTL_MX | ILI9163C_MADCTL_MV | __COLORSPC;
		_width  = _TFTHEIGHT;
		_height = _TFTWIDTH;
		break;
	case 2:
		madctl = ILI9163C_MADCTL_MY | ILI9163C_MADCTL_MX | __COLORSPC;
		_width  = _TFTWIDTH;
		_height = _TFTHEIGHT;
		break;
	case 3:
		madctl = ILI9163C_MADCTL_MY | ILI9163C_MADCTL_MV | __COLORSPC;
		_width  = _TFTWIDTH;
		_height = _TFTHEIGHT;
		break;
	}
	writeCmd(ILI9163C_MADCTL);
	writeData(madctl);
}

// ----------------------------------------------------------
void Arduino_ILI9163C::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) 
{
  uint16_t xs = x0, xe = x1;
  uint16_t ys = y0, ye = y1;

  CS_ACTIVE;
  SPI_START;
  
  DC_COMMAND; writeSPI(ILI9163C_CLMADRS);
  DC_DATA;
  writeSPI(xs >> 8); writeSPI(xs & 0xFF);
  writeSPI(xe >> 8); writeSPI(xe & 0xFF);

  DC_COMMAND; writeSPI(ILI9163C_PGEADRS);
  DC_DATA;
  writeSPI(ys >> 8); writeSPI(ys & 0xFF);
  writeSPI(ye >> 8); writeSPI(ye & 0xFF);

  DC_COMMAND; writeSPI(ILI9163C_RAMWR);
  
  CS_IDLE;
  SPI_END;
}

// ----------------------------------------------------------
void Arduino_ILI9163C::pushColor(uint16_t color) 
{
  SPI_START;
  DC_DATA;
  CS_ACTIVE;

  writeSPI(color >> 8); writeSPI(color);

  CS_IDLE;
  SPI_END;
}

// ----------------------------------------------------------
void Arduino_ILI9163C::drawPixel(int16_t x, int16_t y, uint16_t color) 
{
  if(x<0 ||x>=_width || y<0 || y>=_height) return;
  setAddrWindow(x,y,x+1,y+1);

  SPI_START;
  DC_DATA;
  CS_ACTIVE;

  writeSPI(color >> 8); writeSPI(color);

  CS_IDLE;
  SPI_END;
}

// ----------------------------------------------------------
void Arduino_ILI9163C::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) 
{
  if(x>=_width || y>=_height || h<=0) return;
  if(y+h-1>=_height) h=_height-y;
  setAddrWindow(x, y, x, y+h-1);

  uint8_t hi = color >> 8, lo = color;
    
  SPI_START;
  DC_DATA;
  CS_ACTIVE;

  uint8_t num8 = h>>3;
  while(num8--) {
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
  }
  num8 = (uint8_t)h & 7;
  while(num8--) { writeSPI(hi); writeSPI(lo); }

  CS_IDLE;
  SPI_END;
}

// ----------------------------------------------------------
void Arduino_ILI9163C::drawFastHLine(int16_t x, int16_t y, int16_t w,  uint16_t color) 
{
  if(x>=_width || y>=_height || w<=0) return;
  if(x+w-1>=_width)  w=_width-x;
  setAddrWindow(x, y, x+w-1, y);

  uint8_t hi = color >> 8, lo = color;

  SPI_START;
  DC_DATA;
  CS_ACTIVE;

  uint8_t num8 = w>>3;
  while(num8--) {
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
  }
  num8 = (uint8_t)w & 7;
  while(num8--) { writeSPI(hi); writeSPI(lo); }

  CS_IDLE;
  SPI_END;
}

// ----------------------------------------------------------
void Arduino_ILI9163C::fillScreen(uint16_t color) 
{
  fillRect(0, 0, _width, _height, color);
}

// ----------------------------------------------------------
void Arduino_ILI9163C::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) 
{
  if(x>=_width || y>=_height || w<=0 || h<=0) return;
  if(x+w-1>=_width)  w=_width -x;
  if(y+h-1>=_height) h=_height-y;
  setAddrWindow(x, y, x+w-1, y+h-1);

  uint8_t hi = color >> 8, lo = color;
    
  SPI_START;
  DC_DATA;
  CS_ACTIVE;

  uint32_t num = (uint32_t)w*h;
  uint16_t num16 = num>>4;
  while(num16--) {
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
    writeSPI(hi); writeSPI(lo);
  }
  uint8_t num8 = num & 0xf;
  while(num8--) { writeSPI(hi); writeSPI(lo); }

  CS_IDLE;
  SPI_END;
}

// ----------------------------------------------------------
// draws image from RAM
void Arduino_ILI9163C::drawImage(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *img16) 
{
  if(x>=_width || y>=_height || w<=0 || h<=0) return;
  setAddrWindow(x, y, x+w-1, y+h-1);

  SPI_START;
  DC_DATA;
  CS_ACTIVE;

  uint32_t num = (uint32_t)w*h;
  uint16_t num16 = num>>3;
  uint8_t *img = (uint8_t *)img16;
  while(num16--) {
    writeSPI(*(img+1)); writeSPI(*(img+0)); img+=2;
    writeSPI(*(img+1)); writeSPI(*(img+0)); img+=2;
    writeSPI(*(img+1)); writeSPI(*(img+0)); img+=2;
    writeSPI(*(img+1)); writeSPI(*(img+0)); img+=2;
    writeSPI(*(img+1)); writeSPI(*(img+0)); img+=2;
    writeSPI(*(img+1)); writeSPI(*(img+0)); img+=2;
    writeSPI(*(img+1)); writeSPI(*(img+0)); img+=2;
    writeSPI(*(img+1)); writeSPI(*(img+0)); img+=2;
  }
  uint8_t num8 = num & 0x7;
  while(num8--) { writeSPI(*(img+1)); writeSPI(*(img+0)); img+=2; }

  CS_IDLE;
  SPI_END;
}

// ----------------------------------------------------------
// draws image from flash (PROGMEM)
void Arduino_ILI9163C::drawImageF(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *img16) 
{
  if(x>=_width || y>=_height || w<=0 || h<=0) return;
  setAddrWindow(x, y, x+w-1, y+h-1);

  SPI_START;
  DC_DATA;
  CS_ACTIVE;

  uint32_t num = (uint32_t)w*h;
  uint16_t num16 = num>>3;
  uint8_t *img = (uint8_t *)img16;
  while(num16--) {
    writeSPI(pgm_read_byte(img+1)); writeSPI(pgm_read_byte(img+0)); img+=2;
    writeSPI(pgm_read_byte(img+1)); writeSPI(pgm_read_byte(img+0)); img+=2;
    writeSPI(pgm_read_byte(img+1)); writeSPI(pgm_read_byte(img+0)); img+=2;
    writeSPI(pgm_read_byte(img+1)); writeSPI(pgm_read_byte(img+0)); img+=2;
    writeSPI(pgm_read_byte(img+1)); writeSPI(pgm_read_byte(img+0)); img+=2;
    writeSPI(pgm_read_byte(img+1)); writeSPI(pgm_read_byte(img+0)); img+=2;
    writeSPI(pgm_read_byte(img+1)); writeSPI(pgm_read_byte(img+0)); img+=2;
    writeSPI(pgm_read_byte(img+1)); writeSPI(pgm_read_byte(img+0)); img+=2;
  }
  uint8_t num8 = num & 0x7;
  while(num8--) { writeSPI(pgm_read_byte(img+1)); writeSPI(pgm_read_byte(img+0)); img+=2; }

  CS_IDLE;
  SPI_END;
}

// ----------------------------------------------------------
// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t Arduino_ILI9163C::Color565(uint8_t r, uint8_t g, uint8_t b) 
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// ----------------------------------------------------------
void Arduino_ILI9163C::invertDisplay(boolean mode) 
{
  writeCmd(mode ? ILI9163C_DINVON : ILI9163C_DINVOF);
}

// ----------------------------------------------------------
void Arduino_ILI9163C::partialDisplay(boolean mode) 
{
  writeCmd(mode ? ILI9163C_PTLON : ILI9163C_NORML);
}

// ----------------------------------------------------------
void Arduino_ILI9163C::sleepDisplay(boolean mode) 
{
  writeCmd(mode ? ILI9163C_SLPIN : ILI9163C_SLPOUT);
  delay(5);
}

// ----------------------------------------------------------
void Arduino_ILI9163C::enableDisplay(boolean mode) 
{
  writeCmd(mode ? ILI9163C_DISPON : ILI9163C_DISPOFF);
}

// ----------------------------------------------------------
void Arduino_ILI9163C::idleDisplay(boolean mode) 
{
  writeCmd(mode ? ILI9163C_IDLEON : ILI9163C_IDLEOF);
}

// ----------------------------------------------------------
void Arduino_ILI9163C::resetDisplay() 
{
  writeCmd(ILI9163C_SWRESET);
  delay(500);
}

// ----------------------------------------------------------
void Arduino_ILI9163C::setScrollArea(uint16_t tfa, uint16_t bfa) 
{
  tfa += __OFFSET;
  uint16_t vsa = _GRAMHEIGH-tfa-bfa;
  writeCmd(ILI9163C_VSCLLDEF);
  writeData(tfa >> 8); writeData(tfa);
  writeData(vsa >> 8); writeData(vsa);
  writeData(bfa >> 8); writeData(bfa);
}

// ----------------------------------------------------------
void Arduino_ILI9163C::setScroll(uint16_t vsp) 
{
  writeCmd(ILI9163C_VSSTADRS);
  vsp+=__OFFSET;
  writeData(vsp >> 8);
  writeData(vsp);
}

// ----------------------------------------------------------
void Arduino_ILI9163C::setPartArea(uint16_t sr, uint16_t er) 
{
  writeCmd(ILI9163C_PARTAREA);  // SETPARTAREA = 0x30
  writeData(sr >> 8);
  writeData(sr);
  writeData(er >> 8);
  writeData(er);
}

// ------------------------------------------------
// Input a value 0 to 511 (85*6) to get a color value.
// The colours are a transition R - Y - G - C - B - M - R.
void Arduino_ILI9163C::rgbWheel(int idx, uint8_t *_r, uint8_t *_g, uint8_t *_b)
{
  idx &= 0x1ff;
  if(idx < 85) { // R->Y  
    *_r = 255; *_g = idx * 3; *_b = 0;
    return;
  } else if(idx < 85*2) { // Y->G
    idx -= 85*1;
    *_r = 255 - idx * 3; *_g = 255; *_b = 0;
    return;
  } else if(idx < 85*3) { // G->C
    idx -= 85*2;
    *_r = 0; *_g = 255; *_b = idx * 3;
    return;  
  } else if(idx < 85*4) { // C->B
    idx -= 85*3;
    *_r = 0; *_g = 255 - idx * 3; *_b = 255;
    return;    
  } else if(idx < 85*5) { // B->M
    idx -= 85*4;
    *_r = idx * 3; *_g = 0; *_b = 255;
    return;    
  } else { // M->R
    idx -= 85*5;
    *_r = 255; *_g = 0; *_b = 255 - idx * 3;
   return;
  }
} 

uint16_t Arduino_ILI9163C::rgbWheel(int idx)
{
  uint8_t r,g,b;
  rgbWheel(idx, &r,&g,&b);
  return RGBto565(r,g,b);
}

// ------------------------------------------------