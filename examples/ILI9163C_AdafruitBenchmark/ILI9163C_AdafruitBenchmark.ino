// ILI9163C library benchmark
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
Arduino_ILI9163C tft = Arduino_ILI9163C(TFT_DC, TFT_RST, TFT_CS);
//#include <TFT_ILI9163C.h>
//TFT_ILI9163C tft = TFT_ILI9163C(TFT_CS, TFT_DC, TFT_RST);

// Color definitions
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0  
#define WHITE   0xFFFF
#define RGBto565(r,g,b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3)) 


// ------------------------------------------------
unsigned long FillScreenTest()
{
  unsigned long start = millis();
  for(int i=0;i<5;i++) {
    tft.fillScreen(RED);
    tft.fillScreen(GREEN);
    tft.fillScreen(BLUE);
    tft.fillScreen(YELLOW);
  }
  return millis()-start;
}

// ------------------------------------------------
unsigned long ClearScreenTest()
{
  unsigned long start = millis();
  for(int i=0;i<5*4;i++) 
    tft.fillScreen(BLACK);
  return millis()-start;
}
// ------------------------------------------------

unsigned long orig[14]={ 4061,4061,1015596,81344,558032,88616,73228,
                         1572316,280652,235748,235844,626672,157688,1732376 };

unsigned long res[14];
void result(int i)
{
  Serial.print(res[i]);
  if(res[i]<1000000) Serial.print("\t");
  Serial.print("\t\t\t");
  Serial.print(100*orig[i]/res[i]);
  Serial.println("%");
}

void setup(void) 
{
  Serial.begin(9600);
  Serial.println(F("ILI9163C 128x128"));
  //tft.begin();
  tft.init();
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  tft.setTextSize(1);
  tft.println("ILI9163C 128x128");
  tft.println("Library Benchmark");
  tft.println("starts in 3s ...");
  delay(3000);

  Serial.println(F("Benchmark   Time (microseconds)"));

  res[0]=FillScreenTest();
  Serial.print(F("FillScreen Mbps          "));
  Serial.println(String(res[0])+"ms  "+String(1000*20.0/res[0])+"fps  "+String(128.0*128*16*20.0/res[0]/1000.0)+" Mbps\t"+100*orig[0]/res[0]+"%");

  res[1]=ClearScreenTest();
  Serial.print(F("ClearScreen Mbps         "));
  Serial.println(String(res[1])+"ms  "+String(1000*20.0/res[1])+"fps  "+String(128.0*128*16*20.0/res[1]/1000.0)+" Mbps\t"+100*orig[1]/res[1]+"%");

  res[2]=testFillScreen();
  Serial.print(F("Screen fill              "));
  result(2);
  delay(500);

  res[3]=testText();
  Serial.print(F("Text                     "));
  result(3);
  delay(3000);

  res[4]=testLines(CYAN);
  Serial.print(F("Lines                    "));
  result(4);
  delay(500);

  res[5]=testFastLines(RED, BLUE);
  Serial.print(F("Horiz/Vert Lines         "));
  result(5);
  delay(500);

  res[6]=testRects(GREEN);
  Serial.print(F("Rectangles (outline)     "));
  result(6);
  delay(500);

  res[7]=testFilledRects(YELLOW, MAGENTA);
  Serial.print(F("Rectangles (filled)      "));
  result(7);
  delay(500);

  res[8]=testFilledCircles(10, MAGENTA);
  Serial.print(F("Circles (filled)         "));
  result(8);

  res[9]=testCircles(10, WHITE);
  Serial.print(F("Circles (outline)        "));
  result(9);
  delay(500);

  res[10]=testTriangles();
  Serial.print(F("Triangles (outline)      "));
  result(10);
  delay(500);

  res[11]=testFilledTriangles();
  Serial.print(F("Triangles (filled)       "));
  result(11);
  delay(500);

  res[12]=testRoundRects();
  Serial.print(F("Rounded rects (outline)  "));
  result(12);
  delay(500);

  res[13]=testFilledRoundRects();
  Serial.print(F("Rounded rects (filled)   "));
  result(13);
  delay(500);

  Serial.println(F("Done!"));

  int c1=YELLOW, c2=WHITE;
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.setTextSize(1);
  tft.setTextColor(CYAN);
  //tft.println("RESULTS:");

  //tft.setTextSize(1);
  //tft.println();
  tft.setTextColor(GREEN);
  tft.println(F("Benchmark    Time/us"));
  tft.setTextColor(c1); tft.print(F("FillScr Mbps  "));
  //tft.setTextColor(c2); tft.print(String(res[0])+"ms "+String(128.0*128*16*20.0/res[0]/1000.0)+" Mbps");
  tft.setTextColor(c2); tft.println(String(128.0*128*16*20.0/res[0]/1000.0));
  tft.setTextColor(c1); tft.print(F("ClrScr  Mbps  "));
  //tft.setTextColor(c2); tft.print(String(res[1])+"ms "+String(128.0*128*16*20.0/res[1]/1000.0)+" Mbps");
  tft.setTextColor(c2); tft.println(String(128.0*128*16*20.0/res[1]/1000.0));
  tft.setTextColor(c1); tft.print(F("Screen fill   "));
  tft.setTextColor(c2); tft.println(res[2]);
  tft.setTextColor(c1); tft.print(F("Text          "));
  tft.setTextColor(c2); tft.println(res[3]);
  tft.setTextColor(c1); tft.print(F("Lines         "));
  tft.setTextColor(c2); tft.println(res[4]);
  tft.setTextColor(c1); tft.print(F("H/V Lines     "));
  tft.setTextColor(c2); tft.println(res[5]);
  tft.setTextColor(c1); tft.print(F("Rects O       "));
  tft.setTextColor(c2); tft.println(res[6]);
  tft.setTextColor(c1); tft.print(F("Rects F       "));
  tft.setTextColor(c2); tft.println(res[7]);
  tft.setTextColor(c1); tft.print(F("Circles F     "));
  tft.setTextColor(c2); tft.println(res[8]);
  tft.setTextColor(c1); tft.print(F("Circles O     "));
  tft.setTextColor(c2); tft.println(res[9]);
  tft.setTextColor(c1); tft.print(F("Tris O        "));
  tft.setTextColor(c2); tft.println(res[10]);
  tft.setTextColor(c1); tft.print(F("Tris F        "));
  tft.setTextColor(c2); tft.println(res[11]);
  tft.setTextColor(c1); tft.print(F("Round rects O "));
  tft.setTextColor(c2); tft.println(res[12]);
  tft.setTextColor(c1); tft.print(F("Round rects F "));
  tft.setTextColor(c2); tft.println(res[13]);
  tft.setTextColor(RED); tft.println(F("Done!"));
}

