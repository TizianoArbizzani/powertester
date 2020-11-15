#ifndef tftmanager_h
#define tftmanager_h

#include <Arduino.h>

/** @name Icons
 *  Icons used
 *  
 */
///@{
//#include "icons/top/Hold.h"
//#include "icons/top/NoHold.h"
//#include "icons/top/CutOff.h"
//#include "icons/top/Relay.h"
#include "icons/readings/Bus.h"
#include "icons/readings/Load.h"
#include "icons/readings/Power.h"
#include "icons/readings/Current.h"
#include "icons/readings/MinHold.h"
#include "icons/readings/MaxHold.h"
#include "icons/units/A.h"
#include "icons/units/V.h"
#include "icons/units/W.h"
///@}

/** @name Fonts
 *  Fonts in use
 *  
 */
///@{
#include "fonts/frutiger.h"
///@}

#define FRAME_COLOR TFT_BLUE

#define RIGHT_OFFSET 242 //!< X value for right PSU Data
#define LEFT_OFFSET 4    //!< X value for left PSU Data

#define READ_ICON_AREA (_tft_area + 1) //!< Reading Icon Area
#define UNIT_ICON_AREA (_tft_area + 2) //!< Measurement Unit Icon Area

/** @name TFT Settings
 *  Settings Related to TFT Panel
 *  
 */
///@{
#define TFT_READ_ICONSIZE 0x20        //!< Reading Icon Size
#define TFT_UNIT_ICONSIZE 0x18        //!< Measurement Unit Icon Size
#define TFT_ON_COLOR 0x37E6           //!< (RGB565) Active Output window background
#define TFT_OFF_COLOR 0xFD70          //!< (RGB565) CutOff Output window background
#define TFT_POS_COLOR TFT_BLACK       //!< Positive (or zero) reading background color
#define TFT_NEG_COLOR TFT_RED         //!< Negative reading background color
#define TFT_SPR_ADD_OFF 38            //!< Reading first character indentation from frame
#define TFT_SPR_SML_F FruBoldNarrow34 //!< Small (unfocused) reading font
#define TFT_SPR_SML_W 120             //!< Small (unfocused) sprite width
#define TFT_SPR_SML_H 33              //!< Small (unfocused) sprite heigth
#define TFT_SPR_BIG_F FruBoldNarrow54 //!< Big (focused) reading font
#define TFT_SPR_BIG_W 165             //!< Big (focused) sprite width
#define TFT_SPR_BIG_H 48              //!< Big (focused) sprite heigth
#define TFT_SPR_MEA_F FruBoldNarrow34 //!< Measurement units reading font
#define TFT_SPR_MEA_W 32              //!< Measurement units sprite width
#define TFT_SPR_MEA_H 33              //!< Measurement units sprite heigth
#define TFT_SPR_MEA_X_DISP 205        //!< Measurement units displacement from left frame
#define TFT_SPR_MEA_Y_DISP_BIG 8      //!< Measurement units vertical displacement (32x32)
#define TFT_SPR_UNI_Y_DISP_BIG 11     //!< Reading icon vertical displacement (24x24)
#define TFT_SPR_BAR_WIDTH 190         //!< Value bar width
#define TFT_YN 0                      //!< First Line y
#define TFT_Y1 53                     //!< First Line y
#define TFT_Y2 84                     //!< Second Line y
#define TFT_Y3 115                    //!< Third Line y
#define TFT_YF 193                    //!< Focused Line y
#define TFT_DS 44                     //!< Focused Line y displacement
#define TFT_ATZERO 0, 0
///@}

/** @name TFT Settings
 *  Settings Related to TFT Panel
 *  
 */
///@{
#define MSGSIZE 16     //!< InterProcess Message Size
#define ASCIIZERO 48   //!<
#define TFT_SHOW true  //!<
#define TFT_HIDE false //!<
///@}

/** @name TFT Settings
 *  Settings Related to TFT Panel
 *  
 */
///@{
#define TFT_PROTO_ID 0   //!<
#define TFT_PROTO_AREA 1 //!<
#define TFT_PROTO_SEP 2  //!<
#define TFT_PROTO_DATA 3 //!<
///@}

/** @name TFT Settings
 *  Settings Related to TFT Panel
 *  
 */
///@{
#define TFT_QUEUESIZE 128                 //!<
#define TFT_QUEUE_LIMIT TFT_QUEUESIZE - 5 //!<
#define TFT_QUEUE_MONITOR_MS 5000         //!<
///@}

typedef struct
{
    uint16_t Sender;
    uint16_t Area;
    uint16_t BgColor;
    uint16_t FgColor;
    char Message[16];
} tft_message;

/** @name TFT Areas
 *  Settings Related to TFT Panel
 *  
 */
///@{
#define TFT_A_UNDEFIN 0  //!<
#define TFT_A_L_BUTTN 1  //!<
#define TFT_A_R_BUTTN 2  //!<
#define TFT_A_BACKGRD 3  //!<
#define TFT_A_L1_DATA 4  //!<
#define TFT_A_L1_ICON 5  //!<
#define TFT_A_L1_UNIT 6  //!<
#define TFT_A_L2_DATA 7  //!<
#define TFT_A_L2_ICON 8  //!<
#define TFT_A_L2_UNIT 9  //!<
#define TFT_A_L3_DATA 10 //!<
#define TFT_A_L3_ICON 11 //!<
#define TFT_A_L3_UNIT 12 //!<
#define TFT_A_LT_DATA 13 //!<
#define TFT_A_LT_ICON 14 //!<
#define TFT_A_LT_UNIT 15 //!<
#define TFT_A_LF_DATA 16 //!<
#define TFT_A_LF_ICON 17 //!<
#define TFT_A_LF_UNIT 18 //!<
#define TFT_A_LB_DATA 19 //!<
#define TFT_A_LB_ICON 20 //!<
#define TFT_A_LB_UNIT 21 //!<
#define TFT_A_DATABAR 22 //!<
///@}

#define TFT_ICON_RE_NL 0 //!< Reading NULL Icon
#define TFT_ICON_RE_BU 1 //!<
#define TFT_ICON_RE_LD 2 //!<
#define TFT_ICON_RE_PW 3 //!<
#define TFT_ICON_RE_CU 4 //!<
#define TFT_ICON_RE_MI 5 //!<
#define TFT_ICON_RE_MA 6 //!<
#define TFT_ICON_MU_NL 0 //!< Measure Unit NULL Icon
#define TFT_ICON_MU_AM 7 //!< Measure Unit AMPERE Icon
#define TFT_ICON_MU_VL 8 //!< Measure Unit VOLT Icon
#define TFT_ICON_MU_WT 9 //!< Measure Unit WATT Icon

/**
 * @brief Initialize TFT Manager Thread
 * 
 * @param[in] PrintMode Serial debug level
 */
void tft_manager_init(uint8_t PrintMode);

/**
 * @brief Consumes task queue 
 * 
 * @param[in] parameter Function Parameter (Unuseful at the moment)
 */
void tft_consumer(void *parameter);

#endif