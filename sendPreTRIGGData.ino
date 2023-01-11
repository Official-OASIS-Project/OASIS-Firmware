void sendPreTRIGGData(int source){
  PreCacheIndex++;
  if(source==1){
    Serial.println("<>");
    for(i=PreCacheIndex; i<PRECACHE_SIZE; i++){
      for(k=0; k<BYTES_PER_SAMPLE; k++){
        Serial.write(OASISPreCache[i][k]);
      }
    }
    for(i=0; i<PreCacheIndex; i++){
      for(k=0; k<BYTES_PER_SAMPLE; k++){
        Serial.write(OASISPreCache[i][k]);
      }
    }
  }
  if(source==2){
    udp.beginPacket(udpTargetIP,udpPort);
    udp.println("<>");
    udp.endPacket();

    udp.beginPacket(udpTargetIP,udpPort);
    for(i=PreCacheIndex; i<PRECACHE_SIZE; i++){
      for(k=0; k<BYTES_PER_SAMPLE; k++){
        udp.write(OASISPreCache[i][k]);
      }
    }
    for(i=0; i<PreCacheIndex; i++){
      for(k=0; k<BYTES_PER_SAMPLE; k++){
        udp.write(OASISPreCache[i][k]);
      }
    }
    udp.endPacket();
  }
}
