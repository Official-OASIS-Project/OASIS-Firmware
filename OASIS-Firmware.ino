#include <SPI.h>
#include <EEPROM.h>
#include <OASISPAW.h>
#include <Wire.h>
#include <TEDDY.h>

#define EEPROM_SIZE 40
#define CACHE_SIZE 1500
#define PRECACHE_SIZE 1000
#define PS_CACHE_SIZE 4193992

// OASIS PAW
OASISPAW OASISPaw;
extern volatile int OASISPAW::InterruptFlag;
extern volatile bool OASISPAW::OASISBusy;
extern volatile bool OASISPAW::FaultFlag;
extern portMUX_TYPE OASISPAW::timerMux;

// SPI
SPIClass * OASISSPI = NULL;
SPIClass * OASIS_CC_SPI = NULL;

// Voltage Range
float VoltageRange[4];
int VoltageRangeSet[4];
const byte Range[5] = {0x00, 0x01, 0x02, 0x03, 0x04};

// WiFi Setup
char WiFiSSID[50] = "OASIS v.X.X - ";
const char *pwd = "Vibration";

// UDP Setup
OASISUDP udp;
char *udpTarget;
const int udpPort = 3333;
char udpBuffer[255];
IPAddress udpTargetIP;
IPAddress OASISIP = {192, 168, 4, 1};
IPAddress subnet = {255, 255, 255, 0};
IPAddress gateway = {0, 0, 0, 0};

// #########################################################################################
// --------------------------------------- Variables ---------------------------------------
// #########################################################################################

// Loop Variable
int i = 0;
int k = 0;

// Loop Variable for Core 0
int f = 0;
int u = 0;

// ADC Data Variables
byte Addr;
byte RegisterData;
byte ADCReadback;
unsigned long int ADCData;
float VoltageData;
float BitDivider;
byte BYTES_PER_SAMPLE;
const byte byteReadADC = byte(0b01000000);
const byte byteWriteADC = byte(0b00000000);

// Sampling
volatile int sampleCount = 0;
int T_sample = 0;
float f_sample = 0;
int n_sample = 0;
float t_sample = 0;
float TRIGG_LEVEL = 0;
volatile int TRIGG;

// Command Parsing
String Command;
char StrValues[42];
char delimiter[] = "(,)";
char *StringPointer;

// Errorhandling
bool LEDError = false;
bool KilledWiFi = false;

// Persistant configuration
bool OASISMute = false;
bool OASISWiFi = false;

// Data Cache
byte OASISCacheA[CACHE_SIZE][9];
byte OASISCacheB[CACHE_SIZE][9];
volatile int CachePage = 1;
volatile int CacheIndex;
byte OASISPreCache[PRECACHE_SIZE][9];
volatile int PreCacheIndex;

// Parallel computing
volatile bool PacketToSend = false;
volatile bool SendLastPacket = false;
volatile bool SendSerial = false;
volatile bool SendUDP = false;

// Device Info
byte HW_MAJOR;
byte HW_MINOR;
byte FW_MAJOR;
byte FW_MINOR;
byte ADC_BITS;
byte F_TEDS;
byte F_WSS;
char DeviceName[25];

// PSRAM Buffer
byte* OASISBuffer;

// WSS
byte sync_mode = 0;

// TEDDY
TEDDY teddy(TEDDYPin);

// #########################################################################################
// ----------------------------------- Interrupt Routine -----------------------------------
// #########################################################################################

