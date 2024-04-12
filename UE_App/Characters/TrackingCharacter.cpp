#include "TrackingCharacter.h"
#include "../Weapon/Lightsaber/BaseLightsaber.h"
#include "../AnimInstance/TrackingAnimInstance.h"
#include "../PlayerController/TrackingPlayerController.h"
#include <Components/ChildActorComponent.h>
#include <Camera/CameraComponent.h>
#include <UObject/ConstructorHelpers.h>

ATrackingCharacter::ATrackingCharacter() {
	::ConstructorHelpers::FObjectFinder<USkeletalMesh> mesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Luke/Luke.Luke'"));
	::ConstructorHelpers::FClassFinder<UTrackingAnimInstance> animInst(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprint/AnimInstance/BP_LukeAnimInstance'"));
	if (mesh.Succeeded() && animInst.Succeeded()) {
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
		GetMesh()->SetSkeletalMesh(mesh.Object);
		GetMesh()->SetAnimInstanceClass(animInst.Class);
	}

	m_pCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	m_pCameraComponent->SetRelativeLocation(FVector(-10.f, 4.5f, 0.f));
	m_pCameraComponent->SetRelativeRotation(FRotator(0.f, 90.f, -90.f));
	m_pCameraComponent->SetupAttachment(GetMesh(), TEXT("Head"));

	m_pMotionTrackingComponent = CreateDefaultSubobject<UMotionTrackingComponent>(TEXT("Motion Tracking Component"));
	m_pMotionTrackingComponent->m_changeMotionTrackingDelegate.BindUFunction(this, TEXT("ChangeMotionTracking"));
	m_pMotionTrackingComponent->m_updateAnimBoneDelegate.BindUFunction(this, TEXT("UpdateAnimBoneRotate"));
	m_pMotionTrackingComponent->m_updateAnimFingerDelegate.BindUFunction(this, TEXT("UpdateAnimFingerBoneRotate"));

	m_pForceComponent = CreateDefaultSubobject<UForceComponent>(TEXT("Force Component"));
	m_pForceComponent->SetupAttachment(RootComponent);
	m_pForceComponent->SetAttachmentSelectSocketComponent(m_pCameraComponent);
	m_pForceComponent->SetAttachmentRootComponent(m_pForceComponent);

	/*m_sWeaponUnequipSocketName = TEXT("LightsaberSocket");			have gravity issus
	m_sWeaponEquipSocketName = TEXT("LightsaberEquip");
	m_pLightsaberChildActorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Lightsaber Child Actor Component"));
	m_pLightsaberChildActorComponent->SetChildActorClass(ABaseLightsaber::StaticClass());
	m_pLightsaberChildActorComponent->SetWorldScale3D(FVector(0.35f));
	m_pLightsaberChildActorComponent->SetupAttachment(GetMesh(), m_sWeaponUnequipSocketName);*/

	PrimaryActorTick.bCanEverTick = false;
}

void ATrackingCharacter::OnConstruction(const FTransform& Transform) {
	Super::OnConstruction(Transform);

	/*if (auto pCachedLightsaber = Cast<ABaseLightsaber>(m_pLightsaberChildActorComponent->GetChildActor())) {
		pCachedLightsaber->SetSaberBodyMesh(m_pLightsaberMesh);
	}*/
}

void ATrackingCharacter::BeginPlay() {
	Super::BeginPlay();
	
	if (auto pCachedPC = Cast<ATrackingPlayerController>(GetController())) {
		if (auto pCachedAnimInst = CastChecked<UTrackingAnimInstance>(GetMesh()->GetAnimInstance())) {
			pCachedPC->RegisterFingerTriggerEvents(pCachedAnimInst, m_pForceComponent);
			m_pAnimInstance = pCachedAnimInst;
		}
	}
	//m_pCachedLightsaber = Cast<ABaseLightsaber>(m_pLightsaberChildActorComponent->GetChildActor());
}

void ATrackingCharacter::ChangeMotionTracking(const bool bNewValue) {
	if (m_pAnimInstance)
		m_pAnimInstance->ChangeMotionTrackingState(bNewValue);
}

void ATrackingCharacter::UpdateAnimBoneRotate(const EBPBoneType boneType, const TArray<struct FSkeletonInformation>& skeletonInfoList, float fSmoothDeltaTime, float fSmoothInterpSpeed) {
	if (m_pAnimInstance)
		m_pAnimInstance->UpdateRotateData(boneType, skeletonInfoList, fSmoothDeltaTime, fSmoothInterpSpeed);
}

void ATrackingCharacter::UpdateAnimFingerBoneRotate(const EBPBoneType boneType, const FSensorSkeletonFingerBoneListStruct& fingerRotationInfo, float fSmoothDeltaTime, float fSmoothInterpSpeed) {
	if (m_pAnimInstance)
		m_pAnimInstance->UpdateRotateData(boneType, fingerRotationInfo, fSmoothDeltaTime, fSmoothInterpSpeed);
}

void ATrackingCharacter::LightsaberHoldingActionStart() {
	if (m_pCachedLightsaber)
		m_pCachedLightsaber->AttachmentLightsaber(m_sWeaponEquipSocketName, true);
}

void ATrackingCharacter::LightsaberHoldingActionEnd() {
	if (m_pCachedLightsaber)
		m_pCachedLightsaber->AttachmentLightsaber(m_sWeaponUnequipSocketName, false);
}