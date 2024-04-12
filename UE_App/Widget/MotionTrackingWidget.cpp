#include "MotionTrackingWidget.h"

void UMotionTrackingWidget::EnableButtonClicked() {
	if (m_bIsCalibrationState) {
		if (m_sendCalibrationPacketDelegate.IsBound())
			m_sendCalibrationPacketDelegate.Broadcast();
	}
	else if (m_sendInitPacketDelegate.IsBound())
		m_sendInitPacketDelegate.Broadcast();
}

void UMotionTrackingWidget::ChangeCalibrationState() {
	m_sButtonText = "Calibration";
	m_bIsCalibrationState = true;
}

void UMotionTrackingWidget::ChangeInitializationState() {
	m_sButtonText = "Initialization";
	m_bIsCalibrationState = false;
}

void UMotionTrackingWidget::ChangeButtonEnable(bool bEnable) {
	m_bIsButtonEnable = bEnable;
}

void UMotionTrackingWidget::ChangeCalibrationProcessState(const FString& text) {
	m_sCalibrationProcessText = text;
}