#include <Arduino.h>

#include <defines.h>
#include <macros.h>

#include <SPI.h>
#include <TFT_eSPI.h>
#include <powertester.h>

powertester PT_Left(LEFT_INA_I2C, LEFT_INA_ID);    // Left INA219 Chip (Left PSU)
powertester PT_Right(RIGHT_INA_I2C, RIGHT_INA_ID); // Right INA219 Chip (Right PSU)

TFT_eSPI tft = TFT_eSPI(); // TFT Display

unsigned long NextPrint; // Time to go for next TFT Update

void setup(void)
{
  //-----------------------------------------------
  // Setup Start
  //-----------------------------------------------

  pinMode(ONBOARD_LED, OUTPUT);
  ONBOARD_LED_ON;

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

  Serial.println("* INA219 : Measuring voltage and current ....... [*** Sampling Start ***]");

  NextPrint = millis() + TFT_UPDATES_MS;
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

    PT_Left.display(&Serial);
    PT_Right.display(&Serial);
  }

  //-----------------------------------------------
  // Main Loop End
  //-----------------------------------------------
}
