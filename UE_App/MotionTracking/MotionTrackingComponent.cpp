#include "MotionTrackingComponent.h"
#include "../Network/IOCPComponent.h"
#include "../Widget/MotionTrackingWidget.h"
#include <Engine/World.h>
#include <UObject/ConstructorHelpers.h>
#include <Kismet/KismetMathLibrary.h>

UMotionTrackingComponent::UMotionTrackingComponent() {
	::ConstructorHelpers::FClassFinder<UMotionTrackingWidget> motionTrackingWidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprint/Widget/BP_MotionTrackingWidget'"));
	if (motionTrackingWidgetClass.Succeeded())
		m_pMotionTrackingWidgetClass = motionTrackingWidgetClass.Class;

	::ConstructorHelpers::FObjectFinder<UDataTable> dataTable(TEXT("/Script/Engine.DataTable'/Game/Blueprint/DataTable/BP_BoneStructList.BP_BoneStructList'"));
	if (dataTable.Succeeded())
		m_pBoneSettingTable = dataTable.Object;

	::ConstructorHelpers::FObjectFinder<UDataTable> fingerTable(TEXT("/Script/Engine.DataTable'/Game/Blueprint/DataTable/BP_FingerBoneStructList.BP_FingerBoneStructList'"));
	if (fingerTable.Succeeded())
		m_pFingerBoneSettingTable = fingerTable.Object;

	m_pIOCPComponent = CreateDefaultSubobject<UIOCPComponent>(TEXT("IOCP Server Component"));
	if (m_pIOCPComponent) {
		m_pIOCPComponent->m_packetProcessorList.insert(std::make_pair((uint8_t)EPacketType::E_Tracking, std::bind(&UMotionTrackingComponent::TrackingSensorPacketProcess, this, std::placeholders::_1)));;
		m_pIOCPComponent->m_packetProcessorList.insert(std::make_pair((uint8_t)EPacketType::E_Initialization, std::bind(&UMotionTrackingComponent::InitializationPacketProcess, this, std::placeholders::_1)));
		m_pIOCPComponent->m_packetProcessorList.insert(std::make_pair((uint8_t)EPacketType::E_SensorInitialization, std::bind(&UMotionTrackingComponent::SensorInitializationPacketProcess, this, std::placeholders::_1)));
		m_pIOCPComponent->m_packetProcessorList.insert(std::make_pair((uint8_t)EPacketType::E_Calibration, std::bind(&UMotionTrackingComponent::SensorCalibrationPacketProcess, this, std::placeholders::_1)));
		m_pIOCPComponent->m_packetProcessorList.insert(std::make_pair((uint8_t)EPacketType::E_CalibrationHand, std::bind(&UMotionTrackingComponent::HandTrackingCalibrationPacketProcess, this, std::placeholders::_1)));
		m_pIOCPComponent->m_packetProcessorList.insert(std::make_pair((uint8_t)EPacketType::E_HandTracking, std::bind(&UMotionTrackingComponent::HandTrackingSensorPacketProcess, this, std::placeholders::_1)));
	}

	m_fSmoothDeltaTime = m_fSmoothInterpSpeed = 0.f;
	m_bIsReadyToTracking = m_bIsSoftwareCalibrationDone = m_bIsESPSensorCalibrationDone = false;
	m_iAllSensorInitizationSuccessfulCount = 0;
	m_iMotionTrackingSensorCalibrationSuccessfulCount = 0;
	m_iHandTrackingSensorCalibrationSuccessfulCount = 0;
	m_iCalibrationTimerCalledCount = 0;
	m_pCachedCommandESPConnection = nullptr;
	m_prevSensorCalibrationStatus = m_currntSensorCalibrationStatus = ESensorCalibrationStatus::E_None;
	PrimaryComponentTick.bCanEverTick = true;
}

