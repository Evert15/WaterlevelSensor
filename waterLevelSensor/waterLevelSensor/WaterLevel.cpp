#include "Waterlevel.h"

float waterLevel::pulseMeasurement(){
  //local variables
  long duration;
  float distance;
  // enable the sensor
  digitalWrite(_powPin, HIGH);
  delay(3);
  digitalWrite(_trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(_trigPin, LOW);
  // Clears the trigPin
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(_recPin, HIGH , 3000);
  //switch of the sensor
  digitalWrite(_powPin, LOW);
  // Calculating the distance
  distance= duration*0.034/2;
  return distance;
}

void waterLevel::init(int trigPin , int recPin, int powPin){
  _recPin = recPin;
  _trigPin = trigPin;
  _powPin = powPin;
  pinMode(_powPin, OUTPUT); // Sets the powPin as an Output
  pinMode(_trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(_recPin, INPUT); // Sets the echoPin as an Input  
}

void waterLevel::SetconfigPit( int heigth , int width , int depth , int offset){
  _offset = offset;
  // rectangular base in cm squared
  _baseDimension = width * depth;
  // total volume
  _volume = heigth * _baseDimension;
}

void waterLevel::SetconfigPit( int heigth , int radius , int offset){
   _offset = offset;
   // circular base in cm squared
   _baseDimension = pow((radius) , 2) * PI;
   // total volume
  _volume = heigth * _baseDimension ;
}

float waterLevel::takeABSMeasurement(){
  float distance = 0.0;
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