/*
ILI9163C 128x128 - original
Benchmark                Time (microseconds)
FillScreen Mbps          4061ms  4.92fps  1.29 Mbps
ClearScreen Mbps         4061ms  4.92fps  1.29 Mbps
Screen fill              1015596
Text                     81344
Lines                    558032
Horiz/Vert Lines         88616
Rectangles (outline)     73228
Rectangles (filled)      1572316
Circles (filled)         280652
Circles (outline)        235748
Triangles (outline)      235844
Triangles (filled)       626672
Rounded rects (outline)  157688
Rounded rects (filled)   1732376
Done!

ILI9163C 128x128 - optimized, with CS controlled by MCU
Benchmark                Time (microseconds)
FillScreen Mbps          754ms  26.53fps  6.95 Mbps  538%
ClearScreen Mbps         755ms  26.49fps  6.94 Mbps 537%
Screen fill              188968       537%
Text                     30548        266%
Lines                    185528       300%
Horiz/Vert Lines         17096        518%
Rectangles (outline)     14948        489%
Rectangles (filled)      287904       546%
Circles (filled)         71236        393%
Circles (outline)        78324        300%
Triangles (outline)      78500        300%
Triangles (filled)       187840       333%
Rounded rects (outline)  45224        348%
Rounded rects (filled)   326948       529%
Done!

ILI9163C 128x128 - optimized, with CS connected to the ground and CS_ALWAYS_LOW
Benchmark   Time (microseconds)
FillScreen Mbps          753ms  26.56fps  6.96 Mbps  539%
ClearScreen Mbps         753ms  26.56fps  6.96 Mbps 539%
Screen fill              188648       538%
Text                     30608        265%
Lines                    181280       307%
Horiz/Vert Lines         17092        518%
Rectangles (outline)     14948        489%
Rectangles (filled)      287444       546%
Circles (filled)         71232        393%
Circles (outline)        76528        308%
Triangles (outline)      76700        307%
Triangles (filled)       187840       333%
Rounded rects (outline)  44492        354%
Rounded rects (filled)   326592       530%
Done!
*/

// ------------------------------------------------

void loop(void) 
{
}

// ------------------------------------------------

unsigned long testFillScreen() {
  unsigned long start = micros();
  tft.fillScreen(BLACK);
  tft.fillScreen(RED);
  tft.fillScreen(GREEN);
  tft.fillScreen(BLUE);
  tft.fillScreen(BLACK);
  return micros() - start;
}

