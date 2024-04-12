#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Engine/DataTable.h>
#include "../Network/Packet/PacketInformation.h"
#include "MotionTrackingComponent.generated.h"

namespace SERVER {
	namespace NETWORKMODEL {
		namespace IOCP {
			struct CONNECTION;
		}
	}
}

enum class ESensorCalibrationStatus : uint8 {
	E_None,
	E_StartTimer,
	E_CalibrationStart,
	E_FlatHandCalibrationDone,
	E_BendHandCalibrationDone
};

UENUM(BlueprintType)
enum class EBPBoneType : uint8 {
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

UENUM(BlueprintType)
enum class EBPBonePointType : uint8 {
	E_None = 0,
	E_Upper = 1,
	E_Lower = 2,
	E_End = 3,
	E_Point = 4
};

UENUM(BlueprintType)
enum class EBPFingerPointType : uint8 {
	E_None = 0,
	E_Thumb = 1,
	E_Index = 2,
	E_Middle = 3,
	E_Ring = 4,
	E_Pinky = 5,
};

USTRUCT()
struct STARWARS_API FSensorCalibrationStruct {
public:
	GENERATED_USTRUCT_BODY()
	uint16_t m_iCalibrationCount;
	FQuat m_qSensorOffset;
	bool m_bIsCalibrationDone;

public:
	FSensorCalibrationStruct() : m_iCalibrationCount(0), m_qSensorOffset(), m_bIsCalibrationDone(false) {};
	FSensorCalibrationStruct(bool bCalibrationDone) : m_iCalibrationCount(0), m_qSensorOffset(), m_bIsCalibrationDone(bCalibrationDone) {};

	void UpdateSensorOffset(const FQuat& quaternion) {
		m_qSensorOffset.W = m_qSensorOffset.W + quaternion.W;
		m_qSensorOffset.X = m_qSensorOffset.X + quaternion.X;
		m_qSensorOffset.Y = m_qSensorOffset.Y + quaternion.Y;
		m_qSensorOffset.Z = m_qSensorOffset.Z + quaternion.Z;
	}

	void ChangeCalibrationState(const bool bValue) {
		m_bIsCalibrationDone = bValue;
	}

};

USTRUCT(BlueprintType)
struct STARWARS_API FSensorSkeletonBoneStruct {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBPBonePointType m_bonePointType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EAxis::Type> m_axisX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool m_bInverseX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EAxis::Type> m_axisY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool m_bInverseY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EAxis::Type> m_axisZ;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool m_bInverseZ;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector m_vBoneInitRotate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FQuat m_qBoneRotate;

public:
	FSensorSkeletonBoneStruct() : m_bonePointType(EBPBonePointType::E_None), m_bInverseX(), m_bInverseY(), m_bInverseZ(), m_vBoneInitRotate(FVector(90.f, 0.f, 0.f)), m_qBoneRotate(), m_axisX(EAxis::X), m_axisY(EAxis::Y), m_axisZ(EAxis::Z) {};
	FSensorSkeletonBoneStruct(const EBPBonePointType bonePointType) : m_bonePointType(bonePointType), m_bInverseX(), m_bInverseY(), m_bInverseZ(), m_vBoneInitRotate(FVector(90.f, 0.f, 0.f)), m_qBoneRotate() {};


};

USTRUCT(BlueprintType)
struct STARWARS_API FSensorSkeletonBoneListStruct : public FTableRowBase {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSensorSkeletonBoneStruct> m_listOfBones;

public:
	FSensorSkeletonBoneListStruct() {};

};

USTRUCT(BlueprintType)
struct STARWARS_API FSensorSkeletonFingerBoneListStruct : public FTableRowBase {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSensorSkeletonBoneStruct m_thumbBone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSensorSkeletonBoneStruct m_indexBone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSensorSkeletonBoneStruct m_middleBone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSensorSkeletonBoneStruct m_ringBone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSensorSkeletonBoneStruct m_pinkyBone;

public:
	FSensorSkeletonFingerBoneListStruct() {};

};

USTRUCT()
struct STARWARS_API FSkeletonInformation {
	GENERATED_USTRUCT_BODY()
public:
	FSensorCalibrationStruct m_calibrationInfo;
	FSensorSkeletonBoneStruct m_skeletonBoneInfo;

public:
	FSkeletonInformation() {};
	FSkeletonInformation(const FSensorSkeletonBoneStruct& boneStructure) : m_calibrationInfo(), m_skeletonBoneInfo(boneStructure) {};

public:
	__forceinline void UpdateCalibrationOffset(const FQuat& newQuaternion) {
		m_skeletonBoneInfo.m_qBoneRotate = newQuaternion;
		m_calibrationInfo.UpdateSensorOffset(newQuaternion);
	}

