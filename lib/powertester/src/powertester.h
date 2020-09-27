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
    reading(const char *Unit = "V");

    void reset();
    void set(float ReadData);
    void display(Stream *S);

private:
    float _get_min();
    float _get_max();
    float _get_mean();
    int _get_reads();

    char _Unit[8] = {0};

    float READ_min, READ_mean, READ_max;
    int READ_reads;
};

class powertester : public Adafruit_INA219
{
public:
    powertester(int i2c_address, const char *Id);
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
    char _Id[8];
    int _Address;
    uint16_t _DataRead;
};

#endif
