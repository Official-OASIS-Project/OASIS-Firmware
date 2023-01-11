/*
 OASISPAW.h
 
	OASIS Platform Awareness - ESP32
 
 */

#ifndef OASISPAW_h
#define OASISPAW_h

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "WiFi.h"
#include "esp_wifi.h"

// Pin Setup - ESP32
// ADC SPI
#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_SCLK 14
#define HSPI_CS 15

// CC1101
#define VSPI_MISO 19
#define VSPI_MOSI 23
#define VSPI_SCLK 18
#define VSPI_CS 5
#define CC_GDO2 4
#define CC_GDO0 34

// TEDS
#define TEDDYPin 2

// Portexpander I2C
#define I2C_SCL 22
#define I2C_SDA 21

// ADC Control
#define RESET 32
#define CONVST 33
#define RANGE 25
#define BUSY 35

// Audiovisual Feedback
#define SPEAKER 26
#define LEDPin 27


#define SerialSpeed 2000000

extern bool OASISMute;
extern bool OASISWiFi;

class OASISPAW {
public:
	OASISPAW();
	~OASISPAW();
	hw_timer_t * timer;
	void setTimer(int T_sample);
	void enableTimer();
	void disableTimer();
	void tone(int frequency, int duration);
	void writeRGB(byte PWM_RED, byte PWM_GREEN, byte PWM_BLUE);
	void ledInit();
	void resetDevice();
	static volatile int InterruptFlag;
	static volatile bool OASISBusy;
	static volatile bool FaultFlag;
	static portMUX_TYPE timerMux;
};

// Interrupt Routine Timer
static void IRAM_ATTR TimerISR() {
	portENTER_CRITICAL_ISR(&OASISPAW::timerMux);
	digitalWrite(CONVST, HIGH);
	if (OASISPAW::OASISBusy){
		OASISPAW::FaultFlag = 1;
	}
	OASISPAW::OASISBusy = 1;
	portEXIT_CRITICAL_ISR(&OASISPAW::timerMux);
}

#endif