void UMotionTrackingComponent::BeginPlay() {
	Super::BeginPlay();

	if (m_pMotionTrackingWidgetClass) {
		m_pMotionTrackingWidget = CreateWidget<UMotionTrackingWidget>(GetWorld(), m_pMotionTrackingWidgetClass);
		if (m_pMotionTrackingWidget) {
			m_pMotionTrackingWidget->AddToViewport();

			m_pMotionTrackingWidget->m_sendInitPacketDelegate.AddDynamic(this, &UMotionTrackingComponent::SendSensorInitPacketToSensor);
			m_pMotionTrackingWidget->m_sendCalibrationPacketDelegate.AddDynamic(this, &UMotionTrackingComponent::SendSensorCalibrationPacketToSensor);
		}
	}
}

void UMotionTrackingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (m_bIsReadyToTracking) {
		if (!m_bIsSoftwareCalibrationDone && m_bIsESPSensorCalibrationDone) {
			bool bIsAllSensorCalibrationDone = true;

			m_skeletonInfoMutex.Lock();
			for (auto& iterator : m_skeletonInformation) {
				bool bIsEachSensorCalibrationDone = true;
				for (size_t i = 0; i < iterator.Value.m_list.Num(); i++) {
					if (iterator.Value.m_list[i].m_calibrationInfo.m_iCalibrationCount >= 100) {
						iterator.Value.m_list[i].CalibrationDone();
						continue;
					}

					bIsEachSensorCalibrationDone = false;
					++iterator.Value.m_list[i].m_calibrationInfo.m_iCalibrationCount;

					m_updatedBoneQuatMutex.Lock();
					iterator.Value.m_list[i].UpdateCalibrationOffset(m_updatedBoneQuat[iterator.Key].m_list[i]);
					m_updatedBoneQuatMutex.Unlock();
				}
				bIsAllSensorCalibrationDone &= bIsEachSensorCalibrationDone;
			}
			m_skeletonInfoMutex.Unlock();

			if (bIsAllSensorCalibrationDone) {
				m_bIsSoftwareCalibrationDone = true;
				m_changeMotionTrackingDelegate.ExecuteIfBound(true);

				if (m_pMotionTrackingWidget) {
					m_pMotionTrackingWidget->ChangeCalibrationProcessState(TEXT(""));
					m_pMotionTrackingWidget->ChangeButtonEnable(true);
				}
			}
		}
		else if (m_bIsSoftwareCalibrationDone) {
			m_skeletonInfoMutex.Lock();
			for (auto& iterator : m_skeletonInformation) {
				for (size_t i = 0; i < iterator.Value.m_list.Num(); i++) {
					auto& boneInfo = iterator.Value.m_list[i];
					const FVector& vBoneInitRotate = boneInfo.m_skeletonBoneInfo.m_vBoneInitRotate;

					m_updatedBoneQuatMutex.Lock();
					auto qCachedUpdatedQuaternion = UKismetMathLibrary::Quat_Normalized(m_updatedBoneQuat[iterator.Key].m_list[i] * boneInfo.m_calibrationInfo.m_qSensorOffset);
					m_updatedBoneQuatMutex.Unlock();

					float fCachedX = SelectAxisByBoneAxis(boneInfo.m_skeletonBoneInfo.m_axisX, qCachedUpdatedQuaternion);
					float fCachedY = SelectAxisByBoneAxis(boneInfo.m_skeletonBoneInfo.m_axisY, qCachedUpdatedQuaternion);
					float fCachedZ = SelectAxisByBoneAxis(boneInfo.m_skeletonBoneInfo.m_axisZ, qCachedUpdatedQuaternion);

					const FQuat qNewBoneQuaternion(fCachedX * (boneInfo.m_skeletonBoneInfo.m_bInverseX ? -1.f : 1.f), fCachedY * (boneInfo.m_skeletonBoneInfo.m_bInverseY ? -1.f : 1.f), fCachedZ * (boneInfo.m_skeletonBoneInfo.m_bInverseZ ? -1.f : 1.f), qCachedUpdatedQuaternion.W);
					boneInfo.m_skeletonBoneInfo.m_qBoneRotate = UKismetMathLibrary::Quat_MakeFromEuler(vBoneInitRotate) * qNewBoneQuaternion;
				}
				if (m_updateAnimBoneDelegate.IsBound())
					m_updateAnimBoneDelegate.Execute((EBPBoneType)iterator.Key, iterator.Value.m_list, 0.45f, 1.f);
			}
			m_skeletonInfoMutex.Unlock();

			m_fingerInfoMutex.Lock();
			for (auto& iterator : m_fingerinformation) {
				m_updatedFingerRotationMutex.Lock();
				UpdateFingerInformation(iterator.Key, iterator.Value, m_fingerRotation[iterator.Key]);
				m_updatedFingerRotationMutex.Unlock();

				if (m_updateAnimFingerDelegate.IsBound())
					m_updateAnimFingerDelegate.Execute((EBPBoneType)iterator.Key, iterator.Value, 0.25f, 1.f);
			}
			m_fingerInfoMutex.Unlock();

		}
		// Sensor Calibration Check
		else if (!m_bIsSoftwareCalibrationDone && !m_bIsESPSensorCalibrationDone) {
			// 여기에 위젯 수정 코드 넣을 예정
			// 스레드안에서 위젯을 수정하지 말도록 하자.
			if (m_pMotionTrackingWidget && m_currntSensorCalibrationStatus != m_prevSensorCalibrationStatus) {
				m_sensorCalibrationStatusMutex.Lock();
				switch (m_currntSensorCalibrationStatus) {
				case ESensorCalibrationStatus::E_StartTimer:
					m_pMotionTrackingWidget->ChangeCalibrationProcessState(FString::Printf(TEXT("Calibration will start in %d seconds..."), 5 - m_iCalibrationTimerCalledCount));
					break;
				case ESensorCalibrationStatus::E_CalibrationStart:
					m_pMotionTrackingWidget->ChangeCalibrationProcessState(TEXT("Flat position : Lay the hand flat on a flat surface, so the sensors are at 0 degrees."));
					m_pMotionTrackingWidget->ChangeButtonEnable(false);
					break;
				case ESensorCalibrationStatus::E_FlatHandCalibrationDone:
					m_pMotionTrackingWidget->ChangeCalibrationProcessState(TEXT("Bend position : Clamp your Hand, except the thump, that all sensors are at 90 degrees."));
					break;
				case ESensorCalibrationStatus::E_BendHandCalibrationDone:

					break;
				default:
					break;
				}
				m_prevSensorCalibrationStatus = m_currntSensorCalibrationStatus;
				m_sensorCalibrationStatusMutex.Unlock();
			}

			if (m_iMotionTrackingSensorCalibrationSuccessfulCount + m_iHandTrackingSensorCalibrationSuccessfulCount == m_iAllSensorInitizationSuccessfulCount)
				m_bIsESPSensorCalibrationDone = true;
		}
	}
}

