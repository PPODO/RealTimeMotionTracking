#pragma once
#include <HardwareSerial.h>

class ESPUART {
  static const uint16_t SERIAL_BUFFER_LENGTH = 16;
public:
  ESPUART(const uint8_t iRXPin, const uint8_t iTXPin);

  void Begin(const uint16_t iBaud);

  uint8_t Loop();

  inline size_t Write(const char* sCommand) {
    return m_UARTSerial.write(sCommand);
  }

private:
  inline bool ClampSerialCommand(const char* const sCommand) {
    if(strncmp(m_sSerialBuffer, sCommand, 6) == 0) 
      return true;
    return false;
  }

private:
  uint8_t m_iRXPin, m_iTXPin;
  uint16_t m_iCurrentReadedBytes;

  HardwareSerial m_UARTSerial;
  char m_sSerialBuffer[SERIAL_BUFFER_LENGTH];

};