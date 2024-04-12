#include "ESPUART.h"

ESPUART::ESPUART(const uint8_t iRXPin, const uint8_t iTXPin) : m_UARTSerial(2), m_iRXPin(iRXPin), m_iTXPin(iTXPin), m_iCurrentReadedBytes(0) {}

void ESPUART::Begin(const uint16_t iBaud) {
  m_UARTSerial.begin(iBaud, SERIAL_8N1, m_iRXPin, m_iTXPin);
}

uint8_t ESPUART::Loop() {
  uint8_t result = 0;

  if(m_UARTSerial.available()) {
    m_iCurrentReadedBytes += m_UARTSerial.readBytesUntil('\0', m_sSerialBuffer, SERIAL_BUFFER_LENGTH);
    Serial.println(m_sSerialBuffer);
    if(m_iCurrentReadedBytes >= 6) {
      if(ClampSerialCommand("E:INIT")) 
        result = 3;
      else if(ClampSerialCommand("E:CALI")) 
        result = 2;
      else if(ClampSerialCommand("E:HAND"))
        result = 5;

      m_iCurrentReadedBytes = 0;
    }
  }
  return result;
}