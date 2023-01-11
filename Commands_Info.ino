// Displays information about the device
void C_Info(){
  Serial.println("--------------------------------------------------------------------------");
  Serial.println("                          OASIS Device Information                        ");
  Serial.println("--------------------------------------------------------------------------");
  
  // Hardware Version
  Serial.print("[OASIS] Device Hardware Version is ");
  Serial.print(HW_MAJOR);
  Serial.print(".");
  Serial.println(HW_MINOR);

  // Firmware Version
  Serial.print("[OASIS] Device Firmware Version is ");
  Serial.print(FW_MAJOR);
  Serial.print(".");
  Serial.println(FW_MINOR);

  // ADC Resolution
  Serial.print("[OASIS] ADC resolution: ");
  Serial.println(ADC_BITS);

  // Feature - TEDS
  Serial.print("[OASIS] TEDS module installed (1=yes, 0=no): ");
  Serial.println(F_TEDS);

  // Feature - WSS
  Serial.print("[OASIS] WSS module installed (1=yes, 0=no): ");
  Serial.println(F_WSS);

  // Custom Device Name
  Serial.print("[OASIS] Device name: ");
  Serial.print(DeviceName);
  Serial.println();
  Serial.println("--------------------------------------------------------------------------");
}

// Returns raw values over serial for GUI
void C_RawInfo(){
  // Hardware Version
  Serial.print(HW_MAJOR);
  Serial.print(".");
  Serial.print(HW_MINOR);
  Serial.print(";");

  // Firmware Version
  Serial.print(FW_MAJOR);
  Serial.print(".");
  Serial.print(FW_MINOR);
  Serial.print(";");
  
  // ADC Resolution
  Serial.print(ADC_BITS);
  Serial.print(";");

  // Feature - TEDS
  Serial.print(F_TEDS);
  Serial.print(";");

  // Feature - WSS
  Serial.print(F_WSS);
  Serial.print(";");

  // Custom Device Name
  Serial.print(DeviceName);
  Serial.println(";");
}
