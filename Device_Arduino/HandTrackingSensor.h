#pragma once
#include "Flex_Sensor.h"
#include "PacketHeader.h"

const uint8_t MAX_NUM_OF_CALCULATE = 100;
const uint8_t MAX_OF_FINGER = 5;

class HandTracking_Sensor {
public:
  HandTracking_Sensor(const uint8_t boneType, const bool bIsCommandESP);

  void Setup(const char* const sHostAddress, uint16_t iHostPort, uint8_t* pPinNums);
  void CalcuateFlexSensor(uint16_t iAmount);
  void Loop();

private:
  void ProcessPacket(const EPacketType packetType);
  void CalculateFlatSensor(uint16_t iAmount);
  void CalculateBendSensor(uint16_t iAmount);

  const uint8_t m_boneType;
  const bool m_bIsCommandESP;

  Flex_Sensor m_fingers[MAX_OF_FINGER];
  HandTrackingSensor_Data m_fingerData[MAX_OF_FINGER];
  int16_t m_iFingerCalculateCachedList[MAX_NUM_OF_CALCULATE];
  bool m_bCalibrateFlat;
  
  bool m_bEnableDataSending;

  ESPUART m_uartSerial;

  const char* m_sHostAddress;
  uint16_t m_iHostPort;
  WiFiUDP m_udpClient; // for tracking sensor
  WiFiClient m_tcpClient; // for other packet
  
  PACKET_STRUCT m_PacketStructure; // for packet

};

