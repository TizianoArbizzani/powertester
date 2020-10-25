#include <Arduino.h>

#include <defines.h>
#include <macros.h>

#include <SPI.h>
#include <TFT_eSPI.h>
#include <FS.h>
#include <powertester.h>

#include <TJpg_Decoder.h>
#include <Free_Fonts.h>

#define CALIBRATION_FILE "/calibrationData"
#define TFT_FC TFT_BLUE //!< Static Frame Color

TFT_eSPI tft = TFT_eSPI();           //!<TFT Display
TFT_eSprite img = TFT_eSprite(&tft); //!<TFT Sprite

powertester PT_Left(LEFT_INA_I2C, LEFT_INA_ID, LEFT_OFFSET, LEFT_OUTPIN);      //!<Left INA219 Chip (Left PSU)
powertester PT_Right(RIGHT_INA_I2C, RIGHT_INA_ID, RIGHT_OFFSET, RIGHT_OUTPIN); //!<Right INA219 Chip (Right PSU)

unsigned long NextPrint;       //!<Time to go for next TFT Update
unsigned long NextTouch;       //!<Time to go for next Touch Reading
unsigned long NextHoldSwitch;  //!<Time to go for next switch from hold or not hold
uint8_t PrintMode = D_MACHINE; //!<Serial printing verbose mode

bool CurHoldingMode = false;

void DrawMask();
void TouchManager(uint16_t x, uint16_t y);

void setup(void)
{
  uint16_t calibrationData[5];
  uint8_t calDataOK = 0;
  uint16_t x, y;

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
  // bool font_missing = false;
  // if (SPIFFS.exists("/fonts/NotoSansBold15.vlw") == false)
  //   font_missing = true;
  // if (SPIFFS.exists("/fonts/NotoSansBold36.vlw") == false)
  //   font_missing = true;

  // if (font_missing)
  // {
  //   Serial.printf("* SPIFFS : Fonts .............. [" B_RED "Error" RESET "]\r\n");
  //   while (1)
  //     yield();
  // }
  // else
  // {
  //   Serial.printf("* SPIFFS : Fonts .............. [" B_GRE "Loaded" RESET "]\r\n");
  // }

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  if (tft.getTouch(&x, &y))
  {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setFreeFont(FMBO24);
    tft.setCursor(20, 20);

    tft.print("Entering Calibration Mode");

    delay(1000);

    SPIFFS.remove(CALIBRATION_FILE);
    Serial.printf("* Touch  : Calibration ........ [" B_RED "Reset" RESET "]\r\n");
  }

  if (SPIFFS.exists(CALIBRATION_FILE))
  {
    File f = SPIFFS.open(CALIBRATION_FILE, "r");
    if (f)
    {
      if (f.readBytes((char *)calibrationData, 14) == 14)
      {
        calDataOK = 1;
        Serial.printf("* Touch  : Calibration ........ [" B_GRE "%u:%u:%u:%u:%u" RESET "]\r\n",
                      calibrationData[0], calibrationData[1], calibrationData[2], calibrationData[3], calibrationData[4]);
      }

      f.close();
    }
  }
  if (calDataOK)
  {
    tft.setTouch(calibrationData);
    Serial.printf("* Touch  : Calibration ........ [" B_GRE "Done" RESET "]\r\n");
  }
  else
  {
    Serial.printf("* Touch  : Calibration ........ [" B_YEL "Procedure Start" RESET "]\r\n");
    tft.calibrateTouch(calibrationData, TFT_WHITE, TFT_RED, 15);
    Serial.printf("* Touch  : Calibration ........ [" B_GRE "Done" RESET "]\r\n");

    tft.fillScreen(TFT_BLACK);

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f)
    {
      f.write((const unsigned char *)calibrationData, 14);
      f.close();
    }
  }

  if (PrintMode)
    Serial.printf("* TFT    : Initialization ..... [" B_GRE "Done" RESET "]\r\n");

  PT_Left.setup(PrintMode);
  PT_Right.setup(PrintMode);

  PT_Left.setReading(RS(IR_CURR));
  PT_Right.setReading(RS(IR_CURR));

  if (PrintMode)
    Serial.printf("* INA219 : Focus on reading ... [" B_GRE "Current" RESET "]\r\n");

  DrawMask();

  PT_Left.setTFT(&tft);
  PT_Right.setTFT(&tft);

  if (PrintMode)
    Serial.printf("* TFT    : SubClasses init .... [" B_GRE "Done" RESET "]\r\n");

  PT_Left.setOutputMode(OUTPUT_CUTOFF);
  PT_Right.setOutputMode(OUTPUT_ACTIVE);

  if (PrintMode)
    Serial.printf("* SUPPLY : Output at Start .... [" B_YEL "Cut Off" RESET "]\r\n");

  NextPrint = millis() + TFT_UPDATES_MS;
  NextHoldSwitch = millis() + DBG_SWITCH_HOLD;
  NextTouch = millis();

  if (PrintMode)
    Serial.printf("* INA219 : Measurements ....... [" B_GRE "Start" RESET "]\r\n");
}

void loop(void)
{
  //-----------------------------------------------
  // Main Loop Start
  //-----------------------------------------------

  uint16_t x, y;

  if ((millis() >= NextTouch) && (tft.getTouch(&x, &y)))
  {
    TouchManager(x, y);
    NextTouch = millis() + TFT_TOUCH_MS;
    Serial.printf("* TOUCH  : Data ... [%u:%u]\r\n", x, y);
  }

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
  // Top
  tft.drawFastHLine(0, 0, 480, TFT_FC);
  tft.drawFastHLine(0, 1, 480, TFT_FC);
  tft.drawFastHLine(0, 2, 480, TFT_FC);
  tft.drawFastHLine(0, 3, 480, TFT_FC);

  // Bottom
  tft.drawFastHLine(0, 316, 480, TFT_FC);
  tft.drawFastHLine(0, 317, 480, TFT_FC);
  tft.drawFastHLine(0, 318, 480, TFT_FC);
  tft.drawFastHLine(0, 319, 480, TFT_FC);

  // Left
  tft.drawFastVLine(0, 0, 320, TFT_FC);
  tft.drawFastVLine(1, 0, 320, TFT_FC);
  tft.drawFastVLine(2, 0, 320, TFT_FC);
  tft.drawFastVLine(3, 0, 320, TFT_FC);

  //Right
  tft.drawFastVLine(476, 0, 320, TFT_FC);
  tft.drawFastVLine(477, 0, 320, TFT_FC);
  tft.drawFastVLine(478, 0, 320, TFT_FC);
  tft.drawFastVLine(479, 0, 320, TFT_FC);

  // Center
  tft.drawFastVLine(238, 0, 320, TFT_FC);
  tft.drawFastVLine(239, 0, 320, TFT_FC);
  tft.drawFastVLine(240, 0, 320, TFT_FC);
}

void TouchManager(uint16_t x, uint16_t y)
{
}
