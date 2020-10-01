#include "Arduino.h"
#include "powertester.h"

// #include <iostream>
// std::cout << "---> Rm: (" << Rm << ") GRM: [" << _ReadMask << "] LRM: [" << LocalReadMask << "]" << std::endl;

static void _PrSet(int Value, PrintRead *Pr);

// --------------------------------
// READING - Public
// --------------------------------
reading::reading(const char *Label, const char *Unit) : _IR_min(R_DEF_MIN), _IR_pile(R_DEF_PILE), _IR_max(R_DEF_MAX), _IR_reads(R_DEF_READS), _focus(false)
{
    strncpy(_Unit, Unit, 8);
    strncpy(_Label, Label, 4);
}

void reading::reset()
{
    _IR_max = R_DEF_MAX;
    _IR_pile = R_DEF_PILE;
    _IR_min = R_DEF_MIN;
    _IR_reads = R_DEF_READS;
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

void reading::display(Stream *S, int Xoff)
{
    PrintRead PrMin;
    PrintRead PrMean;
    PrintRead PrMax;

    // S
    _PrSet(_get_min(), &PrMin);
    _PrSet(_get_mean(), &PrMean);
    _PrSet(_get_max(), &PrMax);

    S->printf("%s%s:", _focus ? "<" : "[", _Label);

    if (_focus)
    {
        S->printf("(%03d) ", _get_reads());
    }

    S->printf("%c%s.%s;%c%s.%s;%c%s.%s %s%s ",
              PrMin.Sign, PrMin.Int, PrMin.Fract,
              PrMean.Sign, PrMean.Int, PrMean.Fract,
              PrMax.Sign, PrMax.Int, PrMax.Fract,
              _Unit, _focus ? ">" : "]");

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

int reading::_get_min() { return _IR_min; }

int reading::_get_max() { return _IR_max; }

int reading::_get_mean()
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

powertester::powertester(uint8_t i2c_address, const char *Id, int Xoff) : Adafruit_INA219(i2c_address), _Address(i2c_address), _Xoffset(Xoff)
{
    // Initalizing Reading
    _Readings = {reading("SH", "mV"), reading("BU"), reading("LD"), reading("CU", "mA"), reading("PW", "mW")};

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

    int BusVoltage = 0;
    int ShuntVoltage = 0;

    // Check if is a recurrent read || reverse bitmap
    LocalReadMask = Rm ? ~_ReadMask : _ReadMask;

    // Make every read, if Requested ...
    if ((LocalReadMask[IR_BUS]) || (LocalReadMask[IR_LOAD]))
    {
        BusVoltage = _Active ? getBusVoltage_V() * READ_COEFF : 0;
    }
    if ((LocalReadMask[IR_SHNT]) || (LocalReadMask[IR_LOAD]))
    {
        ShuntVoltage = _Active ? getShuntVoltage_mV() * READ_COEFF : 0;
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
        _Readings.at(IR_CURR).set(_Active ? getCurrent_mA() * READ_COEFF : 0);
    }
    if (LocalReadMask[IR_PWR])
    {
        _Readings.at(IR_PWR).set(_Active ? getPower_mW() * READ_COEFF : 0);
    }
}

void powertester::display(Stream *S)
{
    // Load values usually not read at during high speed sampling ...
    update(IM_SPARSE);

    S->printf("%s ", _Id);
    for (auto it = _Readings.begin(); it != _Readings.end(); ++it)
    {
        it->display(S, _Xoffset);
    }
    S->println("");
}

// --------------------------------
// READING - Public
// --------------------------------

// --------------------------------
// Static functions
// --------------------------------
/**
 * @brief Convert an integer reac to a quasi-float value ...
 * 
 * @param[in] Value Integer value to be converted
 * @param[out] Pr Structure containing the value coverted for display
 */
static void _PrSet(int Value, PrintRead *Pr)
{
    int Ip = abs(Value) / READ_COEFF;
    int Fp = abs(Value) % READ_COEFF;

    Pr->Sign = (Value <= 0) ? '-' : ' ';
    snprintf(Pr->Int, INT_SIZE, "%2d", Ip);
    snprintf(Pr->Fract, FRACT_SIZE, "%03d", Fp);
}