#include "Arduino.h"
#include "powertester.h"

#define UPDATE_TFT

static void _PrSet(int Value, PrintRead *Pr);
static void _tft_push(tft_message *Message);
extern xQueueHandle MsgQueue;

// --------------------------------
// READING - Public
// --------------------------------
reading::reading(const char *label,
                 uint8_t icon,
                 uint8_t unit,
                 uint8_t testerId) : _Icon(icon),
                                     _Unit(unit),
                                     _DisplayID(testerId),
                                     _IR_min(R_DEF_MIN),
                                     _IR_pile(R_DEF_PILE),
                                     _IR_max(R_DEF_MAX),
                                     _IR_reads(R_DEF_READS),
                                     _focus(false),
                                     _hold(NO_HOLD_MODE)
{
    strncpy(_Label, label, 7);
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

void reading::display(Stream *S, int BgColor)
{
    bool tftUpdate = false;
    tft_message Msg;

    char MU;

    switch (_Unit)
    {
    case TFT_ICON_MU_VL:
        MU = 'V';
        break;
    case TFT_ICON_MU_AM:
        MU = 'A';
        break;
    case TFT_ICON_MU_WT:
        MU = 'W';
        break;
    default:
        MU = '?';
        break;
    }

#ifdef UPDATE_TFT
    if (_tft_area)
    {
        tftUpdate = true;
        Msg.Sender = _DisplayID;
        Msg.BgColor = BgColor;
    }
#endif

    PrintRead PrMean;
    _PrSet(_get_mean(), &PrMean);

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
                Msg.FgColor = (PrMax.Sign == '+') ? TFT_POS_COLOR : TFT_NEG_COLOR;
                Msg.Area = TFT_A_LT_DATA;
                sprintf(Msg.Message, "%s.%s", PrMax.Int, PrMax.Fract);
                _tft_push(&Msg);
            }

            // Print MeanValue
            Msg.FgColor = (PrMean.Sign == '+') ? TFT_POS_COLOR : TFT_NEG_COLOR;
            Msg.Area = TFT_A_LF_DATA;
            sprintf(Msg.Message, "%s.%s", PrMean.Int, PrMean.Fract);
            _tft_push(&Msg);

            // Print MinValue
            if (_hold)
            {
                Msg.FgColor = (PrMin.Sign == '+') ? TFT_POS_COLOR : TFT_NEG_COLOR;
                Msg.Area = TFT_A_LB_DATA;
                sprintf(Msg.Message, "%s.%s", PrMin.Int, PrMin.Fract);
                _tft_push(&Msg);
            }

            // Area Bar defining values
            int FullData = (atoi(PrMean.Int) * READ_COEFF) + atoi(PrMean.Fract);
            int Xpart = (FullData * TFT_SPR_BAR_WIDTH) / 1000;

            // Area Bar printing values
            Msg.Area = TFT_A_DATABAR;
            Msg.BgColor = 0; // Remark: Field used in a nonstandard way ...
            Msg.FgColor = 1; // Remark: Field used in a nonstandard way ...
            sprintf(Msg.Message, "%d", Xpart);
            _tft_push(&Msg);
        }

        // Serial results printing
        if (_SerialPrints == D_HUMAN)
        {
            S->printf("<%s: (" N_CYA "%03d" RESET ")  %s  %s%s.%s" RESET "    %s%s.%s" RESET "    %s%s.%s" B_YEL "    %c" RESET ">  ",
                      _Label,
                      _get_reads(),
                      (_hold) ? N_BLU "HOLD" RESET : "    ",
                      (PrMin.Sign == '-') ? B_RED : B_GRE, PrMin.Int, PrMin.Fract,
                      (PrMean.Sign == '-') ? B_RED : B_GRE, PrMean.Int, PrMean.Fract,
                      (PrMax.Sign == '-') ? B_RED : B_GRE, PrMax.Int, PrMax.Fract,
                      MU);
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
            // Print MeanValue
            Msg.FgColor = (PrMean.Sign == '+') ? TFT_POS_COLOR : TFT_NEG_COLOR;
            Msg.Area = _tft_area;
            sprintf(Msg.Message, "%s.%s", PrMean.Int, PrMean.Fract);
            _tft_push(&Msg);
        }

        // Serial results printing
        if (_SerialPrints == D_HUMAN)
        {
            S->printf("<%s: %s%s.%s" B_YEL " %c" RESET ">  ",
                      _Label,
                      (PrMean.Sign == '-') ? B_RED : B_GRE, PrMean.Int, PrMean.Fract,
                      MU);
        }
        else if (_SerialPrints == D_MACHINE)
        {
            S->printf("C;%c%s.%s;", PrMean.Sign, PrMean.Int, PrMean.Fract);
        }
    }
}

