#pragma once
#include "MPU_Sensor.h"
#include "PacketHeader.h"
#include "ESPUART.h"
#include "WiFi.h"
#include "AsyncUDP.h"

const uint8_t MAX_NUM_OF_SENSORS = 3;

class MotionTracking_Sensor {
public:
  MotionTracking_Sensor(const uint8_t boneType, const bool bIsCommandESP, const uint16_t iMaxNumOfSensor);

  void Setup(const char* const sHostAddress, uint16_t iHostPort, TrackingSensor_Packet* pTrackingSensorData);
  void SetupSensors(const uint8_t* iSensorPins);
  void Loop();

  bool CheckAllSensorConnection();

  inline void SetSensorAddressPin(uint8_t* pPinArray) {
    m_pSensorAddressPin = pPinArray;
  }

  inline uint16_t GetNumOfSensor() const {
    return m_iMaxNumOfSensor;
  }

private:
  uint8_t GetEachSensorData(TrackingSensor_Packet* pTrackingSensor);
  void ProcessPacket(const EPacketType packetType);

  const uint8_t m_boneType;
  const uint16_t m_iMaxNumOfSensor;
  const bool m_bIsCommandESP;

  MPU_Sensor m_pMPUSensors[MAX_NUM_OF_SENSORS];
  TrackingSensor_Packet* m_pTrackingSensorDataRef;
  uint8_t* m_pSensorAddressPin;
  bool m_bEnableDataSending;

  ESPUART m_uartSerial;

  const char* m_sHostAddress;
  uint16_t m_iHostPort;
  WiFiUDP m_udpClient; // for tracking sensor
  WiFiClient m_tcpClient; // for other packet
  
  PACKET_STRUCT m_PacketStructure; // for packet

};