void UMotionTrackingComponent::InitializationPacketProcess(SERVER::NETWORK::PACKET::PacketQueueData* const pData) {
	if (pData) {
		Initialization_Packet initPacket = *reinterpret_cast<Initialization_Packet*>(pData->m_packetData.m_sPacketData);

		FScopeLock lock(&m_ESPConnectionMutex);
		auto pConnection = reinterpret_cast<SERVER::NETWORKMODEL::IOCP::CONNECTION*>(pData->m_pOwner);
		
		if (initPacket.m_bIsCommandESP) {
			FScopeLock commandLock(&m_commandESPConnectionMutex);
			m_pCachedCommandESPConnection = pConnection;
		}
		m_pESPConnectionList.push_back(pConnection);
	}
}

void UMotionTrackingComponent::SensorInitializationPacketProcess(SERVER::NETWORK::PACKET::PacketQueueData* const pData) {
	if (pData) {
		SensorInitialization_Packet initPacket = *reinterpret_cast<SensorInitialization_Packet*>(pData->m_packetData.m_sPacketData);
		InterlockedIncrement16(&m_iAllSensorInitizationSuccessfulCount);

		if (m_iAllSensorInitizationSuccessfulCount >= m_pESPConnectionList.size()) {
			m_pMotionTrackingWidget->ChangeCalibrationState();	// todo : move to tick
			m_pMotionTrackingWidget->ChangeButtonEnable(true);

			m_bIsReadyToTracking = true;
		}
	}
}