// Interrupt Routine GPIO
void IRAM_ATTR GPIOISR() {
  digitalWrite(HSPI_CS, LOW);
  
  if(!TRIGG){
    // Before TRIGG
    for(i=0; i<BYTES_PER_SAMPLE; i++){
      OASISPreCache[PreCacheIndex][i] = OASISSPI->transfer(0x00);
    }

    // Assemble Channel 1
    if(BYTES_PER_SAMPLE==9){
      ADCData = (((OASISPreCache[PreCacheIndex][0] << 16) + (OASISPreCache[PreCacheIndex][1] << 8) + OASISPreCache[PreCacheIndex][2]) >> 6);
    }
    if(BYTES_PER_SAMPLE==8){
      ADCData = ((OASISPreCache[PreCacheIndex][0] << 8) + OASISPreCache[PreCacheIndex][1]);
    }
    
    // Conversion to voltage
    if(bitRead(ADCData, (ADC_BITS-1))==0){
      VoltageData = ADCData*VoltageRange[0]/BitDivider;
    }
    else{
      VoltageData = ((ADCData-2*BitDivider)/BitDivider)*VoltageRange[0];
    }

    //Check TRIGG
    if(VoltageData>TRIGG_LEVEL){
      TRIGG = 1;
      //Serial.println("<>");
    }
    else{
      PreCacheIndex++;
      if(PreCacheIndex==PRECACHE_SIZE){
        PreCacheIndex = 0;
      }
    }
  }
  else{
    // After TRIGG
    if(CachePage%2){
      for(i=0; i<BYTES_PER_SAMPLE; i++){
        OASISCacheA[CacheIndex][i] = OASISSPI->transfer(0x00);
      }
    }
    else{
      for(i=0; i<BYTES_PER_SAMPLE; i++){
        OASISCacheB[CacheIndex][i] = OASISSPI->transfer(0x00);
      }
    }

    // Write samples into PSRAM - Not implemented yet
    //for(i=0; i<BYTES_PER_SAMPLE; i++){
    //  OASISBuffer[(sampleCount*8)+i] = 0xFF;//OASISSPI->transfer(0x00);
    //} 
  
    CacheIndex++;
    
    if(CacheIndex==CACHE_SIZE){
      CacheIndex = 0;
      CachePage++;
      PacketToSend = true;
    }
  }
  
  digitalWrite(CONVST, LOW);
  digitalWrite(HSPI_CS, HIGH);
  OASISPaw.InterruptFlag = 1;
}

void IRAM_ATTR WSS_ISR(){
  OASISPaw.enableTimer();
}

// #########################################################################################
// ----------------------------------------- Tasks -----------------------------------------
// #########################################################################################

TaskHandle_t DataSender;

void SendData(void * parameter){
  for(;;){
    
    if(PacketToSend){
      // Send data over Serial
      if(SendSerial){
        if(CachePage%2){
          for(f=0; f<CACHE_SIZE; f++){
            for(u=0; u<BYTES_PER_SAMPLE; u++){
              Serial.write(OASISCacheB[f][u]);
            }
          }
        }
        else{
          for(f=0; f<CACHE_SIZE; f++){
            for(u=0; u<BYTES_PER_SAMPLE; u++){
              Serial.write(OASISCacheA[f][u]);
            }
          }
        }
      }
      // Send data over UDP
      if(SendUDP){
        udp.beginPacket(udpTargetIP,udpPort);
        if(CachePage%2){
          for(f=0; f<CACHE_SIZE; f++){
            for(u=0; u<BYTES_PER_SAMPLE; u++){
              udp.write(OASISCacheB[f][u]);
            }
          }
        }
        else{
          for(f=0; f<CACHE_SIZE; f++){
            for(u=0; u<BYTES_PER_SAMPLE; u++){
              udp.write(OASISCacheA[f][u]);
            }
          }
        }
        udp.endPacket();
      }
      
      PacketToSend = false;
    }
    
    if(SendLastPacket){
      // Send data over Serial
      if(SendSerial){
        if(CachePage%2){
          for(f=0; f<CacheIndex; f++){
            for(u=0; u<BYTES_PER_SAMPLE; u++){
              Serial.write(OASISCacheA[f][u]);
            }
          }
        }
        else{
          for(f=0; f<CacheIndex; f++){
            for(u=0; u<BYTES_PER_SAMPLE; u++){
              Serial.write(OASISCacheB[f][u]);
            }
          }
        }
      }
      // Send data over UDP
      if(SendUDP){
        udp.beginPacket(udpTargetIP,udpPort);
        if(CachePage%2){
          for(f=0; f<CacheIndex; f++){
            for(u=0; u<BYTES_PER_SAMPLE; u++){
              udp.write(OASISCacheA[f][u]);
            }
          }
        }
        else{
          for(f=0; f<CacheIndex; f++){
            for(u=0; u<BYTES_PER_SAMPLE; u++){
              udp.write(OASISCacheB[f][u]);
            }
          }
        }
        udp.endPacket();
        }
      
      SendLastPacket = false;
    }
  vTaskDelay(1);    
  }
}

