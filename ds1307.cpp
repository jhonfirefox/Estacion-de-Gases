//ds1307.cpp
//definicion de las funciones de la clase reloj
#if (ARDUINO < 100)
#include <WProgram.h> //archivos include de arduino
#else
#include <Arduino.h>
#endif

//#include <String>
#include <Wire.h>
#include <EEPROM.h>
#include "ds1307.h"

Reloj::Reloj() {
  
  //lee los registros del arduino
  second = EEPROM.read(0x00);
  minuts = EEPROM.read(0x01);
  hour = EEPROM.read(0x02);
  day = EEPROM.read(0x04);
  wday = EEPROM.read(0x03);
  month = EEPROM.read(0x05);
  year = EEPROM.read(0x06);
}

uint8_t Reloj::configuracionTiempo(int j, uint8_t valor) {

  switch (j) {
    case 0:
      if ((valor >= 0) && (valor < 60 )) {
        second = valor;

      }
      else {
        second = 0;
        valor = second;
      }
      EEPROM.write(0x00, second);	
      break;
    case 1:
      if ((valor >= 0) && (valor < 60 )) {
        minuts = valor;
      }
      else {
        minuts = 0;
        valor = minuts;
      }
      EEPROM.write(0x01, minuts);	
      break;
    case 2:
      if ((valor >= 0) && (valor < 24 )) {
        hour = valor;
      }
      else {
        hour = 0;
        valor = hour;
      }
      EEPROM.write(0x02, hour);	
      break;
    case 3:
      if ((valor >= 1) && (valor < 8 )) {
        wday = valor;
      }
      else {
        wday = 1;
        valor  = wday;
      }
      EEPROM.write(0x03, wday);	
      break;
    case 4:
      if ((valor >= 1) && (valor < 32 )) {
        day = valor;
      }
      else {
        day = 1;
        valor = day;
      }
      EEPROM.write(0x04, day);	
      break;
    case 5:
      if ((valor >= 1) && (valor < 13 )) {
        month = valor;
      }
      else {
        month = 1;
        valor  = month;
      }
      EEPROM.write(0x05, month);	
      break;
    case 6:
      if ((valor >= 1) && (valor < 256 )) {
        year = valor;
      }
      else {
        year = 18;
        valor = year;
      }
      EEPROM.write(0x06, year);	
      break;
  }
  return valor;
}

uint8_t Reloj::obtenerTiempo(int valor)
{
  switch (valor){
    case 0:
           return second; 
           break;
    case 1:
           return minuts; 
           break;
    case 2:
           return hour; 
           break;                      
    case 3:
           return wday; 
           break;           
    case 4:
           return day; 
           break;    
    case 5:
           return month; 
           break;      
    case 6:
           return year; 
           break;     
    default:
           return second; 
           break;                             
  }
}

bool Reloj::setTiempo(int hor, int minus, int sec)
{
  (hor >= 0 && hor < 24)? hour = hor: hour = 0;
  (minus >= 0 && minus < 60)? minuts = minus: minuts = 0;  
  (sec >= 0 && sec < 60)? second = sec: second = 0;  
}

bool Reloj::setFecha(int wd, int d, int m, int a)
{
  (wd >= 1 && wd < 8)? wday = wd: wd = 1;
  (d >= 1 && d < 31)? day = d: day = 1;  
  (m >= 1 && m < 13)? month = m: month = 1;
  (a >= 0 && a < 100)? year = a: year = 0;
}

uint8_t Reloj::desplegar(int i)
{

  lectura();	//leo el modulo ds1307

  switch (i) {
    case 0:
      return second;
      break;
    case 1:
      return minuts;
      break;  
    case 2:
      return hour;
      break;
    case 3:
      return wday;
      break;
    case 4:
      return day;
      break;
    case 5:
      return month;
      break;
    case 6:
      return year;
      break;
  }
}