void UMotionTrackingComponent::TrackingSensorPacketProcess(SERVER::NETWORK::PACKET::PacketQueueData* const pData) {
	if (pData) {
		TrackingSensor_Data packetResult[MAX_TRACKING_SENSOR];
		uint16_t iSensorCount = *reinterpret_cast<uint16_t*>(pData->m_packetData.m_sPacketData);
		MoveMemory(pData->m_packetData.m_sPacketData, pData->m_packetData.m_sPacketData + sizeof(uint16_t), pData->m_packetData.m_packetInfo.m_iPacketDataSize);

		for (int i = 0; i < iSensorCount; i++) {
			packetResult[i].Deserialize(pData);

			m_updatedBoneQuatMutex.Lock();
			auto& refListOfBoneQuat = m_updatedBoneQuat[(EBoneType)packetResult[i].m_iBoneType];
			refListOfBoneQuat.m_list[i] = FQuat(packetResult[i].m_quaternion.x, packetResult[i].m_quaternion.y, packetResult[i].m_quaternion.z, packetResult[i].m_quaternion.w);
			m_updatedBoneQuatMutex.Unlock();
		}
	}
}

void UMotionTrackingComponent::SensorCalibrationPacketProcess(SERVER::NETWORK::PACKET::PacketQueueData* const pData) {
	if (pData) {
		SensorCalibration_Packet calibrationPacket = *reinterpret_cast<SensorCalibration_Packet*>(pData->m_packetData.m_sPacketData);

		if (calibrationPacket.m_bIsCalibrationSucceed) {
			TArray<FSkeletonInformation> listOfBoneInfo;
			TArray<FQuat> listOfUpdateBone;
			size_t iNumOfSensors = calibrationPacket.m_iNumOfSensors;

			for (int i = 0; i < iNumOfSensors; i++) {
				FSensorSkeletonBoneStruct cachedBoneSetting;
				if (GetBoneSettingFromTableByBoneType((EBoneType)calibrationPacket.m_boneType, (EBonePointType)(i + 1), cachedBoneSetting))
					listOfBoneInfo.Add(cachedBoneSetting);

				listOfUpdateBone.Add(FQuat());

				m_skeletonInfoMutex.Lock();
				m_skeletonInformation.Add((EBoneType)calibrationPacket.m_boneType, listOfBoneInfo);
				m_skeletonInfoMutex.Unlock();

				m_updatedBoneQuatMutex.Lock();
				m_updatedBoneQuat.Add((EBoneType)calibrationPacket.m_boneType, listOfUpdateBone);
				m_updatedBoneQuatMutex.Unlock();
			}

			InterlockedIncrement16(&m_iMotionTrackingSensorCalibrationSuccessfulCount);
		}
	}
}

