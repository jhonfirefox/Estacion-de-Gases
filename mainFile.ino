/*
Proyecto estacion de gases
Diseño de la interface de software
*/

/*Notas:
 * Mejorar el programa de las teclas
 * Poner un protector de pantalla, cuando este inactivo el programa durante mas de 1 min apagar la pantalla
 * Visualizar la informacion util en la pantalla
*/
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <LCD.h>
#include "teclas.h"
#include "letreros.h"
//#include "memoriaDS.h"
#include "ds1307.h"
//#include "bluetooth.h"

#define I2C_ADDR 0x3F// <<-Di1ección del esclavo
#define debounceDelay 30    //Delay para el debounce de tecla
#define retrasoTeclas 100 //Retraso de 100 milisegundos despues de presionar la tecla
#define BYTES 70
#define BAUDIOS 9600   
#define NUMERO_TECLAS 4
#define TIMER_INTERRUP 100 //Tiempo entre interrupciones ms
#define KEY 6
#define STATE 1

//Pin de chip slect, pueto SPI
#define CS 10
/*Definicion de los pines para el manejo de la pantalla*/
#define BACKLIGHT_PIN 3
#define En 2
#define Rw 1
#define Rs 0
#define D4 4
#define D5 5
#define D6 6
#define D7 7

//Deficion de direcciones de programa
#define ADRESS_STATE 0x07 //Almacena el bit para habilitar el programa
#define TIEMPO_MEDICION 0x08 //Alamacena el tiempo de medicion
#define ADRESS_SENSOR1 0x09  //Direccion para almacenar Tem ambiente
#define ADRESS_SENSOR2 0x0B  //Direccion para almacenar Hum ambiente
#define ADRESS_SENSOR3 0x0D  //Direccion para almacenar Tem suelo
#define ADRESS_SENSOR4 0x0F  //Direccion para almacenar Hum suelo
#define ADRESS_SENSOR5 0x011 //Direccion para almacenar concentra de Amoniaco
#define ADRESS_SENSOR6 0x013 //Direccion para almacenar concentra de Dioxido de Carbono
#define ADRESS_SENSOR7 0x015 //Direccion para almacenar concentra de Metano
 
//Pines correspondientes a las teclas

enum TECLAS { Tecla1 = 2, Tecla2 , Tecla3, Tecla4};
//#define Tecla1 4
//#define Tecla2 3
//#define Tecla3 2
//#define Tecla4 5

LiquidCrystal_I2C lcd(I2C_ADDR, En, Rw, Rs, D4, D5, D6, D7);
//MemoriaSD MSD; //crea el objeto memoria

Teclas teclas(NUMERO_TECLAS,Tecla1,debounceDelay);
Reloj reloj;      //objeto reloj
//Bluetooth BLUE("ESTACION1", "1234",0,1, KEY, STATE, BAUDIOS);
File miArchivo;
const char *archivo = "datos.txt";

//definicion de funciones
void fechaHora(void); //configura la fecha y la hora
void ejecutarPrograma(void); //Ejecuta el programa
void TiempoMedicion(void);  //tiempo entre medicion
void iniciarMedicion(void);  //Inicia la medicion
void configuracionBluetooth(void); //configura el bluetooth
void habilitarPrograma(void); //Habilitar programa
void enviarDatos(void);
void borrarMemoria(void);

