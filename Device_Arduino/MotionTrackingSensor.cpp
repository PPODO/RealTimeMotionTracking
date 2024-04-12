#include "MotionTrackingSensor.h"

MotionTracking_Sensor::MotionTracking_Sensor(const uint8_t boneType, const bool bIsCommandESP, const uint16_t iMaxNumOfSensor) 
: m_uartSerial(18, 26), m_boneType(boneType), m_bIsCommandESP(bIsCommandESP), m_iMaxNumOfSensor(iMaxNumOfSensor), m_pSensorAddressPin(nullptr), m_bEnableDataSending(false), m_pTrackingSensorDataRef(nullptr),
  m_iHostPort(0), m_sHostAddress(nullptr) {
    
}

void MotionTracking_Sensor::Setup(const char* const sHostAddress, uint16_t iHostPort, TrackingSensor_Packet* pTrackingSensorData) {
  m_sHostAddress = sHostAddress;
  m_iHostPort = iHostPort;
  m_pTrackingSensorDataRef = pTrackingSensorData;

  m_uartSerial.Begin(115200);

  if(!m_tcpClient.connect(sHostAddress, iHostPort)) {
      Serial.println("Connection Failed!");
      while(1) delay(1000);
  }
  
  delay(1000);

  MakingInitializationPacket(m_PacketStructure, Initialization_Packet(m_bIsCommandESP));
  m_tcpClient.write(m_PacketStructure.m_sPacketData, sizeof(PACKET_INFORMATION) + m_PacketStructure.m_packetInfo.m_iPacketDataSize);
}

void MotionTracking_Sensor::Loop() {
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
          case EPacketType::E_CalibrationHand:
            m_uartSerial.Write("E:HAND");
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
    uint8_t iResult = GetEachSensorData(m_pTrackingSensorDataRef);
    
    if(iResult & 7) {
      uint16_t iPacketSize = MakingTrackingSensorToPacket(m_PacketStructure, m_pTrackingSensorDataRef, m_iMaxNumOfSensor);
      if(iPacketSize > 0) {
        m_udpClient.beginPacket(m_sHostAddress, m_iHostPort);
        m_udpClient.write((uint8_t*)m_PacketStructure.m_sPacketData, iPacketSize);
        m_udpClient.endPacket();
      }
    }
  }
}

void MotionTracking_Sensor::SetupSensors(const uint8_t* iSensorPins) {
  for(uint8_t i = 0; i < m_iMaxNumOfSensor; i++) {
    m_pMPUSensors[i].SetPinNumber(iSensorPins[i]);
    m_pMPUSensors[i].ChangeAddressState(HIGH);
  }

  for(uint8_t i = 0; i < m_iMaxNumOfSensor; i++)
    m_pMPUSensors[i].Setup();
}

uint8_t MotionTracking_Sensor::GetEachSensorData(TrackingSensor_Packet* pTrackingSensor) {
  uint8_t iReturnValue = 0;

  for(uint8_t i = 0; i < m_iMaxNumOfSensor; i++) {
    if(m_pMPUSensors[i].GetSensorData(pTrackingSensor[i].m_quaternion)) {
/*      Serial.print(i);
      Serial.print(" : ");
      Serial.print(pTrackingSensor[i].m_quaternion.w);
      Serial.print(", ");
      Serial.print(pTrackingSensor[i].m_quaternion.x);
      Serial.print(", ");
      Serial.print(pTrackingSensor[i].m_quaternion.y);
      Serial.print(", ");
      Serial.println(pTrackingSensor[i].m_quaternion.z);*/
  
      iReturnValue |= (1 << i);
    }
  }

  return iReturnValue;
}

void MotionTracking_Sensor::ProcessPacket(const EPacketType packetType) {
  switch(packetType) {
    case EPacketType::E_Initialization:
      MakingSensorInitializationPacket(m_PacketStructure, SensorInitialization_Packet(CheckAllSensorConnection()));
      m_tcpClient.write(m_PacketStructure.m_sPacketData, sizeof(PACKET_INFORMATION) + m_PacketStructure.m_packetInfo.m_iPacketDataSize);
    break;
    case EPacketType::E_Calibration:
      SetupSensors(m_pSensorAddressPin);
      MakingCalibrationPacket(m_PacketStructure, SensorCalibration_Packet(m_boneType, m_iMaxNumOfSensor, true));
      m_tcpClient.write(m_PacketStructure.m_sPacketData, sizeof(PACKET_INFORMATION) + m_PacketStructure.m_packetInfo.m_iPacketDataSize);
      m_bEnableDataSending = true;
    break;
  }
}

bool MotionTracking_Sensor::CheckAllSensorConnection() {
  bool bResult = true;
  for(uint8_t i = 0; i < m_iMaxNumOfSensor; i++) {
    if(!m_pMPUSensors[i].TestConnection()) {
      bResult = false;
      break;
    }
  }
  return bResult;
}