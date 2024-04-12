#pragma once

#define _WINSOCKAPI_
#include "CoreMinimal.h"
#include <HAL/Runnable.h>
#include <Memory>

THIRD_PARTY_INCLUDES_START
#pragma push_macro("check")
#undef check

#include <ServerLibrary/NetworkModel/IOCP/IOCP.hpp>

#pragma pop_macro("check")
THIRD_PARTY_INCLUDES_END
const size_t MAX_OF_FINGER = 5;
const size_t MAX_TRACKING_SENSOR = 3;
const uint8_t QUATERNION_ELEMENT_BUFFER_LENGTH = 6;

enum class EPacketType : uint8_t {
	E_Tracking = 1,
	E_Calibration = 2,
	E_Initialization = 3,
	E_SensorInitialization = 4,
	E_CalibrationHand = 5,
	E_HandTracking = 6
};

enum class EBoneType : uint8_t {
	E_None = 0,
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
	E_None = 0,
	E_Upper = 1,
	E_Lower = 2,
	E_End = 3,
	E_Point = 4
};

enum class EFingerPointType : uint8_t {
	E_None = 0,
	E_Thumb = 1,
	E_Index = 2,
	E_Middle = 3,
	E_Ring = 4,
	E_Pinky = 5,
};

struct Quaternion {
public:
	float w;
	float x;
	float y;
	float z;

public:
	Quaternion() : w(0.f), x(0.f), y(0.f), z(0.f) {};

public:
	inline void UpdateQuaternionFromBuffer(char* const sBuffer) {
		this->w = std::stof(sBuffer + (QUATERNION_ELEMENT_BUFFER_LENGTH * 0));
		this->x = std::stof(sBuffer + (QUATERNION_ELEMENT_BUFFER_LENGTH * 1));
		this->y = std::stof(sBuffer + (QUATERNION_ELEMENT_BUFFER_LENGTH * 2));
		this->z = std::stof(sBuffer + (QUATERNION_ELEMENT_BUFFER_LENGTH * 3));
	}

};

struct STARWARS_API Initialization_Packet {
public:
	bool m_bIsCommandESP;

public:
	Initialization_Packet(bool bValue = false) : m_bIsCommandESP(bValue) {};

};

struct STARWARS_API SensorInitialization_Packet {
public:
	bool m_bAllSensorConnectionSuccess;

public:
	SensorInitialization_Packet(bool bValue) : m_bAllSensorConnectionSuccess(bValue) {}

};

struct SensorCalibration_Packet {
public:
	uint8_t m_boneType;
	uint16_t m_iNumOfSensors;
	bool m_bIsCalibrationSucceed;

public:
	SensorCalibration_Packet(uint8_t boneType, uint16_t iNumOfSensors, const bool bValue) : m_boneType(boneType), m_iNumOfSensors(iNumOfSensors), m_bIsCalibrationSucceed(bValue) {};

};

struct STARWARS_API TrackingSensor_Data {
public:
	uint8_t m_iBoneType;
	uint8_t m_iBonePointType;

	Quaternion m_quaternion;

public:
	TrackingSensor_Data() : m_iBoneType((uint8_t)0), m_iBonePointType((uint8_t)0), m_quaternion() {};
	TrackingSensor_Data(EBoneType boneType, EBonePointType bonePointType) : m_iBoneType((uint8_t)boneType), m_iBonePointType((uint8_t)bonePointType), m_quaternion() {};

public:
	bool Deserialize(SERVER::NETWORK::PACKET::PacketQueueData* const pPacketData);

};

struct STARWARS_API HandTrackingSensor_Data {
public:
	uint8_t m_iFingerPointType;
	uint16_t m_iFingerAngle;

public:

};

struct STARWARS_API HandTrackingSensor_Packet {
public:
	uint8_t m_iBoneType;
	HandTrackingSensor_Data m_FingerData[MAX_OF_FINGER];

public:
	HandTrackingSensor_Packet() : m_iBoneType(0) { ZeroMemory(&m_FingerData, sizeof(HandTrackingSensor_Data) * MAX_OF_FINGER); };

	bool Deserialize(SERVER::NETWORK::PACKET::PacketQueueData* const pPacketData);

};

struct STARWARS_API HandTrackingCalibration_Packet {
public:
	uint8_t m_iBoneType;
	bool m_bIsFlat;
	bool m_bIsSucceed;

public:
	HandTrackingCalibration_Packet() : m_iBoneType(0), m_bIsFlat(false), m_bIsSucceed(false) {};
	HandTrackingCalibration_Packet(bool bIsFlat) : m_iBoneType(0), m_bIsFlat(bIsFlat), m_bIsSucceed(false) {};
	HandTrackingCalibration_Packet(bool bIsFlat, bool bIsSucceed) : m_iBoneType(0), m_bIsFlat(bIsFlat), m_bIsSucceed(bIsSucceed) {};
	HandTrackingCalibration_Packet(uint8_t iBoneType, bool bIsFlat, bool bIsSucceed) : m_iBoneType(iBoneType), m_bIsFlat(bIsFlat), m_bIsSucceed(bIsSucceed) {};

};

static void MakeSensorInitPacket(SERVER::NETWORK::PACKET::PACKET_STRUCT& outPacket) {
	outPacket.m_packetInfo.m_iPacketType = (uint8_t)EPacketType::E_Initialization;
}

static void MakeSensorCalibrationPacket(SERVER::NETWORK::PACKET::PACKET_STRUCT& outPacket, EPacketType packetType = EPacketType::E_Calibration) {
	outPacket.m_packetInfo.m_iPacketType = (uint8_t)packetType;
}


class STARWARS_API PacketInformation : public FRunnable {
public:
	PacketInformation();
	~PacketInformation();

	bool Init() override;
	uint32 Run() override;
	void Stop() override;

public:
	void SetIOCPInstance(std::shared_ptr<SERVER::NETWORKMODEL::IOCP::IOCP>& pIOCPInstance);
	static void TrackingSensorPacketProcess(SERVER::NETWORK::PACKET::PacketQueueData* const pData);

private:
	std::shared_ptr<SERVER::NETWORKMODEL::IOCP::IOCP> m_pIOCPInstance;

	FRunnableThread* m_workerThread;

	FThreadSafeBool m_bThreadRunState;

};