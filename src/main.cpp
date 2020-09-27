#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <powertester.h>

powertester PT_Left(0x40, "Sx");
powertester PT_Right(0x41, "Dx");

TFT_eSPI tft = TFT_eSPI();

const int PrintEvery = 1000;

unsigned long NextPrint;

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

  PT_Left.SetReading(INAREAD_CURRENT);
  PT_Right.SetReading(INAREAD_CURRENT);

  Serial.println("* INA219 : Verify correct object initialization  [*** Start ***]");

  PT_Left.display(&Serial);
  PT_Right.display(&Serial);

  Serial.println("* INA219 : Measuring voltage and current ....... [*** Sampling Start ***]");

  NextPrint = millis() + PrintEvery;
}

void loop(void)
{
  //-----------------------------------------------
  // Main Loop Start
  //-----------------------------------------------

  PT_Left.update(INAMODE_RECURRENT);
  PT_Right.update(INAMODE_RECURRENT);

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