void UMotionTrackingComponent::HandTrackingCalibrationPacketProcess(SERVER::NETWORK::PACKET::PacketQueueData* const pData) {
	if (pData && m_pCachedCommandESPConnection) {
		HandTrackingCalibration_Packet calibrationPacket = *reinterpret_cast<HandTrackingCalibration_Packet*>(pData->m_packetData.m_sPacketData);

		if (calibrationPacket.m_bIsFlat && calibrationPacket.m_bIsSucceed) {
			SERVER::NETWORK::PACKET::PACKET_STRUCT resultPacket;
			MakeSensorCalibrationPacket(resultPacket, EPacketType::E_CalibrationHand);

			if (m_sensorCalibrationStatusMutex.TryLock()) {
				m_currntSensorCalibrationStatus = ESensorCalibrationStatus::E_FlatHandCalibrationDone;
				m_sensorCalibrationStatusMutex.Unlock();
			}

			FScopeLock lock(&m_commandESPConnectionMutex);
			m_pCachedCommandESPConnection->m_pUser->Send(resultPacket);
		}
		else if (!calibrationPacket.m_bIsFlat && calibrationPacket.m_bIsSucceed) {
			FSensorSkeletonFingerBoneListStruct cachedFingerBoneSetting;
			if (GetFingerBoneSettingFromTableByBoneType((EBoneType)calibrationPacket.m_iBoneType, cachedFingerBoneSetting)) {
				m_fingerInfoMutex.Lock();
				m_fingerinformation.Add((EBoneType)calibrationPacket.m_iBoneType, cachedFingerBoneSetting);
				m_fingerInfoMutex.Unlock();
			}
			m_updatedFingerRotationMutex.Lock();
			m_fingerRotation.Add((EBoneType)calibrationPacket.m_iBoneType, FFingerAngleInfo());
			m_updatedFingerRotationMutex.Unlock();

			InterlockedIncrement16(&m_iHandTrackingSensorCalibrationSuccessfulCount);
		}
	}
}

void UMotionTrackingComponent::HandTrackingSensorPacketProcess(SERVER::NETWORK::PACKET::PacketQueueData* const pData) {
	if (pData) {
		HandTrackingSensor_Packet packetResult;

		packetResult.Deserialize(pData);

		m_updatedFingerRotationMutex.Lock();

		m_fingerRotation[(EBoneType)packetResult.m_iBoneType].m_fThumb = packetResult.m_FingerData[0].m_iFingerAngle;
		m_fingerRotation[(EBoneType)packetResult.m_iBoneType].m_fIndex = packetResult.m_FingerData[1].m_iFingerAngle;
		m_fingerRotation[(EBoneType)packetResult.m_iBoneType].m_fMiddle = packetResult.m_FingerData[2].m_iFingerAngle;
		m_fingerRotation[(EBoneType)packetResult.m_iBoneType].m_fRing = packetResult.m_FingerData[3].m_iFingerAngle;
		m_fingerRotation[(EBoneType)packetResult.m_iBoneType].m_fPinky = packetResult.m_FingerData[4].m_iFingerAngle;

		m_updatedFingerRotationMutex.Unlock();
	}
}

void UMotionTrackingComponent::SendSensorInitPacketToSensor() {
	if (m_pCachedCommandESPConnection) {
		SERVER::NETWORK::PACKET::PACKET_STRUCT resultPacket;
		MakeSensorInitPacket(resultPacket);

		FScopeLock lock(&m_commandESPConnectionMutex);
		m_pCachedCommandESPConnection->m_pUser->Send(resultPacket);

		if (m_pMotionTrackingWidget)
			m_pMotionTrackingWidget->ChangeButtonEnable(false);
	}
}

void UMotionTrackingComponent::SendSensorCalibrationPacketToSensor() {
	m_iCalibrationTimerCalledCount = 0;
	GetWorld()->GetTimerManager().SetTimer(m_hCalibrationTimerHandle, this, &UMotionTrackingComponent::CalibrationTimer, 1.f, true, 1.f);
}

