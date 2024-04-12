#include "TrackingPlayerController.h"
#include "../Characters/TrackingCharacter.h"
#include "../AnimInstance/TrackingAnimInstance.h"
#include "../MotionTracking/MotionTrackingComponent.h"
#include "../Weapon/Force/ForceComponent.h"

ATrackingPlayerController::ATrackingPlayerController() {
}

void ATrackingPlayerController::OnPossess(APawn* aPawn) {
	Super::OnPossess(aPawn);


}

void ATrackingPlayerController::RegisterFingerTriggerEvents(UTrackingAnimInstance* pAnimInstance, class UForceComponent* pForceComponent) {
	if (pAnimInstance && pForceComponent) {
		FTriggerDelegates forcepushSelectDelegate;
		forcepushSelectDelegate.m_OnStartDelegate.BindUFunction(this, TEXT("ForcePushSelectAction"));

		FTriggerDelegates forcepullSelectDelegate;
		forcepullSelectDelegate.m_OnStartDelegate.BindUFunction(this, TEXT("ForcePullSelectAction"));

		FTriggerDelegates forceMovementSelectDelegate;
		forceMovementSelectDelegate.m_OnStartDelegate.BindUFunction(this, TEXT("ForceMovementSelectAction"));

		FTriggerDelegates forceTimeFreezeSelectDelegate;
		forceTimeFreezeSelectDelegate.m_OnStartDelegate.BindUFunction(this, TEXT("ForceTimeFreezeSelectAction"));

		FTriggerDelegates useForceActionDelegate;
		useForceActionDelegate.m_OnStartDelegate.BindUFunction(this, TEXT("UseForceActionStart"));
		useForceActionDelegate.m_OnEndDelegate.BindUFunction(this, TEXT("UseForceActionEnd"));

		FTriggerDelegates lightsaberHoldingActionDelegate;
		lightsaberHoldingActionDelegate.m_OnStartDelegate.BindUFunction(this, TEXT("LightsaberHoldingActionStart"));
		lightsaberHoldingActionDelegate.m_OnEndDelegate.BindUFunction(this, TEXT("LightsaberHoldingActionEnd"));


		pAnimInstance->AddFingerTriggerDelegate(TEXT("ForcePushSelectAction"), forcepushSelectDelegate);
		pAnimInstance->AddFingerTriggerDelegate(TEXT("ForcePullSelectAction"), forcepullSelectDelegate);
		pAnimInstance->AddFingerTriggerDelegate(TEXT("ForceMovementSelectAction"), forceMovementSelectDelegate);
		pAnimInstance->AddFingerTriggerDelegate(TEXT("ForceTimeFreezeSelectAction"), forceTimeFreezeSelectDelegate);
		pAnimInstance->AddFingerTriggerDelegate(TEXT("UseForceAction"), useForceActionDelegate);
		pAnimInstance->AddFingerTriggerDelegate(TEXT("LightsaberHoldingAction"), lightsaberHoldingActionDelegate);

		m_pCachedForceComponent = pForceComponent;
	}
}

void ATrackingPlayerController::UseForceActionStart() {
	if (m_pCachedForceComponent) {
		m_pCachedForceComponent->SetSelectMode(true);
		
		const auto forceType = m_pCachedForceComponent->GetForceType();
		switch (forceType) {
		case EForceType::E_ForcePush:
			break;
		case EForceType::E_ForcePull:
			m_pCachedForceComponent->TraceStartForForcePull();
			break;
		case EForceType::E_ForceMovement:
			m_pCachedForceComponent->InitForceMovement(GetCharacter()->GetMesh()->GetSocketRotation(TEXT("LeftHand")));
			break;
		case EForceType::E_ForceFreeze:
			m_pCachedForceComponent->ActiveForce();
			break;
		}
	}
}

void ATrackingPlayerController::UseForceActionEnd() {
	if (m_pCachedForceComponent) {
		m_pCachedForceComponent->SetSelectMode(false);
		m_pCachedForceComponent->ActiveForce();
	}
}

void ATrackingPlayerController::ForcePushSelectAction() {
	if (m_pCachedForceComponent) 
		m_pCachedForceComponent->SetForceType(EForceType::E_ForcePush);
}

void ATrackingPlayerController::ForcePullSelectAction() {
	if (m_pCachedForceComponent)
		m_pCachedForceComponent->SetForceType(EForceType::E_ForcePull);
}

void ATrackingPlayerController::ForceMovementSelectAction() {
	if (m_pCachedForceComponent)
		m_pCachedForceComponent->SetForceType(EForceType::E_ForceMovement);
}

void ATrackingPlayerController::ForceTimeFreezeSelectAction() {
	if (m_pCachedForceComponent)
		m_pCachedForceComponent->SetForceType(EForceType::E_ForceFreeze);
}