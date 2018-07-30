//definicion de  la clase CH4_SENSOR
// archivo de cabecera ch4.h

#ifndef CH4_H
#define CH4_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#define RL 1  //The value of resistor RL is 47K
#define M -0.362 //Enter calculated Slope 
#define B 1.0956 //Enter calculated intercept
#define R0 0.96 //Enter found Ro value

#define READ_SAMPLE_TIMES 5 //Se leen muestras cada 5 milisegundos
#define READ_SAMPLE_INTERVAL 50 //Se leen muestras cada 5 milisegundos

class CH4_SENSOR{
	
public:
   CH4_SENSOR(int);
   float CH4Read(void);
   float CH4PPM(void);	
private: 
  float VRL; //Voltage drop across the MQ sensor
  float Rs; //Sensor resistance at gas concentration 
  float ratio; //Define variable for ratio
  int dato;
  float ppm;
  uint8_t CH4_PIN;
};
#endif