	__forceinline void CalibrationDone() {
		if (!m_calibrationInfo.m_bIsCalibrationDone) {
			m_calibrationInfo.m_qSensorOffset = FQuat(m_calibrationInfo.m_qSensorOffset.X / m_calibrationInfo.m_iCalibrationCount, m_calibrationInfo.m_qSensorOffset.Y / m_calibrationInfo.m_iCalibrationCount,
				m_calibrationInfo.m_qSensorOffset.Z / m_calibrationInfo.m_iCalibrationCount, m_calibrationInfo.m_qSensorOffset.W / m_calibrationInfo.m_iCalibrationCount);
			m_calibrationInfo.ChangeCalibrationState(true);
		}
	}

};

USTRUCT()
struct STARWARS_API FSkeletonInfoList {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	TArray<FSkeletonInformation> m_list;

public:
	FSkeletonInfoList() {};
	FSkeletonInfoList(const TArray<FSkeletonInformation>& list) : m_list(list) {};

};

USTRUCT()
struct STARWARS_API FQuatList {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	TArray<FQuat> m_list;

public:
	FQuatList() {};
	FQuatList(const TArray<FQuat>& list) : m_list(list) {};

};

struct STARWARS_API FFingerAngleInfo {
public:
	float m_fThumb;
	float m_fIndex;
	float m_fMiddle;
	float m_fRing;
	float m_fPinky;

public:
	FFingerAngleInfo() : m_fThumb(0.f), m_fIndex(0.f), m_fMiddle(0.f), m_fRing(0.f), m_fPinky(0.f) {};
	FFingerAngleInfo(const float fThumb, const float fIndex, const float fMiddle, const float fRing, const float fPinky) : m_fThumb(fThumb), m_fIndex(fIndex), m_fMiddle(fMiddle), m_fRing(fRing), m_fPinky(fPinky) {};

};

DECLARE_DYNAMIC_DELEGATE_OneParam(FChangeMotionTrackingState, const bool, bNewValue);
DECLARE_DYNAMIC_DELEGATE_FourParams(FUpdateAnimBone, const EBPBoneType, boneType, const TArray<FSkeletonInformation>&, skeletonInfoList, float, fSmoothDeltaTime, float, fSmoothInterpSpeed);
DECLARE_DYNAMIC_DELEGATE_FourParams(FUpdateAnimFingerBone, const EBPBoneType, boneType, const FSensorSkeletonFingerBoneListStruct&, fingerRotationInfo, float, fSmoothDeltaTime, float, fSmoothInterpSpeed);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STARWARS_API UMotionTrackingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMotionTrackingComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void InitializationPacketProcess(SERVER::NETWORK::PACKET::PacketQueueData* const pData);
	void SensorInitializationPacketProcess(SERVER::NETWORK::PACKET::PacketQueueData* const pData);
	void TrackingSensorPacketProcess(SERVER::NETWORK::PACKET::PacketQueueData* const pData);
	void SensorCalibrationPacketProcess(SERVER::NETWORK::PACKET::PacketQueueData* const pData);
	void HandTrackingCalibrationPacketProcess(SERVER::NETWORK::PACKET::PacketQueueData* const pData);
	void HandTrackingSensorPacketProcess(SERVER::NETWORK::PACKET::PacketQueueData* const pData);

	UFUNCTION()
	void SendSensorInitPacketToSensor();
	UFUNCTION()
	void SendSensorCalibrationPacketToSensor();
	UFUNCTION()
	void CalibrationTimer();

	bool GetBoneSettingFromTableByBoneType(const EBoneType boneType, const EBonePointType bonePointType, FSensorSkeletonBoneStruct& boneSetting);
	bool GetFingerBoneSettingFromTableByBoneType(const EBoneType boneType, FSensorSkeletonFingerBoneListStruct& boneSetting);
	float SelectAxisByBoneAxis(EAxis::Type axisType, const FQuat& quaternion);
	void UpdateFingerInformation(EBoneType boneType, FSensorSkeletonFingerBoneListStruct& fingerBoneInfo, const FFingerAngleInfo& angleInfo);

public:
	FChangeMotionTrackingState m_changeMotionTrackingDelegate;

	FUpdateAnimBone m_updateAnimBoneDelegate;
	FUpdateAnimFingerBone m_updateAnimFingerDelegate;

private:
	// widget
	TSubclassOf<class UMotionTrackingWidget> m_pMotionTrackingWidgetClass;
	class UMotionTrackingWidget* m_pMotionTrackingWidget;
	FCriticalSection m_sensorCalibrationStatusMutex;
	ESensorCalibrationStatus m_currntSensorCalibrationStatus;
	ESensorCalibrationStatus m_prevSensorCalibrationStatus;

	// esp
	UPROPERTY(VisibleDefaultsOnly)
	class UIOCPComponent* m_pIOCPComponent;


	FCriticalSection m_commandESPConnectionMutex;
	SERVER::NETWORKMODEL::IOCP::CONNECTION* m_pCachedCommandESPConnection;

	FCriticalSection m_ESPConnectionMutex;
	std::vector<SERVER::NETWORKMODEL::IOCP::CONNECTION*> m_pESPConnectionList;

	short m_iAllSensorInitizationSuccessfulCount;
	short m_iMotionTrackingSensorCalibrationSuccessfulCount;
	short m_iHandTrackingSensorCalibrationSuccessfulCount;

	// sensor
	FCriticalSection m_skeletonInfoMutex;
	TMap<EBoneType, FSkeletonInfoList> m_skeletonInformation;

	FCriticalSection m_updatedBoneQuatMutex;
	TMap<EBoneType, FQuatList> m_updatedBoneQuat;


	FCriticalSection m_fingerInfoMutex;
	TMap<EBoneType, FSensorSkeletonFingerBoneListStruct> m_fingerinformation;

	FCriticalSection m_updatedFingerRotationMutex;
	TMap<EBoneType, FFingerAngleInfo> m_fingerRotation;

	FThreadSafeBool  m_bIsReadyToTracking, m_bIsESPSensorCalibrationDone, m_bIsSoftwareCalibrationDone;


	// bone setting;
	UDataTable* m_pBoneSettingTable;
	UDataTable* m_pFingerBoneSettingTable;

	float m_fSmoothDeltaTime;
	float m_fSmoothInterpSpeed;

	FTimerHandle m_hCalibrationTimerHandle;
	uint16_t m_iCalibrationTimerCalledCount;

};
