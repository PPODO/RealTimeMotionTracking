#include "PacketInformation.h"

PacketInformation::PacketInformation() : m_pIOCPInstance(nullptr) {
	m_bThreadRunState.AtomicSet(true);

	m_workerThread = (FRunnableThread::Create(this, TEXT("Packet Process Worker Thread")));
}

PacketInformation::~PacketInformation() {
	if (m_workerThread)
		m_workerThread->Kill();
}

bool PacketInformation::Init() {


	return true;
}

uint32 PacketInformation::Run() {
	while (m_bThreadRunState) {
		if (m_pIOCPInstance)
			m_pIOCPInstance->Run();

		FPlatformProcess::Sleep(0.001f);
	}
	return uint32();
}

void PacketInformation::Stop() {
	m_bThreadRunState.AtomicSet(false);
}

void PacketInformation::SetIOCPInstance(std::shared_ptr<SERVER::NETWORKMODEL::IOCP::IOCP>& pIOCPInstance) {
	if (pIOCPInstance && !m_pIOCPInstance)
		m_pIOCPInstance = pIOCPInstance;
}

void PacketInformation::TrackingSensorPacketProcess(SERVER::NETWORK::PACKET::PacketQueueData* const pData) {
	if (pData) {
		TrackingSensor_Data packetResult[MAX_TRACKING_SENSOR];
		size_t iNumOfSensor(pData->m_packetData.m_packetInfo.m_iPacketDataSize - *reinterpret_cast<uint16_t*>(pData->m_packetData.m_sPacketData));

		for (int i = 0; i < MAX_TRACKING_SENSOR; i++)
			packetResult[i].Deserialize(pData);
	}
}
using namespace std;

bool TrackingSensor_Data::Deserialize(SERVER::NETWORK::PACKET::PacketQueueData* const pPacketData) {
	if (pPacketData) {
		uint16_t iSensorDataSize = *reinterpret_cast<uint16_t*>(pPacketData->m_packetData.m_sPacketData);

		this->m_iBoneType = *reinterpret_cast<uint8_t*>(pPacketData->m_packetData.m_sPacketData + sizeof(uint16_t));
		this->m_iBonePointType = *reinterpret_cast<uint8_t*>(pPacketData->m_packetData.m_sPacketData + (sizeof(uint16_t) + sizeof(uint8_t)));

		this->m_quaternion.UpdateQuaternionFromBuffer(pPacketData->m_packetData.m_sPacketData + sizeof(uint16_t) + sizeof(uint16_t));

		MoveMemory(pPacketData->m_packetData.m_sPacketData, pPacketData->m_packetData.m_sPacketData + iSensorDataSize, pPacketData->m_packetData.m_packetInfo.m_iPacketDataSize - iSensorDataSize);
		return true;
	}
	return false;
}

bool HandTrackingSensor_Packet::Deserialize(SERVER::NETWORK::PACKET::PacketQueueData* const pPacketData) {
	if (pPacketData) {
		this->m_iBoneType = *reinterpret_cast<uint8_t*>(pPacketData->m_packetData.m_sPacketData);
		MoveMemory(pPacketData->m_packetData.m_sPacketData, pPacketData->m_packetData.m_sPacketData + sizeof(uint8_t), pPacketData->m_packetData.m_packetInfo.m_iPacketDataSize - sizeof(uint8_t));

		for (size_t i = 0; i < MAX_OF_FINGER; i++) {
			this->m_FingerData[i] = *reinterpret_cast<HandTrackingSensor_Data*>(pPacketData->m_packetData.m_sPacketData);

			MoveMemory(pPacketData->m_packetData.m_sPacketData, pPacketData->m_packetData.m_sPacketData + sizeof(HandTrackingSensor_Data), pPacketData->m_packetData.m_packetInfo.m_iPacketDataSize - (sizeof(HandTrackingSensor_Data) * (i + 1)));
		}
		return true;
	}
	return false;
}