// Initialize WSS by configuring the CC1101 chip
void initWSS(){
  pinMode(CC_GDO2, INPUT);
  pinMode(VSPI_CS, OUTPUT);
  digitalWrite(VSPI_CS, HIGH);

  // Reset chip
  CC_SendCommand(0x30); // SRES

  delay(10);

  // Set GDO2 to Sync Word Detection
  CC_WriteRegister(0x00, 0x06);
  
  // Set GDO0 to CHIP_RDYn
  CC_WriteRegister(0x02, 0x29);

  // Change Sync Word
  CC_WriteRegister(0x04, 'A');
  CC_WriteRegister(0x05, 'M');

  // Disable Power Saving; RX -> Stay in RX; TX -> FSTXON
  CC_WriteRegister(0x17, 0x3D);

  // Enable 30/32 sync word + carrier sense
  CC_WriteRegister(0x12, 0x07);
}

// Set CC1101 to recieve mode
void WSS_listen(){
  // Check if RXFIFO_OVERFLOW, acknowledge if necessary
  if ((CC_ReadRegister(0x35)>>4)==6){
      CC_SendCommand(0x3A); // SFTX
    }
  CC_SendCommand(0x34); // SRX
}

// Send WSS impulse
void WSS_send(){
  // Check if TXFIFO_UNDERFLOW, acknowledge if necessary
  if ((CC_ReadRegister(0x35)>>4)==7){
    CC_SendCommand(0x3B);
  }
  CC_WriteRegister(0x3F,0xFF);
  CC_SendCommand(0x35); // STX
}

// Read CC1101 register at Addr
byte CC_ReadRegister(byte Addr){
  digitalWrite(VSPI_CS, LOW);
  OASIS_CC_SPI->transfer(Addr + 128);
  RegisterData = OASIS_CC_SPI->transfer(0x00);
  digitalWrite(VSPI_CS, HIGH);
  return RegisterData;
}

// Write CC1101 register at Addr
void CC_WriteRegister(byte Addr, byte RegisterData){
  digitalWrite(VSPI_CS, LOW);
  OASIS_CC_SPI->transfer(Addr);
  OASIS_CC_SPI->transfer(RegisterData);
  digitalWrite(VSPI_CS, HIGH);
}

// Send command byte to CC1101
void CC_SendCommand(byte CMD){
  digitalWrite(VSPI_CS, LOW);
  OASIS_CC_SPI->transfer(CMD);
  digitalWrite(VSPI_CS, HIGH);
}