bool reading::setFocus(bool Focus, uint8_t DisplayArea)
{
    _focus = Focus;
    _tft_area = DisplayArea;

    if (Focus)
    {
        // Message preparation
        tft_message Message;
        Message.Sender = _DisplayID;
        Message.BgColor = TFT_ON_COLOR;
        Message.FgColor = TFT_BLACK;
        Message.Area = TFT_A_L_BUTTN;
        sprintf(Message.Message, "%s", _Label);
        _tft_push(&Message);
    }

    return (_focus);
}

bool reading::hasFocus() { return (_focus); }

void reading::setSerialPrints(uint8_t SerialMode)
{
    _SerialPrints = SerialMode;
}

bool reading::setHoldingMode(bool HoldingMode, uint32_t BgColor)
{
    _hold = HoldingMode;

    if (_focus)
    {
        if (HoldingMode)
        {
            _ShowIconsHold(BgColor); // Show holding mode Icons
        }
        else
        {
            tft_message Msg;
            Msg.Sender = _DisplayID;
            Msg.BgColor = BgColor;

            Msg.Message[0] = 0;

            Msg.Area = TFT_A_LT_DATA; // Remove Maximum Data
            _tft_push(&Msg);
            Msg.Area = TFT_A_LB_DATA; // Remove Minimum Data
            _tft_push(&Msg);

            _RemoveIconsHold(BgColor); // Remove holding mode Icons
        }
    }

    return (_hold);
}

bool reading::getHoldingMode() { return (_hold); }

void reading::ShowIcons(uint32_t BgColor)
{
    tft_message Msg;
    Msg.Sender = _DisplayID;

    // Reading type Icon
    Msg.Area = READ_ICON_AREA;
    Msg.Message[0] = _Icon;
    _tft_push(&Msg);

    // Measure Unit Icon
    Msg.Area = UNIT_ICON_AREA;
    Msg.Message[0] = _Unit;
    _tft_push(&Msg);

    if (_focus)
    {
        if (_hold)
            _ShowIconsHold(BgColor); // Show Min/Max Icons
        else
            _RemoveIconsHold(BgColor); // Show Min/Max Icons
    }
}

void reading::_ShowIconsHold(uint32_t BgColor)
{
    tft_message Msg;
    Msg.BgColor = 0XCC; // Unuseful Value (Debug Only)
    Msg.FgColor = 0XDD; // Unuseful Value (Debug Only)
    Msg.Sender = _DisplayID;

    Msg.Area = TFT_A_LT_ICON;
    Msg.Message[0] = TFT_ICON_RE_MA;
    _tft_push(&Msg);

    Msg.Area = TFT_A_LT_UNIT;
    Msg.Message[0] = _Unit;
    _tft_push(&Msg);

    Msg.Area = TFT_A_LB_ICON;
    Msg.Message[0] = TFT_ICON_RE_MI;
    _tft_push(&Msg);

    Msg.Area = TFT_A_LB_UNIT;
    Msg.Message[0] = _Unit;
    _tft_push(&Msg);
}

void reading::_RemoveIconsHold(uint32_t BgColor)
{
    tft_message Msg;
    Msg.BgColor = BgColor; // Unuseful Value (Debug Only)
    Msg.FgColor = 0XDD;    // Unuseful Value (Debug Only)
    Msg.Sender = _DisplayID;

    Msg.Area = TFT_A_LT_ICON;
    Msg.Message[0] = TFT_ICON_RE_NL;
    _tft_push(&Msg);

    Msg.Area = TFT_A_LT_UNIT;
    Msg.Message[0] = TFT_ICON_MU_NL;
    _tft_push(&Msg);

    Msg.Area = TFT_A_LB_ICON;
    Msg.Message[0] = TFT_ICON_RE_NL;
    _tft_push(&Msg);

    Msg.Area = TFT_A_LB_UNIT;
    Msg.Message[0] = TFT_ICON_MU_NL;
    _tft_push(&Msg);
}

uint8_t reading::getTftArea() { return (_tft_area); }

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

powertester::powertester(uint8_t i2c_address,
                         const char *Id,
                         uint8_t OutPin,
                         uint8_t *DT) : Adafruit_INA219(i2c_address),
                                        _DisplayID(i2c_address - INA219_I2C_OFFSET),
                                        _Address(i2c_address),
                                        _Active(false),
                                        _Output(false),
                                        _HoldingMode(false),
                                        _OutPin(OutPin),
                                        _DT(DT),
                                        _Readings({reading("Bus", TFT_ICON_RE_BU, TFT_ICON_MU_VL, _DisplayID),
                                                   reading("Shnt", TFT_ICON_RE_NL, TFT_ICON_MU_VL, _DisplayID),
                                                   reading("Load", TFT_ICON_RE_LD, TFT_ICON_MU_VL, _DisplayID),
                                                   reading("Curr", TFT_ICON_RE_CU, TFT_ICON_MU_AM, _DisplayID),
                                                   reading("Powr", TFT_ICON_RE_PW, TFT_ICON_MU_WT, _DisplayID)})
{
    // Setting Powertester Identificator
    strncpy(_Id, Id, 8);
}

