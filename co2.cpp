//Definicion de las funciones de la clase CO2_SENSOR
//Archivo: co2.cpp

#include "co2.h"
#include <math.h>

//dos puntos se toman de la curva. 
//con estos dos puntos, se forma una línea que es 
//'aproximadamente equivalente' a la curva original.
//data format:{ x, y, slope}; point1: (lg400, 0.324), point2: (lg4000, 0.280) 
//slope = ( reaction voltage ) / (log400 –log1000) 

CO2_SENSOR::CO2_SENSOR(int ANALOG_PIN = 0){
	porcentaje = 0; 
	leectura = 0;
	CO2_PIN = ANALOG_PIN;	
	
	CO2Curve[0] = 2.602;
	CO2Curve[1] = ZERO_POINT_VOLTAGE; 
	CO2Curve[2] = (REACTION_VOLTAGE/(2.602-3));
}

//Leel valor en ppm de CO2
float CO2_SENSOR::CO2Read(void){
	//Lectura del sensor de CO2
	//a traves del pin analogico CO2_ANALOGPIN
	int dato = 0;
	
	for (uint8_t i = 0; i < READ_SAMPLE_TIMES;i++){	    
	    dato +=  analogRead(CO2_PIN); 
	    delay(READ_SAMPLE_INTERVAL);	    
	}
	
	leectura = (dato/READ_SAMPLE_TIMES)*5.0/1024;
	return leectura;
} 

//Regresa el porcentaje de CO2 leeido
int CO2_SENSOR::CO2Porcentaje(void){
	leectura = CO2Read();
	if ((leectura/DC_GAIN) >= ZERO_POINT_VOLTAGE)
		return -1;
	else
		return pow(10, ((leectura/DC_GAIN) - CO2Curve[1])/CO2Curve[2] + CO2Curve[0]);		
}
