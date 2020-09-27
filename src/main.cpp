#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <powertester.h>

powertester PT_Left(0x40, "Sx");  // Left INA219 Chip (Left PSU)
powertester PT_Right(0x41, "Dx"); // Right INA219 Chip (Right PSU)

TFT_eSPI tft = TFT_eSPI(); // TFT Display

const int PrintEvery = 1000; // Update TFT every x milliseconds

unsigned long NextPrint; // Time to go for next TFT Update

void setup(void)
{
  //-----------------------------------------------
  // Setup Start
  //-----------------------------------------------

  Serial.begin(921600);
  while (!Serial)
  {
    delay(1); // will pause Zero, Leonardo, etc until serial console opens
  }

  Serial.println("* Esp32GetCurrent Operations ................... [Start]");

  PT_Left.setup();
  PT_Right.setup();

  Serial.println("* INA219 : Setting main data reading ........... [*** Current ***]");

  PT_Left.SetReading(IR_CURR);
  PT_Right.SetReading(IR_CURR);

  Serial.println("* INA219 : Verify correct object initialization  [*** Start ***]");

  PT_Left.display(&Serial);
  PT_Right.display(&Serial);

  // Serial.println("* INA219 : Verify correct object initialization  [*** First Update ***]");

  // PT_Left.update(IM_RECURR);
  // PT_Right.update(IM_RECURR);

  // Serial.println("* INA219 : Verify correct object initialization  [*** Second Update ***]");

  // PT_Left.update(IM_RECURR);
  // PT_Right.update(IM_RECURR);

  // Serial.println("* INA219 : Verify correct object initialization  [*** First Display ***]");

  // PT_Left.display(&Serial);
  // PT_Right.display(&Serial);

  Serial.println("* INA219 : Measuring voltage and current ....... [*** Sampling Start ***]");

  NextPrint = millis() + PrintEvery;
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

    NextPrint = millis() + PrintEvery; // Rearm trigger for next operation

    PT_Left.display(&Serial);
    PT_Right.display(&Serial);
  }

  //-----------------------------------------------
  // Main Loop End
  //-----------------------------------------------
}