void setup() {
  
  //BLUE.habilitar(false); //desabilitamos el bluetooth
  
  Serial.begin(BAUDIOS); //Configura la transmision de datos por puerto serial a una 
  
  Wire.begin();//Inicia transmion serial

  //Configurando las entradas de las teclas 
  pinMode(Tecla1,INPUT);
  pinMode(Tecla2,INPUT);//https://www.arduino.cc/en/Tutorial/ReadWrite
  pinMode(Tecla3,INPUT);
  pinMode(Tecla4,INPUT);

  //Configurando la pantalla
  lcd.begin(16, 2); //La pantalla a usar tiene 4 filas por 20 caracteres
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE); 
  lcd.setBacklight(HIGH);
  //lcd.clear();
  //lcd.home();
  //lcd.print(EMPRESA);
    
  //delay(3000);  
  lcd.clear(); 
  lcd.home();

  if (!SD.begin(CS)){
    lcd.print("No SD-Inicializada");
  }
  
  else{  
    //Serial.println("Inicializacion exitosa");     
    
    lcd.print("SD-Inicializada");

    //if(!SD.exists(nombreArchivo)){
    //  miArchivo = SD.open("datos.txt");        
    //  miArchivo.close();
    //}
    
    //delay(2000);  
    
    lcd.setCursor(0,1);
    //lcd.home();
    
    miArchivo = SD.open(archivo, FILE_WRITE);        
    
    if (miArchivo){
      
      lcd.print("ESCRIBIENDO-SD");
      
      //miArchivo.println("Estacion-Gases");

      //miArchivo.print("Hum-A(%)");
      //miArchivo.print(" Tem-A(C)");
      //miArchivo.print(" Hum-S(%)");
      //miArchivo.print(" Tem-S(C)");
      //miArchivo.print(" Tem-S(C)");Medicion Iniciada: 
      //miArchivo.print(" CO2(ppm)");
      //miArchivo.print(" NH3(ppm)");
      //miArchivo.println(" CH4(ppm)");
      delay(10);    
      miArchivo.close();  
    }
    
    else{
      lcd.print("ERR-ABRIR-SD");
    }   
  }

  delay(2000);

  //Configura la fecha y la hora a una valor especifico
  //reloj.steTiempo(hora, minutos, segundos);
  //reloj,.setFecha(dia, dia-semana, mes, anio);
  
  reloj.setTiempo(EEPROM.read(0x02), EEPROM.read(0x01), EEPROM.read(0x00));     
  reloj.setFecha(EEPROM.read(0x03),EEPROM.read(0x04),EEPROM.read(0x05),EEPROM.read(0x06));    

  //reloj.setTiempo(7, 0, 0);
  //reloj.setFecha(1, 1, 1, 18);
  //reloj.escritura();  //escribe en el reloj  
   
  lcd.clear();
}

void loop() {  

 //EEPROM.write(ADRESS_STATE, 0); //Se obliga al programa a entrar al menu
 if (EEPROM.read(ADRESS_STATE) == 0) //Programa activado
    Menu();
 else
    ejecutarPrograma();  
}

//Funcion para desplegar el menu 
void Menu(void)
{
   int i = 0;
   int eleccionMenu = 0; 
   bool salirMenu = false;
   const int OpMenu = 5; //este valor se define en programacion de las estaciones
   char *title;
    
   lcd.setCursor(3, 0);
   lcd.print("Estacion V1");
    //lcd.setCursor(0,1);
    //lcd.print("Fechas: //");
   
   lcd.setCursor(0, 1);
   lcd.print("  Presionar ENT");

   if (teclas.teclaEstado())
   { 
      delay(200);
      if (true)//(teclas.getTecla() == Tecla1) //El programa ingresa a correr
      {   
          teclas.setTecla(Tecla4);
          while (!salirMenu) {

                lcd.clear(); //Limpia la pantalla
                do {
                    //lcd.clear(); //Limpia la pantalla
                    lcd.setCursor(0, 0);
                    lcd.print("      Menu    ");
                    lcd.setCursor(0, 1);
                    lcd.print(i + 1);
                    lcd.print("=> ");

                    //switch para los letreros
                    title = letreros(i+1);
                    lcd.print(title);

                }while (!teclas.teclaEstado()); 
          
                delay(debounceDelay);

                switch (teclas.getTecla()) {//Determina cual tecla presiono
                  case (Tecla1):
                                eleccionMenu = i + 1; //Almacena la opcion elegida
                                break;
                  case (Tecla2):
                                --i;
                                if (i < 0)
                                    i = OpMenu - 1;
                                break;
                  case (Tecla3):
                                ++i;
                                if ( i == OpMenu)
                                    i = 0;
                                break;
                  case (Tecla4):
                                i = 0;
                                break;
                }

                switch (eleccionMenu) { //Elige la funcino del menu
                  case 1:
                         fechaHora(); //configura la fecha y la hora
                         break;
                  case 2://Ejecuta el programa              
                         ejecutarPrograma(); //Ejecuta el programa
                         break;
                  case 3:
                         TiempoMedicion();  //tiempo entre medicion
                         break;
                  /*case 4:
                         configuracionBluetooth* (); //configura el bluetooth
                         break;*/
                  case 4:
                         habilitarPrograma(); //Habilitar programa
                         break;  
                  //case 5://Envio de datos serial mente
                   //      enviarDatos();
                    //     break;
                  //case 6://Envio de datos serial mente
                    //     borrarMemoria();
                     //    break;
                  //case 7: 
                    //     calibrarSensores();       
                      //   break;

                  case 5:
                        salirMenu = true;
                        break;        
                }

          eleccionMenu = 0; //Ninguna opcion elegida                 
        }
     }
    }
   
}

