//definicion de  la clase CO2_SENSOR
// archivo de cabecera co2.h

#ifndef CO2_H
#define C02_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#define DC_GAIN 4.5		//ganancia del amplificador
#define READ_SAMPLE_TIMES 5 //Se leen muestras cada 5 milisegundos
#define READ_SAMPLE_INTERVAL 50 //Se leen muestras cada 5 milisegundos
#define ZERO_POINT_VOLTAGE 0.220 //Define la salida en voltios cuando la concentraccion de Co2 es de 400ppm
#define REACTION_VOLTAGE 0.02		//define la caida de tension cuando se mueve el sensor a 1000ppm		

class CO2_SENSOR{
	
public:
    CO2_SENSOR(int);
	float CO2Read(void);
	int CO2Porcentaje(void);	
private:
	uint8_t porcentaje;
	float leectura;
	float CO2Curve[3];
	uint8_t	CO2_PIN; // Pin analogico 
};
#endif
