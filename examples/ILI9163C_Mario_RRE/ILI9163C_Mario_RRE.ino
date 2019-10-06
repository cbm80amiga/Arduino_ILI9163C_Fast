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

#include "rre_mario0.h"
#include "rre_mario1.h"
#include "rre_mario2.h"
#include "rre_mario3.h"
#include "rre_mario4.h"
#include "rre_mario5.h"
#include "rre_mario6.h"
/*
#include "rre_mario0_v.h"
#include "rre_mario2_v.h"
#include "rre_mario3_v.h"
#include "rre_mario4_v.h"
#include "rre_mario5_v.h"
#include "rre_mario6_v.h"
*/
RREFont font;

// needed for RREFont library initialization, define your fillRect
void customRect(int x, int y, int w, int h, int c) { return lcd.fillRect(x, y, w, h, c); }

int sx=64, sy=64,sx0=0, sy0=0;
// special callback with scale&translation feature
void customRectScale(int x, int y, int w, int h, int c)
{
  int xx=(x-sx0)*sx>>6, yy=(y-sy0)*sy>>6, ww=((w+x-sx0)*sx>>6)-xx, hh=((h+y-sy0)*sy>>6)-yy; 
  xx+=sx0;
  yy+=sy0;
  return lcd.fillRect(xx, yy, ww, hh, c); 
}

void showMarioV()
{
  delay(1000);
  font.setFont(&rre_mario0); font.setScale(1); font.setSpacing(0);
  font.setColor(RGBto565(15,9,8));
  font.printStr(0,0," ");
  delay(1000);
  font.setFont(&rre_mario2); font.setScale(1); font.setSpacing(0);
  font.setColor(RGBto565(113,59,29));
  font.printStr(0,0," ");
  delay(1000);
  font.setFont(&rre_mario3); font.setScale(1); font.setSpacing(0);
  font.setColor(RGBto565(253,212,133));
  font.printStr(0,0," ");
  delay(1000);
  font.setFont(&rre_mario4); font.setScale(1); font.setSpacing(0);
  font.setColor(RGBto565(237,53,15));
  font.printStr(0,0," ");
  delay(1000);
  font.setFont(&rre_mario5); font.setScale(1); font.setSpacing(0);
  font.setColor(RGBto565(249,249,248));
  font.printStr(0,0," ");
  delay(1000);
  font.setFont(&rre_mario6); font.setScale(1); font.setSpacing(0);
  font.setColor(RGBto565(9,69,163));
  font.printStr(0,0," ");
}

uint16_t bg=RGBto565(157,156,154);
int del=0;

void showMario(int x, int y)
{
  font.setFont(&rre_mario1); font.setScale(1);
  font.setColor(bg);
  font.printStr(x,y," "); font.printStr(x+48,y,"!"); font.printStr(x,y+64,"\""); font.printStr(x+48,y+64,"#");
  delay(del);
  font.setFont(&rre_mario0); font.setScale(1);
  font.setColor(RGBto565(15,9,8));
  font.printStr(x,y," "); font.printStr(x+48,y,"!"); font.printStr(x,y+64,"\""); font.printStr(x+48,y+64,"#");
  delay(del);
  font.setFont(&rre_mario2); font.setScale(1);
  font.setColor(RGBto565(113,59,29));
  font.printStr(x,y," "); font.printStr(x+48,y,"!"); font.printStr(x,y+64,"\""); font.printStr(x+48,y+64,"#");
  delay(del);
  font.setFont(&rre_mario3); font.setScale(1);
  font.setColor(RGBto565(253,212,133));
  font.printStr(x,y," "); font.printStr(x+48,y,"!"); font.printStr(x,y+64,"\""); font.printStr(x+48,y+64,"#");
  delay(del);
  font.setFont(&rre_mario4); font.setScale(1);
  font.setColor(RGBto565(237,53,15));
  font.printStr(x,y," "); font.printStr(x+48,y,"!"); font.printStr(x,y+64,"\""); font.printStr(x+48,y+64,"#");
  delay(del);
  font.setFont(&rre_mario5); font.setScale(1);
  font.setColor(RGBto565(249,249,248));
  font.printStr(x,y," "); font.printStr(x+48,y,"!"); font.printStr(x,y+64,"\""); font.printStr(x+48,y+64,"#");
  delay(del);
  font.setFont(&rre_mario6); font.setScale(1);
  font.setColor(RGBto565(9,69,163));
  font.printStr(x,y," "); font.printStr(x+48,y,"!"); font.printStr(x,y+64,"\""); font.printStr(x+48,y+64,"#");
}


void setup() 
{
  Serial.begin(9600);
  lcd.init();
  lcd.fillScreen(bg); // c1
//  font.init(customRect, SCR_WD, SCR_HT); // custom fillRect function and screen width and height values
  font.init(customRectScale, SCR_WD, SCR_HT); // custom fillRect function and screen width and height values

  del=0;
  showMario(16,0);
  delay(2000);
  lcd.fillScreen(bg); // c1
  del=1000;
  showMario(16,0);
  del=0;
  delay(2000);
}


void loop()
{
  lcd.fillScreen(bg); // c1
  for(int j=0;j<4;j++) {
    sx0=64; sy0=64;
    for(int i=64;i>8;i-=2) {
      if(j==2) { sx=64;sy=i; } else
      if(j==3) { sx=i;sy=64; } else { sx=i;sy=i; }
      showMario(16,0);
      int w=96*i>>6;
      int h=128*i>>6;
      if(j<2) {
        lcd.fillRect(64-(48*i>>6), 64-(64*i>>6), w,2, bg);
        lcd.fillRect(64-(48*i>>6), 64+(64*i>>6)-2, w,2, bg);
        lcd.fillRect(64-(48*i>>6)-1, 64-(64*i>>6), 2,h, bg);
        lcd.fillRect(64+(48*i>>6)-2, 64-(64*i>>6), 2,h, bg);
      } else
      if(j==2) {
        lcd.fillRect(16, 64-(64*i>>6), 96,2, bg);
        lcd.fillRect(16, 64+(64*i>>6)-2, 96,2, bg);
      } else
      if(j==3) {
        lcd.fillRect(64-(48*i>>6)-1, 0, 2,128, bg);
        lcd.fillRect(64+(48*i>>6)-2, 0, 2,128, bg);
      }
    }
    for(int i=8;i<=64;i+=2) {
      if(j==2) { sx=64;sy=i; } else
      if(j==3) { sx=i;sy=64; } else { sx=i;sy=i; }
      showMario(16,0);
    }
  }
  for(int j=0;j<200;j++) {
    sx=sy=random(8,64);
    sx0=sy0=0;
    showMario(random(0,128),random(0,128));
  }
}

