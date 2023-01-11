/*
 OASISPAW.cpp
 
	OASIS Platform Awareness - ESP32
 
 */

#include <OASISPAW.h>

OASISPAW::OASISPAW(){
	
	// Timer Setup
	timerMux = portMUX_INITIALIZER_UNLOCKED;
	timer = timerBegin(0, 80, true);
	timerAttachInterrupt(timer, &TimerISR, true);
	
	// Speaker Init
	ledcSetup(0, 2000, 8);
	ledcAttachPin(SPEAKER, 0);
}

OASISPAW::~OASISPAW(){
	
}

void OASISPAW::setTimer(int T_sample){
	timerAlarmWrite(timer, T_sample, true);
	return;
}

void OASISPAW::enableTimer(){
	timerAlarmEnable(timer);
	return;
}

void OASISPAW::disableTimer(){
	timerAlarmDisable(timer);
	return;
}

void OASISPAW::tone(int frequency, int duration){
	if(!OASISMute){
		ledcWriteTone(0, frequency);
	}
		delay(duration);
		ledcWriteTone(0, 0);
}

void OASISPAW::writeRGB(byte PWM_RED, byte PWM_GREEN, byte PWM_BLUE){
	
	Adafruit_NeoPixel pixels(2, LEDPin, NEO_GRB + NEO_KHZ800);
	pixels.begin();
	pixels.setPixelColor(0, pixels.Color(PWM_RED, PWM_GREEN, PWM_BLUE));
	pixels.setPixelColor(1, pixels.Color(50*(OASISWiFi==0), 50*(OASISWiFi==1), 0));
	pixels.show();

}

void OASISPAW::ledInit(){

	Adafruit_NeoPixel pixels(2, LEDPin, NEO_GRB + NEO_KHZ800);
	pixels.begin();
	pixels.show();
	delay(200);
	pixels.setPixelColor(0, pixels.Color(25, 25, 25));
	pixels.setPixelColor(1, pixels.Color(25, 25, 25));
	pixels.show();
	
}

void OASISPAW::resetDevice(){
	ESP.restart();
}