// ------------------------------------------------
unsigned long testText() {
  tft.fillScreen(BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(RED);    tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
  return micros() - start;
}

// ------------------------------------------------
unsigned long testLines(uint16_t color) {
  unsigned long start, t;
  int           x1, y1, x2, y2,
                w = tft.width(),
                h = tft.height();

  tft.fillScreen(BLACK);

  x1 = y1 = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t     = micros() - start; // fillScreen doesn't count against timing

  tft.fillScreen(BLACK);

  x1    = w - 1;
  y1    = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  tft.fillScreen(BLACK);

  x1    = 0;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  tft.fillScreen(BLACK);

  x1    = w - 1;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);

  return micros() - start;
}

// ------------------------------------------------
unsigned long testFastLines(uint16_t color1, uint16_t color2) {
  unsigned long start;
  int           x, y, w = tft.width(), h = tft.height();

  tft.fillScreen(BLACK);
  start = micros();
  for(y=0; y<h; y+=5) tft.drawFastHLine(0, y, w, color1);
  for(x=0; x<w; x+=5) tft.drawFastVLine(x, 0, h, color2);

  return micros() - start;
}

// ------------------------------------------------
unsigned long testRects(uint16_t color) {
  unsigned long start;
  int           n, i, i2,
                cx = tft.width()  / 2,
                cy = tft.height() / 2;

  tft.fillScreen(BLACK);
  n     = min(tft.width(), tft.height());
  start = micros();
  for(i=2; i<n; i+=6) {
    i2 = i / 2;
    tft.drawRect(cx-i2, cy-i2, i, i, color);
  }

  return micros() - start;
}

// ------------------------------------------------
unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
  unsigned long start, t = 0;
  int           n, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(BLACK);
  n = min(tft.width(), tft.height());
  for(i=n; i>0; i-=6) {
    i2    = i / 2;
    start = micros();
    tft.fillRect(cx-i2, cy-i2, i, i, color1);
    t    += micros() - start;
    // Outlines are not included in timing results
    tft.drawRect(cx-i2, cy-i2, i, i, color2);
  }

  return t;
}

// ------------------------------------------------
unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

  tft.fillScreen(BLACK);
  start = micros();
  for(x=radius; x<w; x+=r2) {
    for(y=radius; y<h; y+=r2) {
      tft.fillCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

// ------------------------------------------------
unsigned long testCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int           x, y, r2 = radius * 2,
                w = tft.width()  + radius,
                h = tft.height() + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  start = micros();
  for(x=0; x<w; x+=r2) {
    for(y=0; y<h; y+=r2) {
      tft.drawCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

// ------------------------------------------------
unsigned long testTriangles() {
  unsigned long start;
  int           n, i, cx = tft.width()  / 2 - 1,
                      cy = tft.height() / 2 - 1;

  tft.fillScreen(BLACK);
  n     = min(cx, cy);
  start = micros();
  for(i=0; i<n; i+=5) {
    tft.drawTriangle(
      cx    , cy - i, // peak
      cx - i, cy + i, // bottom left
      cx + i, cy + i, // bottom right
      RGBto565(0, 0, i));
  }

  return micros() - start;
}

// ------------------------------------------------
unsigned long testFilledTriangles() {
  unsigned long start, t = 0;
  int           i, cx = tft.width()  / 2 - 1,
                   cy = tft.height() / 2 - 1;

  tft.fillScreen(BLACK);
  start = micros();
  for(i=min(cx,cy); i>10; i-=5) {
    start = micros();
    tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      RGBto565(0, i, i));
    t += micros() - start;
    tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      RGBto565(i, i, 0));
  }

  return t;
}

// ------------------------------------------------
unsigned long testRoundRects() {
  unsigned long start;
  int           w, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(BLACK);
  w     = min(tft.width(), tft.height());
  start = micros();
  for(i=0; i<w; i+=6) {
    i2 = i / 2;
    tft.drawRoundRect(cx-i2, cy-i2, i, i, i/8, RGBto565(i, 0, 0));
  }

  return micros() - start;
}

// ------------------------------------------------
unsigned long testFilledRoundRects() {
  unsigned long start;
  int           i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(BLACK);
  start = micros();
  for(i=min(tft.width(), tft.height()); i>20; i-=6) {
    i2 = i / 2;
    tft.fillRoundRect(cx-i2, cy-i2, i, i, i/8, RGBto565(0, i, 0));
  }

  return micros() - start;
}
// ------------------------------------------------

