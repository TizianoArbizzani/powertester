#ifndef PT_DEFINES_H
#define PT_DEFINES_H

#define ONBOARD_LED (2) //!<The OnBoard Esp32 DevKit-1 clone led pin
#define TFT_INTPIN (36) //!<TFT Panel touch panel interrupt pin

#define LEFT_INA_I2C (0x40) //!<Left INA219 I2C address
#define LEFT_INA_ID "Sx"    //!<Left INA219 identification label
#define LEFT_OFFSET 4       //!< X value for left PSU Data
#define LEFT_OUTPIN 33      //!< Left PSU Data output relay driving pin

#define RIGHT_INA_I2C (0x41) //!<Right INA219 I2C address
#define RIGHT_INA_ID "Dx"    //!<Right INA219 identification label
#define RIGHT_OFFSET 242     //!< X value for right PSU Data
#define RIGHT_OUTPIN 32      //!< Right PSU Data output relay driving pin

#define TFT_UPDATES_MS 500    //!<TFT Update frequency in milliseconds
#define TFT_TOUCH_MS 250      //!<Grace time between touch events
#define DBG_SWITCH_HOLD 30000 //!<TFT Update frequency in milliseconds

#define CALIBRATION_FILE "/calibrationData" //!<TFT calibration file

#define FRAME_COLOR TFT_BLUE

#endif