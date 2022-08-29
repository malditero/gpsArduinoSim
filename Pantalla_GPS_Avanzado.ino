#include <Wire.h>     // libreria para bus I2C
#include <Adafruit_GFX.h>   // libreria para pantallas graficas
#include <Adafruit_SSD1306.h>   // libreria para controlador SSD1306
#include <TinyGPS++.h>
#include <SoftwareSerial.h> 

 
#define ANCHO 128     // reemplaza ocurrencia de ANCHO por 128
#define ALTO 64       // reemplaza ocurrencia de ALTO por 64

#define OLED_RESET -1     // necesario por la libreria pero no usado SI se pone -1 usa el mismo pinreset de arduino

int RXPin = 3; 
int TXPin = 4; 

int GPSBaud = 9600;

TinyGPSPlus gps;

SoftwareSerial gpsSerial(RXPin, TXPin);

//Variables para antirrebote
const int Banda = 150;
long Time = 0;
int contador250ms = 0;
const int esperadePulso = 900; //Regula el tiempo que se tiene que mantener pulsado para que se ejecute la acción secundaria

volatile bool pulsado = false;
volatile bool nextSupremo = false;
volatile bool permiteAccesoMemoriaNormal = false;
volatile bool permiteAccesoMemoriaSupremo = false;
volatile bool fueSupremo = false;
volatile bool modoDistancia = false;

Adafruit_SSD1306 oled(ANCHO, ALTO, &Wire, OLED_RESET);  // crea objeto


static const double Edif32C[] PROGMEM = {-2.147184,-79.967843}; //FCSH
static const double Edif20F[] PROGMEM = {-2.145896,-79.965545}; //FICT
static const double Edif11A[] PROGMEM = {-2.144706,-79.967714}; //FIEC
static const double Edif24C[] PROGMEM = {-2.145001,-79.965839}; //FIMPC
static const double Edif31C[] PROGMEM = {-2.148743,-79.967853}; //CELEX

const char * bloques[] = {"11A","24C","31C","32C","20F"};

const char * facultades[] = {"FIEC","FIMCP","FCSH","FICT","CELEX"};

int pantalla = 0;
int select = 1;

int b = 0;
char edificio; 

//Funciones

