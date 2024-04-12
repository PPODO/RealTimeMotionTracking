#include "MPU_Sensor.h"

MPU_Sensor::MPU_Sensor() : m_mpu6050(0x68), m_CalibrationFilter(0.75f, 25) {
}

void MPU_Sensor::Setup() {
  ChangeAddressState(LOW);

  m_mpu6050.initialize();
  uint8_t iDmpInitResult = m_mpu6050.dmpInitialize();

  m_mpu6050.setXGyroOffset(0);
  m_mpu6050.setYGyroOffset(0);
  m_mpu6050.setZGyroOffset(0);
  m_mpu6050.setXAccelOffset(0);
  m_mpu6050.setYAccelOffset(0);
  m_mpu6050.setZAccelOffset(0);
  m_mpu6050.CalibrateAccel(25);
  m_mpu6050.CalibrateGyro(25);

  if(iDmpInitResult == 0)
    m_mpu6050.setDMPEnabled(true);

  ChangeAddressState(HIGH);
}

bool MPU_Sensor::GetSensorData(Quaternion& quaternion) {
  ChangeAddressState(LOW);

  uint8_t m_iMPUIntStatus = m_mpu6050.getIntStatus();
  uint16_t iFIFOCount = m_mpu6050.getFIFOCount();

  if(m_iMPUIntStatus & 0x02) {
    m_mpu6050.dmpGetCurrentFIFOPacket(m_sFIFOBuffer);
    m_mpu6050.dmpGetQuaternion(&quaternion, m_sFIFOBuffer);

    quaternion = m_CalibrationFilter.Filter(quaternion);
    
    ChangeAddressState(HIGH);
    return true;
  }
  else if(m_iMPUIntStatus & 0x10 || iFIFOCount >= 1024)
    m_mpu6050.resetFIFO();

  ChangeAddressState(HIGH);
  return false;
}