//definicion de  la clase NH3_SENSOR
// archivo de cabecera nh3.h

#ifndef NH3_H
#define NH3_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#define RL 1  //The value of resistor RL is 47K
#define M -0.263 //Enter calculated Slope 
#define B 0.42 //Enter calculated intercept
#define R0 7.92 //Enter found Ro value

#define READ_SAMPLE_TIMES 5 //Se leen muestras cada 5 milisegundos
#define READ_SAMPLE_INTERVAL 50 //Se leen muestras cada 5 milisegundos

class NH3_SENSOR{
	
public:
   NH3_SENSOR(int);
   float NH3Read(void);
   float NH3PPM(void);	
private: 
  float VRL; //Voltage drop across the MQ sensor
  float Rs; //Sensor resistance at gas concentration 
  float ratio; //Define variable for ratio
  int dato;
  float ppm;
  uint8_t NH3_PIN;
};
#endif