void UMotionTrackingComponent::CalibrationTimer() {
	if (m_iCalibrationTimerCalledCount >= 5) {
		if (m_pCachedCommandESPConnection) {
			SERVER::NETWORK::PACKET::PACKET_STRUCT resultPacket;
			MakeSensorCalibrationPacket(resultPacket);

			FScopeLock lock(&m_commandESPConnectionMutex);
			m_pCachedCommandESPConnection->m_pUser->Send(resultPacket);

			// reset all setting;
			m_skeletonInfoMutex.Lock();
			m_skeletonInformation.Empty();
			m_skeletonInfoMutex.Unlock();


			if (m_sensorCalibrationStatusMutex.TryLock()) {
				m_currntSensorCalibrationStatus = ESensorCalibrationStatus::E_CalibrationStart;
				m_sensorCalibrationStatusMutex.Unlock();
			}
		}

		GetWorld()->GetTimerManager().ClearTimer(m_hCalibrationTimerHandle);
	}
	else {
		if (m_sensorCalibrationStatusMutex.TryLock()) {
			m_currntSensorCalibrationStatus = ESensorCalibrationStatus::E_StartTimer;
			m_sensorCalibrationStatusMutex.Unlock();
		}
		m_iCalibrationTimerCalledCount++;
	}
}

bool UMotionTrackingComponent::GetBoneSettingFromTableByBoneType(const EBoneType boneType, const EBonePointType bonePointType, FSensorSkeletonBoneStruct& boneSetting) {
	if (m_pBoneSettingTable) {
		if (auto pList = m_pBoneSettingTable->FindRow<FSensorSkeletonBoneListStruct>(*FString::Printf(TEXT("%d"), (uint16_t)boneType), TEXT(""))) {
			for (auto iterator : pList->m_listOfBones) {
				if (iterator.m_bonePointType == (EBPBonePointType)bonePointType)
					boneSetting = iterator;
			}
			return true;
		}
	}
	return false;
}

bool UMotionTrackingComponent::GetFingerBoneSettingFromTableByBoneType(const EBoneType boneType, FSensorSkeletonFingerBoneListStruct& boneSetting) {
	if (m_pFingerBoneSettingTable) {
		if (auto pResult = m_pFingerBoneSettingTable->FindRow<FSensorSkeletonFingerBoneListStruct>(*FString::Printf(TEXT("%d"), (uint16_t)boneType), TEXT(""))) {
			boneSetting = *pResult;

			return true;
		}
	}
	return false;
}

float UMotionTrackingComponent::SelectAxisByBoneAxis(EAxis::Type axisType, const FQuat& quaternion) {
	switch (axisType) {
	case EAxis::X:
		return quaternion.X;
	case EAxis::Y:
		return quaternion.Y;
	case EAxis::Z:
		return quaternion.Z;
	}
	return 0.f;
}

void UMotionTrackingComponent::UpdateFingerInformation(EBoneType boneType, FSensorSkeletonFingerBoneListStruct& fingerBoneInfo, const FFingerAngleInfo& angleInfo) {
	fingerBoneInfo.m_thumbBone.m_qBoneRotate = UKismetMathLibrary::Quat_MakeFromEuler(fingerBoneInfo.m_thumbBone.m_vBoneInitRotate) * FRotator(0.f, 0.f, angleInfo.m_fThumb).Quaternion();
	fingerBoneInfo.m_indexBone.m_qBoneRotate = UKismetMathLibrary::Quat_MakeFromEuler(fingerBoneInfo.m_indexBone.m_vBoneInitRotate) * FRotator(0.f, 0.f, angleInfo.m_fIndex * -1.f).Quaternion();
	fingerBoneInfo.m_middleBone.m_qBoneRotate = UKismetMathLibrary::Quat_MakeFromEuler(fingerBoneInfo.m_middleBone.m_vBoneInitRotate) * FRotator(0.f, 0.f, angleInfo.m_fMiddle * -1.f).Quaternion();
	fingerBoneInfo.m_ringBone.m_qBoneRotate = UKismetMathLibrary::Quat_MakeFromEuler(fingerBoneInfo.m_ringBone.m_vBoneInitRotate) * FRotator(0.f, 0.f, angleInfo.m_fRing * -1.f).Quaternion();
	fingerBoneInfo.m_pinkyBone.m_qBoneRotate = UKismetMathLibrary::Quat_MakeFromEuler(fingerBoneInfo.m_pinkyBone.m_vBoneInitRotate) * FRotator(0.f, 0.f, angleInfo.m_fPinky * -1.f).Quaternion();
}
