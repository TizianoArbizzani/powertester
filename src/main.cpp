#include <Arduino.h>

#include <FS.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <powertester.h>

#include <defines.h>
#include <macros.h>

// Global objects
TFT_eSPI tft = TFT_eSPI();                                                     //!<TFT Display
powertester PT_Left(LEFT_INA_I2C, LEFT_INA_ID, LEFT_OFFSET, LEFT_OUTPIN);      //!<Left INA219 Chip (Left PSU)
powertester PT_Right(RIGHT_INA_I2C, RIGHT_INA_ID, RIGHT_OFFSET, RIGHT_OUTPIN); //!<Right INA219 Chip (Right PSU)
TFT_eSprite spr = TFT_eSprite(&tft);                                           // Sprite class needs to be invoked

// Global variables - Timings
unsigned long NextPrint; //!<Time to go for next TFT Update
unsigned long NextTouch; //!<Time to go for next Touch Reading

// Global variables - Generic
uint8_t PrintMode = D_REDUCED; //!<Serial printing verbose mode
bool CurHoldingMode = false;

// Function prototypes
void DrawMask();
void TouchManager();
void IRAM_ATTR TouchDetected();

uint16_t Tx;         //!< Detected Touch X value
uint16_t Ty;         //!< Detected Touch X value
volatile boolean Td; //!< Detected Touch pressure

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED; //!< Mux Management variable

// Mandatory functions

void setup(void)
{
  uint16_t calibrationData[5];
  uint8_t calDataOK = 0;
  uint16_t x, y;

  //-----------------------------------------------
  // Setup Start
  //-----------------------------------------------

  pinMode(TFT_INTPIN, INPUT_PULLUP);

  pinMode(TFT_LED, OUTPUT);

  pinMode(ONBOARD_LED, OUTPUT);
  ONBOARD_LED_ON;

  // Relays Control
  pinMode(LEFT_OUTPIN, OUTPUT);
  RELAY_RELEASE(LEFT_OUTPIN);

  pinMode(RIGHT_OUTPIN, OUTPUT);
  RELAY_RELEASE(RIGHT_OUTPIN);

  Serial.begin(SERIALSPEED);

  if (PrintMode >= D_REDUCED)
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

  if (PrintMode >= D_REDUCED)
    Serial.printf("* SPIFFS : Initialization ..... [" B_GRE "Done" RESET "]\r\n");

  tft.init();
  tft.setRotation(3);
  tft.setTextDatum(ML_DATUM);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);

  TFT_LED_ON;

  if (PrintMode >= D_REDUCED)
    Serial.printf("* TFT    : Initialization ..... [" B_GRE "Done" RESET "]\r\n");

  if (tft.getTouch(&x, &y))
  {
    spr.setColorDepth(16);
    spr.createSprite(220, 48);
    spr.loadFont(FruBoldNarrow54);
    spr.setTextDatum(TL_DATUM);
    spr.fillSprite(TFT_GREEN);
    spr.setTextColor(TFT_BLACK, TFT_GREEN);
    spr.drawString("Calib", 0, 0);
    spr.pushSprite(5, TFT_Y2, TFT_GREEN);

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

        if (PrintMode >= D_REDUCED)
          Serial.printf("* Touch  : Calibration ........ [" B_GRE "%u:%u:%u:%u:%u" RESET "]\r\n",
                        calibrationData[0], calibrationData[1], calibrationData[2], calibrationData[3], calibrationData[4]);
      }

      f.close();
    }
  }
  if (calDataOK)
  {
    tft.setTouch(calibrationData);

    if (PrintMode >= D_REDUCED)
      Serial.printf("* Touch  : Calibration ........ [" B_GRE "Done" RESET "]\r\n");
  }
  else
  {
    if (PrintMode >= D_REDUCED)
      Serial.printf("* Touch  : Calibration ........ [" B_YEL "Procedure Start" RESET "]\r\n");

    tft.calibrateTouch(calibrationData, TFT_WHITE, TFT_RED, 15);

    if (PrintMode >= D_REDUCED)
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

  if (PrintMode >= D_REDUCED)
    Serial.printf("* Touch  : Initialization ..... [" B_GRE "Done" RESET "]\r\n");

  PT_Left.setup(&tft, PrintMode, RS(IR_CURR));
  PT_Right.setup(&tft, PrintMode, RS(IR_CURR));

  if (PrintMode >= D_REDUCED)
    Serial.printf("* INA219 : Focus on reading ... [" B_GRE "Current" RESET "]\r\n");

  DrawMask();

  PT_Left.setOutputMode(OUTPUT_CUTOFF);
  PT_Right.setOutputMode(OUTPUT_CUTOFF);

  if (PrintMode >= D_REDUCED)
    Serial.printf("* SUPPLY : Output at Start .... [" B_YEL "Cut Off" RESET "]\r\n");

  PT_Left.setHoldingMode(false);
  PT_Right.setHoldingMode(false);

  if (PrintMode >= D_REDUCED)
    Serial.printf("* SUPPLY : Hold at Start ...... [" B_YEL "Off" RESET "]\r\n");

  NextPrint = millis() + TFT_UPDATES_MS;
  NextTouch = millis();

  attachInterrupt(TFT_INTPIN, TouchDetected, FALLING);

  if (PrintMode >= D_REDUCED)
  {
    Serial.printf("* INA219 : Measurements ....... [" B_GRE "Start" RESET "]\r\n");
    Serial.printf("* HEAP   : Start .............. [" B_GRE "%u of %u" RESET "]\r\n", ESP.getFreeHeap(), ESP.getHeapSize());
  }
}

