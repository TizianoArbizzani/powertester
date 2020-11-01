#ifndef powertester_h
#define powertester_h

#include "Arduino.h"
#include <Adafruit_INA219.h>
#include <TFT_eSPI.h>

#include <array>
#include <bitset>

/** @name Icons
 *  Icons used
 *  
 */
///@{
#include "icons/top/Hold.h"
#include "icons/top/NoHold.h"
#include "icons/top/CutOff.h"
#include "icons/top/Relay.h"
#include "icons/readings/Bus.h"
#include "icons/readings/Load.h"
#include "icons/readings/Power.h"
#include "icons/readings/Current_Min.h"
#include "icons/readings/Current.h"
#include "icons/readings/Current_Max.h"

///@}

/** @name Fonts
 *  Fonts in use
 *  
 */
///@{
#include "fonts/frutiger.h"
///@}

/** @name Colors
 *  ANSI colors for terminals
 *  
 */
///@{
#define N_BLA "\033[1;30m" //!<Normal Black
#define N_RED "\033[1;31m" //!<Normal Red
#define N_GRE "\033[1;32m" //!<Normal Green
#define N_YEL "\033[1;33m" //!<Normal Yellow
#define N_BLU "\033[1;34m" //!<Normal Blue
#define N_MAG "\033[1;35m" //!<Normal Magenta
#define N_CYA "\033[1;36m" //!<Normal Cyan
#define N_WHI "\033[1;37m" //!<Normal White
#define B_BLA "\033[1;90m" //!<Bright Black
#define B_RED "\033[1;91m" //!<Bright Red
#define B_GRE "\033[1;92m" //!<Bright Green
#define B_YEL "\033[1;93m" //!<Bright Yellow
#define B_BLU "\033[1;94m" //!<Bright Blue
#define B_MAG "\033[1;95m" //!<Bright Magenta
#define B_CYA "\033[1;96m" //!<Bright Cyan
#define B_WHI "\033[1;97m" //!<Bright White
#define RESET "\033[0m"    //!<Reset to standard values
///@}

/** @name Substruct Size
 *  String size for PrintRead Struct
 */
///@{
#define INT_SIZE 3   //!<Size of PrintRead Structure Integer Subpart
#define FRACT_SIZE 4 //!<Size of PrintRead Structure Fractional Subpart
///@}

/** @name Hold Modes
 *  Hold MIN/MAX values modes
 */
///@{
#define NO_HOLD_MODE false //!<Reset MIN/MAX at every display
#define HOLD_MODE true     //!<Retain MIN/MAX values
///@}

/** @struct PrintRead
   */
typedef struct
{
    char Sign;              //!< The sign of the reading
    char Int[INT_SIZE];     //!< The integer part of the reading
    char Fract[FRACT_SIZE]; //!< The fractional part of the reading
} PrintRead;

/** @name Macros
 *  Useful macro defnition
 */
///@{
#define RS(X) (1 << (X))
///@}

/** @name TFT Settings
 *  Settings Related to TFT Panel
 *  
 */
///@{

#define TFT_ON_COLOR 0x37E6  //!< (RGB565) Active Output window background
#define TFT_OFF_COLOR 0xFD70 //!< (RGB565) CutOff Output window background

#define TFT_POS_COLOR TFT_BLACK //!< Positive (or zero) reading background color
#define TFT_NEG_COLOR TFT_RED   //!< Negative reading background color

#define TFT_SPR_ADD_OFF 38 //!< Reading first character indentation from frame

#define TFT_SPR_SML_F FruBoldNarrow34 //!< Small (unfocused) reading font
#define TFT_SPR_SML_W 120             //!< Small (unfocused) sprite width
#define TFT_SPR_SML_H 33              //!< Small (unfocused) sprite heigth

#define TFT_SPR_BIG_F FruBoldNarrow54 //!< Big (focused) reading font
#define TFT_SPR_BIG_W 165             //!< Big (focused) sprite width
#define TFT_SPR_BIG_H 48              //!< Big (focused) sprite heigth

#define TFT_SPR_MEA_F FruBoldNarrow34 //!< Measurement units reading font
#define TFT_SPR_MEA_W 32              //!< Measurement units sprite width
#define TFT_SPR_MEA_H 33              //!< Measurement units sprite heigth
#define TFT_SPR_MEA_X_DISP 200        //!< Measurement units displacement from left frame
#define TFT_SPR_MEA_X_DISP_BIG 7      //!< Measurement units

#define TFT_YN -1       //!< First Line y
#define TFT_Y1 53       //!< First Line y
#define TFT_Y2 84       //!< Second Line y
#define TFT_Y3 115      //!< Third Line y
#define TFT_FC 193      //!< Focused Line y
#define TFT_FL_DISPL 44 //!< Fourth Line y displacement
#define TFT_ATZERO 0, 0

