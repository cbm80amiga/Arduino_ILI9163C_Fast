// ILI9163C library example
// (c) 2019 Pawel A. Hernik

// requires RRE Font library:
// https://github.com/cbm80amiga/RREFont


/*
Pinout (header on the top, from left):
  LED   -> 3.3V
  SCK   -> D13
  SDA   -> D11/MOSI
  A0/DC -> D8  or any digital
  RESET -> D9  or any digital
  CS    -> D10 or any digital
  GND   -> GND
  VCC   -> 3.3V
*/

#define SCR_WD   128
#define SCR_HT   128

#define TFT_CS 10
#define TFT_DC  8
#define TFT_RST 9
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Arduino_ILI9163C_Fast.h>
Arduino_ILI9163C lcd = Arduino_ILI9163C(TFT_DC, TFT_RST, TFT_CS);

#include "RREFont.h"
#include "rre_fjg_8x16.h"

RREFont font;

// needed for RREFont library initialization, define your fillRect
void customRect(int x, int y, int w, int h, int c) { return lcd.fillRect(x, y, w, h, c); }

void setup() 
{
  Serial.begin(9600);
  lcd.init();
  font.init(customRect, SCR_WD, SCR_HT); // custom fillRect function and screen width and height values
  font.setFont(&rre_fjg_8x16);
  font.setScale(1); font.setSpacing(3);
  font.setColor(WHITE);
}

void rainbow()
{
  for(int i=0;i<128;i+=4) {
    uint8_t r,g,b;
    lcd.rgbWheel(i*512L/128,&r,&g,&b);
    lcd.fillRect(0,i,128,4,RGBto565(r,g,b));
  }
}

void loop()
{
  lcd.fillScreen(RGBto565(120,60,30));
  font.printStr(ALIGN_CENTER,45,"ILI9163C");
  font.printStr(ALIGN_CENTER,45+16,"modes");
  delay(2000);

  rainbow();
  font.setColor(BLACK);
  font.printStr(ALIGN_CENTER,50,"Idle mode OFF");
  lcd.idleDisplay(false); delay(2000);
  rainbow();
  font.printStr(ALIGN_CENTER,50,"Idle mode ON");
  lcd.idleDisplay(true); delay(4000);
  rainbow();
  font.printStr(ALIGN_CENTER,50,"Idle mode OFF");
  lcd.idleDisplay(false); delay(2000);

  rainbow();
  font.setColor(WHITE,BLACK);
  lcd.fillRect(10,46,128-20,22,BLACK);
  font.printStr(ALIGN_CENTER,50,"Invert OFF");
  lcd.invertDisplay(false); delay(2000);
  font.printStr(ALIGN_CENTER,50," Invert ON ");
  lcd.invertDisplay(true); delay(4000);
  font.printStr(ALIGN_CENTER,50,"Invert OFF");
  lcd.invertDisplay(false); delay(2000);

  font.setColor(WHITE);
  lcd.fillScreen(RGBto565(180,0,180));
  font.printStr(ALIGN_CENTER,50,"Sleep mode 2s");
  delay(2000);
  //lcd.enableDisplay(false); 
  lcd.sleepDisplay(true); delay(4000);
  lcd.sleepDisplay(false); 
  //lcd.enableDisplay(true);

  lcd.fillScreen(RGBto565(180,0,180));
  font.printStr(ALIGN_CENTER,50,"Display on/off");
  delay(2000);
  lcd.enableDisplay(false);  delay(4000);
  lcd.enableDisplay(true);  delay(1000);

  lcd.fillScreen(RGBto565(180,0,180));
  font.printStr(ALIGN_CENTER,50,"Partial display");
  font.setColor(YELLOW);
  font.printStr(ALIGN_CENTER,4,"Top");
  font.printStr(ALIGN_CENTER,128-18,"Bottom");
  font.setColor(WHITE);
  delay(2000);
  lcd.setPartArea(30*1, 30*3); lcd.partialDisplay(true); delay(4000);
  lcd.setPartArea(30*3, 30*1); lcd.partialDisplay(true); delay(4000);
  lcd.partialDisplay(false);
  delay(1000);
  
  lcd.fillScreen(RGBto565(180,0,0));
  font.printStr(ALIGN_CENTER,50,"Sw reset ...");
  delay(2000);
  lcd.resetDisplay();  delay(2000);
  lcd.init();
  lcd.fillScreen(RGBto565(0,0,180));
  font.printStr(0,0,"After reset"); delay(2000);
}

