#ifndef powertester_h
#define powertester_h

#include "Arduino.h"
#include <Adafruit_INA219.h>

enum DataTable
{
    INAREAD_SHUNT = 1,
    INAREAD_BUS = 2,
    INAREAD_LOAD = 4,
    INAREAD_CURRENT = 8,
    INAREAD_POWER = 16
};

enum ReadingMode
{
    INAMODE_RECURRENT = 0,
    INAMODE_SPARSE = 1,
};

class reading
{
public:
    reading();

    void reset();
    void set(float ReadData);
    void display(Stream *S);

private:
    float _get_min();
    float _get_max();
    float _get_mean();
    int _get_reads();

    float _min = 1;
    float _mean = 0;
    float _max = -1;
    int _reads = 0;
};

class powertester : public Adafruit_INA219
{
public:
    powertester(int i2c_address);
    void setup();
    void update(ReadingMode Rm);
    void SetReading(DataTable Bitmap);
    void display(Stream *S);

    reading shunt_mV;
    reading bus_V;
    reading load_V;
    reading current_Ma;
    reading power_mW;

private:
    int _Address;
    uint16_t _DataRead;
};

#endif
