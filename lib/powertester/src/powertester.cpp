#include "Arduino.h"
#include "powertester.h"

// #include <iostream>
// std::cout << "---> Rm: (" << Rm << ") GRM: [" << _ReadMask << "] LRM: [" << LocalReadMask << "]" << std::endl;

// --------------------------------
// READING - Public
// --------------------------------

reading::reading(const char *Unit) : _IR_min(1), _IR_pile(0), _IR_max(-1), _IR_reads(0)
{
    strncpy(_Unit, Unit, 8);
}

void reading::reset()
{
    _IR_max = -1;
    _IR_pile = 0;
    _IR_min = 1;
    _IR_reads = 0;
}

void reading::set(float ReadData)
{
    _IR_reads++;

    _IR_pile += ReadData;

    if (ReadData > _IR_max)
    {
        _IR_max = ReadData;
    }

    if (ReadData < _IR_min)
    {
        _IR_min = ReadData;
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

int reading::_get_reads() { return _IR_reads; }
float reading::_get_min() { return _IR_min; }
float reading::_get_max() { return _IR_max; }
float reading::_get_mean()
{
    int RetVal = _IR_pile;

    if (_IR_reads)
    {
        RetVal = _IR_pile / _IR_reads;
    }

    return RetVal;
}

// --------------------------------
// POWERTESTER - Public
// --------------------------------

powertester::powertester(int i2c_address, const char *Id) : _Address(i2c_address)
{
    // Initalizing Reading
    _Readings = {reading("mV"), reading(), reading(), reading("mA"), reading("mW")};

    // Initialize Read Mode
    _ReadMask.reset();
    _ReadMask.set(IR_CURR);

    // Setting Powertester Identificator
    strncpy(_Id, Id, 8);
}

bool powertester::setup()
{
    // Initalizing INA219 chip
    if (!begin())
    {
        Serial.printf("* INA219 chip (Addr: %X) ....................... [!!! Init failed !!!]\n", _Address);
        // while (1)
        // {
        //     delay(1000);
        // }
    }
    Serial.printf("* INA219 chip (Addr: %X) ....................... [Initialized]\n", _Address);

    // Calibrating INA219 chip
    setCalibration_16V_400mA(); // 16V, 400mA range (higher precision on volts and amps):
    Serial.printf("* INA219 chip (Addr: %X) ....................... [Set: : 16V, 400mA range]\n", _Address);

    return (true);
}

void powertester::SetReading(DataTable Bitmap = IR_CURR)
{
    _ReadMask.reset();
    _ReadMask.set(Bitmap);
}

void powertester::update(ReadingMode Rm = IM_RECURR)
{
    std::bitset<8> LocalReadMask = _ReadMask;

    float BusVoltage = 0;
    float ShuntVoltage = 0;

    // Check if is a recurrent read || reverse bitmap
    if (Rm)
    {
        LocalReadMask = ~_ReadMask;
    }

    // Make every read, if Requested ...
    if ((LocalReadMask[IR_BUS]) || (LocalReadMask[IR_LOAD]))
    {
        BusVoltage = getBusVoltage_V();
    }
    if ((LocalReadMask[IR_SHNT]) || (LocalReadMask[IR_LOAD]))
    {
        ShuntVoltage = getShuntVoltage_mV();
    }
    if (LocalReadMask[IR_SHNT])
    {
        _Readings.at(IR_SHNT).set(ShuntVoltage);
    }
    if (LocalReadMask[IR_BUS])
    {
        _Readings.at(IR_BUS).set(BusVoltage);
    }
    if (LocalReadMask[IR_LOAD])
    {
        _Readings.at(IR_LOAD).set(BusVoltage + (ShuntVoltage / 1000));
    }
    if (LocalReadMask[IR_CURR])
    {
        _Readings.at(IR_CURR).set(getCurrent_mA());
    }
    if (LocalReadMask[IR_PWR])
    {
        _Readings.at(IR_PWR).set(getPower_mW());
    }
}

void powertester::display(Stream *S)
{
    update(IM_SPARSE);

    S->printf("--> %s: ", _Id);
    for (auto it = _Readings.begin(); it != _Readings.end(); ++it)
    {
        it->display(S);
    }
    S->println("");
}

// --------------------------------
// READING - Public
// --------------------------------