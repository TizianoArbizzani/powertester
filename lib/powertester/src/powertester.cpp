#include "Arduino.h"
#include "powertester.h"

// --------------------------------
// READING - Public
// --------------------------------

reading::reading(const char *Unit) : READ_min(1), READ_mean(0), READ_max(-1), READ_reads(0)
{
    strncpy(_Unit, Unit, 8);
}

void reading::reset()
{
    READ_mean = 0;
    READ_max = -1;
    READ_min = 1;
    READ_reads = 0;
}

void reading::set(float ReadData)
{
    READ_reads++;

    READ_mean += ReadData;

    if (ReadData > READ_max)
    {
        READ_max = ReadData;
    }

    if (ReadData < READ_min)
    {
        READ_min = ReadData;
    }
}

void reading::display(Stream *S)
{
    S->print("[");
    S->printf("%05d", _get_reads());
    S->print(":");
    S->printf("%5.3f", _get_min());
    S->print(";");
    S->printf("%5.3f", _get_mean());
    S->print(";");
    S->printf("%5.3f", _get_max());
    S->printf(" %s]", _Unit);
    reset();
}

// --------------------------------
// READING - Private
// --------------------------------

int reading::_get_reads() { return READ_reads; }
float reading::_get_min() { return READ_min; }
float reading::_get_max() { return READ_max; }
float reading::_get_mean()
{
    if (READ_reads)
    {
        READ_mean /= READ_reads;
        READ_reads = 0;
    }

    return READ_mean;
}

// --------------------------------
// POWERTESTER - Public
// --------------------------------

powertester::powertester(int i2c_address, const char *Id) : shunt_mV("mV"), bus_V(), load_V(), current_Ma("mA"), power_mW("mW"), _Address(i2c_address), _DataRead(INAREAD_CURRENT)
{
    strncpy(_Id, Id, 8);
}

void powertester::setup()
{
    // Initalize tester chip
    if (begin())
    {
        Serial.printf("* INA219 chip (Addr: %X) ....................... [!!! Init failed !!!]\n", _Address);
        while (1)
        {
            delay(1000);
        }
    }
    Serial.printf("* INA219 chip (Addr: %X) ....................... [Initialized]\n", _Address);

    setCalibration_16V_400mA(); // 16V, 400mA range (higher precision on volts and amps):
    Serial.printf("* INA219 chip (Addr: %X) ....................... [Set: : 16V, 400mA range]\n", _Address);
}

void powertester::SetReading(DataTable Bitmap = INAREAD_CURRENT)
{
    _DataRead = Bitmap;
}

void powertester::update(ReadingMode Rm = INAMODE_RECURRENT)
{
    uint16_t Dr = _DataRead;
    float BusVoltage = 0;
    float ShuntVoltage = 0;

    if (Rm)
    {
        Dr ^= Dr; // Invert Reading Mask for Sparse Readings
    }
    if ((Dr && INAREAD_BUS) || (Dr && INAREAD_LOAD))
    {
        BusVoltage = getBusVoltage_V();
    }
    if ((Dr && INAREAD_SHUNT) || (Dr && INAREAD_LOAD))
    {
        ShuntVoltage = getShuntVoltage_mV();
    }
    if (Dr && INAREAD_SHUNT)
    {
        shunt_mV.set(ShuntVoltage);
    }
    if (Dr && INAREAD_BUS)
    {
        bus_V.set(BusVoltage);
    }
    if (Dr && INAREAD_LOAD)
    {
        load_V.set(BusVoltage + (ShuntVoltage / 1000));
    }
    if (Dr && INAREAD_CURRENT)
    {
        current_Ma.set(getCurrent_mA());
    }
    if (Dr && INAREAD_POWER)
    {
        power_mW.set(getPower_mW());
    }
}

void powertester::display(Stream *S)
{
    update(INAMODE_SPARSE);

    S->printf("%s: ", _Id);
    shunt_mV.display(S);
    bus_V.display(S);
    load_V.display(S);
    current_Ma.display(S);
    power_mW.display(S);

    S->println("");
}

// --------------------------------
// READING - Public
// --------------------------------