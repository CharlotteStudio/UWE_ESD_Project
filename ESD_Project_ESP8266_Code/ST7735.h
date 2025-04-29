#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#define TFT_RST 2
#define TFT_CS  0
#define TFT_DC  4


Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void InitST7735(){
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_BLACK);
  Serial.println("Init ST7735");
}

// Default Text Size, one line can print 21 chars, every char height 6 pixie 
void PrintText(String text, uint16_t color, int size, int positionX, int positionY, bool cleanup) {
  if (cleanup)
    tft.fillScreen(ST7735_BLACK); // Clear the Screen to Black
  tft.setCursor(positionX, positionY);
  tft.setTextColor(color);
  tft.setTextSize(size);  // default is 1 
  tft.setTextWrap(true);
  tft.print(text);
}

void PrintText(String text, int positionY, bool cleanup) {
  PrintText(text, ST7735_WHITE, 1, 0, positionY, cleanup);
}

void PrintText(String text, int size) {
  PrintText(text, ST7735_WHITE, size, 0, 0, true);
}

void PrintText(String text) {
  PrintText(text, ST7735_WHITE, 1, 0, 0, true);
}

void ClearScreenFromPosition(int positionY) {
  if (positionY >= 0 && positionY < tft.height()) {
    tft.fillRect(0, positionY, tft.width(), tft.height() - positionY, ST7735_BLACK);
  }
}