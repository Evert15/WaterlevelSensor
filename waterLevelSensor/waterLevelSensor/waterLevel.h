// Waterlevel.h

#ifndef _WaterLevel_h
#define _waterLevel_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif
#endif

class waterLevel {

  public:

    //Set the modes of the pins used by the sensor
    //Arduino can deliver enough power to power the Sonar Sensor.
    void init(int trigPin , int recPin , int powPin);
    
    //Config for a rectangular Pit values are in CM like 100 cm. Decimals are not accepted
    void SetconfigPit( int heigth , int width , int depth , int offset);

    //Config for a cilindrical Pit values are in CM like 100 cm. Decimals are not accepted
    void SetconfigPit( int heigth , int radius , int offset);

    // returns the volume of water in the pit in cubic cm
    float takeABSMeasurement();
    
    // returns the relative volume of water in %
    float takeRELMeasurement();
    

  private:

    // power the sensor and take a measurement. Measurement takes max of 3 seconds
    float pulseMeasurement();
    // distance in cm from the sensor to the maximum hight the water can get
    int  _offset = 0;
    // pin to trigger the sonar sensor
    int _trigPin;
    // pin  for the echo of the sonar sensor
    int _recPin;
    // pin to power the sonar sensor
    int _powPin;
    
    float _volume = 0.0;

    float _baseDimension = 0.0; 
};