///@}
/*! 
 *  @brief     Readings class.
 *  @details   A single INA219 Reading
 *  @author    Tiziano Arbizzani <tiziano.arbizzani@gmail.com>
 *  @version   0.0.1a
 *  @date      2020
 *  @copyright GNU Public License.
 *  @remark    Further documentation.
 */
class reading
{
public:
    /**
     * @brief Construct a new reading object
     * 
     * @param[in] Label Data to be read
     * @param[in] Unit Data measurement unit
     * @param[in] Y Data measurement unit
     */
    reading(const char *Label, const char *Unit, int Y);
    /**
     * @brief  Reset reading to default (empty) values.
     * 
     */
    void reset();
    /**
     * @brief Executes a reading and save it to object properties.
     * 
     * @param[in] ReadData Data read to be saved
     */
    void set(float ReadData);
    /**
     * @brief Show single data reading 
     * 
     * @param[in] S Stream accepting data
     * @param[in] Xoff Offset in LCD Display 
     * @param[in] BgColor BackGround Color (output cutoff or active)
     */
    void display(Stream *S, int Xoff, int BgColor);
    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    bool hasFocus();
    /**
     * @brief Set the Focus to this reading.
     * 
     * @param[in] SerialMode Serial Printout Mode (No print, machine parsed, human readable)
     */
    void setSerialPrints(uint8_t SerialMode);
    /**
     * @brief Set the Focus to this reading.
     * 
     * @remark A focused reading is displayed at the center of lcd and is under hispeed sampling
     * 
     * @param[in] SerialMode Serial Mode
     */
    bool setFocus(bool Focus);
    /**
     * @brief Set Holding Mode for current Reading
     * 
     * @param[in] HoldingMode Holding Mode (Min/Max must be reset or not ...)
     * 
     * @return bool Current (set) Holding Mode 
     */
    bool setHoldingMode(bool HoldingMode);
    /**
     * @brief Set Holding Mode for current Reading
     * 
     * @return bool Current Holding Mode 
     */
    bool getHoldingMode();
    /**
     * @brief Get current Reading Measurement Unit
     * 
     * @return char* Current Measurement Unit
     */
    char *getMeasurementUnit();
    /**
     * @brief Get current Reading y tft print line
     * 
     * @return bool Current Holding Mode y tft print line
     */
    uint16_t gety();
    /**
     * @brief Set the TFT Panel for output
     * 
     * @remark Set the TFT Panel used for data output
     * 
     * @param[in] TFT_eSPI object referencing TFT Panel
     * @param[in] TFT_eSprite object referencing TFT Sprite
     */
    void setTFT(TFT_eSPI *tft, TFT_eSprite *spr);

private:
    /**
     * @brief 
     * 
     * @return int 
     */
    int _get_min();
    /**
     * @brief 
     * 
     * @return int 
     */
    int _get_max();
    /**
     * @brief 
     * 
     * @return int 
     */
    int _get_mean();
    /**
     * @brief 
     * 
     * @return int 
     */
    int _get_reads();

    char _Unit[8] = {0};  ///!< Measurement unit
    char _Label[4] = {0}; ///!< Data to be read

    int _IR_min;   //!< Minimum value read
    int _IR_pile;  //!< Values pile
    int _IR_max;   //!< Maximum value read
    int _IR_reads; //!< Number of readings

    uint8_t _SerialPrints; //!< Serial Printout Mode (No print, machine parsed, human readable)
    bool _focus;           //!< This is the current focused reading
    bool _hold;            //!< Hold mode (retain values)
    uint16_t _y;           //!< TFT y display coordinate
    TFT_eSPI *_tft;        //!< TFT Display
    TFT_eSprite *_spr;     //!< TFT Display Sprite
    uint16_t _tft_width;   //!< TFT Width (x)
    uint16_t _tft_height;  //!< TFT Width (y)
};

/** @name Readings
 *  Which data must be read from INA219.
 * @attention Setting the type of request reading, e.g. using powertester::setReading() function, RS(x) macro must be used ... 
 */
///@{
#define IR_BUS 0  //!<Read Bus Voltage
#define IR_SHNT 1 //!<Read Shunt Voltage
#define IR_LOAD 2 //!<Read Load Voltage
#define IR_CURR 3 //!<Read Load Current
#define IR_PWR 4  //!<Read Load Power
///@}

/** @name Reading Modes
 *  What kind of update is to be done.
 * 
 *  To speed up things ...
 */
///@{
#define IM_RECURR 0 //!<Read everytime you can
#define IM_SPARSE 1 //!<Read at display refresh only
///@}

