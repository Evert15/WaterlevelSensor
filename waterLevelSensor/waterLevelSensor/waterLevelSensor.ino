/*
 Name:		waterLevelSensor.ino
 Created:	7/4/2020 4:38:25 PM
 Author:	Evert
*/

// the setup function runs once when you press reset or power the board

#include "E78LoraWan_Module.h"
#include "Waterlevel.h"
//debug
#include <SoftwareSerial.h>

//debug
SoftwareSerial mySerial(11, 10);



String AppEUI = "70B3D57ED002F57D";
String Appkey = "B4B2261E145CB37B5EB5CD148448AF63";
String DevEUI = "D896E0FF00000241";

bool succes = false;

const int trigPin = 9;
const int echoPin = 8;
const int powPin = 7;
// defines variables
long duration;
int distance;

E78LoraWan_module lorawan(Serial);
waterLevel sensor;

void setup() {
	//default baudrate
	Serial.begin(115200);
  sensor.init(trigPin,echoPin,powPin);
  sensor.SetconfigPit(200,235,40);
	delay(10000);
	//new baudrate
	lorawan.sendRawCommand("AT+CGBR=9600");
	lorawan.sendRawCommand("AT+CSAVE");
	delay(2000);
	mySerial.begin(9600);
	Serial.begin(9600);
	Serial.flush();
	succes = lorawan.initOTAA(AppEUI, Appkey, DevEUI );
  if(succes!=true){
    lorawan.initOTAA(AppEUI, Appkey, DevEUI );
  }
  else mySerial.println("init failed");
}

// the loop function runs over and over again until power down or reset
void loop() {
 float measurement = sensor.takeABSMeasurement();
  int result = lorawan.txCnf(String(measurement));
  //mySerial.println(lorawan.getRx());
  delay(30000);	
}
