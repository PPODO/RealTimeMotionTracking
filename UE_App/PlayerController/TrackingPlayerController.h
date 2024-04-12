#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TrackingPlayerController.generated.h"

UCLASS()
class STARWARS_API ATrackingPlayerController : public APlayerController {
	GENERATED_BODY()
public:
	ATrackingPlayerController();

	void RegisterFingerTriggerEvents(class UTrackingAnimInstance* pAnimInstance, class UForceComponent* pForceComponent);

protected:
	virtual void OnPossess(APawn* aPawn) override;

private:
	// finger command
	UFUNCTION()
	void UseForceActionStart();
	UFUNCTION()
	void UseForceActionEnd();

	UFUNCTION()
	void ForcePushSelectAction();
	UFUNCTION()
	void ForcePullSelectAction();
	UFUNCTION()
	void ForceMovementSelectAction();
	UFUNCTION()
	void ForceTimeFreezeSelectAction();

private:
	class UForceComponent* m_pCachedForceComponent;


};