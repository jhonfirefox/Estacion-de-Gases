//Programa para leer los sensores
#include <SoftwareSerial.h>
#include <Wire.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <math.h>
#include <SHT1x.h>
#include "co2.h" //Sensor CO2
#include "nh3.h" //Sensor de NH3
#include "ch4.h" //Sensore de CH4

//********definicion de variables de programa*****//
#define RX 11
#define TX 12
#define DATA 8
#define CLK 9//serial clock

//Declaraciones de constantes
#define BAUDIOS 9600
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTPIN 6    // what digital pin we're connected to
#define DHTPIN 5    // what digital pin we're connected to
//Declaraciones de los pines para el CD4051B
#define INH 6 //Pin para habilitar canales
//#define INH 2 //Pin para habilitar canales
#define A 4   //Selector de canal A
#define B 3   //Selector de canal B
#define C 2   //Selector de canal C

#define LONGITUD_DATO 10
/************************Hardware Related Macros************************************/
//#define MG_PIN 0 //pin analogico al arduino
//#define BOOL_PIN 7 // pin al  umbral del comparador (puede se interrup)
//#define DC_GAIN 4.5 //ganacia del ampliciador

/***********************Software Related Macros************************************/
//#define READ_SAMPLE_INTERVAL 50 // cuantas muestras se tomaran
//#define READ_SAMPLE_TIMES 5 // definir intervalo entre cada muestra


/**********************Application Related Macros**********************************/

//#define ZERO_POINT_VOLTAGE 0.220 // define la salida del sensor en voltios cuando la concentracion de c02 es de 400 ppm
//#define REACTION_VOLTGAE 0.020 //define la caida de tension cuando   se mueve el sensor a 1000 ppm

/*****************************Globals***********************************************/
//float CO2Curve[3] = {2.602,ZERO_POINT_VOLTAGE,(REACTION_VOLTGAE/(2.602-3))}; 
//dos puntos se toman de la curva. 
//con estos dos puntos, se forma una línea que es 
//'aproximadamente equivalente' a la curva original.
//data format:{ x, y, slope}; point1: (lg400, 0.324), point2: (lg4000, 0.280) 
//slope = ( reaction voltage ) / (log400 –log1000) 

/*****************************Viarables para el sensor de NH3***********************************************/
//#define RL 1  //The value of resistor RL is 47K
//#define m -0.263 //Enter calculated Slope 
//#define b 0.42 //Enter calculated intercept
//#define Ro 20 //Enter found Ro value
//#define MQ_sensor A0 //Sensor is connected to A4

SoftwareSerial puerto1(RX, TX);
DHT dht(DHTPIN, DHTTYPE); //Objeto tipo sensor de Humedad Aire
SHT1x th_sensor(DATA, CLK); //Objeto tipo sensor de Humedad Suelo

#define HUM_A 0
#define TEM_A 1
#define HUM_S 2
#define TEM_S 3

#define SENSOR_CO2 8
#define SENSOR_NH3 7
#define SENSOR_CH4 5

#define HumAm 0
#define TemAm 1
#define HumSue 2
#define TemSue 3
#define CO2_CON 4 
#define NH3_CON 5 
#define CH4_CON 6 


CO2_SENSOR sen1(0); //Se le dal el pin donde esta ubicado el sensor
NH3_SENSOR sen2(0);
CH4_SENSOR sen3(0);

float datos[7] ={0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; 
               //Hum-A, Tem-A, Hum-S, Tem-S, CO2, NH3, CH4
void setup() {
  // put your setup code here, to run once: 
  Serial.begin(BAUDIOS);
  puerto1.begin(BAUDIOS);
  dht.begin();
  pinMode(INH, OUTPUT);
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);

  digitalWrite(INH, HIGH); //Des-Habilita los canales
  
}