int mostrarEnPantalla(int pantalla, int select){
  if (pantalla == 0)
  {
    oled.clearDisplay();      // limpia pantalla      
    oled.drawRect(0, 0, 128, 64, WHITE); // dibuja rectangulo
    oled.setTextSize(2);      // establece tamano de texto en 2
    oled.setTextColor(WHITE);   // establece color al unico disponible (pantalla monocromo) SOLO BLANCO Y NEGRO
    oled.setCursor(36, 5);   // ubica cursor en coordenadas 28,34 X,Y
    oled.print("ESPOL");      // escribe texto
    oled.setCursor(16, 24);   // ubica cursor en coordenadas 28,34 X,Y
    oled.print("Sistemas");
    oled.setCursor(11, 44);   // ubica cursor en coordenadas 28,34 X,Y
    oled.print("Embebidos");
    oled.display();     // muestra en pantalla todo lo establecido anteriormente
  }
  if (pantalla == 1)
  {
    oled.clearDisplay();      // limpia pantalla      
    //oled.drawRect(0, 0, 128, 64, WHITE); // dibuja rectangulo
    oled.setTextSize(2);      // establece tamano de texto en 2
    oled.setTextColor(WHITE);   // establece color al unico disponible (pantalla monocromo) SOLO BLANCO Y NEGRO
    oled.setCursor(36, 5);   // ubica cursor en coordenadas 28,34 X,Y
    oled.print("ESPOL");
    if (select == 1)
    {
      oled.drawRect(10, 30, 104, 24, WHITE); // dibuja rectangulo
      oled.setCursor(16, 34);   // ubica cursor en coordenadas 28,34 X,Y
      oled.print("Edificio");      // escribe texto
      oled.display();     // muestra en pantalla todo lo establecido anteriormente
      
      return 2;
    }
    if (select == 2)
    {
      oled.drawRect(10, 30, 104, 24, WHITE);
      oled.setCursor(16, 34);   // ubica cursor en coordenadas 28,34 X,Y
      oled.print("Facultad");      // escribe texto
      oled.display();     // muestra en pantalla todo lo establecido anteriormente
      return 3;
    }
    if (select == 3)
    {
      oled.drawRect(30, 30, 68, 24, WHITE);
      oled.setCursor(36, 34);   // ubica cursor en coordenadas 28,34 X,Y
      oled.print("Atras");      // escribe texto
      oled.display();     // muestra en pantalla todo lo establecido anteriormente
      return 0;
    }  
  }
  if (pantalla == 2)
  {
    oled.clearDisplay();      // limpia pantalla      
    //oled.drawRect(20, 20, 88, 44, WHITE); // dibuja rectangulo
    oled.setCursor(12, 4);   // ubica cursor en coordenadas 28,34 X,Y
    oled.setTextSize(2);      // establece tamano de texto en 2
    oled.setTextColor(WHITE);   // establece color al unico disponible (pantalla monocromo) SOLO BLANCO Y NEGRO
    oled.print("Edificio:");
    oled.setTextSize(3);
    if (select == 1)
    {
      oled.drawRect(26, 28, 64, 34, WHITE);
      oled.setCursor(32, 34);
      oled.print(bloques[select-1]);      
      oled.display();     // muestra en pantalla todo lo establecido anteriormente
      return 777;
    }
    if (select == 2)
    {
      oled.drawRect(26, 28, 64, 34, WHITE);
      oled.setCursor(32, 34);
      oled.print(bloques[select-1]);      // escribe texto
      oled.display();     // muestra en pantalla todo lo establecido anteriormente
      return 777;
    }
    if (select == 3)
    {
      oled.drawRect(26, 28, 64, 34, WHITE);
      oled.setCursor(32, 34);
      oled.print(bloques[select-1]);      // escribe texto
      oled.display();     // muestra en pantalla todo lo establecido anteriormente
      return 777;
    }
    if (select == 4)
    {
      oled.drawRect(26, 28, 64, 34, WHITE);
      oled.setCursor(32, 34);
      oled.print(bloques[select-1]);      // escribe texto
      oled.display();     // muestra en pantalla todo lo establecido anteriormente
      return 777;
    } 
    if (select == 5)
    {
      oled.drawRect(26, 28, 64, 34, WHITE);
      oled.setCursor(32, 34);
      oled.print(bloques[select-1]);      // escribe texto
      oled.display();     // muestra en pantalla todo lo establecido anteriormente
      return 777;
    } 
    if (select == 6)
    {
      oled.drawRect(30, 30, 68, 24, WHITE);
      oled.setCursor(36, 34);
      oled.setTextSize(2);
      oled.print("Atras");      // escribe texto
      oled.display();     // muestra en pantalla todo lo establecido anteriormente
      return 1;
    }  
  }
  return 9;
}

void interrupt_INT0_Init(){
 cli();
 EICRA = 0b01; //Cualquier cambio lógico
 EIMSK = 1; //INT0
 sei();
}

void interrupt_TIMER2_Init(){
  cli(); //deshabilitar momentáneamente las interrupciones
  TCCR2A = 0; //
  TCCR2B = 0b101; //preescalador 1024
  TIMSK2 = 1; // habilitar interrupcion por desbordamiento
  sei(); //habilitar las interrupciones
  TCNT2 = 12; //250ms // tiempo de la interrupcion
}

ISR(TIMER2_OVF_vect){ //INTERRUPCIONES //se hace el desplazamiento
  if(pulsado){
    if (contador250ms < esperadePulso)
    {
      contador250ms ++;
      TCNT2 = 12;
    }
  }
  if (pulsado)
  {
    if (contador250ms == esperadePulso)
    {
      if (!fueSupremo)
      {
        contador250ms = 0;

        nextSupremo = true;
        fueSupremo = true;
        permiteAccesoMemoriaSupremo = true;

        TCNT2 = 12;
      }
    }
  }
}

ISR(INT0_vect){
  if (millis()-Time > Banda)
  {
    pulsado = !pulsado;
    
    if (!pulsado)
    {
      if (contador250ms < esperadePulso){
        if (!fueSupremo)
        {
          select ++;
          permiteAccesoMemoriaNormal = true;

        }
      }
    }
    if (pulsado)
    {
      contador250ms = 0;
      fueSupremo = false;
    }
    
    
    Time = millis();
  }
  
 
}

