#ifndef powertester_h
#define powertester_h

#include "Arduino.h"
#include <Adafruit_INA219.h>

#include <array>
#include <bitset>

#define IR_SHNT 0 // Read Shunt Voltage
#define IR_BUS 1  // Read Bus Voltage
#define IR_LOAD 2 // Read Load Voltage
#define IR_CURR 3 // Read Load Current
#define IR_PWR 4  // Read Load Power

#define RS(X) (1 << X) // Convert from position to value

#define IM_RECURR 0 // Read everytime you can
#define IM_SPARSE 1 // Read at display refresh only

class reading
{
public:
    reading(const char *Unit = "V");

    void reset();
    void set(float ReadData);
    void display(Stream *S);
    bool hasFocus();
    bool setFocus(bool Focus);

private:
    float _get_min();
    float _get_max();
    float _get_mean();
    int _get_reads();

    char _Unit[8] = {0}; // Measurement unit label

    float _IR_min;  // Minimum value read
    float _IR_pile; // Values pile
    float _IR_max;  // Maximum value read
    int _IR_reads;  // Number of readings

    bool _focus; // This is the current focused reading
};

class powertester : public Adafruit_INA219
{
public:
    powertester(int i2c_address, const char *Id);
    bool setup();
    void update(uint16_t Rm);
    void SetReading(int Bitmap);
    void display(Stream *S);

private:
    char _Id[8];                      // INA219 Chip Label
    int _Address;                     // INA219 I2C address (default 0x40)
    bool _Active;                     // INA219 is active (I2C Reachable??)
    std::bitset<32> _ReadMask;        // Which INA219 field must be read at max speed
    std::array<reading, 5> _Readings; // INA219 Reading fields
};

#endif
