#include "Arduino.h"
#include <TFT_eSPI.h>
#include <powertester.h>

extern xQueueHandle MsgQueue;
extern TFT_eSPI tft;

TaskHandle_t ConsumerTask; //!< Task Handle

/**
 * @brief Consumes task queue 
 * 
 * @param[in] sender Tester Identification code
 */
void tft_manager_init(uint8_t PrintMode)
{

    if (PrintMode >= D_MESSAGES)
        Serial.printf(MAN_COLOR "* SPIFFS" RESET ": Initialization ..... [" B_GRE "Done" RESET "]\r\n");

    MsgQueue = xQueueCreate(TFT_QUEUESIZE, sizeof(tft_message));

    if (PrintMode >= D_MESSAGES)
        Serial.printf(MAN_COLOR "* TFT   " RESET ": Messages queue ..... [" B_GRE "Built" RESET "]\r\n");

    xTaskCreatePinnedToCore(tft_consumer, "TFT_Consumer", 50000, NULL, 1, &ConsumerTask, 1);
}

/**
 * @brief Consumes task queue 
 * 
 * @param[in] sender Tester Identification code
 */
void tft_consumer(void *parameter)
{
    BaseType_t xStatus;                                          //!< keep the status of sending data
    UBaseType_t MaxQueue = 0;                                    //!< Max number of elements in queue
    UBaseType_t CurrQueue = 0;                                   //!< Current number of elements in queue
    const TickType_t xTicksToWait = pdMS_TO_TICKS(100);          //!<time to block the task until the queue has free space
    tft_message Message;                                         //!< Message READ from queue
    unsigned long DisplayTime = millis() + TFT_QUEUE_MONITOR_MS; //!< (DEBUG) Interval printing queue size

    Serial.printf(TFT_COLOR "* TFT   " RESET ": Manager thread ..... [" B_GRE "Start" RESET "]\r\n");

    TFT_eSprite *_spr_big = new TFT_eSprite(&tft);         //!< TFT Display Sprite (LARGE FONT)
    TFT_eSprite *_spr_small = new TFT_eSprite(&tft);       //!< TFT Display Sprite (SMALL FONT)
    TFT_eSprite *_spr_leftbutton = new TFT_eSprite(&tft);  //!< TFT Display Sprite (LEFT BUTTON - RELAY)
    TFT_eSprite *_spr_rightbutton = new TFT_eSprite(&tft); //!< TFT Display Sprite (RIGHT BUTTON - HOLD)
    TFT_eSprite *_spr_databar = new TFT_eSprite(&tft);     //!< TFT Display Sprite (DATABAR)

    Serial.printf(TFT_COLOR "* TFT   " RESET ": Sprites ............ [" B_GRE "Created" RESET "]\r\n");

    tft.fillRect(0, 0, tft.width(), 4, FRAME_COLOR);                //!< Top
    tft.fillRect(0, tft.height() - 4, tft.width(), 4, FRAME_COLOR); //!< Bottom
    tft.fillRect(0, 0, 4, tft.width(), FRAME_COLOR);                //!< Left
    tft.fillRect(tft.width() - 4, 0, 4, tft.height(), FRAME_COLOR); //!< Right
    tft.fillRect(234 + 4, 0, 4, tft.height(), FRAME_COLOR);         //!< Center

    Serial.printf(TFT_COLOR "* TFT   " RESET ": Bounding boxes ..... [" B_GRE "Created" RESET "]\r\n");

    _spr_small->createSprite(TFT_SPR_SML_W, TFT_SPR_SML_H);
    _spr_small->setColorDepth(16);
    _spr_small->loadFont(TFT_SPR_SML_F);
    _spr_small->setTextDatum(TL_DATUM);

    _spr_big->createSprite(TFT_SPR_BIG_W, TFT_SPR_BIG_H);
    _spr_big->setColorDepth(16);
    _spr_big->loadFont(TFT_SPR_BIG_F);
    _spr_big->setTextDatum(TL_DATUM);

    _spr_leftbutton->setColorDepth(16);
    _spr_leftbutton->createSprite(116, 45);
    _spr_leftbutton->loadFont(TFT_SPR_SML_F);
    _spr_leftbutton->setTextDatum(TL_DATUM);
    _spr_leftbutton->fillSprite(TFT_BLACK);

    _spr_rightbutton->setColorDepth(16);
    _spr_rightbutton->createSprite(116, 45);
    _spr_rightbutton->loadFont(TFT_SPR_SML_F);
    _spr_rightbutton->setTextDatum(TL_DATUM);
    _spr_rightbutton->fillSprite(TFT_BLACK);

    _spr_databar->setColorDepth(16);
    _spr_databar->createSprite(233, 30);
    _spr_databar->loadFont(TFT_SPR_SML_F);
    _spr_databar->setTextDatum(TR_DATUM);
    _spr_databar->fillSprite(TFT_BLACK);
    _spr_databar->setTextColor(TFT_WHITE, TFT_BLACK);

    Serial.printf(TFT_COLOR "* TFT   " RESET ": Sprites ............ [" B_GRE "Defaults" RESET "]\r\n");
    Serial.printf(TFT_COLOR "* TFT   " RESET ": Main loop .......... [" B_GRE "Starting" RESET "]\r\n");

    for (;;)
    {
        // Queue size analysis
        CurrQueue = uxQueueMessagesWaiting(MsgQueue);
        MaxQueue = (CurrQueue > MaxQueue) ? CurrQueue : MaxQueue; //!< Checking for MaxQueue

        // Queue limit warning
        if (MaxQueue > TFT_QUEUE_LIMIT)
        {
            Serial.printf(B_RED "* TFT   " RESET ": Queue limit ........ [" B_GRE "%d" RESET "]\r\n", MaxQueue);
            MaxQueue = 0;
        }

        // Queue debug print
        if (millis() > DisplayTime)
        {
            DisplayTime = millis() + TFT_QUEUE_MONITOR_MS;
            Serial.printf(TFT_COLOR "* TFT   " RESET ": Queue Reached Max .. [" B_GRE "%d" RESET "]\r\n", MaxQueue);
            MaxQueue = 0;
        }

        // Queue Management
        xStatus = xQueueReceive(MsgQueue, &Message, xTicksToWait);
        if (xStatus == pdPASS)
        {
            //Serial.printf(TFT_COLOR "* TFT   " RESET ": Message ............ [" B_GRE "S:%u A:%02u C:%04X:%04X M0:%u" RESET "]\r\n",
            //             Message.Sender, Message.Area, Message.BgColor, Message.FgColor, Message.Message[0]);

            // General Variables
            uint16_t Xoff = (Message.Sender) ? RIGHT_OFFSET : LEFT_OFFSET;
            uint16_t Yoff = 0;
            int32_t IconSize = 0;

            // Sprite
            TFT_eSprite *CurrentSprite = NULL;

            // Icon variables
            uint16_t Xmargin = 0;

            switch (Message.Area)
            {
            case TFT_A_L_BUTTN:
            {
                Serial.printf(TFT_COLOR "* TFT   " RESET ": Message ............ [" B_GRE "S:%u LEFT C:%04X:%04X M0:%u" RESET "]\r\n",
                              Message.Sender, Message.BgColor, Message.FgColor, Message.Message[0]);

                if (Message.Message[0])
                {
                    _spr_leftbutton->setTextColor(TFT_BLACK, TFT_GREEN);
                    _spr_leftbutton->fillRoundRect(0, 0, 116, 45, 8, TFT_DARKGREEN);
                    _spr_leftbutton->fillRoundRect(4, 4, 108, 37, 8, TFT_GREEN);
                    _spr_leftbutton->drawString("On", 10, 7);
                }
                else
                {
                    _spr_leftbutton->setTextColor(TFT_BLACK, TFT_RED);
                    _spr_leftbutton->fillRoundRect(0, 0, 116, 45, 8, TFT_MAROON);
                    _spr_leftbutton->fillRoundRect(4, 4, 108, 37, 8, TFT_RED);
                    _spr_leftbutton->drawString("Off", 10, 7);
                }
                _spr_leftbutton->pushSprite(Xoff, 4);
            }
            break;
            case TFT_A_R_BUTTN:
            {
                Serial.printf(TFT_COLOR "* TFT   " RESET ": Message ............ [" B_GRE "S:%u RGHT C:%04X:%04X M0:%u" RESET "]\r\n",
                              Message.Sender, Message.BgColor, Message.FgColor, Message.Message[0]);

                if (Message.Message[0])
                {
                    _spr_rightbutton->setTextColor(TFT_BLACK, TFT_GREEN);
                    _spr_rightbutton->fillRoundRect(0, 0, 116, 45, 8, TFT_DARKGREEN);
                    _spr_rightbutton->fillRoundRect(4, 4, 108, 37, 8, TFT_GREEN);
                    _spr_rightbutton->drawString("Hold", 10, 7);
                }
                else
                {
                    _spr_rightbutton->setTextColor(TFT_BLACK, TFT_RED);
                    _spr_rightbutton->fillRoundRect(0, 0, 116, 45, 8, TFT_MAROON);
                    _spr_rightbutton->fillRoundRect(4, 4, 108, 37, 8, TFT_RED);
                    _spr_rightbutton->drawString("Hold", 10, 7);
                }
                _spr_rightbutton->pushSprite(Xoff + 117, 4);
            }
            break;
            case TFT_A_L1_UNIT:
            case TFT_A_L1_ICON:
                Xmargin = (Message.Area == TFT_A_L1_ICON) ? 2 : TFT_SPR_MEA_X_DISP;
                IconSize = (Message.Area == TFT_A_L1_ICON) ? TFT_READ_ICONSIZE : TFT_UNIT_ICONSIZE;
            case TFT_A_L1_DATA:
                CurrentSprite = (Message.Area == TFT_A_L1_DATA) ? _spr_small : nullptr;
                Yoff = TFT_Y1;
                break;
            case TFT_A_L2_UNIT:
            case TFT_A_L2_ICON:
                Xmargin = (Message.Area == TFT_A_L2_ICON) ? 2 : TFT_SPR_MEA_X_DISP;
                IconSize = (Message.Area == TFT_A_L2_ICON) ? TFT_READ_ICONSIZE : TFT_UNIT_ICONSIZE;
            case TFT_A_L2_DATA:
                CurrentSprite = (Message.Area == TFT_A_L2_DATA) ? _spr_small : nullptr;
                Yoff = TFT_Y2;
                break;
            case TFT_A_L3_UNIT:
            case TFT_A_L3_ICON:
                Xmargin = (Message.Area == TFT_A_L3_ICON) ? 2 : TFT_SPR_MEA_X_DISP;
                IconSize = (Message.Area == TFT_A_L3_ICON) ? TFT_READ_ICONSIZE : TFT_UNIT_ICONSIZE;
            case TFT_A_L3_DATA:
                CurrentSprite = (Message.Area == TFT_A_L3_DATA) ? _spr_small : nullptr;
                Yoff = TFT_Y3;
                break;
            case TFT_A_LT_UNIT:
            case TFT_A_LT_ICON:
                Xmargin = (Message.Area == TFT_A_LT_ICON) ? 2 : TFT_SPR_MEA_X_DISP;
                IconSize = (Message.Area == TFT_A_LT_ICON) ? TFT_READ_ICONSIZE : TFT_UNIT_ICONSIZE;
            case TFT_A_LT_DATA:
            {
                if (Message.Area == TFT_A_LT_ICON)
                    Yoff = TFT_YF - TFT_DS + TFT_SPR_MEA_Y_DISP_BIG;
                else if (Message.Area == TFT_A_LT_UNIT)
                    Yoff = TFT_YF - TFT_DS + TFT_SPR_UNI_Y_DISP_BIG;
                else
                    Yoff = TFT_YF - TFT_DS;

                CurrentSprite = (Message.Area == TFT_A_LT_DATA) ? _spr_big : nullptr;
            }
            break;
            case TFT_A_LF_UNIT:
            case TFT_A_LF_ICON:
            {
                Xmargin = (Message.Area == TFT_A_LF_ICON) ? 2 : TFT_SPR_MEA_X_DISP;
                IconSize = (Message.Area == TFT_A_LF_ICON) ? TFT_READ_ICONSIZE : TFT_UNIT_ICONSIZE;
            case TFT_A_LF_DATA:
                if (Message.Area == TFT_A_LF_ICON)
                    Yoff = TFT_YF + TFT_SPR_MEA_Y_DISP_BIG;
                else if (Message.Area == TFT_A_LF_UNIT)
                    Yoff = TFT_YF + TFT_SPR_UNI_Y_DISP_BIG;
                else
                    Yoff = TFT_YF;

                CurrentSprite = (Message.Area == TFT_A_LF_DATA) ? _spr_big : nullptr;
            }
            break;
            case TFT_A_LB_UNIT:
            case TFT_A_LB_ICON:
            {
                Xmargin = (Message.Area == TFT_A_LB_ICON) ? 2 : TFT_SPR_MEA_X_DISP;
                IconSize = (Message.Area == TFT_A_LB_ICON) ? TFT_READ_ICONSIZE : TFT_UNIT_ICONSIZE;
            case TFT_A_LB_DATA:
                if (Message.Area == TFT_A_LB_ICON)
                    Yoff = TFT_YF + TFT_DS + TFT_SPR_MEA_Y_DISP_BIG;
                else if (Message.Area == TFT_A_LB_UNIT)
                    Yoff = TFT_YF + TFT_DS + TFT_SPR_UNI_Y_DISP_BIG;
                else
                    Yoff = TFT_YF + TFT_DS;
                CurrentSprite = (Message.Area == TFT_A_LB_DATA) ? _spr_big : nullptr;
            }
            break;
            case TFT_A_DATABAR:
            {
                int Data = atoi(Message.Message);
                _spr_databar->drawString(String(Message.BgColor), 4, 0);   // Remark: Field used in a nonstandard way ...
                _spr_databar->drawString(String(Message.FgColor), 228, 0); // Remark: Field used in a nonstandard way ...
                _spr_databar->fillRect(20, 0, Data + 20, 28, TFT_RED);
                _spr_databar->fillRect(Data + 20, 0, TFT_SPR_BAR_WIDTH - Data, 28, TFT_GREEN);
                _spr_databar->pushSprite(Xoff, 286);
            }
            break;
            case TFT_A_BACKGRD:
            {
                Serial.printf(TFT_COLOR "* TFT   " RESET ": Message ............ [" B_GRE "S:%u BACK C:%04X:%04X M0:%u" RESET "]\r\n",
                              Message.Sender, Message.BgColor, Message.FgColor, Message.Message[0]);

                if (Message.Message[0])
                {
                    tft.fillRect(Xoff, 50, 233, 235, TFT_ON_COLOR);
                }
                else
                {
                    tft.fillRect(Xoff, 50, 233, 235, TFT_OFF_COLOR);
                }
            }
            break;
            default:
                /* code */
                break;
            }

            if (CurrentSprite) //!< Sprite to be displayed
            {
                CurrentSprite->fillSprite(Message.BgColor);
                CurrentSprite->setTextColor(Message.FgColor, Message.BgColor);
                CurrentSprite->drawString(String(Message.Message), TFT_ATZERO);
                CurrentSprite->pushSprite(Xoff + TFT_SPR_ADD_OFF, Yoff);
            }

            if (Xmargin) //!< Icon to be displayed
            {
                unsigned short *Icon = NULL;

                // Serial.printf(TFT_COLOR "* TFT   " RESET ": Message ............ [" B_GRE "S:%u ICON C:%04X:%04X M0:%u" RESET "]\r\n",
                //               Message.Sender, Message.BgColor, Message.FgColor, Message.Message[0]);

                switch (Message.Message[0])
                {
                case TFT_ICON_RE_BU:
                    Icon = (short unsigned int *)IconBus;
                    break;
                case TFT_ICON_RE_MI:
                    Icon = (short unsigned int *)IconMinHold;
                    break;
                case TFT_ICON_RE_MA:
                    Icon = (short unsigned int *)IconMaxHold;
                    break;
                case TFT_ICON_RE_CU:
                    Icon = (short unsigned int *)IconCurrent;
                    break;
                case TFT_ICON_RE_LD:
                    Icon = (short unsigned int *)IconLoad;
                    break;
                case TFT_ICON_RE_PW:
                    Icon = (short unsigned int *)IconPower;
                    break;
                case TFT_ICON_MU_VL:
                    Icon = (short unsigned int *)IconV;
                    break;
                case TFT_ICON_MU_AM:
                    Icon = (short unsigned int *)IconA;
                    break;
                case TFT_ICON_MU_WT:
                    Icon = (short unsigned int *)IconW;
                    break;
                default:
                    // Icons must be remain at NULL to be deleted if no icon is requested (e.g. exiting from HoldMode)
                    break;
                }

                if (Icon)
                    tft.pushImage(Xoff + Xmargin, Yoff, IconSize, IconSize, Icon, 0); // Push Icon
                else
                    tft.fillRect(Xoff + Xmargin, Yoff, IconSize, IconSize, Message.BgColor); // Delete Icon
            }
        }
    }

    vTaskDelete(NULL);
}