void setup() {
  //Serial.begin(9600);
  delay(100);
  gpsSerial.begin(GPSBaud); 
  delay(100);
  Wire.begin();         // inicializa bus I2C
  delay(100);
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
#ifdef __DEBUG__
    //Serial.println("No se encuentra la pantalla OLED");
#endif
    while (true);
  } // inicializa pantalla con direccion 0x3C y la primera variable corresponde a alimentación del arduino (se puede poner externa)
  mostrarEnPantalla(pantalla,0);
  PORTD |= (1<<PD2); //Activar resistencia de PULLUP en PIN0 del puerto D
  interrupt_INT0_Init();
  interrupt_TIMER2_Init();

  oled.clearDisplay();
}
 
void loop() {

  if (permiteAccesoMemoriaSupremo || permiteAccesoMemoriaNormal)
  {
    permiteAccesoMemoriaNormal = false;
    permiteAccesoMemoriaSupremo = false;
    switch (pantalla) { 
    case 0:
      mostrarEnPantalla(pantalla,0);
      if (select != 1)
      {
        pantalla = 1;
        select = 1;
        mostrarEnPantalla(pantalla,select);
        contador250ms = 0;
        nextSupremo = false;
      }
      

      break;
    case 1:
      if (select > 3)
      {
        select = 1;
      }
      mostrarEnPantalla(pantalla,select);
        if (nextSupremo)
        {
          pantalla = mostrarEnPantalla(pantalla,select);
          select = 1;
          mostrarEnPantalla(pantalla,select);
          nextSupremo = false;
          contador250ms = 0;
        }
      break;
    case 2:
      if (select > 6)
      {
        select = 1;
      }
      mostrarEnPantalla(pantalla,select);
      if (nextSupremo)
        {
          pantalla = mostrarEnPantalla(pantalla,select);
          if (pantalla == 777)
          {
            modoDistancia = true;
            nextSupremo = false;
            contador250ms = 0;
            break;
          }
          select = 1;
          mostrarEnPantalla(pantalla,select);
          nextSupremo = false;
          contador250ms = 0;
        }
      break;
     case 3:
        pantalla = 0;
        select = 0;
        mostrarEnPantalla(pantalla,select);
        contador250ms = 0;
        nextSupremo = false;
     break;
     }
  } 
  while (modoDistancia)
  {
    if(gpsSerial.available() > 0){
      if (gps.encode(gpsSerial.read()))
      displayInfo();
    }
    
   if (permiteAccesoMemoriaNormal)
   {
      modoDistancia = false; //RECORDAR AL FINAL DE LA FUNCION WHILE DE MODO DISTANCIA AÑADIR UN IF PULSO EL BOTON, RESETEAR TODAS LAS VARIABLES A SU ESTADO INICIAL Y REGRESAR AL INICIO
      select = 1;
      pantalla = 0;
      mostrarEnPantalla(pantalla,select);
      nextSupremo = false;
      contador250ms = 0;
      permiteAccesoMemoriaNormal = false;
      permiteAccesoMemoriaSupremo = false;

   }
    
    delay(2000);
  }
  

}
void displayInfo()
{
  if (gps.location.isValid())
  {
    navegacion(-2.144706,-79.967714); //Bloque 11A
  }
  delay(1000);
}

void navegacion(double latitDestino, double longitDestino){
  double distanciaKm = gps.distanceBetween(gps.location.lat(),gps.location.lng(),latitDestino,longitDestino);
  double rumbo = gps.courseTo(gps.location.lat(),gps.location.lng(),latitDestino,longitDestino);
  oled.clearDisplay();      // limpia pantalla      
  oled.setTextSize(2);      // establece tamano de texto en 2
  oled.setTextColor(WHITE);   // establece color al unico disponible (pantalla monocromo) SOLO BLANCO Y NEGRO
  oled.setCursor(2, 5);   // ubica cursor en coordenadas 28,34 X,Y
  oled.print("Km:");
  oled.print(distanciaKm);
  oled.setCursor(2, 30);
  oled.print("Rumbo:");
  oled.print(gps.cardinal(rumbo));
  oled.display();
    
}
