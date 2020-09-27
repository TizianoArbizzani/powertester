#ifndef powertester_h
#define powertester_h

#include "Arduino.h"
#include <Adafruit_INA219.h>

#include <array>
#include <bitset>

enum DataTable
{
    IR_SHNT = 0, // Read Shunt Voltage
    IR_BUS,      // Read Bus Voltage
    IR_LOAD,     // Read Load Voltage
    IR_CURR,     // Read Load Current
    IR_PWR       // Read Load Power
};

enum ReadingMode
{
    IM_RECURR = 0, // Read everytime you can
    IM_SPARSE,     // Read at display refresh only
};

class reading
{
public:
    reading(const char *Unit = "V");

    void reset();
    void set(float ReadData);
    void display(Stream *S);

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
};

class powertester : public Adafruit_INA219
{
public:
    powertester(int i2c_address, const char *Id);
    bool setup();
    void update(ReadingMode Rm);
    void SetReading(DataTable Bitmap);
    void display(Stream *S);

private:
    char _Id[8];
    int _Address;
    std::bitset<8> _ReadMask;
    std::array<reading, 5> _Readings;
};

#endif
