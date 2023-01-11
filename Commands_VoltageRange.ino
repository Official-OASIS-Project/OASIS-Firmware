// Set voltage ranges in ADC
void C_SetVoltageRange(){
  Serial.println("[OASIS] Setting voltage ranges...");
      
  // Get Voltage Ranges from command
  for(i=0; i<4; i++){
    StringPointer = strtok(NULL, delimiter);
    VoltageRangeSet[i] = int(strtof(StringPointer, NULL));

    if((ADC_BITS==16)&&(VoltageRangeSet[i]==1||VoltageRangeSet[i]==3||VoltageRangeSet[i]==5)){
      Serial.print("[OASIS] Error - Invalid voltage range for channel ");
      Serial.print(i+1);
      Serial.print(": ");
      Serial.print(VoltageRangeSet[i]);
      Serial.println(". Setting previous value!");
      ledError();
      Sound_Bad_Command();
    }
    else if(VoltageRangeSet[i]==1){
      VoltageRange[i] = 2.5;
    }
    else if(VoltageRangeSet[i]==2){
      VoltageRange[i] = 5;
    }
    else if(VoltageRangeSet[i]==3){
      VoltageRange[i] = 6.25;
    }
    else if(VoltageRangeSet[i]==4){
      VoltageRange[i] = 10;
    }
    else if(VoltageRangeSet[i]==5){
      VoltageRange[i] = 12.5;
    }
    else{
      Serial.print("[OASIS] Error - Invalid voltage range for channel ");
      Serial.print(i+1);
      Serial.print(": ");
      Serial.print(VoltageRangeSet[i]);
      Serial.println(". Setting previous value!");
      ledError();
      Sound_Bad_Command();
    }
    
    Serial.print("[OASIS] Voltage Range for channel ");
    Serial.print(i+1);
    Serial.print(": ");
    Serial.print(VoltageRange[i]);
    Serial.println("V");
  }

  // Write voltage range to ADC
  if(ADC_BITS==16){
    if(VoltageRangeSet[1]!=VoltageRangeSet[0] || VoltageRangeSet[2]!=VoltageRangeSet[0] || VoltageRangeSet[3]!=VoltageRangeSet[0]){
      Serial.print("[OASIS] Error - All voltage ranges must be identical for ADC7606-4. Applying range ");
      Serial.print(VoltageRange[0]);
      Serial.println("V to all channels.");
      for(int i=1; i<4; i++){
        VoltageRange[i] = VoltageRange[0];
      }
      ledError();
      Sound_Bad_Command();
    }
    Serial.println("[OASIS] Setting voltage ranges...");
    if(VoltageRange[0]==5){
      digitalWrite(RANGE, LOW);
    }
    else{
      digitalWrite(RANGE, HIGH);
    }
  }
  if(ADC_BITS==18){
    Serial.println("[OASIS] Setting voltage ranges in ADC...");
    writeADCConfig(0x03, (Range[VoltageRangeSet[1]-1] << 4) + Range[VoltageRangeSet[0]-1]);
    writeADCConfig(0x04, (Range[VoltageRangeSet[3]-1] << 4) + Range[VoltageRangeSet[2]-1]);
  }

  Serial.println("[OASIS] Voltage ranges set.");
  Serial.println();

  if(OASISWiFi){
    udp.beginPacket(udpTargetIP,udpPort);
    udp.print("[OASIS] Set voltage ranges: Ch1: ");
    udp.print(VoltageRange[0]);
    udp.print("V, Ch2: ");
    udp.print(VoltageRange[1]);
    udp.print("V, Ch3: ");
    udp.print(VoltageRange[2]);
    udp.print("V, Ch4: ");
    udp.print(VoltageRange[3]);
    udp.print("V");
    udp.endPacket();
  }
}

// Get voltage ranges from ADC
void C_GetVoltageRange(){
  Serial.println("[OASIS] Printing voltage ranges...");
      
  // Get Voltage Ranges
  for(i=0; i<4; i++){
    Serial.print("[OASIS] Voltage Range for channel ");
    Serial.print(i+1);
    Serial.print(": ");
    Serial.print(VoltageRange[i]);
    Serial.println("V");
  }
  Serial.println();
}
