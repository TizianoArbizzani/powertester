#ifndef powertester_h
#define powertester_h

#include "Arduino.h"
#include <Adafruit_INA219.h>
#include <TFT_eSPI.h>

#include <tftmanager.h>

#include <array>
#include <bitset>

#define READINGS 5 //!<Number of readings for chip

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

/** @name Terminal Debug Colors
 *  Colors used for debugging lines differentiation
 *  
 */
///@{
#define MAN_COLOR B_GRE
#define TFT_COLOR B_YEL
#define TCH_COLOR B_MAG
#define ACT_COLOR B_RED
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
#define RELAY_THROW(P) digitalWrite((P), LOW)    //!<Relay Throw
#define RELAY_RELEASE(P) digitalWrite((P), HIGH) //!<Relay Release
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
     * @param[in] MP Data must be printed?
     * @param[in] RTOS Queue
     */
    reading(const char *label, uint8_t icon, uint8_t unit, uint8_t testerId);
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
     * @param[in] BgColor BackGround Color (output cutoff or active)
     */
    void display(Stream *S, int BgColor);
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
    bool setFocus(bool Focus, uint8_t DisplayArea);
    /**
     * @brief Set Holding Mode for current Reading
     * 
     * @param[in] HoldingMode Holding Mode (Min/Max must be reset or not ...)
     * @param[in] BgColor Background color
     * @param[in] Xoff X Offset
     * 
     * @return bool Current (set) Holding Mode 
     */
    bool setHoldingMode(bool HoldingMode, uint32_t BgColor);
    /**
     * @brief Get Holding Mode for current Reading
     * 
     * @return bool Current Holding Mode 
     */
    bool getHoldingMode();
    /**
     * @brief Display TFT icons
     * 
     * @remark A focused reading is displayed at the center of lcd and is under hispeed sampling
     */
    void ShowIcons(uint32_t BgColor);
    /**
     * @brief Get Reading Tft Area
     * 
     * @return uint8_t Tft Area tied to Reading
     */
    uint8_t getTftArea();

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
    /**
     * @brief Display TFT icons (Hold Mode Only)
     * 
     * @remark A focused reading is displayed at the center of lcd and is under hispeed sampling
     */
    void _ShowIconsHold(uint32_t BgColor);
    /**
     * @brief Display TFT icons (Hold Mode Only)
     * 
     * @remark A focused reading is displayed at the center of lcd and is under hispeed sampling
     */
    void _RemoveIconsHold(uint32_t BgColor);

    char _Label[8] = {0};     //!< Measurement unit
    uint8_t _Icon;            //!< Reading icon
    uint8_t _Unit;            //!< Measurement unit icon
    unsigned char _DisplayID; //!< TFT Display ID

    int _IR_min;   //!< Minimum value read
    int _IR_pile;  //!< Values pile
    int _IR_max;   //!< Maximum value read
    int _IR_reads; //!< Number of readings

    uint8_t _SerialPrints; //!< Serial Printout Mode (No print, machine parsed, human readable)
    bool _focus;           //!< This is the current focused reading
    bool _hold;            //!< Hold mode (retain values)
    uint16_t _tft_area;    //!< TFT display area
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
#define D_OFF 0      //!<Serial output disabled
#define D_MESSAGES 1 //!<Serial output is intended to be parsed by an automatic parser (machine)
#define D_MACHINE 2  //!<Serial output is intended to be parsed by an automatic parser (machine)
#define D_REDUCED 3  //!<Serial will output is intended to be read from an human operator (colors, readable layout)
#define D_HUMAN 4    //!<Serial will output is intended to be read from an human operator (colors, readable layout)
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
#define OUTPUT_CUTOFF false //!<PSU output disabled
#define OUTPUT_ACTIVE true  //!<PSU output enabled
///@}

#define INA219_I2C_OFFSET 0X40

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
    powertester(uint8_t i2c_address, const char *Id, uint8_t OutPin, uint8_t *DT);
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
    bool setup(TFT_eSPI *tft, uint8_t SerialMode);
    /**
     * @brief Complete Powertest Object settings
     *  
     * @param[in] Rm High Speed Reading or read on refresh only
     * 
     */
    void update(uint16_t Rm);
    /**
     * @brief Print the requested Reading
     * 
     * @param[in] S The Stream in use for the output
     */
    void display(Stream *S);
    /**
     * @brief Get Holding Mode
     * 
     * @return bool Current Holding Mode 
     */
    bool getHoldingMode();
    /**
     * @brief Set Holding Mode for current Reading
     * 
     * @param[in] HoldingMode Holding Mode (Min/Max must be reset or not ...)
     * 
     * @return bool Current (set) Holding Mode 
     */
    bool setHoldingMode(bool HoldingMode);
    /**
     * @brief Set the focused Reading object
     * 
     * @param[in] Bitmap The type of requested reading (bus voltage, shunt voltage, load current, load power)
     * @attention Setting the type of request reading (using Readings definitions), RS(x) macro must be used ... 
     */
    void setFocus(uint8_t FR);
    /**
     * @brief Increase the focused Reading object
     * 
     * @attention Setting the type of request reading (using Readings definitions), RS(x) macro must be used ... 
     */
    void updateFocus();
    /**
     * @brief Activats/Deactivates output line
     * 
     * @param[in] OutputMode Current must be connected to output "bocchettone"
     * 
     * @return bool Current Output Mode 
     */
    bool setOutputMode(bool OutputMode);
    /**
     * @brief Get output line status
     * 
     * @return bool Current Output Mode 
     */
    bool getOutputMode();

private:
    char _Id[8];                             //!< INA219 Chip Label
    unsigned char _DisplayID;                //!< TFT Display ID
    int _Address;                            //!< INA219 I2C address (default 0x40)
    bool _Active;                            //!< INA219 is active (I2C Reachable??)
    bool _Output;                            //!< Output is connected ...
    bool _HoldingMode;                       //!< Holding mode
    uint8_t _OutPin;                         //!< The pin driving output relay
    uint8_t _FocusedReading;                 //!< Which INA219 field must be read at max speed
    uint8_t _SerialPrints;                   //!< Serial Printout Mode (No print, machine parsed, human readable)
    uint8_t *_DT;                            //!< Display table
    std::array<reading, READINGS> _Readings; //!< INA219 Reading fields
    std::bitset<32> _ReadMask;               //!< Which INA219 field must be read at max speed
};

#endif
