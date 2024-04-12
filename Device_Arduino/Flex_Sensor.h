#pragma once
#include "ESPUART.h"
#include "WiFi.h"
#include "AsyncUDP.h"

class Flex_Sensor {
public:
  Flex_Sensor() : m_iPinNum(0), m_fFlat(0.f), m_fBend(0.f) {};
  
  float ReadAngle(uint16_t iAmount, bool bReturningAvg);
  void CalibrateFlat(uint16_t iAmount, int16_t* calculateCacheList);
  void CalibrateBend(uint16_t iAmount, int16_t* calculateCacheList);
  
  inline void SetPinNum(uint8_t iPinNum) {
    m_iPinNum = iPinNum;
    pinMode(m_iPinNum, INPUT);
  }

private:
  uint8_t m_iPinNum;
  float m_fFlat;
  float m_fBend;
  
  static int sort_desc(const void* cmp1, const void* cmp2) {
    int a = *((int*)cmp1);
    int b = *((int*)cmp2);

    return a > b ? (a < b ? 0 : 1) : -1;
  }

};

