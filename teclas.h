//teclas.h
//definicion de clase Teclas

#ifndef TECLAS_H
#define TECLAS_H

class Teclas{
public:
   Teclas(uint8_t = 1, uint8_t  = 7, uint8_t = 20); //numero de teclas que desea
   Teclas();	
   bool teclaEstado(); 	//determina si una tecla es presionada
   int getTecla();   	//Devuelve la tecla presionada
   bool setTecla(uint8_t);
   //long duracion();      	//Regresa la duracion de precion de la tecla
private:
   bool debounce(uint8_t);     	//Debounce implementado por software
   bool *estadoTeclas;  //Niniguna tecla presionada
   int puerto;		//el puerto donde esta la primera tecla	
   int numeroTeclas;
   int teclaPresionada;
   int debounceDelay; 
   //long duracion; = 0;   	
};
#endif
