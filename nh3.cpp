//Definicion de las funciones de la clase NH3_SENSOR
//Archivo: nh3.cpp

#include "nh3.h"
#include <math.h>

NH3_SENSOR::NH3_SENSOR(int ANALOG_PIN = 0){
	VRL = 0;
	Rs = 0;
	ratio = 0;
	dato = 0;
	ppm = 0;
		
	NH3_PIN = ANALOG_PIN;	
}

//Leel valor en ppm de CO2
float NH3_SENSOR::NH3Read(void){
	//Lectura del sensor de CO2
	//a traves del pin analogico CO2_ANALOGPIN
	int dato = 0;
	
	for (uint8_t i = 0; i < READ_SAMPLE_TIMES;i++){	    
	    dato +=  analogRead(NH3_PIN); 
	    delay(READ_SAMPLE_INTERVAL);	    
	}
	
	VRL = (dato/READ_SAMPLE_TIMES)*5.0/1024;
	return VRL;
} 

//Regresa el porcentaje de CO2 leeido
float NH3_SENSOR::NH3PPM(void){
   VRL = NH3Read();
	
   Rs = ((5.0*RL)/VRL)-RL; //Use formula to get Rs value
   ratio = Rs/R0;  // find ratio Rs/Ro
 
   ppm = pow(10, ((log10(ratio)-B)/M)); //use formula to calculate ppm  
	
   return ppm;	
}