bool powertester::setup(TFT_eSPI *tft, uint8_t SerialMode = D_HUMAN)
{
    // Initalizing INA219 chip
    if (!begin())
    {
        if (SerialMode >= D_MESSAGES)
            Serial.printf(B_BLU "* INA219" RESET ": Addr: 0x%X ......... [" N_RED "!!! Init failed !!!" RESET "]\n", _Address);
        _Active = false;
    }
    else
    {
        _Active = true;

        if (SerialMode >= D_MESSAGES)
            Serial.printf(B_BLU "* INA219" RESET ": Addr: 0x%X ......... [" N_GRE "Initialized" RESET "]\n", _Address);

        // Calibrating INA219 chip
        setCalibration_16V_400mA(); // 16V, 400mA range (higher precision on volts and amps):
        if (SerialMode >= D_MESSAGES)
            Serial.printf(B_BLU "* INA219" RESET ": Addr: 0x%X ......... [" N_GRE "Set: : 16V, 400mA range" RESET "]\n", _Address);
    }

    _SerialPrints = SerialMode;

    for (auto Curr_Reading = _Readings.begin(); Curr_Reading != _Readings.end(); ++Curr_Reading)
    {
        Curr_Reading->setSerialPrints(_SerialPrints);
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
        it->display(S, (_Output) ? TFT_ON_COLOR : TFT_OFF_COLOR);
        it->reset();
    }
    if (_SerialPrints >= D_MACHINE)
    {
        S->println("");
    }
}

bool powertester::setHoldingMode(bool HoldingMode)
{
    tft_message Message;
    Message.Sender = _DisplayID;
    Message.Area = TFT_A_R_BUTTN;
    Message.BgColor = 0;
    Message.FgColor = 0;
    Message.Message[0] = (HoldingMode) ? 1 : 0;
    _tft_push(&Message);

    _HoldingMode = HoldingMode;

    for (reading *it = _Readings.begin(); it != _Readings.end(); ++it)
    {
        it->setHoldingMode(HoldingMode, (_Output) ? TFT_ON_COLOR : TFT_OFF_COLOR);
    }

    return (HoldingMode);
}

void powertester::setFocus(uint8_t FR)
{
    int i = 0;

    _FocusedReading = FR;

    Serial.printf(TFT_COLOR "* TFT   " RESET ": SetFocus Init ...... [" B_GRE "(%s) FR: %u" RESET "]\r\n", _Id, FR);

    // Initialize Read Mode
    _ReadMask.reset();
    _ReadMask.set(FR);

    for (auto Curr_Reading = _Readings.begin(); Curr_Reading != _Readings.end(); ++Curr_Reading)
    {
        //Serial.printf(B_RED "* TFT   " RESET ": (%s) SetFocus ........... [" B_GRE "Focus: %u Area: %u" RESET "]\r\n",
        // _Id, (FR == i), _DT[(FR * TFT_CONFIGS) + i]);

        Curr_Reading->setFocus((FR == i), _DT[(FR * TFT_CONFIGS) + i]);
        i++;
    }
}

void powertester::updateFocus()
{
    uint8_t CurFocus = _FocusedReading + 1;

    setFocus((CurFocus == READINGS) ? IR_BUS : CurFocus);

    for (reading *it = _Readings.begin(); it != _Readings.end(); ++it)
    {
        if (it->getTftArea())
            it->ShowIcons((_Output) ? TFT_ON_COLOR : TFT_OFF_COLOR);
    }
}

bool powertester::setOutputMode(bool OutputMode)
{
    tft_message Message;

    _Output = OutputMode;

    // Background change
    Message.Sender = _DisplayID;
    Message.BgColor = 0;
    Message.FgColor = 0;
    Message.Message[0] = (OutputMode) ? 1 : 0;
    Message.Area = TFT_A_BACKGRD;
    _tft_push(&Message);

    // Relay Management
    if (OutputMode)
        RELAY_THROW(_OutPin);
    else
        RELAY_RELEASE(_OutPin);

    for (reading *it = _Readings.begin(); it != _Readings.end(); ++it)
    {
        if (it->getTftArea())
            it->ShowIcons((_Output) ? TFT_ON_COLOR : TFT_OFF_COLOR);
    }

    return (_Output);
}

bool powertester::getOutputMode()
{
    return (_Output);
}

bool powertester::getHoldingMode()
{
    return (_HoldingMode);
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

/**
 * @brief Enqueue a message 
 * 
 * @param[in] data Data to be used foe update
 */
static void _tft_push(tft_message *Message)
{
    BaseType_t xStatus;                                 // keep the status of sending data
    const TickType_t xTicksToWait = pdMS_TO_TICKS(100); //time to block the task until the queue has free space

    xStatus = xQueueSendToBack(MsgQueue, Message, xTicksToWait);
    if (xStatus != pdPASS)
    {
        Serial.println("[Error] ... Producer Sending Data");
    }
}