/*Esta suprutina regresa la hora y la fecha
dependendo del para metro que se le pase
(0 para Hora) (1 para Fecha)
Hora regresada= hora:minutos:segundos (12:02:55)
Fecha regresada= Dia-Semana dia/mes/anio (Mi 05/12/18):w
*/
String Reloj::desplegarFechaHora(int i)
{
   String s;
   //char s[10];// = "\0";
   //char dato[3] = "\0";
   int digit = 0;
    
   if (i == 0){	 //se regresa la hora
    for(int j = 2; j >= 0; j--){
      digit = desplegar(j);
      digit < 10? (s += ("0" + String(digit))): (s += String(digit));
      (j == 0)? (s += ""):(s += ":"); 
      //digit< 10? sprintf(dato, "0%d", digit): sprintf(dato, "%d", digit);
      //strcat(s, dato);         
      //(j == 0)? strcat(s, ""):strcat(s, ":");
                 
    }
   }

   else if (i == 1){ //se regresa la fecha
    for(int k = 4; k <= 6; k++){
      digit = desplegar(k);      
      digit < 10? (s += ("0" + String(digit))): (s += String(digit));
      (k == 6)? (s += ""):(s += "/");
      //digit < 10? sprintf(dato, "0%d", digit): sprintf(dato, "%d", digit);
      //strcat(s, dato);         	
      //(k == 6)? strcat(s, ""):strcat(s, "/");
    }

   } 
   
   return s;
	
}

bool Reloj::escritura(void)
{

  //Leemos las direcciones en memoria del arduino
  //de las variables de configuracion 	 	

  EEPROM.write(0x00, second);
  EEPROM.write(0x01, minuts);
  EEPROM.write(0x02, hour);
  EEPROM.write(0x03, wday);
  EEPROM.write(0x04, day);
  EEPROM.write(0x05, month);
  EEPROM.write(0x06, year);

  //Iniciar intercambio con el modulo RTC
  Wire.beginTransmission(0x68); //0x68 es la direccion del modulos de reloj

  Wire.write(0x00);//Borra la direcion de segunderos del modulo
  //Al escribir valores en los registros del modulo, hay que asegurar que  el bit clock halt
  //del registro del segundero este en LOW
  
  Wire.write(bintoBCD(second & 0x7F)); //Operacion binaria and
  Wire.write(bintoBCD(minuts));
  Wire.write(bintoBCD(hour));
  Wire.write(bintoBCD(wday));
  Wire.write(bintoBCD(day));
  Wire.write(bintoBCD(month));
  Wire.write(bintoBCD(year));

  //Terminamos la escritura y verificamos que el modulo RTC respondiendo
  if (Wire.endTransmission() != 0){
    Serial.println("PPA");
    return false;
  }
  else   //Almacenamos los valores de fecha y tiempo
    return true;
  
}

bool Reloj::lectura(void)
{
  uint8_t ctrl;
  // Iniciar el intercambio de información con el DS1307 (0xD0)
  Wire.beginTransmission(0x68);

  // Escribir la dirección del segundero
  Wire.write(0x00);

  // Terminamos la escritura y verificamos si el DS1307 respondio
  // Si la escritura se llevo a cabo el metodo endTransmission retorna 0
  if (Wire.endTransmission() != 0)
    return false;
  //else { Serial.println("Inicio de lectura correcto");}

  // Si el DS1307 esta presente, comenzar la lectura de 8 bytes  
  Wire.requestFrom(0x68, 8);
  //Serial.println("Comienzo de leectura");
  // Recibimos el byte del registro 0x00 y lo convertimos a binario

  // Continuamos recibiendo cada uno de los registros
  second = BCDtobin(Wire.read());
  minuts = BCDtobin(Wire.read()); 
  hour = BCDtobin(Wire.read());
  wday = BCDtobin(Wire.read());
  day = BCDtobin(Wire.read());
  month = BCDtobin(Wire.read());
  year = BCDtobin(Wire.read());

  ctrl = Wire.read();  

  //Alamacenamos el la memoria del microcontrolador 	
  /*
  EEPROM.write(0x00, second);	
  EEPROM.write(0x01, minuts);
  EEPROM.write(0x02, hour);
  EEPROM.write(0x03, wday);
  EEPROM.write(0x04, day);
  EEPROM.write(0x05, month);
  EEPROM.write(0x06, year);
  */
  return true;
}

/*
    Esta función convierte un número BCD a binario. Al dividir el número guardado
    en el parametro BCD entre 16 y multiplicar por 10 se convierten las decenas
    y al obtener el módulo 16 obtenemos las unidades. Ambas cantidades se suman
    para obtener el valor binario.
*/

//Funcion para convertir de binario a Bcd
uint8_t Reloj::bintoBCD(uint8_t bin)
{
  return (bin / 10 * 16) + (bin % 10);
}

uint8_t Reloj::BCDtobin(uint8_t bcd)
{
  // Convertir decenas y luego unidades a un numero binario
  return (bcd / 16 * 10) + (bcd % 16);
}
//**********************Fin de Clase para el manejo del reloj **********************//
