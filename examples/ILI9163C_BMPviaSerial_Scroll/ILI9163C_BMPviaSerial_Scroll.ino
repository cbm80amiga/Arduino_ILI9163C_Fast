// Photo slideshow via serial port
// ILI9163C library example
// (c) 2019 Pawel A. Hernik
// YT videos: https://youtu.be/P4cwA9jNsfQ and https://youtu.be/vQY5ILjSZBc

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

// define here display size = desired BMP dimensions
#define SCR_WD 128
#define SCR_HT 128

#define TFT_CS 10
#define TFT_DC  8
#define TFT_RST 9
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Arduino_ILI9163C_Fast.h>
Arduino_ILI9163C lcd = Arduino_ILI9163C(TFT_DC, TFT_RST, TFT_CS);

void setup() 
{
  Serial.begin(115200);
  lcd.init();
  lcd.fillScreen(BLACK);
  lcd.drawRect(0,0,128,128,RED);
  lcd.setTextColor(WHITE);
  lcd.setTextSize(1);
  lcd.setCursor((SCR_WD-15*6)/2,SCR_HT/2-8);
  lcd.println("WAITING FOR BMP");
  lcd.setCursor((SCR_WD-7*6)/2,SCR_HT/2+8);
  lcd.print(SCR_WD); lcd.print("x"); lcd.print(SCR_HT);
  lcd.setScrollArea(0,0);
}

int rgbIdx=0;
int xp=0,yp=127;
uint8_t r,g,b,c;
int header=54;
unsigned long ms;
char buf[30];
int bmpIdx=0;
int palIdx=0, palCol=0;
int numBits=0;
int numCols=0;
uint16_t palette[256],col,col2;
int xt=0,yt=0;
int bmpWd,bmpHt;

void showTime()
{
    snprintf(buf,20,"Time: %d ms",millis()-ms);
    xt=(SCR_WD-13*6)/2; yt=8*SCR_HT/10;
    lcd.setTextColor(BLACK);
    lcd.setCursor(xt+1,yt+1); lcd.println(buf);
    lcd.setCursor(xt-0,yt+1); lcd.println(buf);
    lcd.setCursor(xt-1,yt+1); lcd.println(buf);
    lcd.setCursor(xt-1,yt-1); lcd.println(buf);
    lcd.setCursor(xt-0,yt-1); lcd.println(buf);
    lcd.setCursor(xt+1,yt-1); lcd.println(buf);
    lcd.setCursor(xt-1,yt-0); lcd.println(buf);
    lcd.setCursor(xt+1,yt-0); lcd.println(buf);
    lcd.setTextColor(YELLOW);
    lcd.setCursor(xt,yt); lcd.println(buf);
}

void loop()
{
  while(Serial.available())  {
    c = Serial.read();
    if(header) {
      if(bmpIdx==18) bmpWd=c; else
      if(bmpIdx==19) bmpWd+=(int)c<<8; else
      if(bmpIdx==22) bmpHt=c; else
      if(bmpIdx==23) bmpHt+=(int)c<<8; else
      if(bmpIdx==28) numBits=c; else
      if(bmpIdx==46) numCols=c; else
      if(bmpIdx==47) {
        numCols+=(int)c*256;
        Serial.print(bmpWd); Serial.print(" x "); Serial.print(bmpHt); Serial.print(" x "); 
        Serial.print(numBits); Serial.print(" bpp"); 
        if(numBits<=8) { Serial.print(" / "); Serial.print(numCols); Serial.print(" colors"); }
        Serial.println(); 
        if(bmpWd!=SCR_WD || bmpHt!=SCR_HT) Serial.println("Wrong BMP dimensions!"); 
      }
      if(numBits<=8 && bmpIdx>=54) {
        palIdx = bmpIdx-54;
        rgbIdx = palIdx&3;
        if(rgbIdx==0) b=c; else
        if(rgbIdx==1) g=c; else
        if(rgbIdx==2) palette[palIdx/4]=RGBto565(c,g,b);
      }
      bmpIdx++;
      if((numBits>8 && bmpIdx>=54) || (numBits<=8 && bmpIdx>=54+numCols*4)) {
        header=0;
        ms=millis();
      }
      continue; 
    }
    if(numBits>8) {
      if(rgbIdx==0) b = c; else
      if(rgbIdx==1) g = c; else
      if(rgbIdx==2) col = RGBto565(c,g,b);
    } else {
      if(numBits==8) col = palette[c]; else
      if(numBits==4) { col = palette[c>>4]; col2 = palette[c&0xf]; }
      rgbIdx=2;
    }
    if(rgbIdx==2) {
      rgbIdx=-1;
      lcd.drawPixel(xp,yp,col); xp++;
      if(numBits==4) { lcd.drawPixel(xp,yp,col2); xp++; }
      lcd.setScroll((yp)&0x7f);
      if(xp>=SCR_WD) {
        xp=0; yp--;
        if(yp<0) {
          yp=SCR_HT-1; 
          header=54; 
          bmpIdx=0;
          showTime();
        }
        lcd.setScroll((yp+1)&0x7f);
      }
    }
    rgbIdx++;
  }
}

