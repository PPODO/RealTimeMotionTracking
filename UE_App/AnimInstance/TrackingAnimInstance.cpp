#include "TrackingAnimInstance.h"
#include <Kismet/KismetMathLibrary.h>
#include <UObject/ConstructorHelpers.h>

UTrackingAnimInstance::UTrackingAnimInstance() {
	::ConstructorHelpers::FObjectFinder<UDataTable> fingerFlipTriggersTable(TEXT("/Script/Engine.DataTable'/Game/Blueprint/DataTable/BP_FingerCommandDataTable.BP_FingerCommandDataTable'"));
	if (fingerFlipTriggersTable.Succeeded())
		m_pFingerFlipTriggersTable = fingerFlipTriggersTable.Object;

	m_rHeadRotator = FRotator(00.f, 0.f, 90.f);
	m_boneRotator.Add(EBPBoneType::E_Head, BoneRotator(EBPBonePointType::E_Upper, m_rHeadRotator));

	m_boneRotator.Add(EBPBoneType::E_Spline1, BoneRotator(EBPBonePointType::E_Upper, m_rSpline1Rotator));
	m_boneRotator.Add(EBPBoneType::E_Spline3, BoneRotator(EBPBonePointType::E_Upper, m_rSpline2Rotator));
	m_boneRotator.Add(EBPBoneType::E_Spline2, BoneRotator(EBPBonePointType::E_Upper, m_rSpline3Rotator));

	m_rLeftUpperArmRotator = FRotator(-180.f, -90.f, -180.f);
	m_rLeftLowerArmRotator = FRotator(180.f, -90.f, 180.f);
	m_rLeftHandRotator = FRotator(12.5f, 20.f, 90.f);
	m_boneRotator.Add(EBPBoneType::E_Left_Arm, BoneRotator(EBPBonePointType::E_Upper, m_rLeftUpperArmRotator));
	m_boneRotator.Add(EBPBoneType::E_Left_Arm, BoneRotator(EBPBonePointType::E_Lower, m_rLeftLowerArmRotator));
	m_boneRotator.Add(EBPBoneType::E_Left_Arm, BoneRotator(EBPBonePointType::E_End, m_rLeftHandRotator));

	m_rRightUpperArmRotator = FRotator(0.f, -90.f, 0.f);
	m_rRightLowerArmRotator = FRotator(0.f, -90.f, 0.f);
	m_rRightHandRotator = FRotator(-12.5f, -20.f, 90.f);
	m_boneRotator.Add(EBPBoneType::E_Right_Arm, BoneRotator(EBPBonePointType::E_Upper, m_rRightUpperArmRotator));
	m_boneRotator.Add(EBPBoneType::E_Right_Arm, BoneRotator(EBPBonePointType::E_Lower, m_rRightLowerArmRotator));
	m_boneRotator.Add(EBPBoneType::E_Right_Arm, BoneRotator(EBPBonePointType::E_End, m_rRightHandRotator));

	m_boneRotator.Add(EBPBoneType::E_Left_Leg, BoneRotator(EBPBonePointType::E_Upper, m_rLeftUpperLegRotator));
	m_boneRotator.Add(EBPBoneType::E_Left_Leg, BoneRotator(EBPBonePointType::E_Lower, m_rLeftLowerLegRotator));
	m_boneRotator.Add(EBPBoneType::E_Left_Leg, BoneRotator(EBPBonePointType::E_Upper, m_rLeftFootRotator));

	m_boneRotator.Add(EBPBoneType::E_Right_Leg, BoneRotator(EBPBonePointType::E_Upper, m_rRightUpperLegRotator));
	m_boneRotator.Add(EBPBoneType::E_Right_Leg, BoneRotator(EBPBonePointType::E_Lower, m_rRightLowerLegRotator));
	m_boneRotator.Add(EBPBoneType::E_Right_Leg, BoneRotator(EBPBonePointType::E_End, m_rRightFootRotator));

	m_rRightHandThumbRotator = FRotator(0.f, 0.f, 0.f);
	m_rRightHandIndexRotator = FRotator(35.f, 0.f, 0.f);
	m_rRightHandMiddleRotator = FRotator(35.f, 0.f, 0.f);
	m_rRightHandRingRotator = FRotator(35.f, 0.f, 0.f);
	m_rRightHandPinkyRotator = FRotator(35.f, 0.f, 0.f);
	m_boneRotator.Add(EBPBoneType::E_Right_Finger, BoneRotator((EBPBonePointType)EBPFingerPointType::E_Thumb, m_rRightHandThumbRotator));
	m_boneRotator.Add(EBPBoneType::E_Right_Finger, BoneRotator((EBPBonePointType)EBPFingerPointType::E_Index, m_rRightHandIndexRotator));
	m_boneRotator.Add(EBPBoneType::E_Right_Finger, BoneRotator((EBPBonePointType)EBPFingerPointType::E_Middle, m_rRightHandMiddleRotator));
	m_boneRotator.Add(EBPBoneType::E_Right_Finger, BoneRotator((EBPBonePointType)EBPFingerPointType::E_Ring, m_rRightHandRingRotator));
	m_boneRotator.Add(EBPBoneType::E_Right_Finger, BoneRotator((EBPBonePointType)EBPFingerPointType::E_Pinky, m_rRightHandPinkyRotator));

	m_rLeftHandThumbRotator = FRotator(0.f, 0.f, 0.f);
	m_rLeftHandIndexRotator = FRotator(-35.f, 0.f, 0.f);
	m_rLeftHandMiddleRotator = FRotator(-35.f, 0.f, 0.f);
	m_rLeftHandRingbRotator = FRotator(-35.f, 0.f, 0.f);
	m_rLeftHandPinkyRotator = FRotator(-35.f, 0.f, 0.f);
	m_boneRotator.Add(EBPBoneType::E_Left_Finger, BoneRotator((EBPBonePointType)EBPFingerPointType::E_Thumb, m_rLeftHandThumbRotator));
	m_boneRotator.Add(EBPBoneType::E_Left_Finger, BoneRotator((EBPBonePointType)EBPFingerPointType::E_Index, m_rLeftHandIndexRotator));
	m_boneRotator.Add(EBPBoneType::E_Left_Finger, BoneRotator((EBPBonePointType)EBPFingerPointType::E_Middle, m_rLeftHandMiddleRotator));
	m_boneRotator.Add(EBPBoneType::E_Left_Finger, BoneRotator((EBPBonePointType)EBPFingerPointType::E_Ring, m_rLeftHandRingbRotator));
	m_boneRotator.Add(EBPBoneType::E_Left_Finger, BoneRotator((EBPBonePointType)EBPFingerPointType::E_Pinky, m_rLeftHandPinkyRotator));

	m_currentRightFingerFlipTriggerInfo.m_triggerHandType = EEventTriggerHandType::E_Right;
	m_currentLeftFingerFlipTriggerInfo.m_triggerHandType = EEventTriggerHandType::E_Left;
	m_bIsTrackingEnable = false;
}

