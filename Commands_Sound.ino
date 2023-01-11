// Mute the buzzer
void C_Mute(){
  EEPROM.write(0, 1); // Write variable to address 0x00
  EEPROM.commit();
  OASISMute = EEPROM.read(0);
  Serial.println("[OASIS] Muted buzzer.");
}

// Unmute the buzzer
void C_Unmute(){
  EEPROM.write(0, 0); // Write variable to address 0x00
  EEPROM.commit();
  OASISMute = EEPROM.read(0);
  Serial.println("[OASIS] Unmuted buzzer.");
  StartupSound();
}

// Startup Sound
void StartupSound(){
  Wire.beginTransmission(0x70>>1);
  Wire.write(0x01); // Select output register
  Wire.write(0x10);
  Wire.endTransmission();
  OASISPaw.tone(500, 120);

  Wire.beginTransmission(0x70>>1);
  Wire.write(0x01); // Select output register
  Wire.write(0x30);
  Wire.endTransmission();
  OASISPaw.tone(600, 120);

  Wire.beginTransmission(0x70>>1);
  Wire.write(0x01); // Select output register
  Wire.write(0x70);
  Wire.endTransmission();
  OASISPaw.tone(700, 120);

  Wire.beginTransmission(0x70>>1);
  Wire.write(0x01); // Select output register
  Wire.write(0xF0);
  Wire.endTransmission();
  OASISPaw.tone(800, 120);
}

// Sample Start Sound
void Sound_Sample_Start(){
  OASISPaw.tone(1047, 125);  
}

// Sample End Sound
void Sound_Sample_End(){
  OASISPaw.tone(947, 125); 
}

void Sound_Fatal_Error(){
  OASISPaw.tone(1047, 300);
  delay(300);
  OASISPaw.tone(1047, 300);
  delay(300);
  OASISPaw.tone(1047, 300);
}

void Sound_Bad_Command(){
  OASISPaw.tone(1047, 200);
  delay(100);
  OASISPaw.tone(1047, 500);
}

void Sound_WSS_armed(){
  OASISPaw.tone(500, 100);
  delay(100);
  OASISPaw.tone(500, 200);
}
