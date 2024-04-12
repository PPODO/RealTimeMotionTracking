#pragma once
#include "MPU6050_6Axis_MotionApps612.h"
#include "QuaternionKalmanFilter.h"
#include "Wire.h"
#include "I2Cdev.h"

#define FIFO_BUFFER_SIZE 64

class MPU_Sensor {
public:
  MPU_Sensor();

  void Setup();

  bool GetSensorData(Quaternion&);

  inline void SetPinNumber(uint8_t iPinNum) {
    pinMode(iPinNum, OUTPUT);

    m_iPinNumber = iPinNum;
  }

  inline void ChangeAddressState(uint8_t iValue) {
    digitalWrite(m_iPinNumber, iValue);
  }

  inline bool TestConnection() {
    return m_mpu6050.testConnection();
  }

private:
  MPU6050 m_mpu6050;
  QuaternionKalmanFilter m_CalibrationFilter;

  uint8_t m_sFIFOBuffer[FIFO_BUFFER_SIZE];
  uint8_t m_iPinNumber;

};