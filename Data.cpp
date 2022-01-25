#include <iostream>
#include <fstream>
#include <sstream>
#include "Data.h"
#include "SPIFFS.h"

#define FORMAT_SPIFFS_IF_FAILED true
#define RANGE_DATA_FILE_NAME "/range_data.csv"
// 以下の順のCSVとする
// "kind", "min", "max"
// 例
// "range_A", "500", "1200"

#define CSV_KIND_TITLE "kind"
#define CSV_MIN_TITLE "min"
#define CSV_MAX_TITLE "max"

#define CSV_RANGE_KIND_A "range_A"
#define CSV_RANGE_KIND_B "range_B"
#define CSV_RANGE_KIND_C "range_C"

void
Data::setup() {
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  loadData();
}

int
Data::setRangeData(DATA_IDX idx, const int min, const int max) {
  // check range
  if(min >= max) {
    return -1;
  }
  m_range_data[idx].setRange(min, max);
  return 0;
}

int
Data::setRangeData(DATA_IDX idx, const String& min, const String& max) {
  int min_num, max_num;

  min_num = atoi(min.c_str());
  max_num = atoi(max.c_str());

  return setRangeData(idx, min_num, max_num);
}

void
Data::getRangeData(DATA_IDX idx, int *min, int *max) {
  m_range_data[idx].getRange(min, max);
}

int
Data::setMeasuredValue(DATA_IDX idx, const int val) {
  int min, max;
  int ret = 0;
  m_range_data[idx].getRange(&min, &max);
  // range over
  if(val < min || max < val) {
    ret = -1;
  }
  measured_value[idx] = val;
  return ret;
}

int
Data::setMeasuredValue(DATA_IDX idx, const String& val){
  int val_num;

  val_num = atoi(val.c_str());

  return setMeasuredValue(idx, val_num);
}

int
Data::getMeasuredValue(DATA_IDX idx) {
  return measured_value[idx];
}

void
Data::save() {
  saveData();
}

void
Data::loadData() {
  fs::FS fs = SPIFFS;

  // ファイルがある場合は、読み込み
  if(!fs.exists(RANGE_DATA_FILE_NAME)) {
    Serial.printf("not exist dataFile\n");
    return;
  }

  File file = fs.open(RANGE_DATA_FILE_NAME, FILE_READ);
  int len = file.size();
  uint8_t tmpBuff[len];
  file.read(tmpBuff, len);
  file.close();

  std::string buff = std::string((char*)tmpBuff);

  std::istringstream line_stream(buff);
  std::string line_buf;
  uint8_t count = 0;
  DATA_IDX idx = DATA_IDX_MAX;
  int col = 0;
  int range_min, range_max;

  while(getline(line_stream, line_buf, '\n')) {
    // Serial.printf("stream = %s \n", line_buf.c_str());
    // 1行目はタイトルなので無視
    if(count == 0) {
      count++;
      continue;
    }
    std::istringstream col_stream(line_buf);
    std::string col_buff;
    idx = DATA_IDX_MAX;
    col = 0;
    range_min = range_max = 0;
    while(getline(col_stream, col_buff, ',')) {
      if(col_buff == CSV_RANGE_KIND_A) {
        idx = DATA_IDX_A;
        continue;
      }
      else if(col_buff == CSV_RANGE_KIND_B) {
        idx = DATA_IDX_B;
        continue;
      }
      else if(col_buff == CSV_RANGE_KIND_C) {
        idx = DATA_IDX_C;
        continue;
      }
      if(idx < DATA_IDX_MAX) {
        // 数値でなければ取り込まない
        if(col_buff.find_first_of("0123456789") == std::string::npos) {
          col++;
          Serial.println("no digit data");
          continue;
        }
        // Serial.printf("col = %d str = %s \n", col, col_buff.c_str() );
        switch(col) {
          case 0: // range_min_value
            range_min = atoi(col_buff.c_str());
            col++;
            break;
          case 1: // range_max_value
            range_max = atoi(col_buff.c_str());
            col++;
            break;
          default:
            break;
        }
      }
    }
    if(idx < DATA_IDX_MAX) {
      setRangeData(idx, range_min, range_max);
    }
  }
}

void
Data::saveData() {
  int range_min, range_max;
  DATA_IDX idx, next = DATA_IDX_MAX;

  std::string writeBuf;

  // まずはタイトル
  writeBuf = CSV_KIND_TITLE;
  writeBuf += ',';
  writeBuf += CSV_MIN_TITLE;
  writeBuf += ',';
  writeBuf += CSV_MAX_TITLE;
  writeBuf += '\n';

  for(idx = DATA_IDX_A; idx != DATA_IDX_MAX; idx = next) {
    switch(idx) {
      case DATA_IDX_A:
        writeBuf += CSV_RANGE_KIND_A;
        next = DATA_IDX_B;
        break;
      case DATA_IDX_B:
        writeBuf += CSV_RANGE_KIND_B;
        next = DATA_IDX_C;
        break;
      case DATA_IDX_C:
        writeBuf += CSV_RANGE_KIND_C;
        next = DATA_IDX_MAX;
        break;
      default:
        break;
    }
    writeBuf += ',';
    getRangeData(idx, &range_min, &range_max);
    writeBuf += String(range_min).c_str();
    writeBuf += ',';
    writeBuf += String(range_max).c_str();
    writeBuf += '\n';
  }
  writeBuf += '\0';

  fs::FS fs = SPIFFS;

  File file = fs.open(RANGE_DATA_FILE_NAME, FILE_WRITE);
  int size = writeBuf.size();
  char* cstr = new char[size + 1];
  memset(cstr, 0, size + 1);
  strcpy(cstr, writeBuf.c_str());

  Serial.printf("write buf = %s \n", cstr);
  file.write((uint8_t*)cstr, size+1);
  file.close();
  delete[] cstr;
}