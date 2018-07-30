//definicion de las funciones de teclas
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#if (ARDUINO < 100)
#include <WProgram.h> //archivos include de arduino
#else
#include <Arduino.h>
#endif

#include "teclas.h"

Teclas::Teclas()
{
  numeroTeclas = 1;			//pordefecto solo hay una tecla
  puerto = 7; 				//por defecto el puerto de la primera tecla es el 7	
  estadoTeclas = new bool [1]; 		//define un vector para el numero de teclas
  debounceDelay = 20;
 	
  for (int i = 0; i < 1; i++) 		//inicilia el estado de las teclas
      estadoTeclas[i] = false;  		//Niniguna tecla presionada

  teclaPresionada = -1; 		//si es -1 no se a presionado niniguna tecla

}

Teclas::Teclas(uint8_t numT, uint8_t puer, uint8_t debounceDel)
{
  numeroTeclas = numT;
  puerto = puer;	
  estadoTeclas = new bool [numT]; 	//define un vector para el numero de teclas
  debounceDelay = debounceDel; 	
 	
  for (uint8_t i = 0; i < numT; i++) 	//inicilia el estado de las teclas
      estadoTeclas[i] = false;  		//Niniguna tecla presionada

  teclaPresionada = -1; 		//si es -1 no se a presionado niniguna tecla

}

bool Teclas::teclaEstado() {		//si la tecla es presionada almacena cual tecla  y el estado	

  bool tecla = false;

  for(uint8_t i = 0; i < numeroTeclas; i++) //Reinicio es el estado de las teclas
     estadoTeclas[i] = false; 
 
  for (uint8_t i = 0; i < numeroTeclas; i++) {    
    estadoTeclas[i] = debounce(puerto + i);

    if (estadoTeclas[i]) {
      teclaPresionada = i+puerto;		//almacena la tecla presionada	
      tecla = true;
      break;
    }
  }

  if (tecla)
     return true;
  return false;  //Ninguna tecla presionada
}

int Teclas::getTecla() {		//regresa la tecla presionada  
   return teclaPresionada; //devuelve el estado anterior de la teclas
}

bool Teclas::setTecla(uint8_t t){
  teclaPresionada = t;
}

bool Teclas::debounce(uint8_t pin)
{
  bool state;
  bool previusState;

  previusState = digitalRead(pin);   //store switch state

  for (int counter = 0; counter <= debounceDelay; counter++)
  {
    delay(1);
    state = digitalRead(pin);
    if (state != previusState)
    {
      counter = 0;
      previusState = state;
    }
  }

  if (state == LOW) //LOW means pressed
    return true;
  else
    return false;
}
