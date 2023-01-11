// Switch RGB LED to green
void ledReady() {
  Wire.beginTransmission(0x70>>1);
  Wire.write(0x01); // Select output register
  Wire.write(0x00);
  Wire.endTransmission();
  OASISPaw.writeRGB(0,35,0);
}

// Switch RGB LED to blue
void ledBlue(){
  OASISPaw.writeRGB(0,0,35);
}

// Switch RGB LED to purple
void ledPurple(){
  OASISPaw.writeRGB(35,0,35);
}

// Switch RGB LED to cyan
void ledCyan(){
  OASISPaw.writeRGB(0,35,35);
}

// Switch RGB LED to yellow
void ledWaiting(){
  OASISPaw.writeRGB(35,35,0);
}

// Switch RGB LED to red and lock state
void ledError(){
  LEDError = true;
  OASISPaw.writeRGB(35,0,0);
}