void UTrackingAnimInstance::NativeBeginPlay() {
	Super::NativeBeginPlay();

	if (m_pFingerFlipTriggersTable)
		m_pFingerFlipTriggersTable->GetAllRows(TEXT(""), m_cachedFingerTriggersDataTable);
}

void UTrackingAnimInstance::NativeUpdateAnimation(float fDeltaSeconds) {
	Super::NativeUpdateAnimation(fDeltaSeconds);

	if (1/*m_bIsTrackingEnable*/) {
		UpdateFingerFlipTrigger();

		UpdateFingerTriggeredEvents(m_currentRightFingerFlipTriggerInfo, m_rightFingerTriggeredEventList);
		UpdateFingerTriggeredEvents(m_currentLeftFingerFlipTriggerInfo, m_leftFingerTriggeredEventList);

		CheckFingerTriggerEvent(m_currentRightFingerFlipTriggerInfo, m_fingerTriggerEventInfoList, m_rightFingerTriggeredEventList);
		CheckFingerTriggerEvent(m_currentLeftFingerFlipTriggerInfo, m_fingerTriggerEventInfoList, m_leftFingerTriggeredEventList);
	}
}

void UTrackingAnimInstance::ChangeMotionTrackingState(const bool bNewValue) {
	m_bIsTrackingEnable = bNewValue;
}

void UTrackingAnimInstance::UpdateRotateData(const EBPBoneType boneType, const TArray<struct FSkeletonInformation>& skeletonInfoList, float fSmoothDeltaTime, float fSmoothInterpSpeed) {
	TArray<BoneRotator> m_bonePointList;
	m_boneRotator.MultiFind(boneType, m_bonePointList);

	for (auto& iterator : m_bonePointList)
		iterator.m_qBoneRotator = UKismetMathLibrary::RInterpTo(iterator.m_qBoneRotator, UKismetMathLibrary::Quat_Rotator(skeletonInfoList[((uint16_t)iterator.m_bonePointType) - 1].m_skeletonBoneInfo.m_qBoneRotate), fSmoothDeltaTime, fSmoothInterpSpeed);
}

