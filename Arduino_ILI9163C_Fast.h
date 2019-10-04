// Fast ILI9163C 128x128 LCD SPI display library
// (c) 2019 by Pawel A. Hernik

#ifndef _ILI9163C_FAST_H_
#define _ILI9163C_FAST_H_

// ------------------------------
// Define LCD PCB type
//#define __144_RED_PCB__ //128x128
#define __144_BLACK_PCB__ //128x128
//#define __22_RED_PCB__  //240x320

// remove "define COMPATIBILITY_MODE" for best performance on 16MHz AVR Arduinos
// if defined - the library should work on all Arduino compatible boards
//#define COMPATIBILITY_MODE

// define for LCD boards where CS pin is internally connected to the ground
//#define CS_ALWAYS_LOW
// ------------------------------

#include "Arduino.h"
#include "Print.h"
#include <Adafruit_GFX.h>
#include <avr/pgmspace.h>


#define ST_CMD_DELAY   0x80

// ---------- ILI9163C registers -------------------
// taken from https://github.com/sumotoy/TFT_ILI9163C
#define ILI9163C_NOP     	0x00  // Non operation
#define ILI9163C_SWRESET 	0x01  // Soft Reset
#define ILI9163C_SLPIN   	0x10  // Sleep ON
#define ILI9163C_SLPOUT  	0x11  // Sleep OFF
#define ILI9163C_PTLON   	0x12  // Partial Mode ON
#define ILI9163C_NORML   	0x13  // Normal Display ON
#define ILI9163C_DINVOF  	0x20  // Display Inversion OFF
#define ILI9163C_DINVON   0x21  // Display Inversion ON
#define ILI9163C_GAMMASET 0x26  // Gamma Set (0x01[1],0x02[2],0x04[3],0x08[4])
#define ILI9163C_DISPOFF 	0x28  // Display OFF
#define ILI9163C_DISPON  	0x29  // Display ON
#define ILI9163C_IDLEON  	0x39  // Idle Mode ON
#define ILI9163C_IDLEOF  	0x38  // Idle Mode OFF
#define ILI9163C_CLMADRS  0x2A  // Column Address Set
#define ILI9163C_PGEADRS  0x2B  // Page Address Set

#define ILI9163C_RAMWR   	0x2C  // Memory Write
#define ILI9163C_RAMRD   	0x2E  // Memory Read
#define ILI9163C_CLRSPACE 0x2D  // Color Space : 4K/65K/262K
#define ILI9163C_PARTAREA	0x30  // Partial Area
#define ILI9163C_VSCLLDEF	0x33  // Vertical Scroll Definition
#define ILI9163C_TEFXLON	0x35  // Tearing Effect Line ON
#define ILI9163C_TEFXLOF	0x34  // Tearing Effect Line OFF
#define ILI9163C_MADCTL  	0x36  // Memory Access Control
#define ILI9163C_VSSTADRS	0x37  // Vertical Scrolling Start address
#define ILI9163C_PIXFMT  	0x3A  // Interface Pixel Format
#define ILI9163C_FRMCTR1 	0xB1  // Frame Rate Control (In normal mode/Full colors)
#define ILI9163C_FRMCTR2 	0xB2  // Frame Rate Control(In Idle mode/8-colors)
#define ILI9163C_FRMCTR3 	0xB3  // Frame Rate Control(In Partial mode/full colors)
#define ILI9163C_DINVCTR	0xB4  // Display Inversion Control
#define ILI9163C_RGBBLK		0xB5  // RGB Interface Blanking Porch setting
#define ILI9163C_DFUNCTR 	0xB6  // Display Fuction set 5
#define ILI9163C_SDRVDIR 	0xB7  // Source Driver Direction Control
#define ILI9163C_GDRVDIR 	0xB8  // Gate Driver Direction Control 

#define ILI9163C_PWCTR1  	0xC0  // Power_Control1
#define ILI9163C_PWCTR2  	0xC1  // Power_Control2
#define ILI9163C_PWCTR3  	0xC2  // Power_Control3
#define ILI9163C_PWCTR4  	0xC3  // Power_Control4
#define ILI9163C_PWCTR5  	0xC4  // Power_Control5
#define ILI9163C_VCOMCTR1 0xC5  // VCOM_Control 1
#define ILI9163C_VCOMCTR2 0xC6  // VCOM_Control 2
#define ILI9163C_VCOMOFFS 0xC7  // VCOM Offset Control
#define ILI9163C_PGAMMAC	0xE0  // Positive Gamma Correction Setting
#define ILI9163C_NGAMMAC	0xE1  // Negative Gamma Correction Setting
#define ILI9163C_GAMRSEL	0xF2  // GAM_R_SEL 

// ------- bits in MADCTL -------
#define ILI9163C_MADCTL_MY  0x80
#define ILI9163C_MADCTL_MX  0x40
#define ILI9163C_MADCTL_MV  0x20
#define ILI9163C_MADCTL_ML  0x10
#define ILI9163C_MADCTL_BGR 0x08
#define ILI9163C_MADCTL_RGB 0x00

