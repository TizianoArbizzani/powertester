#include <Wire.h>
#include <Adafruit_INA219.h>

typedef struct {
  float CurrentSamples = 0;
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;
} Reading;

Adafruit_INA219 ina219_0(0x40);
Adafruit_INA219 ina219_1(0x41);

Reading R0;

// ina219.setCalibration_32V_1A(); // 32V, 1A range (higher precision on amps):
// Serial.println("* INA219 chip: 32V, 1A range ................... [Set]");

//const int SampleEvery = 50;
const int PrintEvery = 1000;

unsigned long NextSample;
unsigned long NextPrint;

//float MAX_shuntvoltage = -99999;
//float MAX_busvoltage = -99999;
float MAX_current_mA = -99999;
//float MAX_loadvoltage = -99999;
float MAX_power_mW = -99999;

//float MIN_shuntvoltage = 99999;
//float MIN_busvoltage = 99999;
float MIN_current_mA = 99999;
//float MIN_loadvoltage = 99999;
float MIN_power_mW = 99999;

void setup(void)
{
  //////////////////////////////////////////////
  // Main Loop Start
  //////////////////////////////////////////////

  Serial.begin(115200);
  while (!Serial)
  {
    // will pause Zero, Leonardo, etc until serial console opens
    delay(1);
  }

  Serial.println("* Esp32GetCurrent Operations ................... [Start]");

  //////////////////////////////////////////////
  // Main Loop Start
  //////////////////////////////////////////////

  if (! ina219_0.begin())
  {
    Serial.println("* INA219 chip (0) .............................. [!!! Init failed !!!]");
    while (1)
    {
      delay(1000);
    }
  }

  Serial.println("* INA219 chip (0) .............................. [Initialized]");

  ina219_0.setCalibration_16V_400mA();  // 16V, 400mA range (higher precision on volts and amps):
  Serial.println("* INA219 chip (0) .............................. [Set: : 16V, 400mA range]");

  if (! ina219_1.begin())
  {
    Serial.println("* INA219 chip (1) .............................. [!!! Init failed !!!]");
    while (1)
    {
      delay(1000);
    }
  }

  Serial.println("* INA219 chip (1) .............................. [Initialized]");

  ina219_1.setCalibration_16V_400mA();  // 16V, 400mA range (higher precision on volts and amps):
  Serial.println("* INA219 chip (1) .............................. [Set: : 16V, 400mA range]");


  Serial.println("* INA219 : Measuring voltage and current ....... [*** Sampling Start ***]");

  R0.shuntvoltage = 0;
  R0.busvoltage = 0;
  R0.current_mA = 0;
  R0.loadvoltage = 0;
  R0.power_mW = 0;
  R0.CurrentSamples = 0;

  NextSample = millis();
  NextPrint = millis();
}

