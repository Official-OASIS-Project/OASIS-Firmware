// Function to read TEDS and return contents over serial interface - Not implemented yet
void C_ReadTEDS(){
  // Init TEDS reading on Channel X

  // SwitchChannelXToTEDS(); (not implemented yet)
  Serial.print("TEDS read status: ");
  Serial.println(teddy.read());
  Serial.print("ROM device type: ");
  Serial.println(teddy.getROMDeviceType());
  Serial.print("Transducer manufacturer: ");
  Serial.println(teddy.getManufacturer());
  Serial.print("Transducer type: ");
  Serial.println(teddy.getTransducerType());
  Serial.print("Serial number: ");
  unsigned long int SerialNum = teddy.getSerialNumber();
  Serial.println(SerialNum);
  Serial.print("Sensor direction: ");
  Serial.println(teddy.getSensorDirection());
  Serial.print("Sensor sensitivity [V/(m/s^2)]: ");
  Serial.printf("%.8f", teddy.getSensitivity());
  Serial.println();
  Serial.print("Sensor sensitivity [mV/g]: ");
  Serial.printf("%.8f",teddy.getSensitivity()*9806.65);
  Serial.println();
  Serial.print("Calibration date (days since January 1st, 1998): ");
  Serial.println(teddy.getCalibrationDate());
  Serial.println();
  // SwitchChannelXToICP(); (not implemented yet)
}