// ##########################################################################################
// --------------------------------------- VOID SETUP ---------------------------------------
// ##########################################################################################

void setup() {

  OASISPaw.ledInit();

  // EEPROM Setup
  EEPROM.begin(EEPROM_SIZE);

  // Serial Setup
  Serial.begin(SerialSpeed);
  Serial.println();

  // ADC SPI Setup
  OASISSPI = new SPIClass(HSPI);
  OASISSPI->begin();
  OASISSPI->beginTransaction(SPISettings(25000000, MSBFIRST, SPI_MODE3));

  // WSS SPI Setup
  OASIS_CC_SPI = new SPIClass(VSPI);
  OASIS_CC_SPI->begin();
  OASIS_CC_SPI->beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));

  // Port Expander Setup
  initpEXP();

  // Load Device Info from EEPROM
  HW_MAJOR = EEPROM.read(0x08);
  HW_MINOR = EEPROM.read(0x09); 
  FW_MAJOR = EEPROM.read(0x0A);
  FW_MINOR = EEPROM.read(0x0B);
  ADC_BITS = EEPROM.read(0x0C);
  F_TEDS = EEPROM.read(0x0D);
  F_WSS = EEPROM.read(0x0E);
  for(int i=0; i<24; i++){
      if(EEPROM.read(0x10 + i)!=0){
        DeviceName[i] = (char)EEPROM.read(0x10 + i);
      }
  }
  
  // Announce Startup
  Serial.print("[OASIS] This is ");
  Serial.print(DeviceName);
  Serial.println(" starting up...");

  // Read Firmware Version
  Serial.print("[OASIS] Firmware Version ");
  Serial.print(FW_MAJOR);
  Serial.print(".");
  Serial.print(FW_MINOR);
  Serial.println(" initializing...");

  // Load settings in EEPROM
  OASISMute = EEPROM.read(0); // Option to mute all sounds, 1 = yes
  OASISWiFi = EEPROM.read(1);
  
  // Pin Setup
  pinMode(HSPI_CS, OUTPUT);
  pinMode(RESET, OUTPUT);
  pinMode(CONVST, OUTPUT);
  pinMode(RANGE, OUTPUT);
  pinMode(BUSY, INPUT_PULLDOWN);
  digitalWrite(HSPI_CS, HIGH);
  digitalWrite(RESET, LOW);
  digitalWrite(CONVST, LOW);

  StartupSound();

  // ADC Config
  Serial.println("[OASIS] Resetting ADC...");
  digitalWrite(RESET, HIGH);
  delay(1);
  digitalWrite(RESET, LOW);
  delay(1);
  BitDivider = pow(2,ADC_BITS)/2;
  BYTES_PER_SAMPLE = ADC_BITS/2; // 4 channels / 8 bits/byte
  // Config for AD7606C-18
  if(ADC_BITS==18){
    // Set DOUT_FORMAT to 1 data line
    writeADCConfig(0x02,0x00);
    // Set Channel Range to +/- 2.5 V
    writeADCConfig(0x03,0x00);
    writeADCConfig(0x04,0x00);
    for(int i=0; i<4; i++){
      VoltageRange[i] = 2.5;
    }
  }
  // Config for AD7606-4
  if(ADC_BITS==16){
    // Set Channel Range to +/- 5 V
    digitalWrite(RANGE, LOW);
    for(int i=0; i<4; i++){
      VoltageRange[i] = 5.0;
    }
  }

  // Setup WiFi, if activated
  if(OASISWiFi){

    // Assemble WiFi SSID
    sprintf(WiFiSSID, "OASIS v.%d.%d - %s", HW_MAJOR, HW_MINOR, DeviceName);
    
    // Create & configure WiFi Access Point
    Serial.print("[OASIS] Starting WiFi Access Point '");
    Serial.print(WiFiSSID);
    Serial.println("'");
    WiFi.softAP(WiFiSSID, pwd);
    WiFi.softAPConfig(OASISIP, subnet, gateway);
    udp.begin(3333);
    Serial.print("[OASIS] UDP reachable at: ");
    Serial.print(WiFi.softAPIP());
    Serial.print(":");
    Serial.println(udpPort);
  }
  else{
    Serial.println("[OASIS] WiFi Access Point is DISABLED!");
  }

  // Attach GPIO interrupt
  attachInterrupt(BUSY, GPIOISR, FALLING);

  // Allocate PSRAM - Not implemented yet
  //Serial.println(psramInit());
  //Serial.println(ESP.getFreePsram());
  //byte* OASISBuffer = (byte*)ps_malloc(PS_CACHE_SIZE);
  //Serial.println(ESP.getFreePsram());
  //Serial.println(*OASISBuffer);
  //OASISBuffer[0] = 0xFF;
  //Serial.println(OASISBuffer[0]);

  // Init WSS
  initWSS();

  delay(500);
  ledReady();
  Serial.println("[OASIS] Finished booting.");
}