void loop(void)
{
  //////////////////////////////////////////////
  // Main Loop Start
  //////////////////////////////////////////////

  //if (millis() >= NextSample)
  //{
    //////////////////////////////////////////////
    // New Sample needed
    //////////////////////////////////////////////

    // NextSample = millis() + SampleEvery; // Rearm trigger for next operation

    float LOCAL_current_mA = ina219_0.getCurrent_mA();
    float LOCAL_power_mW = ina219_0.getPower_mW();

    R0.CurrentSamples += 1;
    //R0.shuntvoltage += LOCAL_shuntvoltage;
    //R0.busvoltage += LOCAL_busvoltage;
    R0.current_mA += LOCAL_current_mA;
    //R0.loadvoltage += LOCAL_loadvoltage;
    R0.power_mW += LOCAL_power_mW;

    //MAX_shuntvoltage = (LOCAL_shuntvoltage > MAX_shuntvoltage) ? LOCAL_shuntvoltage : MAX_shuntvoltage;
    //MAX_busvoltage = (LOCAL_busvoltage > MAX_busvoltage) ? LOCAL_busvoltage : MAX_busvoltage;
    MAX_current_mA = (LOCAL_current_mA > MAX_current_mA) ? LOCAL_current_mA : MAX_current_mA;
    //MAX_loadvoltage = (LOCAL_loadvoltage > MAX_loadvoltage) ? LOCAL_loadvoltage : MAX_loadvoltage;
    MAX_power_mW = (LOCAL_power_mW > MAX_power_mW) ? LOCAL_power_mW : MAX_power_mW;

    //MIN_shuntvoltage = (LOCAL_shuntvoltage < MIN_shuntvoltage) ? LOCAL_shuntvoltage : MIN_shuntvoltage;
    //MIN_busvoltage = (LOCAL_busvoltage < MIN_busvoltage) ? LOCAL_busvoltage : MIN_busvoltage;
    MIN_current_mA = (LOCAL_current_mA < MIN_current_mA) ? LOCAL_current_mA : MIN_current_mA;
    //MIN_loadvoltage = (LOCAL_loadvoltage < MIN_loadvoltage) ? LOCAL_loadvoltage : MIN_loadvoltage;
    MIN_power_mW = (LOCAL_power_mW < MIN_power_mW) ? LOCAL_power_mW : MIN_power_mW;

  //}

  if (millis() >= NextPrint)
  {
    //////////////////////////////////////////////
    // New print update needed
    //////////////////////////////////////////////

    float LOCAL_shuntvoltage = ina219_0.getShuntVoltage_mV();
    float LOCAL_busvoltage = ina219_0.getBusVoltage_V();
    float LOCAL_loadvoltage = LOCAL_busvoltage + (LOCAL_shuntvoltage / 1000);

    NextPrint = millis() + PrintEvery; // Rearm trigger for next operation

    //Serial.print("["); Serial.print(millis()); Serial.print("]");
    Serial.print("["); Serial.print(R0.CurrentSamples); Serial.print("] : ");

    Serial.print("(Bv): "); Serial.print(R0.busvoltage / R0.CurrentSamples, 3 ); Serial.print(" ["); Serial.print(MIN_busvoltage, 3); Serial.print(" > ") ; Serial.print(MAX_busvoltage, 3); Serial.print("] V ");
    Serial.print("(Sv): "); Serial.print(R0.shuntvoltage / R0.CurrentSamples, 3); Serial.print(" ["); Serial.print(MIN_shuntvoltage, 3); Serial.print(" > "); Serial.print(MAX_shuntvoltage, 3) ; Serial.print("] mV ");
    Serial.print("(Lv): "); Serial.print(R0.loadvoltage / R0.CurrentSamples, 3); Serial.print(" ["); Serial.print( MIN_loadvoltage, 3); Serial.print(" > "); Serial.print( MAX_loadvoltage, 3) ; Serial.print("] V ");
    Serial.print("(Cu): "); Serial.print(R0.current_mA / R0.CurrentSamples, 4); Serial.print(" ["); Serial.print( MIN_current_mA, 4); Serial.print(" > "); Serial.print( MAX_current_mA, 4) ; Serial.print("] mA ");
    Serial.print("(Pw): "); Serial.print(R0.power_mW / R0.CurrentSamples, 4); Serial.print(" ["); Serial.print(MIN_power_mW, 4); Serial.print(" > "); Serial.print(MAX_power_mW, 4) ; Serial.print("] mA ");

    Serial.println("");

    float LOCAL_shuntvoltage = ina219_1.getShuntVoltage_mV();
    float LOCAL_busvoltage = ina219_1.getBusVoltage_V();
    float LOCAL_current_mA = ina219_1.getCurrent_mA();
    float LOCAL_power_mW = ina219_1.getPower_mW();
    float LOCAL_loadvoltage = LOCAL_busvoltage + (LOCAL_shuntvoltage / 1000);

    Serial.print("["); Serial.print("AA.BB"); Serial.print("] : ");
    Serial.print("(Bv): "); Serial.print(LOCAL_busvoltage, 3 ); Serial.print(" ["); Serial.print(MIN_busvoltage, 3); Serial.print(" > ") ; Serial.print(MAX_busvoltage, 3); Serial.print("] V ");
    Serial.print("(Sv): "); Serial.print(LOCAL_shuntvoltage, 3); Serial.print(" ["); Serial.print(MIN_shuntvoltage, 3); Serial.print(" > "); Serial.print(MAX_shuntvoltage, 3) ; Serial.print("] mV ");
    Serial.print("(Lv): "); Serial.print(LOCAL_loadvoltage, 3); Serial.print(" ["); Serial.print( MIN_loadvoltage, 3); Serial.print(" > "); Serial.print( MAX_loadvoltage, 3) ; Serial.print("] V ");
    Serial.print("(Cu): "); Serial.print(LOCAL_current_mA, 4); Serial.print(" ["); Serial.print( MIN_current_mA, 4); Serial.print(" > "); Serial.print( MAX_current_mA, 4) ; Serial.print("] mA ");
    Serial.print("(Pw): "); Serial.print(LOCAL_power_mW, 4); Serial.print(" ["); Serial.print(MIN_power_mW, 4); Serial.print(" > "); Serial.print(MAX_power_mW, 4) ; Serial.print("] mA ");
    Serial.println("");

    R0.CurrentSamples = 0;
    R0.shuntvoltage = 0;
    R0.busvoltage = 0;
    R0.current_mA = 0;
    R0.loadvoltage = 0;
    R0.power_mW = 0;

    MAX_shuntvoltage = -99999;
    MAX_busvoltage = -99999;
    MAX_current_mA = -99999;
    MAX_loadvoltage = -99999;
    MAX_power_mW = -99999;

    MIN_shuntvoltage = 99999;
    MIN_busvoltage = 99999;
    MIN_current_mA = 99999;
    MIN_loadvoltage = 99999;
    MIN_power_mW = 99999;

  }

  //////////////////////////////////////////////
  // Main Loop End
  //////////////////////////////////////////////
}