/** @name Display Modes
 *  What kind of display is to be done.
 * 
 *  To speed up things ...
 */
///@{
#define D_OFF 0     //!<Serial output disabled
#define D_MACHINE 1 //!<Serial output is intended to be parsed by an automatic parser (machine)
#define D_REDUCED 2 //!<Serial will output is intended to be read from an human operator (colors, readable layout)
#define D_HUMAN 3   //!<Serial will output is intended to be read from an human operator (colors, readable layout)
///@}

/** @name Reading Defaults
 *  Default values for Reading fields
 */
///@{
#define R_DEF_READS 0      //!<Default Read Number
#define R_DEF_MIN 1000000  //!<Default MINIMUM Read
#define R_DEF_PILE 0       //!<Default PILE Value
#define R_DEF_MAX -1000000 //!<Default MINIMUM Read
#define READ_COEFF 1000    //!<Use three decimals
///@}

/** @name Output Modes
 *  Output states.
 * 
 *  Output can be cut off by relay
 */
///@{
#define OUTPUT_CUTOFF 0 //!<PSU output disabled
#define OUTPUT_ACTIVE 1 //!<PSU output enabled
///@}

/*! 
 *  @brief     Main PowerTester class,
 *  @details   This class is used to demonstrate a number of section commands.
 *  @author    Tiziano Arbizzani <tiziano.arbizzani@gmail.com>
 *  @version   0.0.1a
 *  @date      2020
 *  @copyright GNU Public License.
 *  @see       Further documentation.
 */
class powertester : public Adafruit_INA219
{
public:
    /**
     * @brief Construct a new powertester object
     * 
     * @param[in] i2c_address I2C Address of INA219 chip
     * @param[in] Id INA219 chip Textual label (debug purposes)
     */
    powertester(uint8_t i2c_address, const char *Id, int Xoff, uint8_t OutPin);
    /**
     * @brief Initialize INA219 chip
     * 
     * @attention Double check I2C connections
     * 
     * @internal Reserved data
     * 
     * @param[in] TFT_eSPI Output TFT Panel Reference 
     * @param[in] SerialMode SerialMode Serial Mode
     * @param[in] Bitmap The type of requested reading (bus voltage, shunt voltage, load current, load power)
     * 
     * @return true Init Ok
     * @return false Init failed (i2c bus problems?)
     */
    bool setup(TFT_eSPI *tft, uint8_t SerialMode, int Bitmap);
    /**
     * @brief Complete Powertest Object settings
     *  
     * @param[in] Rm High Speed Reading or read on refresh only
     * 
     */
    void update(uint16_t Rm);
    /**
     * @brief Set the Reading object
     * 
     * @param[in] Bitmap The type of requested reading (bus voltage, shunt voltage, load current, load power)
     * @attention Setting the type of request reading (using Readings definitions), RS(x) macro must be used ... 
     */
    void setReading(int Bitmap);
    /**
     * @brief Print the requested Reading
     * 
     * @param[in] S The Stream in use for the output
     */
    void display(Stream *S);
    /**
     * @brief Set Holding Mode for current Reading
     * 
     * @param[in] HoldingMode Holding Mode (Min/Max must be reset or not ...)
     * 
     * @return bool Current (set) Holding Mode 
     */
    bool setHoldingMode(bool HoldingMode);
    /**
     * @brief Activats/Deactivates output line
     * 
     * @param[in] OutputMode Current must be connected to output "bocchettone"
     * 
     * @return bool Current Output Mode 
     */
    bool setOutputMode(bool OutputMode);

private:
    char _Id[8];                      //!< INA219 Chip Label
    int _Address;                     //!< INA219 I2C address (default 0x40)
    bool _Active;                     //!< INA219 is active (I2C Reachable??)
    int _Xoffset;                     //!< TFT Horizontal Offset for PSU
    bool _Output;                     //!< Output is connected ...
    uint8_t _OutPin;                  //!< The pin driving output relay
    std::bitset<32> _ReadMask;        //!< Which INA219 field must be read at max speed
    uint8_t _SerialPrints;            //!< Serial Printout Mode (No print, machine parsed, human readable)
    TFT_eSPI *_tft;                   //!< TFT Display
    TFT_eSprite *_spr_big;            //!< TFT Display Sprite (LARGE FONT)
    TFT_eSprite *_spr_small;          //!< TFT Display Sprite (SMALL FONT)
    TFT_eSprite *_spr_measunit;       //!< TFT Display Sprite (MEASUREMENT UNIY)
    uint16_t _tft_width;              //!< TFT Width (x)
    uint16_t _tft_height;             //!< TFT Width (y)
    std::array<reading, 5> _Readings; //!< INA219 Reading fields
};

#endif
