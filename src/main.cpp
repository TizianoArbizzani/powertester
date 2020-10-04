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

void setup(void)
{
  //-----------------------------------------------
  // Setup Start
  //-----------------------------------------------

  pinMode(ONBOARD_LED, OUTPUT);
  ONBOARD_LED_ON;

  Serial.begin(SERIALSPEED);
  while (!Serial)
  {
    delay(1); // will pause Zero, Leonardo, etc until serial console opens
  }

  if (PrintMode)
    Serial.printf("* ESP32 : Initialized serial ... [Start: %u Bps]\n", SERIALSPEED);

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  if (PrintMode)
    Serial.println("* TFT : Initialization ........ [Done]");

  PT_Left.setup(PrintMode);
  PT_Right.setup(PrintMode);

  if (PrintMode)
    Serial.println("* INA219 : Focus on reading ... [Current]");

  PT_Left.SetReading(RS(IR_CURR));
  PT_Right.SetReading(RS(IR_CURR));

  if (PrintMode)
    Serial.println("* INA219 : Measurements ....... [Start]");

  NextPrint = millis() + TFT_UPDATES_MS;

  ONBOARD_LED_OFF;
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
    PT_Left.display(&Serial, PrintMode);
    PT_Right.display(&Serial, PrintMode);
    ONBOARD_LED_OFF;
  }

  //-----------------------------------------------
  // Main Loop End
  //-----------------------------------------------
}
