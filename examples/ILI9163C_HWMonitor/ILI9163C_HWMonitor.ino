// Arduino HWMonitor - version with 4 separate graphs
// ILI9163C & RRE library required
// (c) 2019 Pawel A. Hernik
// YT video: https://youtu.be/dp_P0gZkbL0

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
#include "rre_5x8.h"

RREFont font;

// needed for RREFont library initialization, define your fillRect
void customRect(int x, int y, int w, int h, int c) { return lcd.fillRect(x, y, w, h, c); }

// ---------------------------------------------
// Configuration
// ---------------------------------------------
#define MIN_RAM 3000
#define MAX_RAM 8192
#define MIN_CLOCK 400
#define MAX_CLOCK 2900
#define MIN_TEMP 30
#define MAX_TEMP 80
// define for vertical bars instead of connecting lines
#define GRAPH_BAR

// dark theme
uint16_t bgCol   = BLACK;
uint16_t txtCol  = CYAN;
uint16_t frBgCol = RGBto565(0,0,100);
uint16_t frCol   = WHITE;
uint16_t valCol  = WHITE;
/*
// bright theme
uint16_t bgCol   = RGBto565(200,200,200);
uint16_t txtCol  = RGBto565(0,0,220);
uint16_t frBgCol = RGBto565(240,240,240);
uint16_t frCol   = BLACK;
uint16_t valCol  = BLACK;
*/
// ---------------------------------------------

String inputString = "";
String cpuLoadString;
String cpuTempString;
String cpuClockString;
String ramString;
char buf[30];
int cpuLoad;
int cpuClock;
int cpuTemp;
int usedRam;
int inp=0;

#define NUM_VAL (12+1)
int tempTab[NUM_VAL];
int loadTab[NUM_VAL];
int clockTab[NUM_VAL];
int ramTab[NUM_VAL];
int i,v,ght=60-8-2;


void setup() 
{
  Serial.begin(9600);
  lcd.init();
  lcd.fillScreen(bgCol);
  font.init(customRect, SCR_WD, SCR_HT); // custom fillRect function and screen width and height values
  font.setFont(&rre_5x8);
  font.setColor(txtCol);
  font.setScale(2);
  font.printStr(ALIGN_CENTER, 40, "Connecting");
  font.setScale(1);
}

int readSerial() 
{
  while (Serial.available()) {
    char ch = (char)Serial.read();
    inputString += ch;

    if(ch == '|') {  // full info chunk received
      int st,en;
      st = inputString.indexOf("CHC");  // CPU clock: "CHC1768"
      if(st>=0) {
        en = inputString.indexOf("|", st);
        cpuClockString = inputString.substring(st+3, en);
        cpuClock = cpuClockString.toInt();
        inp=3;
      } else {

        st = inputString.indexOf("R");  // used RAM: "R6.9"
        if(st>=0) {
          en = inputString.indexOf("|", st);
          ramString = inputString.substring(st+1 , en-1);
          st = ramString.indexOf(",");
          if(st>=0) ramString.setCharAt(st,'.');
          usedRam = ramString.toFloat()*1024;
          inp=2;
        }

        int cpuTempStart = inputString.indexOf("C"); // CPU temperature: "C52"
        int cpuLoadStart = inputString.indexOf("c"); // CPU load: "c18%"
        if(cpuLoadStart>=0 && cpuTempStart>=0) {
          en = inputString.indexOf("|");
          cpuTempString = inputString.substring(cpuTempStart+1, cpuLoadStart);
          cpuLoadString = inputString.substring(cpuLoadStart+1, en-1);
          cpuTemp = cpuTempString.toInt();
          cpuLoad = cpuLoadString.toInt();
          inp=1;
        }
      }
      inputString = "";
      return 1;
    }
  }
  return 0;
}

void addLoad()
{
  for(i=0;i<NUM_VAL-1;i++) loadTab[i]=loadTab[i+1];
  loadTab[NUM_VAL-1] = cpuLoad*(ght-1)/100;
}