void fechaHora(void) //configura la fecha y la hora
{  
  //char *titulo;     //imprime el MENU1   
  int contador1 = 0;
  int contador2 = 0;
  int opcion = 0;
  int posicionCursor = 11;

  //titulo = letreros(1);
  
  //#if 0
  lcd.blink();
  
  while(true){
    lcd.clear(); //limpia la pantalla
    //lcd.cursorHome();   //pone el cursor en la posicion(0,0)
    lcd.setCursor(0,0);
    lcd.print(MENU1);    

    do{              
      if (opcion == 0)
         imprimirTiempo();
      else
        imprimirFecha();
        
      switch(contador1){
         case 0:
                posicionCursor = 11;
                break;
         case 1:
                posicionCursor = 8;
                break;              
         case 2:
                posicionCursor = 5;
                break;              
         case 3:
                posicionCursor = 0;
                break;              
         case 4:
                posicionCursor = 5;
                break;              
         case 5:
                posicionCursor = 8;
                break;              
         case 6:
                posicionCursor = 11;
                break;              
      }    
    
      lcd.setCursor(posicionCursor,1);       
            
    }while(!teclas.teclaEstado());

    delay(debounceDelay-10);  
    
    switch(teclas.getTecla()){
      case Tecla1:                    
                  if (++contador1 > 6)
                     contador1 = 6;  
                      
                  if (contador1 >= 3)
                     opcion = 1;
                  else
                     opcion = 0;                    
                  break;
      case Tecla2:                  
                  if (++contador2 > 100)
                       contador2 = 0;                       
                    
                  contador2 = reloj.configuracionTiempo(contador1, contador2);                    
                  break;
      case Tecla3:
                  if (--contador2 < 0)
                       contador2 = 100;                       
                    
                  contador2 = reloj.configuracionTiempo(contador1, contador2);                  
                  break;
      case Tecla4:
                  if (--contador1 < 0)
                     contador1 = 0;
                  if (contador1 >= 3)
                     opcion = 1;    //pasa al segundo menu 
                  else
                     opcion = 0;
                  break;                                                      
    }          

    delay(200);

    if (contador1 == 6){
       lcd.noBlink();
       lcd.clear();
       lcd.home();
       lcd.print("Establecer?");
       lcd.setCursor(0,1);
       lcd.print("Fecha y Hora");

       do{
       }while(!teclas.teclaEstado());

       lcd.clear();
       lcd.home();  
       
       if (teclas.getTecla() == Tecla1){
          reloj.escritura();
          //lcd.clear();
          //lcd.home();
          lcd.print("Hora-Fecha");
          lcd.setCursor(0,1);
          lcd.print("Establecida");
          //delay(3000); //espera tres segundo y sale
          break;
       }
       else if (teclas.getTecla() == Tecla4){
          //lcd.clear();
          //lcd.home();
          lcd.print("Hora-Fecha");
          lcd.setCursor(0,1);
          lcd.print("No Establecida");
          //delay(3000); //espera tres segundo y sale       
          break;
       }
       
       else{
          contador1 = 0;
          contador2 = 0;
          opcion = 0;
       }
       delay(3000); //espera tres segundo y sale                                
    }
    
  }
  //#endif  
  delay(2000);
}

void imprimirTiempo(void)
{  
   char s[10] = "\0";
   char dato[3] = "\0";
   int digit = 0;
 

    for(int j = 2; j >= 0; j--){
      digit = reloj.obtenerTiempo(j);
      digit< 10? sprintf(dato, "0%d", digit): sprintf(dato, "%d", digit);
      strcat(s, dato);         
      (j == 0)? strcat(s, ""):strcat(s, ":");
                 
    }

   lcd.setCursor(4,1);
   lcd.print(s);       
}

void imprimirFecha(void)
{
   char s[13] = "\0";
   char dato[3] = "\0";
   int digit = 0;

   digit= reloj.obtenerTiempo(3); 
   switch(digit){
      case 1:
            sprintf(s, "Lu  ");
            break;
      case 2:
            sprintf(s, "Ma  ");
            break;           
      case 3:
            sprintf(s, "Mi  ");
            break;             
      case 4:
            sprintf(s, "Ju  ");
            break;         
      case 5:
            sprintf(s, "Vi  ");
            break;
      case 6:
            sprintf(s, "Sa  ");
            break;                           
      case 7:
            sprintf(s, "Do  ");
            break;            
   }
      
    for(int k = 4; k <= 6; k++){
      digit = reloj.obtenerTiempo(k);
      digit < 10? sprintf(dato, "0%d", digit): sprintf(dato, "%d", digit);
      strcat(s, dato);           
      (k == 6)? strcat(s, ""):strcat(s, "/");
    }
    lcd.setCursor(0,1);
    lcd.print(s);
}

