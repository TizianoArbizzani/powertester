#include "Arduino.h"
#include "powertester.h"

// --------------------------------
// READING - Public
// --------------------------------

reading::reading()
{
    _min = 2;
    _max = -2;
}

void reading::reset()
{
    _mean = 0;
    _max = -1;
    _min = 1;
    _reads = 0;
}

void reading::set(float ReadData)
{
    _reads++;

    _mean += ReadData;

    if (ReadData > _max)
    {
        _max = ReadData;
    }

    if (ReadData < _min)
    {
        _min = ReadData;
    }
}

void reading::display(Stream *S)
{
    S->print("[");
    S->print(_get_reads());
    S->print(":");
    S->print(_get_min());
    S->print(";");
    S->print(_get_mean());
    S->print(";");
    S->print(_get_max());
    S->print("]");
    reset();
}

// --------------------------------
// READING - Private
// --------------------------------

int reading::_get_reads() { return _reads; }
float reading::_get_min() { return _min; }
float reading::_get_max() { return _max; }
float reading::_get_mean()
{
    _mean /= _reads;
    _reads = 0;
    return _mean;
}

// --------------------------------
// POWERTESTER - Public
// --------------------------------

powertester::powertester(int i2c_address)
{
    _Address = i2c_address;
    _DataRead = INAREAD_CURRENT;
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