// #########################################################################################
// --------------------------------------- VOID LOOP ---------------------------------------
// #########################################################################################

void loop() {

  // Wait for Serial command
  if(Serial.available()) {
    Command = Serial.readString();
    Command.toCharArray(StrValues,42);
    StringPointer = strtok(StrValues, delimiter);
    Command = String(StringPointer);

    LEDError = false;

    // Sample data
    if(Command=="OASIS.Sample"){C_KillWiFi(); C_Sample(1);}

    // Set voltage ranges in ADC
    else if(Command=="OASIS.SetVoltageRange"){C_SetVoltageRange();}

    // Get voltage ranges from ADC
    else if(Command=="OASIS.GetVoltageRange"){C_GetVoltageRange();}

    // Mute the buzzer
    else if(Command=="OASIS.Mute"){C_Mute();}

    // Unmute the buzzer
    else if(Command=="OASIS.Unmute"){C_Unmute();}

    // Enable WiFi Access Point
    else if(Command=="OASIS.EnableWiFi"){C_WiFiEnable();}

    // Disable WiFi Access Point
    else if(Command=="OASIS.DisableWiFi"){C_WiFiDisable();}

    // List connected WiFi clients and send an UDP package to all of them
    else if(Command=="OASIS.WiFiDebug"){C_WiFiDebug();}

    // Display Information about the device
    else if(Command=="OASIS.Info"){C_Info();}

    // Return raw Information about the device
    else if(Command=="OASIS.RawInfo"){C_RawInfo();}

    // Liaten for WSS impulse
    else if(Command=="OASIS.WSSlisten"){Serial.println("[OASIS] Listening for WSS..."); WSS_listen();}

    // Send WSS impulse
    else if(Command=="OASIS.WSSsend"){Serial.println("[OASIS] Sending WSS impulse..."); WSS_send();}

    // Read TEDS - Not implemented yet
    //else if(Command=="OASIS.ReadTEDS"){C_ReadTEDS();}
    
    // Unknown Command
    else{
      Serial.print("[OASIS] Error - Command is not valid: ");
      Serial.print(StringPointer);
      Serial.println();
      delay(10);
      Sound_Bad_Command();
      ledError();
    }
    if(!LEDError){
      ledReady();
    }
  }

  // Read UDP Packets  
  if(udp.parsePacket()){
    Serial.print("[OASIS] Received command from ");
    udpTargetIP = udp.remoteIP();
    udp.read(udpBuffer, 255);
    Serial.print(udpTargetIP);
    Serial.print(": ");
    Serial.println(udpBuffer);

    // Parse the command
    StringPointer = strtok(udpBuffer, delimiter);
    Command = String(StringPointer);
    
    LEDError = false;

    // Sample data
    if(Command=="OASIS.Sample"){C_Sample(2);}

    // Set voltage ranges in ADC
    else if(Command=="OASIS.SetVoltageRange"){C_SetVoltageRange();}

    // Unknown Command
    else{
      Serial.print("[OASIS] Error - Command is not valid: ");
      Serial.println(StringPointer);
      Serial.println();
      udp.beginPacket(udpTargetIP,udpPort);
      udp.print("[OASIS] Error - Command is not valid: ");
      udp.print(StringPointer);
      udp.println();
      udp.endPacket();
      delay(10);
      Sound_Bad_Command();
      ledError();
    }
    if(!LEDError){
      ledReady();
    }
  }
}