void loop(void)
{
  //-----------------------------------------------
  // Touch Management
  //-----------------------------------------------

  if (Td)
  {
    unsigned long Now = millis();

    if (Now >= NextTouch)
    {
      uint16_t X;
      uint16_t Y;

      NextTouch = Now + TFT_TOUCH_MS;
      tft.getTouch(&X, &Y);
      if ((X != Tx) && (Y != Ty))
      {
        Tx = X;
        Ty = Y;

        Serial.printf("* TOUCH  : Correct ... [X:%u Y:%u]\r\n", Tx, Ty);
        TouchManager();
      }
      //else
      //{
      //  Serial.printf("* TOUCH  : Repeated ... [X:%u Y:%u]\r\n", Tx, Ty);
      //}
    }

    portENTER_CRITICAL(&mux);
    Td = false;
    portEXIT_CRITICAL(&mux);
  }

  //-----------------------------------------------
  // Update Readings
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
  // Main Loop End
  //-----------------------------------------------
}

void DrawMask()
{
  tft.fillRect(0, 0, tft.width(), 4, FRAME_COLOR);                // Top
  tft.fillRect(0, tft.height() - 4, tft.width(), 4, FRAME_COLOR); // Bottom
  tft.fillRect(0, 0, 4, tft.width(), FRAME_COLOR);                // Left
  tft.fillRect(tft.width() - 4, 0, 4, tft.height(), FRAME_COLOR); // Right
  tft.fillRect(234 + 4, 0, 4, tft.height(), FRAME_COLOR);         // Center
}

void IRAM_ATTR TouchDetected()
{
  portENTER_CRITICAL_ISR(&mux);
  Td = true;
  portEXIT_CRITICAL_ISR(&mux);
}

void TouchManager()
{
  if (Ty < 50)
  {
    switch (Tx / 120)
    {
    case 0:
      PT_Left.setOutputMode(!PT_Left.getOutputMode());
      break;
    case 1:
      PT_Left.setHoldingMode(!PT_Left.getHoldingMode());
      break;
    case 2:
      PT_Right.setOutputMode(!PT_Right.getOutputMode());
      break;
    case 3:
      PT_Right.setHoldingMode(!PT_Right.getHoldingMode());
      break;
    default:
      Serial.printf("* TOUCH  : Button .... [UNKNOWN]\r\n");
      break;
    }
  }
}
