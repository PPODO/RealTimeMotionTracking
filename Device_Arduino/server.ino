#include "Wire.h"
#include "MotionTrackingSensor.h"
#include "HandTrackingSensor.h"

const char * g_Ssid = "KT_GiGA_2G_Wave2_61E8";
const char * g_Password = "3akc7hc259";
const char * g_HostAddress = "172.30.1.27";
const int g_HostPort = 3550;

// tracking sensor
// upper, lower, end

extern const uint8_t MAX_NUM_OF_SENSORS;

uint8_t g_iPinNums[MAX_NUM_OF_SENSORS] = { 32, 5, 33 };
MotionTracking_Sensor g_motionTracking((uint8_t)EBoneType::E_Right_Arm, false, 3);
TrackingSensor_Packet g_TrackingSensorData[MAX_NUM_OF_SENSORS] = { TrackingSensor_Packet(EBoneType::E_Right_Arm, EBonePointType::E_Upper), TrackingSensor_Packet(EBoneType::E_Right_Arm, EBonePointType::E_Lower), TrackingSensor_Packet(EBoneType::E_Right_Arm, EBonePointType::E_End) };


// flex sensor right hand is 26 18, left 18 26
/*
extern const uint8_t MAX_OF_FINGER;

uint8_t g_iPinNums[MAX_OF_FINGER] = { 34, 39, 35, 33, 32 };
HandTracking_Sensor g_handTracking((uint8_t)EBoneType::E_Left_Finger, false);
*/

void setup() {
  Serial.begin(115200);

  Wire.begin();
  Wire.setClock(100000);
  Wire.setTimeout(3000);

  WiFi.mode(WIFI_STA);
  WiFi.begin(g_Ssid, g_Password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("WiFi Failed!");
      while(1) delay(1000);
  }

  delay(1000);
  
//  g_handTracking.Setup(g_HostAddress, g_HostPort, g_iPinNums);
  g_motionTracking.Setup(g_HostAddress, g_HostPort, g_TrackingSensorData);
  g_motionTracking.SetSensorAddressPin(g_iPinNums);
}

void loop() {
//  g_handTracking.Loop();
  g_motionTracking.Loop();
}

// 두가지 방법.
// 1. 모든 센서 데이터가 모였을 때 전송하느냐
// 2. 한 번 루프 돌 때마다 성공한 것만 모아서 전송하느냐
// 트래픽을 생각하면 아무래도 1번이 훨 좋긴 할 듯,.