void ejecutarPrograma(void) //Ejecuta el programa
{
  teclas.setTecla(Tecla1); //Si el programa inicia con la Tecla 4 presionada no corre
  
  unsigned long contadorT = 0;  //Variable para almacenar la duracion del tiempo
  unsigned long duracion = EEPROM.read(TIEMPO_MEDICION); //se lee cuantas interrupciones

  if (duracion == 0 || duracion < 0)
     duracion = 1000; //almacena mediciones cada segundo
  else
     duracion = duracion*60000; //el tiempo quede en milisegundos                                         

  contadorT = millis(); //Inicia el conteo del tiempo

  //Se debe leer el reloj antes  
  reloj.lectura();
  almacenarReloj();

  miArchivo = SD.open(archivo, FILE_WRITE); //Se abre el archivo para almacenar los datos

  //Comprobar que las primeras lineas este almacenado el tiempo respectivo
  if(miArchivo)
  {
    miArchivo.println("Medicion Iniciada: ");
    miArchivo.print("Hora: ");
    miArchivo.println(reloj.desplegarFechaHora(0));
    miArchivo.print("Fecha: ");
    miArchivo.println(reloj.desplegarFechaHora(1));
  }

  miArchivo.close();
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Ejecutando");
  lcd.setCursor(4,1);
  lcd.print("Programa");
  
  do{
    
    reloj.lectura(); //Actualizo la fecha y hora
    //Pantalla1(); //Se despliega la fecha y la hora
    //Serial.println(reloj.desplegarFechaHora(0));

    if ((millis() - contadorT) >= duracion)
    {        
        
        miArchivo = SD.open(archivo, FILE_WRITE);
        
        if (reloj.obtenerTiempo(4) != EEPROM.read(0x04)){ //Se obtiene el dia           
           miArchivo.print("Hora: ");
           miArchivo.println(reloj.desplegarFechaHora(0));
           miArchivo.print("Fecha: ");
           miArchivo.println(reloj.desplegarFechaHora(1));
           almacenarReloj();
        }

        //Despliegue de rutina de medicion        
        //Wire.requestFrom(2,BYTES); //Espera N bytes del Esclavo 2         
        
        //while(Wire.available()){
        miArchivo.print(reloj.desplegarFechaHora(0));
        miArchivo.print(" - ");
        while(Serial.available()){  
          char c = Serial.read();
          //Serial.print(c);

          if (c != ' ')
            miArchivo.print(c);
          else
            miArchivo.print(" - ");
        }       

        //Serial.println("");//Linea para eliminarse    
        miArchivo.println("");        
        miArchivo.close();
        contadorT = millis(); //Actualiza millis
        
    }
    
    if (teclas.teclaEstado())
      if(teclas.getTecla() == Tecla4)
        break;
    
  }while(true); //no se sale del programas hasta que se presione la tecla 4
  
  EEPROM.write(ADRESS_STATE, 0); //Se vuelve al menu principal
  lcd.clear();
  lcd.home();
  //miArchivo.close();  
  delay(500);
}

void TiempoMedicion(void){  //tiempo entre medicion
  lcd.clear();

  int contador1 = 1;    
  
  do{
    lcd.clear(); //limpia la pantalla
    //lcd.cursorHome();   //pone el cursor en la posicion(0,0)
    lcd.setCursor(0,0);
    lcd.print(MENU3);    

    do{              
       lcd.setCursor(0,1);
       lcd.print(">>> ");    
       imprimirTiempoMedicion(contador1);     
    }while(!teclas.teclaEstado());

    delay(debounceDelay);
     
    
    switch(teclas.getTecla()){
      case Tecla1:
                  switch(contador1){
                    case 1:
                          EEPROM.write(TIEMPO_MEDICION,0); //Si es cero tiempo m. 1 segundo
                          break;
                    case 2:
                          EEPROM.write(TIEMPO_MEDICION,1); //Si es cero tiempo m. 1 minuto
                          break;
                    case 3:
                          EEPROM.write(TIEMPO_MEDICION,10); //Si es cero tiempo m. 10 minutos
                          break;

                    case 4:
                          EEPROM.write(TIEMPO_MEDICION,30); //Si es cero tiempo m. 30 minutos
                          break;                                                           
                    case 5:
                          EEPROM.write(TIEMPO_MEDICION,60); //Si es cero tiempo m. 60 minutos
                          break;                                             
                  }
                  break;
      case Tecla2:
                  if(++contador1 > 5)
                     contador1 = 1;                   
                  break;
      case Tecla3:
                  if(--contador1 < 0)
                    contador1 = 5;  
                  break ;
      case Tecla4:
                  
                  break;                                                      
    }             
    
    delay(200);    

  }while(teclas.getTecla() != Tecla4);  

  lcd.clear();
  lcd.home();
  lcd.print("Tiempo Medion");
  lcd.setCursor(0,1);  
  lcd.print("a  ");
  imprimirTiempoMedicion(contador1);  
  
  delay(2000);
}

