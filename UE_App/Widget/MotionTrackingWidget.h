#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MotionTrackingWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSendInitPacketToSensor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSendCalibrationPacketToSensor);

UCLASS()
class STARWARS_API UMotionTrackingWidget : public UUserWidget {
	GENERATED_BODY()
public:
	// for test
	UFUNCTION(BlueprintCallable)
		void EnableButtonClicked();

	void ChangeCalibrationState();
	void ChangeInitializationState();
	void ChangeButtonEnable(bool bEnable);

	void ChangeCalibrationProcessState(const FString& text);

public:
	FSendInitPacketToSensor m_sendInitPacketDelegate;
	FSendCalibrationPacketToSensor m_sendCalibrationPacketDelegate;

private:
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	FString m_sButtonText = "Initialization";
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	FString m_sCalibrationProcessText;
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	bool m_bIsButtonEnable = true;

	FThreadSafeBool m_bIsCalibrationState = false;

};