void loop() {
  // put your main code here, to run repeatedly:  
  int leectura = 0;
  float humedad = 0;       //Variable para humedad del ambiente  
  float temperatura = 0;   //Variable Temperatura ambiente
  float temp_c;            //Temperatura suelo
  float humid;             //Humedad suelo
  //uint16_t dato;

  //Se lee la humedad y temperatura ambiente
  //***********************************************************************//
  //Se mide la humedad y la temperatura del ambiente
  humedad = dht.readHumidity();  
  temperatura = dht.readTemperature();
  //***********************************************************************//

  //Se lee la humedad y temperatura del ambiente
  //***********************************************************************//
  humid = th_sensor.readHumidity();// Read values from the sensor  
  temp_c = th_sensor.readTemperatureC();
  //***********************************************************************//
  datos[0] = humedad;     //Almacena el valor de la humedad del aire
  datos[1] = temperatura; //Almacena el valor de la temperatura del aire
  datos[2] = humid;       //Alamcena la humedad del suelo
  datos[3] = temp_c;      //almacena la temperatura del suelo
  
  //Serial.println("**********CO2***********");
  //Se lee el sensor de CO_2, en porcentaje
  leerCanal(SENSOR_CO2);
  datos[CO2_CON] = sen1.CO2Porcentaje();
  digitalWrite(INH,HIGH); //Deshabilitamos las mediciones
  
  //Se lee el sensor de NH3 (Amoniaco)
  leerCanal(SENSOR_NH3);
  datos[NH3_CON] = sen2.NH3PPM();
  digitalWrite(INH,HIGH); //Deshabilitamos las mediciones

  //Se lee el sensor de CH4 (Metano)
  leerCanal(SENSOR_CH4);
  datos[CH4_CON] = sen3.CH4PPM();
  digitalWrite(INH,HIGH); //Deshabilitamos las mediciones  

  enviarDatos();    
}

bool leerCanal(uint8_t canal)//, uint16_t *dato)
{  
  switch(canal){
    case 1://Canal 0 del  cd4051B
          digitalWrite(A, LOW);
          digitalWrite(B, LOW);
          digitalWrite(C, LOW);                    
          break;  
    case 2://Canal 1 del  cd4051B
          digitalWrite(A, HIGH);
          digitalWrite(B, LOW);
          digitalWrite(C, LOW);                    
          break;  
    case 3://Canal 2 del  cd4051B
          digitalWrite(A, LOW);
          digitalWrite(B, HIGH);
          digitalWrite(C, LOW);                    
          break;  
    case 4://Canal 3 del  cd4051B
          digitalWrite(A, HIGH);
          digitalWrite(B, HIGH);
          digitalWrite(C, LOW);                    
          break;  
    case 5://Canal 4 del  cd4051B
          digitalWrite(A, LOW);
          digitalWrite(B, LOW);
          digitalWrite(C, HIGH);                    
          break;  
    case 6://Canal 5 del  cd4051B
          digitalWrite(A, HIGH);
          digitalWrite(B, LOW);
          digitalWrite(C, HIGH);                    
          break;  
    case 7://Canal 6 del  cd4051B
          digitalWrite(A, LOW);
          digitalWrite(B, HIGH);
          digitalWrite(C, HIGH);                    
          break;  
    case 8://Canal 7 del  cd4051B
          digitalWrite(A, HIGH);
          digitalWrite(B, HIGH);
          digitalWrite(C, HIGH);                    
          break;
     default:
          digitalWrite(A, LOW);
          digitalWrite(B, LOW);
          digitalWrite(C, LOW);
          break;
  }

  digitalWrite(INH, LOW); //Habilitamos los canales
  delay(10);
  //dato = analogRead(0);
  //digitalWrite(INH,HIGH); //Deshabilitamos las mediciones  
  
  return true;
}

void enviarDatos(void)
{
   String valor;  
   
   for(uint8_t i = 0; i <= 6; i++){
    if (i == 0)
       valor += String(datos[i], 5);     
    else
       valor += " " + String(datos[i], 5);    

   }   
      
   puerto1.print(valor);
   Serial.println(valor);    
}