void UTrackingAnimInstance::UpdateRotateData(const EBPBoneType boneType, const FSensorSkeletonFingerBoneListStruct& fingerRotationInfo, float fSmoothDeltaTime, float fSmoothInterpSpeed) {
	TArray<BoneRotator> m_bonePointList;
	m_boneRotator.MultiFind(boneType, m_bonePointList);
	const auto iNumOfFinger = m_bonePointList.Num();

	m_bonePointList[iNumOfFinger - 1].m_qBoneRotator = UKismetMathLibrary::RInterpTo(m_bonePointList[iNumOfFinger - 1].m_qBoneRotator, fingerRotationInfo.m_thumbBone.m_qBoneRotate.Rotator(), fSmoothDeltaTime, fSmoothInterpSpeed);
	m_bonePointList[iNumOfFinger - 2].m_qBoneRotator = UKismetMathLibrary::RInterpTo(m_bonePointList[iNumOfFinger - 2].m_qBoneRotator, fingerRotationInfo.m_indexBone.m_qBoneRotate.Rotator(), fSmoothDeltaTime, fSmoothInterpSpeed);
	m_bonePointList[iNumOfFinger - 3].m_qBoneRotator = UKismetMathLibrary::RInterpTo(m_bonePointList[iNumOfFinger - 3].m_qBoneRotator, fingerRotationInfo.m_middleBone.m_qBoneRotate.Rotator(), fSmoothDeltaTime, fSmoothInterpSpeed);
	m_bonePointList[iNumOfFinger - 4].m_qBoneRotator = UKismetMathLibrary::RInterpTo(m_bonePointList[iNumOfFinger - 4].m_qBoneRotator, fingerRotationInfo.m_ringBone.m_qBoneRotate.Rotator(), fSmoothDeltaTime, fSmoothInterpSpeed);
	m_bonePointList[iNumOfFinger - 5].m_qBoneRotator = UKismetMathLibrary::RInterpTo(m_bonePointList[iNumOfFinger - 5].m_qBoneRotator, fingerRotationInfo.m_pinkyBone.m_qBoneRotate.Rotator(), fSmoothDeltaTime, fSmoothInterpSpeed);

	UE_LOG(LogTemp, Error, L"%s", *m_bonePointList[iNumOfFinger - 1].m_qBoneRotator.ToString());
	UE_LOG(LogTemp, Error, L"%s", *m_bonePointList[iNumOfFinger - 2].m_qBoneRotator.ToString());
	UE_LOG(LogTemp, Error, L"%s", *m_bonePointList[iNumOfFinger - 3].m_qBoneRotator.ToString());
	UE_LOG(LogTemp, Error, L"%s", *m_bonePointList[iNumOfFinger - 4].m_qBoneRotator.ToString());
	UE_LOG(LogTemp, Error, L"%s", *m_bonePointList[iNumOfFinger - 5].m_qBoneRotator.ToString());
}

void UTrackingAnimInstance::CheckFingerTriggerEvent(const FFingerFlipTriggerStruct& currentFlipTriggerData, TArray<FFingerFlipTriggerEvent>& fingerTriggerList, TArray<FFingerFlipTriggerEvent*>& outTriggeredEventsList) {
	for (auto& iterator : fingerTriggerList) {
		if (!iterator.m_bIsBinded && 
			(iterator.m_fingerInfo.m_triggerHandType == EEventTriggerHandType::E_Both || iterator.m_fingerInfo.m_triggerHandType == currentFlipTriggerData.m_triggerHandType) && 
			iterator.m_fingerInfo.m_iTriggerFingersFlipState == currentFlipTriggerData.m_iTriggerFingersFlipState) {
			iterator.m_bIsBinded = true;
			outTriggeredEventsList.Add(&iterator);
		}
	}
}

void UTrackingAnimInstance::AddFingerTriggerDelegate(const TCHAR* sEventName, const FTriggerDelegates& eventDelegate) {
	FFingerFlipTriggerStruct** pFindResult = nullptr;
	auto iTriggerNameHashValue = TextKeyUtil::HashString(sEventName);
	pFindResult = m_cachedFingerTriggersDataTable.FindByPredicate([&](const FFingerFlipTriggerStruct* value) {
		return value->m_iTriggerNameHashValue == iTriggerNameHashValue;
		});

	if (pFindResult)
		m_fingerTriggerEventInfoList.Add(FFingerFlipTriggerEvent(*(*pFindResult), eventDelegate));
}

