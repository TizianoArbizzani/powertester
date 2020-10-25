#include <Arduino.h>

#include <defines.h>
#include <macros.h>

#include <TJpg_Decoder.h>

#include <SPI.h>
#include <TFT_eSPI.h>
#include <FS.h>
#include <powertester.h>

#include "Free_Fonts.h"
#include "panda.h"

uint16_t dmaBuffer1[16 * 16]; // Toggle buffer for 16*16 MCU block, 512bytes
uint16_t dmaBuffer2[16 * 16]; // Toggle buffer for 16*16 MCU block, 512bytes
uint16_t *dmaBufferPtr = dmaBuffer1;
bool dmaBufferSel = 0;

powertester PT_Left(LEFT_INA_I2C, LEFT_INA_ID, LEFT_OFFSET);     //!<Left INA219 Chip (Left PSU)
powertester PT_Right(RIGHT_INA_I2C, RIGHT_INA_ID, RIGHT_OFFSET); //!<Right INA219 Chip (Right PSU)

TFT_eSPI tft = TFT_eSPI();   //!<TFT Display
unsigned long NextPrint;     //!<Time to go for next TFT Update
uint8_t PrintMode = D_HUMAN; //!<Serial printing verbose mode

bool CurHoldingMode = false;
unsigned long NextHoldSwitch; //!<Time to go for next switch from hold or not hold

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
  // Stop further decoding as image is running off bottom of screen
  if (y >= tft.height())
    return 0;

    // STM32F767 processor takes 43ms just to decode (and not draw) jpeg (-Os compile option)
    // Total time to decode and also draw to TFT:
    // SPI 54MHz=71ms, with DMA 50ms, 71-43 = 28ms spent drawing, so DMA is complete before next MCU block is ready
    // Apparent performance benefit of DMA = 71/50 = 42%, 50 - 43 = 7ms lost elsewhere
    // SPI 27MHz=95ms, with DMA 52ms. 95-43 = 52ms spent drawing, so DMA is *just* complete before next MCU block is ready!
    // Apparent performance benefit of DMA = 95/52 = 83%, 52 - 43 = 9ms lost elsewhere
#ifdef USE_DMA
  // Double buffering is used, the bitmap is copied to the buffer by pushImageDMA() the
  // bitmap can then be updated by the jpeg decoder while DMA is in progress
  if (dmaBufferSel)
    dmaBufferPtr = dmaBuffer2;
  else
    dmaBufferPtr = dmaBuffer1;
  dmaBufferSel = !dmaBufferSel; // Toggle buffer selection
  //  pushImageDMA() will clip the image block at screen boundaries before initiating DMA
  tft.pushImageDMA(x, y, w, h, bitmap, dmaBufferPtr); // Initiate DMA - blocking only if last DMA is not complete
  // The DMA transfer of image block to the TFT is now in progress...
#else
  // Non-DMA blocking alternative
  tft.pushImage(x, y, w, h, bitmap); // Blocking, so only returns when image block is drawn
#endif
  // Return 1 to decode next block.
  return 1;
}

void DrawMask();

void setup(void)
{
  //-----------------------------------------------
  // Setup Start
  //-----------------------------------------------

  pinMode(TFT_LED, OUTPUT);
  TFT_LED_OFF;

  pinMode(ONBOARD_LED, OUTPUT);
  ONBOARD_LED_ON;

  Serial.begin(SERIALSPEED);

  if (PrintMode)
  {
    Serial.printf("* SYS    : Initialized serial ... [Start: " B_GRE "%u " RESET "Bps]\r\n", SERIALSPEED);
    Serial.printf("* SYS    : Revision 001 ....... [" N_GRE "%s" RESET "]\r\n", PIO_SRC_REV);
  }

  if (!SPIFFS.begin())
  {
    Serial.printf("* SPIFFS : Initialization ..... [" B_RED "Error" RESET "]\r\n");

    while (1)
    {
      yield(); // Stay here twiddling thumbs waiting
    }
  }
  Serial.printf("* SPIFFS : Initialization ..... [" B_GRE "Done" RESET "]\r\n");

  // ESP32 will crash if any of the fonts are missing
  bool font_missing = false;
  if (SPIFFS.exists("/fonts/NotoSansBold15.vlw") == false)
    font_missing = true;
  if (SPIFFS.exists("/fonts/NotoSansBold36.vlw") == false)
    font_missing = true;

  if (font_missing)
  {
    Serial.printf("* SPIFFS : Fonts .............. [" B_RED "Error" RESET "]\r\n");
    while (1)
      yield();
  }
  else
  {
    Serial.printf("* SPIFFS : Fonts .............. [" B_GRE "Loaded" RESET "]\r\n");
  }

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_RED);

  if (PrintMode)
    Serial.printf("* TFT    : Initialization ..... [" B_GRE "Done" RESET "]\r\n");

