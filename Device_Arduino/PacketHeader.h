#pragma once
#include "helper_3dmath.h"

/*
  1. bones 
    head = 1
    spline1 = 2
    spline2 = 3
    spline3 = 4
    right_arm = 5
    left_arm = 6
    pelvis = 7
    right_leg = 8
    left_leg = 9

  2. point
    upper = 1
    lower = 2
    end = 3
    point = 4
*/

static const uint16_t QUATERNION_ELEMENT_BUFFER_LENGTH = 6;

enum class EPacketType : uint8_t {
  E_Tracking = 1,
	E_Calibration = 2,
  E_Initialization = 3,
  E_SensorInitialization = 4,
  E_CalibrationHand = 5,
	E_HandTracking = 6
};

enum class EBoneType : uint8_t {
  E_Head = 1,
  E_Spline1 = 2,
  E_Spline2 = 3,
  E_Spline3 = 4,
  E_Right_Arm = 5,
  E_Left_Arm = 6,
  E_Pelvis = 7,
  E_Right_Leg = 8,
  E_Left_Leg = 9,
  E_Right_Finger = 10,
  E_Left_Finger = 11
};

enum class EBonePointType : uint8_t {
  E_Upper = 1,
  E_Lower = 2,
  E_End = 3,
  E_Point = 4
};

enum class EFingerPointType : uint8_t {
  E_Thumb = 1,
  E_Index = 2,
  E_Middle = 3,
  E_Ring = 4,
  E_Pinky = 5,
};

struct PACKET_INFORMATION {
public:
  uint8_t m_iPacketType;
  uint16_t m_iPacketDataSize;
  uint16_t m_iPacketNumber;

public:
  PACKET_INFORMATION() : m_iPacketType(0), m_iPacketDataSize(0), m_iPacketNumber(0) {};
  PACKET_INFORMATION(const uint8_t iPacketType, const uint16_t iPacketDataSize) : m_iPacketType(iPacketType), m_iPacketDataSize(iPacketDataSize), m_iPacketNumber(0) {};

  static size_t GetStructSize() { return sizeof(PACKET_INFORMATION); }
};

struct PACKET_STRUCT {
  static const size_t BUFFER_LENGTH = 512;
public:
  PACKET_INFORMATION m_packetInfo;
  char m_sPacketData[BUFFER_LENGTH];

public:
  PACKET_STRUCT() : m_packetInfo() { memset(m_sPacketData, 0, BUFFER_LENGTH); };
  PACKET_STRUCT(const PACKET_INFORMATION& packetInfo, char* const sPacketData) : m_packetInfo(packetInfo) {
    memset(m_sPacketData, 0, BUFFER_LENGTH);
    memcpy(m_sPacketData, sPacketData, packetInfo.m_iPacketDataSize);
  }

};


struct TrackingSensor_Packet {
public:
  uint8_t m_iBoneType;
  uint8_t m_iBonePointType;

  Quaternion m_quaternion;

public:
  TrackingSensor_Packet() : m_iBoneType(0), m_iBonePointType(0) {};
  TrackingSensor_Packet(EBoneType boneType, EBonePointType bonePointType) : m_iBoneType((uint8_t)boneType), m_iBonePointType((uint8_t)bonePointType) {};

  uint16_t ConvertToString(char* sBuffer) {
    uint16_t iDataSize = sizeof(uint16_t);

    memcpy(sBuffer + iDataSize, &m_iBoneType, sizeof(uint8_t));
    memcpy(sBuffer + iDataSize + sizeof(uint8_t), &m_iBonePointType, sizeof(uint8_t));
    iDataSize += sizeof(uint8_t) * 2;

    dtostrf(m_quaternion.w, 2, 4, sBuffer + iDataSize + (QUATERNION_ELEMENT_BUFFER_LENGTH * 0));
    dtostrf(m_quaternion.x, 2, 4, sBuffer + iDataSize + (QUATERNION_ELEMENT_BUFFER_LENGTH * 1));
    dtostrf(m_quaternion.y, 2, 4, sBuffer + iDataSize + (QUATERNION_ELEMENT_BUFFER_LENGTH * 2));
    dtostrf(m_quaternion.z, 2, 4, sBuffer + iDataSize + (QUATERNION_ELEMENT_BUFFER_LENGTH * 3));
    iDataSize += QUATERNION_ELEMENT_BUFFER_LENGTH * 4;

    memcpy(sBuffer, &iDataSize, sizeof(uint16_t));

    return iDataSize;
  }

};