void imprimirTiempoMedicion(int t)
{
  switch(t){
       case 1:
             lcd.print(LETRERO3);
             break;     
       case 2:
             lcd.print(LETRERO4);
             break;             
       case 3:
             lcd.print(LETRERO5);
             break;             
       case 4:
             lcd.print(LETRERO6);
             break;                         
       case 5:
             lcd.print(LETRERO7);
             break;                            
    }    
}

/*Este funcion deshabilita el programa
 * Por menu se pude configurar si se habilita o no el programa 
 * Si se presiona la tecla 2 se habilita el programa despues de presionar enter(Tecla 1)
 * Si se presiona la tecla 3 se deshabilita el programa despues de presionar la tecla1
 * Si se presiona la tecla de salir, se deshabilita el programa
 */
 
void habilitarPrograma(void) //Habilitar programa
{  
  bool habilitar = false; //no esta habilitado el programa
  
  lcd.clear();
  do{
      lcd.home();
      lcd.print(MENU4);
      lcd.setCursor(4,1);
      lcd.print(">>> "); 
      
      if (habilitar)
         lcd.print("SI");
      else
         lcd.print("NO");
               
      do{      
      }while(!teclas.teclaEstado());    
      
      switch(teclas.getTecla())
      {
         case Tecla1:
                     if (habilitar)
                      EEPROM.write(ADRESS_STATE, 1);  //Se habilita el programa
                     else 
                      EEPROM.write(ADRESS_STATE, 0);                     

                     lcd.clear();
                     lcd.home();
                     lcd.print(MENU4);                                   
                     lcd.setCursor(0,1);
                     
                     if (habilitar)
                        lcd.print("Habilitado");
                     else
                        lcd.print("No Habilitado");

                     teclas.setTecla(Tecla4);                        

                     break;
         case Tecla2:                     
                     habilitar = true;
                     break;
         case Tecla3:                     
                     habilitar = false;                            
                     break;
         case Tecla4:  
                     EEPROM.write(ADRESS_STATE, 0); //Se deshabilita el programa    
                     lcd.clear();
                     lcd.home();
                     lcd.print(MENU4);                                   
                     lcd.setCursor(0,1);
                     lcd.print("No Habilitado");
                     break;
      }      
               
  }while(teclas.getTecla() != Tecla4);

  delay(2000);   
}

char *letreros(int i)
{
   char let[16] = "\0"; 
   
   switch (i){
      case 1:
            strcpy(let, MENU1);
            break;
      case 2:
            strcpy(let, MENU2);
            break;                        
      case 3:
            strcpy(let, MENU3);
            break;
      case 4:
            strcpy(let, MENU4);
            break;
      //case 5:
        //    strcpy(let, MENU5);
          //  break;
      //case 6:
        //    strcpy(let, MENU6);
          //  break;
      //case 7:
       //     strcpy(let, MENU7);
        //    break;
      case 5:
            strcpy(let, MENU5);
            break;
      /*case 9:
            strcpy(let, MENU9);
            break;*/            
      default:
            strcpy(let, MENU1);
            break;                                                                                                                  
   }
   
   return let;
}

/*
void Pantalla1(void)
{
  String formato;  
  char cadena[9];
  
  //lcd.clear(); 
  //reloj.lectura();
  lcd.home();
  lcd.print("P. Ejecutandose");  
  
  //lcd.setCursor(4,1);  
  //formato = reloj.desplegarFechaHora(1);
  formato = reloj.desplegarFechaHora(0);  //Se desplega la hora    
  
  formato.toCharArray(cadena, formato.length()+1);    
  cadena[9] = '\0';
  lcd.print(cadena);
  //Serial.println(formato);
    
  //lcd.print(" ");
  //formato = reloj.desplegarFechaHora(1);  
  //formato.toCharArray(cadena, formato.length()+1);    
  //lcd.print(cadena); //desplegar el tiempo 0
}*/

//Esta funcion almacena la hora actual del reloj RTC en la 
//direccion 0x00  - 0x06 de la memoria de datos del arduino
void almacenarReloj(void){
  for (uint8_t direccion = 0; direccion <= 6; direccion++)
      EEPROM.write(direccion, reloj.obtenerTiempo(direccion));
}
