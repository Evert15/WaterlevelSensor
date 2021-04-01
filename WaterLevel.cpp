#include "Waterlevel.h"
#include <EEPROM.h>
float waterLevel::pulseMeasurement(){
  //local variables
  long duration;
  float distance;
  // enable the sensor
  digitalWrite(_powPin, HIGH);
  delay(100); // Sets the trigPin as an Output
 // pinMode(_trigPin,OUTPUT);
  delay(5);
  digitalWrite(_trigPin, LOW);
  delayMicroseconds(10);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(_trigPin, LOW);
  // Clears the trigPin
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(_recPin, HIGH);
  //switch of the sensor
  digitalWrite(_powPin, LOW);
  //powerdown the sonar
 // pinMode(_trigPin, INPUT_PULLUP);
  //disable the leakage current on the pin
  // Calculating the distance
  distance= duration*0.034/2;
  return distance;
}

void waterLevel::init(int trigPin , int recPin, int powPin){
  _recPin = recPin;
  _trigPin = trigPin;
  _powPin = powPin;
  pinMode(_powPin, OUTPUT); // Sets the powPin as an Output
  pinMode(_trigPin,OUTPUT); // Sets the trigPin as an Output
  pinMode(_recPin, INPUT); // Sets the echoPin as an Input
    
}

void waterLevel::SetconfigPit( ){
    struct Parameters myparameters;
    EEPROM.get(0, myparameters);
    if(myparameters.pitconfig==0){
       _offset = myparameters.offset;
       // circular base in cm squared
       _baseDimension =(float)(pow((myparameters.radius) , 2)) * PI;
       // total volume
       _volume = (float)myparameters.heigth * _baseDimension ;
     }
    else{
      _offset = myparameters.offset;
       // rectangular base in cm squared
      _baseDimension = myparameters.width * myparameters.depth;
      // total volume
      _volume = myparameters.heigth * _baseDimension;
    }
}



float waterLevel::takeABSMeasurement(){
  float distance = 0.00;
  distance = pulseMeasurement();
  // distance to the water
  distance = distance - (float)_offset;
  // heigth the air column above the water
  if( distance >= 0){
    //volume water is the total volume of the pit - the air column measured
   return _volume-(distance*_baseDimension);
  }
  else return 0;
}

float waterLevel::takeRELMeasurement(){
   float distance = 0.0;
   float volume = 0.0;
  distance = pulseMeasurement();
  
  // distance to the water
  distance = distance - (float)_offset;
  // heigth the air column above the water
  if( distance >= 0){
    //volume water is the total volume of the pit - the air column 
    volume = _volume-(distance*_baseDimension);
    // calculate relative value
    return (volume/_volume)*100.0;
  }
  else return 0;
}

int waterLevel::batterylevel(int pin){
  pinMode(pin , INPUT);
  return analogRead(pin);
}

void waterLevel::PitparamLora(String RX){
    if(RX != ""){
      struct Parameters myparameters;
    if(RX.startsWith("0"))
    {
      //for cilindrical pits
      int temp = RX.indexOf("X");
      int temp1 = RX.indexOf("X",temp+1);
      int heigth = RX.substring(temp+1,temp1).toInt();
      temp=RX.indexOf("X",temp1+1);
      int radius=RX.substring(temp1+1,temp).toInt();
      int offset=RX.substring(temp+1,RX.length()).toInt();
      myparameters.heigth=heigth;
      myparameters.radius=radius;
      myparameters.offset=offset;
      myparameters.pitconfig=0;
    }
    
    if(RX.startsWith("1"))
    {
      //for cubic pits
      int temp = RX.indexOf("X");
      int temp1 = RX.indexOf("X",temp+1);
      int heigth = RX.substring(temp+1,temp1).toInt();
      
      temp=RX.indexOf("X",temp1+1);
      int width=RX.substring(temp1+1,temp).toInt();
      temp1 = RX.indexOf("X",temp+1);
      int depth=RX.substring(temp+1,temp1).toInt();
      int offset=RX.substring(temp1+1,RX.length()).toInt();
      myparameters.heigth=heigth;
      myparameters.width=width;
      myparameters.depth=depth;
      myparameters.offset=offset;
      myparameters.pitconfig=1;
      }
    
     //Serial.println(myparameters.heigth);
     EEPROM.put(0, myparameters);
     SetconfigPit();
    }
    else;
}





