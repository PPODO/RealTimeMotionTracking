#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "../MotionTracking/MotionTrackingComponent.h"
#include <Internationalization/TextKey.h>
#include "TrackingAnimInstance.generated.h"

DECLARE_DYNAMIC_DELEGATE(FFingerTriggerDelegate);

USTRUCT(BlueprintType)
struct STARWARS_API FFingerFlipInformation {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool m_ThumbFlipped;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool m_IndexFlipped;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool m_MiddleFlipped;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool m_RingFlipped;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool m_PinkyFlipped;

public:
	FFingerFlipInformation() {};
	FFingerFlipInformation(const bool bThumb, const bool bIndex, const bool bMiddle, const bool bRing, const bool bPinky) : m_ThumbFlipped(bThumb), m_IndexFlipped(bIndex), m_MiddleFlipped(bMiddle), m_RingFlipped(bRing), m_PinkyFlipped(bPinky) {};

};

USTRUCT()
struct STARWARS_API FTriggerDelegates {
	GENERATED_USTRUCT_BODY()
public:
	FFingerTriggerDelegate m_OnStartDelegate;
	FFingerTriggerDelegate m_OnEndDelegate;

public:


};

UENUM(BlueprintType)
enum class EEventTriggerType : uint8 {
	E_None = 0,
	E_Trigger = 1,
	E_Press = 2
};

UENUM(BlueprintType)
enum class EEventTriggerHandType : uint8 {
	E_Both = 0,
	E_Left = 1,
	E_Right = 2
};

USTRUCT(BlueprintType)
struct STARWARS_API FFingerFlipTriggerStruct : public FTableRowBase {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EEventTriggerType> m_triggerType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EEventTriggerHandType> m_triggerHandType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFingerFlipInformation m_triggerInputFlipInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (InlineEditConditionToggle))
	bool m_bUseReleaseFlipInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Editcondition = m_bUseReleaseFlipInfo))
	FFingerFlipInformation m_releaseInputFlipInfo;

	uint8 m_iTriggerFingersFlipState;
	uint8 m_iReleaseFingersFlipState;
	int32 m_iTriggerNameHashValue;

public:
	FFingerFlipTriggerStruct() : m_iTriggerFingersFlipState(0), m_iReleaseFingersFlipState(0), m_iTriggerNameHashValue(0), m_triggerType(EEventTriggerType::E_None), m_triggerHandType(EEventTriggerHandType::E_Both), m_bUseReleaseFlipInfo(false) {};
	FFingerFlipTriggerStruct(const TCHAR* sTriggerName, const FFingerFlipInformation& triggerFlipInfo) : m_iTriggerFingersFlipState(0), m_iReleaseFingersFlipState(0), m_iTriggerNameHashValue(0), m_triggerType(EEventTriggerType::E_None), m_triggerHandType(EEventTriggerHandType::E_Both), m_triggerInputFlipInfo(triggerFlipInfo) {
		m_iTriggerNameHashValue = TextKeyUtil::HashString(sTriggerName);
		UpdateFingerFlipStateValue();
	};

	void UpdateFingerFlipStateValue() {
		UpdateFingerFlipStateValue(m_iTriggerFingersFlipState, this->m_triggerInputFlipInfo);
		UpdateFingerFlipStateValue(m_iReleaseFingersFlipState, this->m_releaseInputFlipInfo);
	}

	void UpdateFingerFlipStateValue(uint8& iFlipState, FFingerFlipInformation& flipInfo) {
		uint8 iCachedFingerFlipState = 0;

		iCachedFingerFlipState |= (flipInfo.m_ThumbFlipped << 0);
		iCachedFingerFlipState |= (flipInfo.m_IndexFlipped << 1);
		iCachedFingerFlipState |= (flipInfo.m_MiddleFlipped << 2);
		iCachedFingerFlipState |= (flipInfo.m_RingFlipped << 3);
		iCachedFingerFlipState |= (flipInfo.m_PinkyFlipped << 4);

		iFlipState = iCachedFingerFlipState;
	}

	virtual void OnDataTableChanged(const UDataTable* pInDataTable, const FName sInRowName) override {
		Super::OnDataTableChanged(pInDataTable, sInRowName);

		if (auto pFindResult = pInDataTable->FindRow<FFingerFlipTriggerStruct>(sInRowName, TEXT(""))) {
			m_iTriggerNameHashValue = TextKeyUtil::HashString(sInRowName.ToString());
			UpdateFingerFlipStateValue(m_iTriggerFingersFlipState, pFindResult->m_triggerInputFlipInfo);
			UpdateFingerFlipStateValue(m_iReleaseFingersFlipState, pFindResult->m_releaseInputFlipInfo);
		}
	};

};

