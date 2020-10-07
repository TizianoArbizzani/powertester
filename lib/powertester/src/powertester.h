#ifndef powertester_h
#define powertester_h

#include "Arduino.h"
#include <Adafruit_INA219.h>

#include <array>
#include <bitset>

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
#define RS(X) (1 << X) //!< Convert from position to value.
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
     */
    reading(const char *Label = "Unk", const char *Unit = "V");
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
     */
    void display(Stream *S, int Xoff);
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
#define D_HUMAN 2   //!<Serial will output is intended to be read from an human operator (colors, readable layout)
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
    powertester(uint8_t i2c_address, const char *Id, int Xoff);
    /**
     * @brief Initialize INA219 chip
     * 
     * @attention Double check I2C connections
     * 
     * @internal Reserved data
     * 
     * @param[in] SerialMode SerialMode Serial Mode
     * 
     * @return true Init Ok
     * @return false Init failed (i2c bus problems?)
     */
    bool setup(uint8_t SerialMode);
    /**
     * @brief Read a new dataset
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

private:
    char _Id[8];                      //!< INA219 Chip Label
    int _Address;                     //!< INA219 I2C address (default 0x40)
    bool _Active;                     //!< INA219 is active (I2C Reachable??)
    int _Xoffset;                     //!< TFT Horizontal Offset for PSU
    uint8_t _SerialPrints;            //!< Serial Printout Mode (No print, machine parsed, human readable)
    std::bitset<32> _ReadMask;        //!< Which INA219 field must be read at max speed
    std::array<reading, 5> _Readings; //!< INA219 Reading fields
};

#endif
