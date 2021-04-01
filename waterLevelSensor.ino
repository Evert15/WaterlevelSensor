/*
 Name:		waterLevelSensor.ino
 Created:	7/4/2020 4:38:25 PM
 Author:	Evert
*/

// the setup function runs once when you press reset or power the board

#include "E78LoraWan_Module.h"
#include "Waterlevel.h"
#include "LowPower.h"
//debug
#include <SoftwareSerial.h>

//pin definitions
#define trigPin 9
#define echoPin 8
#define powPin 7
#define batteryPin A0 

//lorawan join strings
#define AppEUI "70B3D57ED002F57D"
#define Appkey "B4B2261E145CB37B5EB5CD148448AF63"
#define DevEUI "D896E0FF00000241"

//debug
//rx : 11
//tx : 10
SoftwareSerial mySerial(11, 10);

// defines variables
long duration;
int distance;
int counter = 74;
bool succes = false;

//definition of classes
E78LoraWan_module lorawan(Serial);
waterLevel sensor;

void setup() {
	//default baudrate
	Serial.begin(115200);
  sensor.init(trigPin,echoPin,powPin);
	//new baudrate
	lorawan.sendRawCommand("AT+CGBR=9600");
	lorawan.sendRawCommand("AT+CSAVE");
	delay(2000);
	mySerial.begin(9600);
	Serial.begin(9600);
  sensor.SetconfigPit();
	Serial.flush();
	succes = lorawan.initOTAA(AppEUI, Appkey, DevEUI );
  if(succes!=true){
    lorawan.initOTAA(AppEUI, Appkey, DevEUI );
  }
  else mySerial.println("init failed");
}

// the loop function runs over and over again until power down or reset
void loop() {
  
  bool succes = false;
  // measurement will be sent every 10 minutes
  // set to 74 times
  if(counter>=30){
  sensor.init(trigPin,echoPin,powPin);
  sensor.SetconfigPit();
  lorawan.wakeUp();
  float measurement = sensor.takeABSMeasurement();
  mySerial.println(measurement);
   // try to send 2 times if one failed
  int result = lorawan.txUncnf("V" + String(measurement) + "B" + String(sensor.batterylevel(batteryPin)));
  //mySerial.println(result);
  if( result == 0) lorawan.txUncnf("V" + String(measurement) + "B" + String(sensor.batterylevel(batteryPin)));
  sensor.PitparamLora(lorawan.getRx());
  while(succes!=true){
   //resend the command till the module is in deep sleep
  succes = lorawan.enterDeepSleep();
  delay(20);
  }
  counter = 0;
  }
  else 
  {
    counter++;
  }
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}