USTRUCT(BlueprintType)
struct STARWARS_API FFingerFlipTriggerEvent {
	GENERATED_USTRUCT_BODY()
public:
	FFingerFlipTriggerStruct m_fingerInfo;

	FTriggerDelegates m_delegatesInfo;

	bool m_bIsCalled;
	bool m_bIsBinded;

public:
	FFingerFlipTriggerEvent() : m_bIsCalled(false), m_bIsBinded(false) {};
	FFingerFlipTriggerEvent(const FFingerFlipTriggerStruct& info) : m_fingerInfo(info), m_bIsCalled(false), m_bIsBinded(false) {};
	FFingerFlipTriggerEvent(const FFingerFlipTriggerStruct& info, const FTriggerDelegates& delegates) : m_fingerInfo(info), m_delegatesInfo(delegates), m_bIsCalled(false), m_bIsBinded(false) {};

};


struct STARWARS_API BoneRotator {
public:
	EBPBonePointType m_bonePointType;
	FRotator& m_qBoneRotator;

public:
	BoneRotator(const EBPBonePointType bonePointType, FRotator& rotator) : m_bonePointType(bonePointType), m_qBoneRotator(rotator) {};

};

UCLASS()
class STARWARS_API UTrackingAnimInstance : public UAnimInstance {
	GENERATED_BODY()
public:
	UTrackingAnimInstance();

	void AddFingerTriggerDelegate(const TCHAR* sEventName, const FTriggerDelegates& eventDelegate);

	void ChangeMotionTrackingState(const bool bNewValue);

	void UpdateRotateData(const EBPBoneType boneType, const TArray<struct FSkeletonInformation>& skeletonInfoList, float fSmoothDeltaTime, float fSmoothInterpSpeed);
	void UpdateRotateData(const EBPBoneType boneType, const struct FSensorSkeletonFingerBoneListStruct& fingerRotationInfo, float fSmoothDeltaTime, float fSmoothInterpSpeed);

protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float fDeltaSeconds) override;

private:
	void CheckFingerTriggerEvent(const FFingerFlipTriggerStruct& currentFlipTriggerData, TArray<FFingerFlipTriggerEvent>& fingerTriggerList, TArray<FFingerFlipTriggerEvent*>& outTriggeredEventsList);
	void UpdateFingerTriggeredEvents(const FFingerFlipTriggerStruct& currentFlipTriggerData, TArray<FFingerFlipTriggerEvent*>& triggeredEventsList);

	void UpdateFingerFlipTrigger();
	void CheckFingerFlipTrigger(float fAngle, float fTargetAngle, bool& bFingerTrigger);

	bool m_bIsTrackingEnable;

	FFingerFlipTriggerStruct m_currentRightFingerFlipTriggerInfo;
	FFingerFlipTriggerStruct m_currentLeftFingerFlipTriggerInfo;

private:
	TMultiMap<EBPBoneType, BoneRotator> m_boneRotator;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rHeadRotator;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rSpline1Rotator;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rSpline2Rotator;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rSpline3Rotator;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rLeftUpperArmRotator;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rLeftLowerArmRotator;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rLeftHandRotator;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rRightUpperArmRotator;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rRightLowerArmRotator;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rRightHandRotator;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rLeftUpperLegRotator;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rLeftLowerLegRotator;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rLeftFootRotator;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rRightUpperLegRotator;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rRightLowerLegRotator;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rRightFootRotator;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rRightHandThumbRotator;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rRightHandIndexRotator;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rRightHandMiddleRotator;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rRightHandRingRotator;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rRightHandPinkyRotator;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rLeftHandThumbRotator;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rLeftHandIndexRotator;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rLeftHandMiddleRotator;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rLeftHandRingbRotator;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator m_rLeftHandPinkyRotator;

	UDataTable* m_pFingerFlipTriggersTable;
	TArray<FFingerFlipTriggerStruct*> m_cachedFingerTriggersDataTable;

	TArray<FFingerFlipTriggerEvent> m_fingerTriggerEventInfoList;

	TArray<FFingerFlipTriggerEvent*> m_rightFingerTriggeredEventList;
	TArray<FFingerFlipTriggerEvent*> m_leftFingerTriggeredEventList;

};