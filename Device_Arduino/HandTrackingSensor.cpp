#include "HandTrackingSensor.h"

HandTracking_Sensor::HandTracking_Sensor(const uint8_t boneType, const bool bIsCommandESP) : m_uartSerial(18, 26), m_boneType(boneType), m_bIsCommandESP(bIsCommandESP), m_bEnableDataSending(false), m_bCalibrateFlat(true) {
  memset(m_iFingerCalculateCachedList, 0, sizeof(int16_t) * MAX_NUM_OF_CALCULATE);
}

void HandTracking_Sensor::Setup(const char* const sHostAddress, uint16_t iHostPort, uint8_t* pPinNums) {
  m_sHostAddress = sHostAddress;
  m_iHostPort = iHostPort;

  m_uartSerial.Begin(115200);

  for(uint i = 0; i < MAX_OF_FINGER; i++) {
    m_fingers[i].SetPinNum(pPinNums[i]);
    m_fingerData[i].m_iFingerPointType = (i + 1);
  }

  if(!m_tcpClient.connect(sHostAddress, iHostPort)) {
      Serial.println("Connection Failed!");
      while(1) delay(1000);
  }
  
  delay(1000);

  MakingInitializationPacket(m_PacketStructure, Initialization_Packet(m_bIsCommandESP));
  m_tcpClient.write(m_PacketStructure.m_sPacketData, sizeof(PACKET_INFORMATION) + m_PacketStructure.m_packetInfo.m_iPacketDataSize);
}

void HandTracking_Sensor::Loop() {
  if(m_bIsCommandESP) {
    if(m_tcpClient.available()) {
      uint16_t iRecvBytes = m_tcpClient.read((uint8_t*)m_PacketStructure.m_sPacketData, m_PacketStructure.BUFFER_LENGTH);

      if(iRecvBytes >= sizeof(PACKET_INFORMATION)) {
        m_PacketStructure.m_packetInfo = *(PACKET_INFORMATION*)m_PacketStructure.m_sPacketData;
        
        switch((EPacketType)m_PacketStructure.m_packetInfo.m_iPacketType) {
          case EPacketType::E_Initialization:
            m_uartSerial.Write("E:INIT");
          break;
          case EPacketType::E_Calibration:
            m_uartSerial.Write("E:CALI");
          break;
        }
        ProcessPacket((EPacketType)m_PacketStructure.m_packetInfo.m_iPacketType);
      }
    }
  }
  else {
    uint8_t uartResult = m_uartSerial.Loop();

    ProcessPacket((EPacketType)uartResult);
  }

  if(m_bEnableDataSending) {
    for(uint8_t i = 0; i < MAX_OF_FINGER; i++) {
      float fAngleCache = m_fingers[i].ReadAngle(50, false);
      m_fingerData[i].m_iFingerAngle = constrain(fAngleCache, 0.0f, 90.f);

/*      Serial.print("Sensor Angle : ");
      Serial.print(fAngleCache);
      Serial.print(", ");*/

      uint16_t iTotalPacketSize = MakingHandTrackingSensorToPacket(m_PacketStructure, HandTrackingSensor_Packet((uint8_t)m_boneType, m_fingerData));
      
      m_udpClient.beginPacket(m_sHostAddress, m_iHostPort);
      m_udpClient.write((uint8_t*)m_PacketStructure.m_sPacketData, iTotalPacketSize);
      m_udpClient.endPacket();  

      delay(10);
    }
  }
}

void HandTracking_Sensor::CalcuateFlexSensor(uint16_t iAmount) {
  Serial.println("Waiting for input to start calibrating flat position....");
  
  delay(2000);
    
  Serial.println("Calibrating Flat position: ");
  Serial.println("Lay the hand flat on a flat surface, so the sensors are at 0 degrees.");

  for(uint i = 0; i < MAX_OF_FINGER; i++)
    m_fingers[i].CalibrateFlat(iAmount, m_iFingerCalculateCachedList);

  delay(2000);

  Serial.println("Waiting for input to start calibrating bend position....");
    
  delay(2000);
    
  Serial.println("Calibrating Bend position: ");
  Serial.println("Clamp your Hand, except the thump, that all sensors are at 90 degrees.");

  for(uint i = 0; i < MAX_OF_FINGER; i++)
    m_fingers[i].CalibrateBend(iAmount, m_iFingerCalculateCachedList);
}

void HandTracking_Sensor::ProcessPacket(const EPacketType packetType) {
  switch(packetType) {
    case EPacketType::E_Initialization:
      MakingSensorInitializationPacket(m_PacketStructure, SensorInitialization_Packet(true));
      m_tcpClient.write(m_PacketStructure.m_sPacketData, sizeof(PACKET_INFORMATION) + m_PacketStructure.m_packetInfo.m_iPacketDataSize);
    break;
    case EPacketType::E_Calibration:
    case EPacketType::E_CalibrationHand:
      MakingHandTrackingCalibrationPacket(m_PacketStructure, HandTrackingCalibration_Packet(m_boneType, m_bCalibrateFlat, true));
      if(m_bCalibrateFlat) {
        CalculateFlatSensor(100);
        m_bCalibrateFlat = false;
      }
      else {
        CalculateBendSensor(100);
        m_bCalibrateFlat = true;
        m_bEnableDataSending = true;
      }
      m_tcpClient.write(m_PacketStructure.m_sPacketData, sizeof(PACKET_INFORMATION) + m_PacketStructure.m_packetInfo.m_iPacketDataSize);
    break;
  }
}

void HandTracking_Sensor::CalculateFlatSensor(uint16_t iAmount) {
  delay(2000);

  Serial.println("Lay the hand flat on a flat surface, so the sensors are at 0 degrees.");
  for(uint i = 0; i < MAX_OF_FINGER; i++)
    m_fingers[i].CalibrateFlat(iAmount, m_iFingerCalculateCachedList);
}

void HandTracking_Sensor::CalculateBendSensor(uint16_t iAmount) {
  delay(2000);

  Serial.println("Clamp your Hand, except the thump, that all sensors are at 90 degrees.");
  for(uint i = 0; i < MAX_OF_FINGER; i++)
    m_fingers[i].CalibrateBend(iAmount, m_iFingerCalculateCachedList);
}