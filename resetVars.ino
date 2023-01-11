void resetVars(){
  sampleCount = 0;
  OASISPaw.InterruptFlag = 0;
  OASISPaw.FaultFlag = 0;
  CachePage = 1;
  CacheIndex = 0;
  PreCacheIndex = 0;
  PacketToSend = false;
  SendLastPacket = false;
  SendSerial = false;
  SendUDP = false;
  OASISPaw.OASISBusy = 0;
  digitalWrite(CONVST, LOW);
}