void addClock()
{
  for(i=0;i<NUM_VAL-1;i++) clockTab[i]=clockTab[i+1];
  v = constrain(cpuClock, MIN_CLOCK, MAX_CLOCK);
  clockTab[NUM_VAL-1] = (long)(v-MIN_CLOCK)*ght/(MAX_CLOCK-MIN_CLOCK);
}

void addTemp()
{
  for(i=0;i<NUM_VAL-1;i++) tempTab[i]=tempTab[i+1];
  v = constrain(cpuTemp, MIN_TEMP, MAX_TEMP);
  tempTab[NUM_VAL-1] = (long)(v-MIN_TEMP)*ght/(MAX_TEMP-MIN_TEMP);
}

void addRAM()
{
  for(i=0;i<NUM_VAL-1;i++) ramTab[i]=ramTab[i+1];
  v = constrain(usedRam, MIN_RAM, MAX_RAM);
  ramTab[NUM_VAL-1] = (long)(v-MIN_RAM)*ght/(MAX_RAM-MIN_RAM);
}

void drawGraph(int xg, int yg, int valTab[])
{
  lcd.drawRect(xg,yg,60+2,ght+2,frCol);
  int hh, nx=5;
  uint8_t r,g,b;
#ifdef GRAPH_BAR
  for(i=1;i<NUM_VAL;i++) {
    hh = valTab[i];
    lcd.rgbWheel(2*85*(ght-hh)/ght,&r,&g,&b);
    lcd.fillRect(1+xg+(i-1)*nx, yg+1, nx, ght-hh, frBgCol);
    lcd.fillRect(1+xg+(i-1)*nx, yg+1+ght-hh, nx, hh, RGBto565(r,g,b));
    lcd.drawFastHLine(1+xg+(i-1)*nx, yg+1+ght-hh, nx, valCol);
    if(i>1) lcd.drawFastVLine(1+xg+(i-1)*nx, yg+1+ght-valTab[valTab[i-1]<valTab[i] ? i : i-1], abs(valTab[i-1]-valTab[i]), valCol);
  }
#else
  for(i=0;i<NUM_VAL-1;i++) {
    int dy = valTab[i+1]-valTab[i];
    for(int j=0;j<nx;j++) {
      hh = valTab[i]+dy*j/nx;
      lcd.rgbWheel(2*85*(ght-hh)/ght,&r,&g,&b);
      lcd.drawFastVLine(1+xg+i*nx+j, yg+1, ght-hh, frBgCol);
      lcd.drawFastVLine(1+xg+i*nx+j, yg+1+ght-hh, hh, RGBto565(r,g,b));
    }
    lcd.drawLine(1+xg+i*nx,yg+1+ght-valTab[i],1+xg+(i+1)*nx,yg+1+ght-valTab[i+1],valCol);
  }
#endif
}

void loop()
{
  int xt=8, xl=64+13, xc=2, xr=64+3; // good for 5x8
  
  //readSerial();
  if(readSerial()) 
  {
    int xs=68;
    //lcd.fillScreen(bgCol);
    font.setColor(txtCol,bgCol);
    snprintf(buf,30,"Temp: %d'C  ",cpuTemp);  font.printStr(xt, 1, buf);
    snprintf(buf,30,"Load: %d%%  ",cpuLoad);  font.printStr(xl, 1, buf);
    snprintf(buf,30,"Clk: %dMHz  ",cpuClock); font.printStr(xc, 128-9, buf);
    snprintf(buf,30,"RAM: %dMB  ",usedRam);   font.printStr(xr, 128-9, buf);

    if(inp==3) addClock();
    if(inp==2) addRAM();
    if(inp==1) { addLoad(); addTemp(); }
    drawGraph(1, 10 ,tempTab);
    drawGraph(64+1, 10, loadTab);
    drawGraph(1, 64, clockTab);
    drawGraph(64+1, 64, ramTab);
    lcd.fillRect(64-1,40,2,16,bgCol); // removed leftovers from "Connecting"
  }
  if(inp>=3) { delay(1000); inp=0; }
}