#ifdef USE_DMA_TO_TFT
  //tft.initDMA(); // To use SPI DMA you must call initDMA() to setup the DMA engine
  Serial.printf("* TFT    : Initialize DMA ..... [" B_GRE "Done" RESET "]\r\n");
#endif

  PT_Left.setup(PrintMode);
  PT_Right.setup(PrintMode);

  PT_Left.setReading(RS(IR_CURR));
  PT_Right.setReading(RS(IR_CURR));

  if (PrintMode)
    Serial.printf("* INA219 : Focus on reading ... [" B_GRE "Current" RESET "]\r\n");

  if (PrintMode)
    Serial.printf("* INA219 : Measurements ....... [" B_GRE "Start" RESET "]\r\n");

  NextPrint = millis() + TFT_UPDATES_MS;
  NextHoldSwitch = millis() + DBG_SWITCH_HOLD;

  // DrawMask();

  // tft.setTextColor(TFT_BLACK, TFT_GREEN);
  // tft.setFreeFont(FSSB18);
  // tft.drawString("- 5.521 mA", 10, 50, GFXFF); // Draw the text string in the selected GFX free font

  tft.startWrite();

  // Draw the image, top left at 0,0 - DMA request is handled in the call-back tft_output() in this sketch
  TJpgDec.drawJpg(0, 0, panda, sizeof(panda));

  // Must use endWrite to release the TFT chip select and release the SPI channel
  tft.endWrite();
}

void loop(void)
{
  //-----------------------------------------------
  // Main Loop Start
  //-----------------------------------------------

  PT_Left.update(IM_RECURR);
  PT_Right.update(IM_RECURR);

  //-----------------------------------------------
  // Update LCD
  //-----------------------------------------------

  if (millis() >= NextPrint)
  {
    //-----------------------------------------------
    // New print update needed
    //-----------------------------------------------

    NextPrint = millis() + TFT_UPDATES_MS; // Rearm trigger for next operation

    ONBOARD_LED_ON;
    PT_Left.display(&Serial);
    PT_Right.display(&Serial);
    ONBOARD_LED_OFF;
  }

  //-----------------------------------------------
  // Switch hold mode (DEBUG)
  //-----------------------------------------------

  if (millis() >= NextHoldSwitch)
  {
    NextHoldSwitch = millis() + DBG_SWITCH_HOLD;

    CurHoldingMode = !CurHoldingMode;

    PT_Left.setHoldingMode(CurHoldingMode);
    PT_Right.setHoldingMode(CurHoldingMode);
  }

  //-----------------------------------------------
  // Main Loop End
  //-----------------------------------------------
}

void DrawMask()
{
  int MaskColor = TFT_MAGENTA;

  tft.drawFastHLine(0, 0, 480, MaskColor);
  tft.drawFastHLine(0, 1, 480, MaskColor);
  tft.drawFastHLine(0, 2, 480, MaskColor);

  tft.drawFastVLine(0, 0, 320, MaskColor);
  tft.drawFastVLine(1, 0, 320, MaskColor);
  tft.drawFastVLine(2, 0, 320, MaskColor);

  tft.drawFastHLine(0, 317, 480, MaskColor);
  tft.drawFastHLine(0, 318, 480, MaskColor);
  tft.drawFastHLine(0, 319, 480, MaskColor);

  tft.drawFastVLine(477, 0, 320, MaskColor);
  tft.drawFastVLine(478, 0, 320, MaskColor);
  tft.drawFastVLine(479, 0, 320, MaskColor);

  tft.drawFastVLine(238, 0, 320, MaskColor);
  tft.drawFastVLine(239, 0, 320, MaskColor);
  tft.drawFastVLine(240, 0, 320, MaskColor);

  tft.fillRect(3, 3, 316, 237, TFT_GREEN);
  tft.fillRect(241, 3, 320, 237, TFT_RED);
}
