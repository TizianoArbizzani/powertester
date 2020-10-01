#ifndef powertester_h
#define powertester_h

#include "Arduino.h"
#include <Adafruit_INA219.h>

#include <array>
#include <bitset>

/** @name Substruct Size
 *  String size for PrintRead Struct
 */
///@{
#define INT_SIZE 3   //!<Size of PrintRead Structure Integer Subpart
#define FRACT_SIZE 4 //!<Size of PrintRead Structure Fractional Subpart
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
     * @brief  Reset reading to empty default
     * 
     */
    void reset();
    /**
     * @brief Save a reading to the structure
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
     * @remark A focused reading is displayed at the center of lcd and is under hispeed sampling
     * 
     * @param[in] Focus Is this field under focus
     * @return true Focus is now set
     * @return false Focus in now unset
     */
    bool setFocus(bool Focus);

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

    bool _focus; //!< This is the current focused reading
};

/** @name Readings
 *  Which data must be read from INA219.
 * @attention Setting the type of request reading, e.g. using powertester::SetReading() function, RS(x) macro must be used ... 
 */
///@{
#define IR_SHNT 0 //!<Read Shunt Voltage
#define IR_BUS 1  //!<Read Bus Voltage
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
    powertester(uint8_t i2c_address, const char *Id);
    /**
     * @brief Initialize INA219 chip
     * 
     * @attention Double check I2C connections
     * 
     * @internal Reserved data
     * 
     * @return true Init Ok
     * @return false Init failed (i2c bus problems?)
     */
    bool setup();
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
    void SetReading(int Bitmap);
    /**
     * @brief Read the requested data
     * 
     * @param[in] S The Stream in use for the output
     */
    void display(Stream *S);
    /**
     * @brief 
     * 
     */

private:
    char _Id[8];                      //!< INA219 Chip Label
    int _Address;                     //!< INA219 I2C address (default 0x40)
    bool _Active;                     //!< INA219 is active (I2C Reachable??)
    std::bitset<32> _ReadMask;        //!< Which INA219 field must be read at max speed
    std::array<reading, 5> _Readings; //!< INA219 Reading fields
};

#endif
