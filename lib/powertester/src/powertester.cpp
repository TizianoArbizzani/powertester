#include "Arduino.h"
#include "powertester.h"

// #include <iostream>
// std::cout << "---> Rm: (" << Rm << ") GRM: [" << _ReadMask << "] LRM: [" << LocalReadMask << "]" << std::endl;

// --------------------------------
// READING - Public
// --------------------------------
reading::reading(const char *Label, const char *Unit) : _IR_min(1), _IR_pile(0), _IR_max(-1), _IR_reads(0), _focus(false)
{
    strncpy(_Unit, Unit, 8);
    strncpy(_Label, Label, 4);
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
    S->printf("%s%s:", _focus ? "<" : "[", _Label);

    if (_focus)
    {
        S->printf("(%03d) ", _get_reads());
    }

    S->printf("%5.2f;%5.2f;%5.2f %s%s ", _get_min(), _get_mean(), _get_max(), _Unit, _focus ? ">" : "]");

    reset();
}

bool reading::setFocus(bool Focus = true)
{
    _focus = Focus;
    return (_focus);
}

bool reading::hasFocus() { return (_focus); }

// --------------------------------
//  READING - Private
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
    _Readings = {reading("Shn", "mV"), reading("Bus"), reading("Loa"), reading("Cur", "mA"), reading("Pwr", "mW")};

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
        _Active = false;
    }
    else
    {
        Serial.printf("* INA219 chip (Addr: %X) ....................... [Initialized]\n", _Address);
        _Active = true;

        // Calibrating INA219 chip
        setCalibration_16V_400mA(); // 16V, 400mA range (higher precision on volts and amps):
        Serial.printf("* INA219 chip (Addr: %X) ....................... [Set: : 16V, 400mA range]\n", _Address);
    }

    return (_Active);
}

void powertester::SetReading(int Bitmap = IR_CURR)
{
    int i = 0;

    std::bitset<32> TBS(Bitmap);
    _ReadMask = TBS;

    // Set New Readings Focus()
    for (auto it = _Readings.begin(); it != _Readings.end(); ++it)
    {
        it->setFocus(_ReadMask[i++]);
    }
}

void powertester::update(uint16_t Rm = IM_RECURR)
{
    std::bitset<32> LocalReadMask = _ReadMask;

    float BusVoltage = 0;
    float ShuntVoltage = 0;

    // Check if is a recurrent read || reverse bitmap
    LocalReadMask = Rm ? ~_ReadMask : _ReadMask;

    // Make every read, if Requested ...
    if ((LocalReadMask[IR_BUS]) || (LocalReadMask[IR_LOAD]))
    {
        BusVoltage = _Active ? getBusVoltage_V() : 0;
    }
    if ((LocalReadMask[IR_SHNT]) || (LocalReadMask[IR_LOAD]))
    {
        ShuntVoltage = _Active ? getShuntVoltage_mV() : 0;
    }

    if (LocalReadMask[IR_SHNT])
    {
        _Readings.at(IR_SHNT).set(_Active ? ShuntVoltage : 0);
    }
    if (LocalReadMask[IR_BUS])
    {
        _Readings.at(IR_BUS).set(_Active ? BusVoltage : 0);
    }
    if (LocalReadMask[IR_LOAD])
    {
        _Readings.at(IR_LOAD).set(BusVoltage + (ShuntVoltage / 1000));
    }
    if (LocalReadMask[IR_CURR])
    {
        _Readings.at(IR_CURR).set(_Active ? getCurrent_mA() : 0);
    }
    if (LocalReadMask[IR_PWR])
    {
        _Readings.at(IR_PWR).set(_Active ? getPower_mW() : 0);
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