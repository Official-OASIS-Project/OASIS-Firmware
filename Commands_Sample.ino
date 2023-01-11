// Sample routine
void C_Sample(int source){

  resetVars();
  
  // Get Acquisition parameters
  StringPointer = strtok(NULL, delimiter);
  t_sample = strtof(StringPointer, NULL);
  StringPointer = strtok(NULL, delimiter);
  f_sample = strtof(StringPointer, NULL);
  StringPointer = strtok(NULL, delimiter);
  TRIGG_LEVEL = strtof(StringPointer, NULL);
  StringPointer = strtok(NULL, delimiter);
  sync_mode = strtof(StringPointer, NULL);
  T_sample = 1/f_sample*1000000; //Sample periode in microseconds
  n_sample = f_sample*t_sample;

  if(n_sample>PS_CACHE_SIZE*BYTES_PER_SAMPLE){
    Serial.print("[OASIS] FATAL ERROR: Sample size too long to fit into buffer. Aborting data acquisition.");
    ledError();
    Sound_Fatal_Error();
    return;
  }
  
  OASISPaw.setTimer(T_sample);

  // Switch between normal and triggered mode
  if(TRIGG_LEVEL==0){
    TRIGG = 1;
  }
  else {
    TRIGG = 0;
  }

  // Filter triggered mode if synchronization enabled
  if(sync_mode){
    TRIGG = 1;
  }

  // Confirming Acquisition parameters (Serial)
  Serial.print("[OASIS] Sampling for ");
  Serial.print(t_sample);
  Serial.print(" second(s) with ");
  Serial.print(f_sample);
  Serial.println(" Hz.");
  if(!TRIGG){
    Serial.print("[OASIS] Waiting for TRIGGER on channel 1; when surpassing ");
    Serial.print(TRIGG_LEVEL);
    Serial.println(" V ...");  
  }

  // Confirming Acquisition parameters (UDP)
  if(source==2){
    udp.beginPacket(udpTargetIP,udpPort);
    udp.print("[OASIS] Sampling for ");
    udp.print(t_sample);
    udp.print(" second(s) with ");
    udp.print(f_sample);
    udp.println(" Hz.");
    if(!TRIGG){
      udp.print("[OASIS] Waiting for TRIGGER on channel 1; when surpassing ");
      udp.print(TRIGG_LEVEL);
      udp.println(" V ...");
    }
    udp.endPacket();
  }

  // Set send API
  if(source==1){
    SendSerial = true;
  }
  if(source==2){
    SendUDP = true;
  }

  // Announce Data Incoming for normal sampling
  //if(TRIGG){
    if(source==1){
      Serial.println("<>");
    }
    if(source==2){
      Serial.println("[OASIS] Data incoming on UDP.");
      udp.beginPacket(udpTargetIP,udpPort);
      udp.println("<>");
      udp.endPacket(); 
    }
  //}

  xTaskCreatePinnedToCore(SendData, "DataSendTask", 10000, NULL, 0, &DataSender, 0);
  if(sync_mode==0){
    // Start Sampling
    ledBlue();
    Sound_Sample_Start();
    OASISPaw.enableTimer();
  }
  else {
    attachInterrupt(CC_GDO2, WSS_ISR, RISING);
    
    // Trigger Sync Source
    if(sync_mode==1){
      ledCyan();
      Sound_Sample_Start();
      WSS_send();
    }
    // Arm Sync Sink
    else{
      ledPurple();
      Sound_WSS_armed();
      WSS_listen();
    }
  }

  // Sample Loop
  while(sampleCount<n_sample){
    if(OASISPaw.FaultFlag){
        OASISPaw.disableTimer();
        resetVars();
        Serial.print("[OASIS] FATAL ERROR: Sampling too fast! Data processing did not finish in time.");
        Serial.println();
        ledError();
        Sound_Fatal_Error();
        break;
      }
    if(OASISPaw.InterruptFlag){
      OASISPaw.InterruptFlag = 0;
      if(TRIGG){sampleCount++;}
      OASISPaw.OASISBusy = 0;
    }
  }

  // Disable Sampling
  OASISPaw.disableTimer();
  if(sync_mode){
    detachInterrupt(CC_GDO2);
  }
  ledWaiting();
  Sound_Sample_End();

  // Handle last data packet
  while(PacketToSend){
    delay(1); // Wait
  }
  if(CacheIndex){
    SendLastPacket = true;
  }
  while(SendLastPacket){
    delay(1); // Wait
  }
  vTaskDelete(DataSender);

  // Send Data from PSRAM - Not implemented yet
  //for(long l=0; l<n_sample*8; l++){
  //  Serial.write(OASISBuffer[l]);
  //}

  // Send preTRIGG data
  if(TRIGG_LEVEL!=0){
    while(true){
      if(Serial.available()){
        Command = Serial.readString();
        Command.toCharArray(StrValues,42);
        StringPointer = strtok(StrValues, delimiter);
        Command = String(StringPointer);
      }
      if(udp.parsePacket()){
        udp.read(udpBuffer, 255);
        StringPointer = strtok(udpBuffer, delimiter);
        Command = String(StringPointer);
      }
      if(Command=="Drq"){
          break;
      }
    }
    sendPreTRIGGData(source);
  }

  if(source==2){
    Serial.println("[OASIS] Finished Data Acquisition over UDP.");
    Serial.println();
  }
  ledReady();
}
