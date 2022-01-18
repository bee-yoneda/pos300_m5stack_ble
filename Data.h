#ifndef DATA_H
#define DATA_H

#include <M5Stack.h>

#define RANGE_VALUE_A_MIN 10
#define RANGE_VALUE_A_MAX 100
#define RANGE_VALUE_B_MIN 1000
#define RANGE_VALUE_B_MAX 5000
#define RANGE_VALUE_C_MIN 10000
#define RANGE_VALUE_C_MAX 40000

class RangeData {
private:
  int m_min;
  int m_max;

public:
  RangeData(int min, int max) {
    m_min = min;
    m_max = max;
  }

  void setRange(int min, int max) {
    m_min = min;
    m_max = max;
  }
  void getRange(int *min, int *max) {
    *min = m_min;
    *max = m_max;
  }
};

typedef enum {
  DATA_IDX_A = 0,
  DATA_IDX_B,
  DATA_IDX_C,
  DATA_IDX_MAX
} DATA_IDX;

class Data {
private:
  RangeData m_range_data[DATA_IDX_MAX] =
  {
    {RANGE_VALUE_A_MIN, RANGE_VALUE_A_MAX},
    {RANGE_VALUE_B_MIN, RANGE_VALUE_B_MAX},
    {RANGE_VALUE_C_MIN, RANGE_VALUE_C_MAX}
  };
  int measured_value[DATA_IDX_MAX] = {-1, -1, -1};

  void loadData();
  void saveData();

public:
  void setup();
  int setRangeData(DATA_IDX idx, const int min, const int max);
  int setRangeData(DATA_IDX idx, const String& min, const String& max);
  void getRangeData(DATA_IDX idx, int *min, int *max);

  int setMeasuredValue(DATA_IDX idx, const int val);
  int setMeasuredValue(DATA_IDX idx, const String& val);
  int getMeasuredValue(DATA_IDX idx);

  void save();
};
#endif // DATA