struct HandTrackingSensor_Data {
public:
  uint8_t m_iFingerPointType;
  uint16_t m_iFingerAngle;

public:

};

struct HandTrackingSensor_Packet {
public:
  uint8_t m_iBoneType;
  HandTrackingSensor_Data* m_pFingerData;

public:
  HandTrackingSensor_Packet() : m_iBoneType(0), m_pFingerData(nullptr) {};
  HandTrackingSensor_Packet(const uint8_t iBoneType, HandTrackingSensor_Data* pFingerData) : m_iBoneType(iBoneType), m_pFingerData(pFingerData) {};

};

struct Initialization_Packet {
public:
  bool m_bIsCommandESP;

public:
  Initialization_Packet(const bool bValue = false) : m_bIsCommandESP(bValue) {};
  
};

struct SensorInitialization_Packet {
public:
  bool m_bAllSensorConnectionSuccess;

public:
  SensorInitialization_Packet(const bool bValue) : m_bAllSensorConnectionSuccess(bValue) {}

};

struct SensorCalibration_Packet {
public:
	uint8_t m_boneType;
	uint16_t m_iNumOfSensors;
	bool m_bIsCalibrationSucceed;

public:
	SensorCalibration_Packet(uint8_t boneType, uint16_t iNumOfSensors, const bool bValue) : m_boneType(boneType), m_iNumOfSensors(iNumOfSensors), m_bIsCalibrationSucceed(bValue) {};

};

struct HandTrackingCalibration_Packet {
public:
	uint8_t m_iBoneType;
	bool m_bIsFlat;
	bool m_bIsSucceed;

public:
	HandTrackingCalibration_Packet() : m_iBoneType(0),m_bIsFlat(false), m_bIsSucceed(false) {};
	HandTrackingCalibration_Packet(bool bIsFlat) : m_iBoneType(0), m_bIsFlat(bIsFlat), m_bIsSucceed(false) {};
	HandTrackingCalibration_Packet(bool bIsFlat, bool bIsSucceed) : m_iBoneType(0), m_bIsFlat(bIsFlat), m_bIsSucceed(bIsSucceed) {};
	HandTrackingCalibration_Packet(uint8_t iBoneType, bool bIsFlat, bool bIsSucceed) : m_iBoneType(iBoneType), m_bIsFlat(bIsFlat), m_bIsSucceed(bIsSucceed) {};

};

static uint16_t MakingTrackingSensorToPacket(PACKET_STRUCT& packet, TrackingSensor_Packet* pSensors, uint16_t iNumOfSensor = 1) {
  memset(&packet, 0, sizeof(PACKET_STRUCT));
  if(pSensors) {
    uint16_t iPacketWriteBytes = sizeof(PACKET_INFORMATION) + sizeof(uint16_t);
    for(uint16_t i = 0; i < iNumOfSensor; i++) {
      iPacketWriteBytes += pSensors[i].ConvertToString(packet.m_sPacketData + iPacketWriteBytes);
    }

    memcpy(packet.m_sPacketData + sizeof(PACKET_INFORMATION), &iNumOfSensor, sizeof(uint16_t));

    packet.m_packetInfo.m_iPacketType = (uint8_t)EPacketType::E_Tracking;
    packet.m_packetInfo.m_iPacketDataSize = iPacketWriteBytes - sizeof(PACKET_INFORMATION);

    memcpy(packet.m_sPacketData, &packet.m_packetInfo, sizeof(PACKET_INFORMATION));
    return iPacketWriteBytes;
  }
  return 0;
}

