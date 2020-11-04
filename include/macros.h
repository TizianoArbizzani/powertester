#ifndef PT_MACROS_H
#define PT_MACROS_H

#define ONBOARD_LED_ON digitalWrite(ONBOARD_LED, HIGH) //!<Light Onboard Led
#define ONBOARD_LED_OFF digitalWrite(ONBOARD_LED, LOW) //!<Shutdown Onboard Led

#define TFT_LED_ON digitalWrite(TFT_LED, LOW)   //!<Light TFT BackLight Led
#define TFT_LED_OFF digitalWrite(TFT_LED, HIGH) //!<Shutdown TFT BackLight Led

#endif