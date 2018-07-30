// Definicion de clase Reloj
#ifndef DS1307_H
#define DS1307_H

class Reloj {
  public:
    Reloj();
    uint8_t configuracionTiempo(int, uint8_t);
    uint8_t obtenerTiempo(int);
    bool setTiempo(int, int , int); //(hora, minutos, segundos)
    bool setFecha(int, int, int, int);	//(DiaSemana, dia, mes, anio)
    String desplegarFechaHora(int);
    uint8_t desplegar(int);		//obtiene cualquier cantidad de tiempo	
    bool escritura();
    bool lectura();

  private:
    //variables para el tiempo
    uint8_t bintoBCD(uint8_t);
    uint8_t BCDtobin(uint8_t);
    uint8_t second;   //Varaible para los segundos 0-59
    uint8_t minuts;   //variables para los minutos 0-59
    uint8_t hour;     //variable para las horas 0-24
    uint8_t wday;     //varaible para el dia de la semana 1-7
    //Variables para la fecha
    uint8_t day;      //variable para el dia 1-31
    uint8_t month;    //variable para el dia 1-12
    uint8_t year;     //varaible para el año
};

#endif
