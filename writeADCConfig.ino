// Write ADC Config
void writeADCConfig(byte Addr, byte RegisterData){
  
  // Enable writing by reading a register
  digitalWrite(HSPI_CS, LOW);
  OASISSPI->transfer16(0b0100001000000000);
  digitalWrite(HSPI_CS, HIGH);
  
  // Write register
  digitalWrite(HSPI_CS, LOW);
  OASISSPI->transfer(byteWriteADC+Addr);
  OASISSPI->transfer(RegisterData);
  digitalWrite(HSPI_CS, HIGH);
  
  // Read back written register
  digitalWrite(HSPI_CS, LOW);
  OASISSPI->transfer(byteReadADC+Addr);
  ADCReadback = OASISSPI->transfer(0b00000000);
  digitalWrite(HSPI_CS, HIGH);

  // Exit Register Mode
  digitalWrite(HSPI_CS, LOW);
  OASISSPI->transfer16(0b0000000000000000);
  digitalWrite(HSPI_CS, HIGH);

  // Check written Register
  if(ADCReadback==RegisterData){
    Serial.print("[OASIS] Successfully written 0x");
    Serial.print(RegisterData, HEX);
    Serial.print(" to address 0x");
    Serial.print(Addr, HEX);
    Serial.println();
    return;
  }
  else{
    Serial.println("[OASIS] FATAL ERROR: Register write or readback failed!");
    ledError();
    Sound_Fatal_Error();
  }
}
