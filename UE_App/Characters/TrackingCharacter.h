#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../MotionTracking/MotionTrackingComponent.h"
#include "../Weapon/Force/ForceComponent.h"
#include "TrackingCharacter.generated.h"

UCLASS()
class STARWARS_API ATrackingCharacter : public ACharacter {
	GENERATED_BODY()
public:
	ATrackingCharacter();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

private:
	UFUNCTION()
		void ChangeMotionTracking(const bool bNewValue);
	UFUNCTION()
		void UpdateAnimBoneRotate(const EBPBoneType boneType, const TArray<struct FSkeletonInformation>& skeletonInfoList, float fSmoothDeltaTime, float fSmoothInterpSpeed);
	UFUNCTION()
		void UpdateAnimFingerBoneRotate(const EBPBoneType boneType, const struct FSensorSkeletonFingerBoneListStruct& fingerRotationInfo, float fSmoothDeltaTime, float fSmoothInterpSpeed);

private:
	UFUNCTION()
		void LightsaberHoldingActionStart();
	UFUNCTION()
		void LightsaberHoldingActionEnd();

private:
	UPROPERTY(VisibleDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		class UMotionTrackingComponent* m_pMotionTrackingComponent;
	UPROPERTY(VisibleDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* m_pCameraComponent;
	UPROPERTY(VisibleDefaultsOnly, meta = (AllowPrivateAccess = "true"))
		class UForceComponent* m_pForceComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UChildActorComponent* m_pLightsaberChildActorComponent;
		
	UPROPERTY(EditAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
		class UStaticMesh* m_pLightsaberMesh;
	
	class ABaseLightsaber* m_pCachedLightsaber;
	FName m_sWeaponUnequipSocketName;
	FName m_sWeaponEquipSocketName;

	class UTrackingAnimInstance* m_pAnimInstance;
};