static uint16_t MakingHandTrackingSensorToPacket(PACKET_STRUCT& packet, const HandTrackingSensor_Packet& data, uint16_t iNumOfFingerSensor = 5) {
  memset(&packet, 0, sizeof(PACKET_STRUCT));
  
  uint16_t iPacketWriteBytes = sizeof(PACKET_INFORMATION);
  packet.m_packetInfo.m_iPacketType = (uint8_t)EPacketType::E_HandTracking;
  packet.m_packetInfo.m_iPacketDataSize = sizeof(uint8_t) + (sizeof(HandTrackingSensor_Data) * iNumOfFingerSensor);

  memcpy(packet.m_sPacketData, &packet.m_packetInfo, sizeof(PACKET_INFORMATION));
  memcpy(packet.m_sPacketData + iPacketWriteBytes, &data.m_iBoneType, sizeof(uint8_t));
  iPacketWriteBytes += sizeof(uint8_t);

  for(uint16_t i = 0; i < iNumOfFingerSensor; i++) {
    memcpy(packet.m_sPacketData + iPacketWriteBytes, &data.m_pFingerData[i], sizeof(HandTrackingSensor_Data));
    iPacketWriteBytes += sizeof(HandTrackingSensor_Data);
  }
  return iPacketWriteBytes;
}

static void MakingSensorInitializationPacket(PACKET_STRUCT& packet, const SensorInitialization_Packet& initPacket) {
  memset(&packet, 0, sizeof(PACKET_STRUCT));
  packet.m_packetInfo.m_iPacketType = (uint8_t)EPacketType::E_SensorInitialization;
  packet.m_packetInfo.m_iPacketDataSize = sizeof(SensorInitialization_Packet);

  memcpy(packet.m_sPacketData, &packet.m_packetInfo, sizeof(PACKET_INFORMATION));
  memcpy(packet.m_sPacketData + sizeof(PACKET_INFORMATION), &initPacket.m_bAllSensorConnectionSuccess, sizeof(bool));
}

static void MakingInitializationPacket(PACKET_STRUCT& packet, const Initialization_Packet& initPacket) {
  memset(&packet, 0, sizeof(PACKET_STRUCT));
  packet.m_packetInfo.m_iPacketType = (uint8_t)EPacketType::E_Initialization;
  packet.m_packetInfo.m_iPacketDataSize = sizeof(Initialization_Packet);

  memcpy(packet.m_sPacketData, &packet.m_packetInfo, sizeof(PACKET_INFORMATION));
  memcpy(packet.m_sPacketData + sizeof(PACKET_INFORMATION), &initPacket, packet.m_packetInfo.m_iPacketDataSize);
}

static void MakingCalibrationPacket(PACKET_STRUCT& packet, const SensorCalibration_Packet& caliPacket) {
  memset(&packet, 0, sizeof(PACKET_STRUCT));
  packet.m_packetInfo.m_iPacketType = (uint8_t)EPacketType::E_Calibration;
  packet.m_packetInfo.m_iPacketDataSize = sizeof(SensorCalibration_Packet);

  memcpy(packet.m_sPacketData, &packet.m_packetInfo, sizeof(PACKET_INFORMATION));
  memcpy(packet.m_sPacketData + sizeof(PACKET_INFORMATION), &caliPacket, packet.m_packetInfo.m_iPacketDataSize);
}

static void MakingHandTrackingCalibrationPacket(PACKET_STRUCT& packet, const HandTrackingCalibration_Packet& caliPacket) {
  memset(&packet, 0, sizeof(PACKET_STRUCT));
  packet.m_packetInfo.m_iPacketType = (uint8_t)EPacketType::E_CalibrationHand;
  packet.m_packetInfo.m_iPacketDataSize = sizeof(HandTrackingCalibration_Packet);

  memcpy(packet.m_sPacketData, &packet.m_packetInfo, sizeof(PACKET_INFORMATION));
  memcpy(packet.m_sPacketData + sizeof(PACKET_INFORMATION), &caliPacket, packet.m_packetInfo.m_iPacketDataSize);
}