void UTrackingAnimInstance::UpdateFingerTriggeredEvents(const FFingerFlipTriggerStruct& currentFlipTriggerData, TArray<FFingerFlipTriggerEvent*>& triggeredEventsList) {
	for (size_t i = 0; i < triggeredEventsList.Num(); i++) {
		if (triggeredEventsList[i]->m_fingerInfo.m_iTriggerFingersFlipState == currentFlipTriggerData.m_iTriggerFingersFlipState) {
			if (triggeredEventsList[i]->m_fingerInfo.m_triggerType == EEventTriggerType::E_Trigger && triggeredEventsList[i]->m_bIsCalled)
				continue;

			triggeredEventsList[i]->m_bIsCalled = true;
			triggeredEventsList[i]->m_delegatesInfo.m_OnStartDelegate.ExecuteIfBound();
		}
		else {
			const bool bRelease = triggeredEventsList[i]->m_fingerInfo.m_bUseReleaseFlipInfo ? 
								  triggeredEventsList[i]->m_fingerInfo.m_iReleaseFingersFlipState == currentFlipTriggerData.m_iTriggerFingersFlipState : true;
			if (bRelease) {
				triggeredEventsList[i]->m_bIsCalled = false;
				triggeredEventsList[i]->m_bIsBinded = false;
				triggeredEventsList[i]->m_delegatesInfo.m_OnEndDelegate.ExecuteIfBound();
				triggeredEventsList.RemoveAt(i);
			}
		}
	}
}

void UTrackingAnimInstance::UpdateFingerFlipTrigger() {
	CheckFingerFlipTrigger(FMath::Abs(m_rRightHandThumbRotator.Yaw), 35.f, m_currentRightFingerFlipTriggerInfo.m_triggerInputFlipInfo.m_ThumbFlipped);
	CheckFingerFlipTrigger(FMath::Abs(m_rRightHandIndexRotator.Yaw), 35.f, m_currentRightFingerFlipTriggerInfo.m_triggerInputFlipInfo.m_IndexFlipped);
	CheckFingerFlipTrigger(FMath::Abs(m_rRightHandMiddleRotator.Yaw), 35.f, m_currentRightFingerFlipTriggerInfo.m_triggerInputFlipInfo.m_MiddleFlipped);
	CheckFingerFlipTrigger(FMath::Abs(m_rRightHandRingRotator.Yaw), 35.f, m_currentRightFingerFlipTriggerInfo.m_triggerInputFlipInfo.m_RingFlipped);
	CheckFingerFlipTrigger(FMath::Abs(m_rRightHandPinkyRotator.Yaw), 35.f, m_currentRightFingerFlipTriggerInfo.m_triggerInputFlipInfo.m_PinkyFlipped);
	m_currentRightFingerFlipTriggerInfo.UpdateFingerFlipStateValue();

	CheckFingerFlipTrigger(FMath::Abs(m_rLeftHandThumbRotator.Yaw), 35.f, m_currentLeftFingerFlipTriggerInfo.m_triggerInputFlipInfo.m_ThumbFlipped);
	CheckFingerFlipTrigger(FMath::Abs(m_rLeftHandIndexRotator.Yaw), 35.f, m_currentLeftFingerFlipTriggerInfo.m_triggerInputFlipInfo.m_IndexFlipped);
	CheckFingerFlipTrigger(FMath::Abs(m_rLeftHandMiddleRotator.Yaw), 35.f, m_currentLeftFingerFlipTriggerInfo.m_triggerInputFlipInfo.m_MiddleFlipped);
	CheckFingerFlipTrigger(FMath::Abs(m_rLeftHandRingbRotator.Yaw), 35.f, m_currentLeftFingerFlipTriggerInfo.m_triggerInputFlipInfo.m_RingFlipped);
	CheckFingerFlipTrigger(FMath::Abs(m_rLeftHandPinkyRotator.Yaw), 35.f, m_currentLeftFingerFlipTriggerInfo.m_triggerInputFlipInfo.m_PinkyFlipped);
	m_currentLeftFingerFlipTriggerInfo.UpdateFingerFlipStateValue();
}

void UTrackingAnimInstance::CheckFingerFlipTrigger(float fAngle, float fTargetAngle, bool& bFingerTrigger) {
	if (fAngle >= fTargetAngle)
		bFingerTrigger = true;
	else
		bFingerTrigger = false;
}
