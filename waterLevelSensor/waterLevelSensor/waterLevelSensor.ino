/*
 Name:		waterLevelSensor.ino
 Created:	7/4/2020 4:38:25 PM
 Author:	Evert
*/

// the setup function runs once when you press reset or power the board
#include <SoftwareSerial.h>
#include "E78LoraWan_Module.h"

SoftwareSerial mySerial(11, 10);

E78LoraWan_module lorawan(Serial);

String AppEUI = "70B3D57ED002F57D";
String Appkey = "B4B2261E145CB37B5EB5CD148448AF63";
String DevEUI = "D896E0FF00000241";

bool succes = false;

void setup() {
	//default baudrate
	Serial.begin(115200);
	delay(10000);
	//new baudrate
	lorawan.sendRawCommand("AT+CGBR=9600");
	lorawan.sendRawCommand("AT+CSAVE");
	delay(2000);
	mySerial.begin(9600);
	Serial.begin(9600);
	Serial.flush();
	succes = lorawan.initOTAA(AppEUI, Appkey, DevEUI );
}

// the loop function runs over and over again until power down or reset
void loop() {
	
	
}