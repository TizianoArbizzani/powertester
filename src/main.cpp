#include <Arduino.h>

#include <defines.h>
#include <macros.h>

#include <SPI.h>
#include <TFT_eSPI.h>
#include <powertester.h>

powertester PT_Left(LEFT_INA_I2C, LEFT_INA_ID, LEFT_OFFSET);     //!<Left INA219 Chip (Left PSU)
powertester PT_Right(RIGHT_INA_I2C, RIGHT_INA_ID, RIGHT_OFFSET); //!<Right INA219 Chip (Right PSU)

TFT_eSPI tft = TFT_eSPI();   //!<TFT Display
unsigned long NextPrint;     //!<Time to go for next TFT Update
uint8_t PrintMode = D_HUMAN; //!<Serial printing verbose mode

bool CurHoldingMode = false;
unsigned long NextHoldSwitch; //!<Time to go for next switch from hold or not hold

void setup(void)
{
  //-----------------------------------------------
  // Setup Start
  //-----------------------------------------------

  pinMode(ONBOARD_LED, OUTPUT);
  ONBOARD_LED_ON;

  Serial.begin(SERIALSPEED);

  //while (!Serial)
  //{
  // delay(1); // will pause Zero, Leonardo, etc until serial console opens
  //}

  if (PrintMode)
  {
    Serial.printf("* SYS    : Initialized serial ... [Start: " B_GRE "%u " RESET "Bps]\n", SERIALSPEED);
    Serial.printf("* SYS    : Revision 001 ....... [" N_GRE "%s" RESET "]\n", PIO_SRC_REV);
  }

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  if (PrintMode)
    Serial.println("* TFT    : Initialization ..... [" B_GRE "Done" RESET "]");

  PT_Left.setup(PrintMode);
  PT_Right.setup(PrintMode);

  PT_Left.setReading(RS(IR_CURR));
  PT_Right.setReading(RS(IR_CURR));

  if (PrintMode)
    Serial.println("* INA219 : Focus on reading ... [" B_GRE "Current" RESET "]");

  ONBOARD_LED_OFF;

  if (PrintMode)
    Serial.println("* INA219 : Measurements ....... [" B_GRE "Start" RESET "]");

  NextPrint = millis() + TFT_UPDATES_MS;
  NextHoldSwitch = millis() + DBG_SWITCH_HOLD;
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
