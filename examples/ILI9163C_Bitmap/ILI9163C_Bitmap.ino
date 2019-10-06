// ILI9163C library example
// (c) 2019 Pawel A. Hernik

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

#define TFT_CS 10
#define TFT_DC  8
#define TFT_RST 9
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Arduino_ILI9163C_Fast.h>
Arduino_ILI9163C lcd = Arduino_ILI9163C(TFT_DC, TFT_RST, TFT_CS);

#include "bitmap.h"

#define BARWD 24
uint16_t colorBar[BARWD];

void setup(void) 
{
  Serial.begin(9600);
  lcd.init();
  lcd.fillScreen(BLACK);

  int i,j;
  for(j=0;j<4;j++) 
    for(i=0;i<4;i++)
      lcd.drawImageF(i*32,j*32,32,32,mario);
  delay(4000);

  for(i=0;i<BARWD/2;i++) {
    colorBar[i]         = RGBto565(i*256/(BARWD/2),0,i*256/(BARWD/2));
    colorBar[i+BARWD/2] = RGBto565((BARWD/2-i)*255/(BARWD/2),0,(BARWD-i)*255/(BARWD/2));
  }
  for(i=0;i<128;i++) {
    lcd.drawImage(i,0,1,BARWD,colorBar);
    lcd.drawImage(i,128-BARWD,1,BARWD,colorBar);
  }
  for(i=BARWD;i<128-BARWD;i++) {
    lcd.drawImage(0,i,BARWD,1,colorBar);
    lcd.drawImage(128-BARWD,i,BARWD,1,colorBar);
  }
  delay(4000);
}

void loop()
{
  lcd.drawImageF(random(0,128-32),random(0,128-32),32,32,mario);
}

