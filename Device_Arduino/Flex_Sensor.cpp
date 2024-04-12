#include "Flex_Sensor.h"

float Flex_Sensor::ReadAngle(uint16_t iAmount, bool bReturningAvg) {
  int16_t iFlexADC = analogRead(m_iPinNum);
  int16_t iCalcuateCache[iAmount];

  for(uint16_t i = 0; i < iAmount; i++)
     iCalcuateCache[i] = analogRead(m_iPinNum);

  qsort(iCalcuateCache, iAmount, sizeof(int16_t), sort_desc);

  int iSum = 0;
  for(int i = 1 * iAmount / 10; i < 9 * iAmount / 10; i++)
    iSum += iCalcuateCache[i];

  float fAvg = iSum / (8 * iAmount / 10);
  if(bReturningAvg) return fAvg;

  return map(fAvg, m_fFlat, m_fBend, 0.f, 90.f);
}

void Flex_Sensor::CalibrateFlat(uint16_t iAmount, int16_t* calculateCacheList) {
//  Serial.println("Calibrating in Float Position");

  for(uint16_t i = 0; i < iAmount; i++) {
    calculateCacheList[i] = ReadAngle(100, true);
    delay(10);
  }

  qsort(calculateCacheList, iAmount, sizeof(int16_t), sort_desc);
  int iSum = 0;
  for(int i = 1 * iAmount / 10; i < 9 * iAmount / 10; i++)
    iSum += calculateCacheList[i];
    
  m_fFlat = iSum / (8 * iAmount / 10);
}

void Flex_Sensor::CalibrateBend(uint16_t iAmount, int16_t* calculateCacheList) {
//  Serial.println("Calibrating in Bend Position");

  for(int i = 0; i < iAmount; i++) {
    calculateCacheList[i] = ReadAngle(100, true);
    delay(10);
  }

  qsort(calculateCacheList, iAmount, sizeof(int16_t), sort_desc);
  int iSum = 0;
  for(int i = 1 * iAmount / 10; i < 9 * iAmount / 10; i++)
    iSum += calculateCacheList[i];
  
  m_fBend = iSum / (8 * iAmount / 10);
}
