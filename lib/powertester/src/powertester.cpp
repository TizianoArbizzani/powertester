#include "Arduino.h"
#include "powertester.h"

// #include <iostream>
// std::cout << "---> Rm: (" << Rm << ") GRM: [" << _ReadMask << "] LRM: [" << LocalReadMask << "]" << std::endl;

#define UPDATE_TFT

static void _PrSet(int Value, PrintRead *Pr);

// --------------------------------
// READING - Public
// --------------------------------
reading::reading(const char *Label, const char *Unit, int Y, const unsigned short *Icon) : _IR_min(R_DEF_MIN),
                                                                                           _IR_pile(R_DEF_PILE),
                                                                                           _IR_max(R_DEF_MAX),
                                                                                           _IR_reads(R_DEF_READS),
                                                                                           _focus(false),
                                                                                           _hold(NO_HOLD_MODE),
                                                                                           _y(Y),
                                                                                           _tft_MU_icon(Icon)
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
    //if (ReadData >= 0)
    //{
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
    //}
}

void reading::display(Stream *S, int Xoff, int BgColor)
{
    bool tftUpdate = false;

#ifdef UPDATE_TFT
    if (_y >= 0)
    {
        tftUpdate = true;
    }
#endif

    PrintRead PrMean;
    _PrSet(_get_mean(), &PrMean);

    _spr->fillSprite(BgColor);

    if (_focus) // Focused HiSpeed reading
    {
        PrintRead PrMin;
        PrintRead PrMax;

        // Prepare specifical data for display
        _PrSet(_get_min(), &PrMin);
        _PrSet(_get_max(), &PrMax);

        // Tft Update
        if (tftUpdate)
        {
            // Print MaxValue
            if (_hold)
            {
                _spr->fillSprite(BgColor);
                _spr->setTextColor((PrMax.Sign == '+') ? TFT_POS_COLOR : TFT_NEG_COLOR, BgColor);
                _spr->drawString(String(PrMax.Int) + "." + String(PrMax.Fract), TFT_ATZERO);
                _spr->pushSprite(Xoff + TFT_SPR_ADD_OFF, _y - TFT_FL_DISPL);
            }

            // Print MaxValue
            _spr->fillSprite(BgColor);
            _spr->setTextColor((PrMean.Sign == '+') ? TFT_POS_COLOR : TFT_NEG_COLOR, BgColor);
            _spr->drawString(String(PrMean.Int) + "." + String(PrMean.Fract), TFT_ATZERO);
            _spr->pushSprite(Xoff + TFT_SPR_ADD_OFF, _y);

            int FullData = (atoi(PrMean.Int) * READ_COEFF) + atoi(PrMean.Fract);
            int Xpart = (FullData * TFT_SPR_BAR_WIDTH) / 1000;

            _bar->drawString("0", 4, 0);
            _bar->fillRect(20, 0, Xpart + 20, 28, TFT_RED);
            _bar->fillRect(Xpart + 20, 0, TFT_SPR_BAR_WIDTH - Xpart, 28, TFT_GREEN);
            _bar->drawString("1", 228, 0);
            _bar->pushSprite(Xoff, 286);

            // Print MinValue
            if (_hold)
            {
                _spr->fillSprite(BgColor);
                _spr->setTextColor((PrMin.Sign == '+') ? TFT_POS_COLOR : TFT_NEG_COLOR, BgColor);
                _spr->drawString(String(PrMin.Int) + "." + String(PrMin.Fract), TFT_ATZERO);
                _spr->pushSprite(Xoff + TFT_SPR_ADD_OFF, _y + TFT_FL_DISPL);
            }
        }

        // Serial results printing
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
        else if (_SerialPrints == D_REDUCED)
        {
            S->printf("%lu:%d", millis(), _get_reads());
        }
    }
    else // Non Focused Slow Speed reading
    {
        // Tft Update
        if (tftUpdate)
        {
            _spr->fillSprite(BgColor);
            _spr->setTextColor((PrMean.Sign == '+') ? TFT_POS_COLOR : TFT_NEG_COLOR, BgColor);
            _spr->drawString(String(PrMean.Int) + "." + String(PrMean.Fract), TFT_ATZERO);
            _spr->pushSprite(Xoff + TFT_SPR_ADD_OFF, _y);
        }

        // Serial results printing
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

char *reading::getMeasurementUnit() { return (_Unit); }

uint16_t reading::gety() { return (_y); }

void reading::setTFT(TFT_eSPI *tft, TFT_eSprite *spr, TFT_eSprite *bar)
{
    _tft = tft;
    _tft_width = tft->width();
    _tft_height = tft->width();
    _spr = spr;
    _bar = bar;
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
                                                                                          _OutPin(OutPin),
                                                                                          _Readings({reading("BU", "V", TFT_Y1, IconV),
                                                                                                     reading("SH", "mV", TFT_YN, IconV),
                                                                                                     reading("LD", "V", TFT_Y2, IconV),
                                                                                                     reading("CU", "A", TFT_FC, IconA),
                                                                                                     reading("PW", "W", TFT_Y3, IconW)})
{
    // Initialize Read Mode
    _ReadMask.reset();
    _ReadMask.set(IR_CURR);

    // Setting Powertester Identificator
    strncpy(_Id, Id, 8);
}

bool powertester::setup(TFT_eSPI *tft, uint8_t SerialMode = D_HUMAN, int Bitmap = IR_CURR)
{
    int i = 0;

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

    _tft = tft;

    _spr_small = new TFT_eSprite(_tft);
    _spr_small->createSprite(TFT_SPR_SML_W, TFT_SPR_SML_H);
    _spr_small->setColorDepth(16);
    _spr_small->loadFont(TFT_SPR_SML_F);
    _spr_small->setTextDatum(TL_DATUM);

    _spr_big = new TFT_eSprite(_tft);
    _spr_big->createSprite(TFT_SPR_BIG_W, TFT_SPR_BIG_H);
    _spr_big->setColorDepth(16);
    _spr_big->loadFont(TFT_SPR_BIG_F);
    _spr_big->setTextDatum(TL_DATUM);

    _spr_leftbutton = new TFT_eSprite(_tft);
    _spr_leftbutton->setColorDepth(16);
    _spr_leftbutton->createSprite(116, 45);
    _spr_leftbutton->loadFont(TFT_SPR_SML_F);
    _spr_leftbutton->setTextDatum(TL_DATUM);
    _spr_leftbutton->fillSprite(TFT_BLACK);
    _spr_leftbutton->fillRoundRect(0, 0, 116, 45, 8, TFT_BLACK);

    _spr_rightbutton = new TFT_eSprite(_tft);
    _spr_rightbutton->setColorDepth(16);
    _spr_rightbutton->createSprite(116, 45);
    _spr_rightbutton->loadFont(TFT_SPR_SML_F);
    _spr_rightbutton->setTextDatum(TL_DATUM);
    _spr_rightbutton->fillSprite(TFT_BLACK);
    _spr_rightbutton->fillRoundRect(0, 0, 116, 45, 8, TFT_BLACK);

    _spr_databar = new TFT_eSprite(_tft);
    _spr_databar->setColorDepth(16);
    _spr_databar->createSprite(233, 30);
    _spr_databar->loadFont(TFT_SPR_SML_F);
    _spr_databar->setTextDatum(TR_DATUM);
    _spr_databar->fillSprite(TFT_BLACK);
    _spr_databar->setTextColor(TFT_WHITE, TFT_BLACK);

    _SerialPrints = SerialMode;

    std::bitset<32> TBS(Bitmap);
    _ReadMask = TBS;

    setHoldingMode(false);

    for (auto Curr_Reading = _Readings.begin(); Curr_Reading != _Readings.end(); ++Curr_Reading)
    {
        Curr_Reading->setSerialPrints(_SerialPrints);
        Curr_Reading->setTFT(_tft, (_ReadMask[i]) ? _spr_big : _spr_small, _spr_databar);
        Curr_Reading->setFocus(_ReadMask[i++]);
    }

    return (_Active);
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
        it->display(S, _Xoffset, (_Output) ? TFT_ON_COLOR : TFT_OFF_COLOR);
        it->reset();
    }
    S->println("");
}

bool powertester::setHoldingMode(bool HoldingMode)
{
    if (HoldingMode)
    {
        _spr_rightbutton->setTextColor(TFT_BLACK, TFT_RED);
        _spr_rightbutton->fillRoundRect(4, 4, 108, 37, 8, TFT_RED);
        _spr_rightbutton->drawString("Hold", 10, 7);
    }
    else
    {
        _spr_rightbutton->setTextColor(TFT_BLACK, TFT_GREEN);
        _spr_rightbutton->fillRoundRect(4, 4, 108, 37, 8, TFT_GREEN);
        _spr_rightbutton->drawString("Hold", 10, 7);
    }
    _spr_rightbutton->pushSprite(_Xoffset + 117, 4);

    for (reading *it = _Readings.begin(); it != _Readings.end(); ++it)
    {
        it->setHoldingMode(HoldingMode);
    }

    return (HoldingMode);
}

bool powertester::setOutputMode(bool OutputMode)
{
    _Output = OutputMode;

    uint32_t BgColor = (_Output) ? TFT_ON_COLOR : TFT_OFF_COLOR;

    _tft->fillRect(_Xoffset, 50, 233, 235, BgColor);

    if (OutputMode)
    {
        _spr_leftbutton->setTextColor(TFT_BLACK, TFT_GREEN);
        _spr_leftbutton->fillRoundRect(4, 4, 108, 37, 8, TFT_GREEN);
        _spr_leftbutton->drawString("On", 10, 7);
    }
    else
    {
        _spr_leftbutton->setTextColor(TFT_BLACK, TFT_RED);
        _spr_leftbutton->fillRoundRect(4, 4, 108, 37, 8, TFT_RED);
        _spr_leftbutton->drawString("Off", 10, 7);
    }
    _spr_leftbutton->pushSprite(_Xoffset, 4);

    // for (auto Curr_Reading = _Readings.begin(); Curr_Reading != _Readings.end(); ++Curr_Reading)
    // {
    //     _spr_measunit->fillSprite(BgColor);
    //     _spr_measunit->setTextColor(TFT_BLUE, BgColor);
    //     _spr_measunit->drawString(String(Curr_Reading->getMeasurementUnit()), 0, 0);

    //     if (Curr_Reading->hasFocus())
    //     {
    //         _spr_measunit->pushSprite(_Xoffset + TFT_SPR_MEA_X_DISP, Curr_Reading->gety() - TFT_FL_DISPL + TFT_SPR_MEA_X_DISP_BIG);
    //         _spr_measunit->pushSprite(_Xoffset + TFT_SPR_MEA_X_DISP, Curr_Reading->gety() + TFT_SPR_MEA_X_DISP_BIG);
    //         _spr_measunit->pushSprite(_Xoffset + TFT_SPR_MEA_X_DISP, Curr_Reading->gety() + TFT_FL_DISPL + TFT_SPR_MEA_X_DISP_BIG);
    //     }
    //     else
    //     {
    //         _spr_measunit->pushSprite(_Xoffset + TFT_SPR_MEA_X_DISP, Curr_Reading->gety());
    //     }
    // }

    _tft->pushImage(_Xoffset + 2, TFT_Y1, IconBus_width, IconBus_height, IconBus, 0);
    _tft->pushImage(_Xoffset + 2, TFT_Y2, IconLoad_width, IconLoad_height, IconLoad, 0);
    _tft->pushImage(_Xoffset + 2, TFT_Y3, IconPower_width, IconPower_height, IconPower, 0);
    _tft->pushImage(_Xoffset + 2, TFT_FC - TFT_FL_DISPL + TFT_SPR_MEA_X_DISP_BIG, IconCurrent_Max_width, IconCurrent_Max_height, IconCurrent_Max, 0);
    _tft->pushImage(_Xoffset + 2, TFT_FC + TFT_SPR_MEA_X_DISP_BIG, IconCurrent_width, IconCurrent_height, IconCurrent, 0);
    _tft->pushImage(_Xoffset + 2, TFT_FC + TFT_FL_DISPL + TFT_SPR_MEA_X_DISP_BIG, IconCurrent_Min_width, IconCurrent_Min_height, IconCurrent_Min, 0);

    _tft->pushImage(_Xoffset + TFT_SPR_MEA_X_DISP, TFT_Y1, 0x18, 0x18, IconV, 0);
    _tft->pushImage(_Xoffset + TFT_SPR_MEA_X_DISP, TFT_Y2, 0x18, 0x18, IconV, 0);
    _tft->pushImage(_Xoffset + TFT_SPR_MEA_X_DISP, TFT_Y3, 0x18, 0x18, IconW, 0);
    _tft->pushImage(_Xoffset + TFT_SPR_MEA_X_DISP, TFT_FC - TFT_FL_DISPL + TFT_SPR_MEA_X_DISP_BIG, 0x18, 0x18, IconA, 0);
    _tft->pushImage(_Xoffset + TFT_SPR_MEA_X_DISP, TFT_FC + TFT_SPR_MEA_X_DISP_BIG, 0x18, 0x18, IconA, 0);
    _tft->pushImage(_Xoffset + TFT_SPR_MEA_X_DISP, TFT_FC + TFT_FL_DISPL + TFT_SPR_MEA_X_DISP_BIG, 0x18, 0x18, IconA, 0);

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