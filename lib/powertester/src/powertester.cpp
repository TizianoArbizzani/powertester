#include "Arduino.h"
#include "powertester.h"
#include "Free_Fonts.h"

// #include <iostream>
// std::cout << "---> Rm: (" << Rm << ") GRM: [" << _ReadMask << "] LRM: [" << LocalReadMask << "]" << std::endl;

static void _PrSet(int Value, PrintRead *Pr);

// --------------------------------
// READING - Public
// --------------------------------
reading::reading(const char *Label, const char *Unit) : _IR_min(R_DEF_MIN),
                                                        _IR_pile(R_DEF_PILE),
                                                        _IR_max(R_DEF_MAX),
                                                        _IR_reads(R_DEF_READS),
                                                        _focus(false),
                                                        _hold(NO_HOLD_MODE)
{
    strncpy(_Unit, Unit, 8);
    strncpy(_Label, Label, 4);
}

void reading::reset()
{
    _IR_pile = R_DEF_PILE;
    _IR_reads = R_DEF_READS;

    if (!_hold)
    {
        _IR_max = R_DEF_MAX;
        _IR_min = R_DEF_MIN;
    }
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

void reading::display(Stream *S, int Xoff, int BgColor)
{
    PrintRead PrMean;
    _PrSet(_get_mean(), &PrMean);

    _tft->setTextColor(TFT_BLACK, BgColor);
    _tft->setTextPadding(180);
    _tft->setTextDatum(ML_DATUM);

    if (_focus)
    {
        // Focused HiSpeed reading

        char Linea[16];

        PrintRead PrMin;
        PrintRead PrMax;
        _PrSet(_get_min(), &PrMin);
        _PrSet(_get_max(), &PrMax);

        _tft->setFreeFont(TFT_BC);

        if (_hold)
        {
            sprintf(Linea, "%c%s.%s", PrMin.Sign, PrMin.Int, PrMin.Fract);
            _tft->drawString(Linea, Xoff + 2, TFT_Y2, GFXFF);
        }
        else
        {
            _tft->drawString("", Xoff + 2, TFT_Y2, GFXFF);
        }

        sprintf(Linea, "%c%s.%s", PrMean.Sign, PrMean.Int, PrMean.Fract);
        _tft->setFreeFont(TFT_BC);
        _tft->drawString(Linea, Xoff + 2, TFT_Y3, GFXFF);

        _tft->setFreeFont(TFT_BC);
        if (_hold)
        {
            sprintf(Linea, "%c%s.%s", PrMax.Sign, PrMax.Int, PrMax.Fract);
            _tft->drawString(Linea, Xoff + 2, TFT_Y4, GFXFF);
        }
        else
        {
            _tft->drawString("", Xoff + 2, TFT_Y4, GFXFF);
        }

        if (_SerialPrints == D_HUMAN)
        {
            S->printf("<%s: (" N_CYA "%03d" RESET ")  %s  %s%s.%s" RESET "    %s%s.%s" RESET "    %s%s.%s" B_YEL "    %s" RESET ">  ",
                      _Label,
                      _get_reads(),
                      (_hold) ? N_BLU "HOLD" RESET : "    ",
                      (PrMin.Sign == '-') ? B_RED : B_GRE, PrMin.Int, PrMin.Fract,
                      (PrMean.Sign == '-') ? B_RED : B_GRE, PrMean.Int, PrMean.Fract,
                      (PrMax.Sign == '-') ? B_RED : B_GRE, PrMax.Int, PrMax.Fract,
                      _Unit);
        }
        else if (_SerialPrints == D_MACHINE)
        {
            S->printf("F;%03d;%c%s.%s;%c%s.%s;%c%s.%s;",
                      _get_reads(),
                      PrMin.Sign, PrMin.Int, PrMin.Fract,
                      PrMean.Sign, PrMean.Int, PrMean.Fract,
                      PrMax.Sign, PrMax.Int, PrMax.Fract);
        }
    }
    else
    {
        char Linea[16];
        _tft->setFreeFont(TFT_LC);
        sprintf(Linea, "%c%s.%s", PrMean.Sign, PrMean.Int, PrMean.Fract);

        if (strcmp(_Label, "BU") == 0)
        {
            _tft->drawString(Linea, Xoff + 2, TFT_Y0, GFXFF);
        }

        if (strcmp(_Label, "LD") == 0)
        {
            _tft->drawString(Linea, Xoff + 2, TFT_Y1, GFXFF);
        }

        if (strcmp(_Label, "PW") == 0)
        {
            _tft->drawString(Linea, Xoff + 2, TFT_Y5, GFXFF);
        }

        // Unfocused _display-refresh-only_ reading
        if (_SerialPrints == D_HUMAN)
        {
            S->printf("<%s: %s%s.%s" B_YEL " %s" RESET ">  ",
                      _Label,
                      (PrMean.Sign == '-') ? B_RED : B_GRE, PrMean.Int, PrMean.Fract,
                      _Unit);
        }
        else if (_SerialPrints == D_MACHINE)
        {

            S->printf("C;%c%s.%s;", PrMean.Sign, PrMean.Int, PrMean.Fract);
        }
    }
}

bool reading::setFocus(bool Focus = true)
{
    _focus = Focus;
    return (_focus);
}

bool reading::hasFocus() { return (_focus); }

void reading::setSerialPrints(uint8_t SerialMode)
{
    _SerialPrints = SerialMode;
}

bool reading::setHoldingMode(bool HoldingMode)
{
    _hold = HoldingMode;

    return (_hold);
}

bool reading::getHoldingMode() { return (_hold); }

void reading::setTFT(TFT_eSPI *tft, uint16_t Width, uint16_t Height)
{
    _tft = tft;
    _tft_width = Width;
    _tft_height = Height;
}

// --------------------------------
//  READING - Private
// --------------------------------

int reading::_get_reads()
{
    return _IR_reads;
}

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

powertester::powertester(uint8_t i2c_address, const char *Id, int Xoff, uint8_t OutPin) : Adafruit_INA219(i2c_address),
                                                                                          _Address(i2c_address),
                                                                                          _Active(false),
                                                                                          _Xoffset(Xoff),
                                                                                          _Output(false),
                                                                                          _OutPin(OutPin)
{
    // Initalizing Reading
    _Readings = {reading("BU"), reading("SH", "mV"), reading("LD"), reading("CU", "mA"), reading("PW", "mW")};

    // Initialize Read Mode
    _ReadMask.reset();
    _ReadMask.set(IR_CURR);
    // Setting Powertester Identificator
    strncpy(_Id, Id, 8);
}

bool powertester::setup(uint8_t SerialMode)
{
    // Initalizing INA219 chip
    if (!begin())
    {
        Serial.printf("* INA219 : Addr: 0x%X ......... [" N_RED "!!! Init failed !!!" RESET "]\n", _Address);
        _Active = false;
    }
    else
    {
        _Active = true;

        if (SerialMode)
            Serial.printf("* INA219 : Addr: 0x%X ......... [" N_GRE "Initialized" RESET "]\n", _Address);

        // Calibrating INA219 chip
        setCalibration_16V_400mA(); // 16V, 400mA range (higher precision on volts and amps):
        if (SerialMode)
            Serial.printf("* INA219 : Addr: 0x%X ......... [" N_GRE "Set: : 16V, 400mA range" RESET "]\n", _Address);
    }

    _SerialPrints = SerialMode;

    for (reading *it = _Readings.begin(); it != _Readings.end(); ++it)
    {
        it->setSerialPrints(SerialMode);
    }

    return (_Active);
}

void powertester::setReading(int Bitmap = IR_CURR)
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

void powertester::setTFT(TFT_eSPI *tft)
{
    _tft = tft;
    _tft_width = tft->width();
    _tft_height = tft->height();

    tft->setSwapBytes(true);
    tft->pushImage(10 + _Xoffset, 8, infoWidth, infoHeight, info);
    tft->pushImage(50 + _Xoffset, 8, alertWidth, alertHeight, alert);
    tft->pushImage(90 + _Xoffset, 8, closeWidth, closeHeight, closeX);
    tft->setSwapBytes(false);

    // Set New Readings Focus()
    for (auto it = _Readings.begin(); it != _Readings.end(); ++it)
    {
        it->setTFT(tft, _tft_width, _tft_height);
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
        _Readings.at(IR_BUS).set(_Active ? BusVoltage : 0);
    }
    if ((LocalReadMask[IR_SHNT]) || (LocalReadMask[IR_LOAD]))
    {
        ShuntVoltage = _Active ? getShuntVoltage_mV() : 0;
        _Readings.at(IR_SHNT).set(_Active ? ShuntVoltage : 0);
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

    if (_SerialPrints == D_HUMAN)
    {
        S->printf(B_MAG "%s " RESET, _Id);
    }
    else if (_SerialPrints == D_MACHINE)
    {
        S->printf("%s;", _Id);
    }

    for (reading *it = _Readings.begin(); it != _Readings.end(); ++it)
    {
        it->display(S, _Xoffset, (_Output) ? TFT_GREEN : TFT_RED);
        //it->display(S, _Xoffset, (_Output) ? TFT_RED : TFT_GREEN);
        it->reset();
    }
    S->println("");
}

bool powertester::setHoldingMode(bool HoldingMode)
{
    for (reading *it = _Readings.begin(); it != _Readings.end(); ++it)
    {
        //if (it->hasFocus())
        it->setHoldingMode(HoldingMode);
    }

    return (HoldingMode);
}

bool powertester::setOutputMode(bool OutputMode)
{
    _Output = OutputMode;

    _tft->fillRect(_Xoffset + 1, 50, _Xoffset + 225, 266, (_Output) ? TFT_GREEN : TFT_RED);

    return (_Output);
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

    Pr->Sign = (Value >= 0) ? '+' : '-';

    snprintf(Pr->Int, INT_SIZE, "%02d", Ip);
    snprintf(Pr->Fract, FRACT_SIZE, "%03d", Fp);
}