#if defined(__144_RED_PCB__)
	#define _TFTWIDTH  		128 // real W resolution of the TFT
	#define _TFTHEIGHT 		128 // real H resolution of the TFT
	#define _GRAMWIDTH    128
	#define _GRAMHEIGH    160
	#define _GRAMSIZE		  _GRAMWIDTH * _GRAMHEIGH
	#define __COLORSPC		ILI9163C_MADCTL_BGR
	#define __GAMMASET3
	#define __OFFSET		  32
#elif defined (__144_BLACK_PCB__)
	#define _TFTWIDTH  		128 // real W resolution of the TFT
	#define _TFTHEIGHT 		128 // real H resolution of the TFT
	#define _GRAMWIDTH    128
	#define _GRAMHEIGH    128
	#define _GRAMSIZE		  _GRAMWIDTH * _GRAMHEIGH
	#define __COLORSPC		ILI9163C_MADCTL_BGR
	#define __GAMMASET1
	#define __OFFSET	    0
#elif defined (__22_RED_PCB__)
	#define _TFTWIDTH  		240 // real W resolution of the TFT
	#define _TFTHEIGHT 		320 // real H resolution of the TFT
	#define _GRAMWIDTH    240
	#define _GRAMHEIGH    320
	#define _GRAMSIZE		  _GRAMWIDTH * _GRAMHEIGH
	#define __COLORSPC		ILI9163C_MADCTL_BGR
	#define __GAMMASET1
	#define __OFFSET	    0
#else
	#define _TFTWIDTH  		128
	#define _TFTHEIGHT 		160
	#define _GRAMWIDTH    128
	#define _GRAMHEIGH    160
	#define _GRAMSIZE		  _GRAMWIDTH * _GRAMHEIGH
	#define __COLORSPC		ILI9163C_MADCTL_BGR
	#define __GAMMASET1
	#define __OFFSET      0
#endif

#if defined(__GAMMASET1)
const uint8_t pGammaSet[15]= {0x36,0x29,0x12,0x22,0x1C,0x15,0x42,0xB7,0x2F,0x13,0x12,0x0A,0x11,0x0B,0x06};
const uint8_t nGammaSet[15]= {0x09,0x16,0x2D,0x0D,0x13,0x15,0x40,0x48,0x53,0x0C,0x1D,0x25,0x2E,0x34,0x39};
#elif defined(__GAMMASET2)
const uint8_t pGammaSet[15]= {0x3F,0x21,0x12,0x22,0x1C,0x15,0x42,0xB7,0x2F,0x13,0x02,0x0A,0x01,0x00,0x00};
const uint8_t nGammaSet[15]= {0x09,0x18,0x2D,0x0D,0x13,0x15,0x40,0x48,0x53,0x0C,0x1D,0x25,0x2E,0x24,0x29};
#elif defined(__GAMMASET3)
const uint8_t pGammaSet[15]= {0x3F,0x26,0x23,0x30,0x28,0x10,0x55,0xB7,0x40,0x19,0x10,0x1E,0x02,0x01,0x00};
const uint8_t nGammaSet[15]= {0x09,0x18,0x2D,0x0D,0x13,0x15,0x40,0x48,0x53,0x0C,0x1D,0x25,0x2E,0x24,0x29};
#else
const uint8_t pGammaSet[15]= {0x3F,0x25,0x1C,0x1E,0x20,0x12,0x2A,0x90,0x24,0x11,0x00,0x00,0x00,0x00,0x00};
const uint8_t nGammaSet[15]= {0x20,0x20,0x20,0x20,0x05,0x15,0x00,0xA7,0x3D,0x18,0x25,0x2A,0x2B,0x2B,0x3A};
#endif	

// Color definitions
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define RGBto565(r,g,b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3)) 

class Arduino_ILI9163C : public Adafruit_GFX {

 public:
  Arduino_ILI9163C(int8_t DC, int8_t RST, int8_t CS = -1);

  void init();
  void begin() { init(); }
  int errorCode() { return 0; }
  void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
  void pushColor(uint16_t color);
  void fillScreen(uint16_t color=BLACK);
  void clearScreen() { fillScreen(BLACK); }
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void drawImage(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *img);
  void drawImageF(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *img16);
  void setRotation(uint8_t r);
  void invertDisplay(boolean mode);
  void partialDisplay(boolean mode);
  void sleepDisplay(boolean mode);
  void enableDisplay(boolean mode);
  void idleDisplay(boolean mode);
  void resetDisplay();
  void setScrollArea(uint16_t tfa, uint16_t bfa);
  void setScroll(uint16_t vsp);
  void setPartArea(uint16_t sr, uint16_t er);

  uint16_t Color565(uint8_t r, uint8_t g, uint8_t b);
  uint16_t color565(uint8_t r, uint8_t g, uint8_t b) { return Color565(r, g, b); } 
  void rgbWheel(int idx, uint8_t *_r, uint8_t *_g, uint8_t *_b);

 protected:
  void displayInit(const uint8_t *addr);
  void writeSPI(uint8_t);
  void writeCmd(uint8_t c);
  void writeData(uint8_t d);
  void writeData16(uint16_t d);
  void commonILI9163CInit(const uint8_t *cmdList);
	
 private:
  int8_t  csPin, dcPin, rstPin;
  uint8_t  csMask, dcMask;
  volatile uint8_t  *csPort, *dcPort;

};

#endif
