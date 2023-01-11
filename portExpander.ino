// Configure Port Expander
void initpEXP(){
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.beginTransmission(0x70>>1);
  Wire.write(0x01); // Write all outputs low
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.beginTransmission(0x70>>1);
  Wire.write(0x03); // Select configuration register
  Wire.write(0x00); // Set all ports as output
  Wire.endTransmission();
  delay(600);
}
