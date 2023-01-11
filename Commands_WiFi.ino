// Enable WiFi Access Point
void C_WiFiEnable(){
  EEPROM.write(1, 1); // Write variable to adress 0x01
  EEPROM.commit();
  Serial.println("[OASIS] Enabling WiFi after RESET");
  Serial.println("[OASIS] Reseting System...");
  OASISPaw.resetDevice();
}

// Disable WiFi Access Point
void C_WiFiDisable(){
  EEPROM.write(1, 0); // Write variable to adress 0x01
  EEPROM.commit();
  Serial.println("[OASIS] Disabling WiFi after RESET");
  Serial.println("[OASIS] Reseting System...");
  OASISPaw.resetDevice();
}

// List connected WiFi clients and send an UDP package to all of them
void C_WiFiDebug(){
  Serial.println("--------------------------------------------------------------------------");
  Serial.println("                          Connected WiFi Clients                          ");
  Serial.println("--------------------------------------------------------------------------");
  Serial.println();

  if(WiFi.softAPgetStationNum()==0){
    Serial.println("No WiFi clients connected.");
    Serial.println();
  }
  else{

    wifi_sta_list_t wifi_sta_list;
    tcpip_adapter_sta_list_t adapter_sta_list;
 
    memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));
    memset(&adapter_sta_list, 0, sizeof(adapter_sta_list));
 
    esp_wifi_ap_get_sta_list(&wifi_sta_list);
    tcpip_adapter_get_sta_list(&wifi_sta_list, &adapter_sta_list);
 
    for(int i = 0; i < adapter_sta_list.num; i++){
 
      tcpip_adapter_sta_info_t station = adapter_sta_list.sta[i];
 
      Serial.print("Client #");
      Serial.print(i+1);
      Serial.print(" | MAC: ");
 
      for(int i = 0; i< 6; i++){
      
      Serial.printf("%02X", station.mac[i]);  
      if(i<5)Serial.print(":");
      }
 
      Serial.print(" | IP: ");  
      Serial.println(ip4addr_ntoa(&(station.ip)));
      udpTarget = ip4addr_ntoa(&(station.ip));

      udp.beginPacket(udpTarget,udpPort);
      udp.print("OASIS UDP Debug Package - You are client #");
      udp.print(i+1);
      udp.print(" with IP-Address: ");
      udp.println(udpTarget);
      udp.endPacket();
      Serial.print("Sent debug package over UDP to ");
      Serial.print(udpTarget);
      Serial.print(":");
      Serial.println(udpPort);
      Serial.println();
    }
  }
  
  Serial.println("--------------------------------------------------------------------------");
  Serial.println();
}

// Function for killing the WiFi when data is sampled over the serial connection
void C_KillWiFi(){
  if(OASISWiFi && !KilledWiFi){
    Serial.println("[OASIS] WiFi is ON. Disabling WiFi for Data Acquisition over Serial...");
    C_WiFiDisable();
  }
}
