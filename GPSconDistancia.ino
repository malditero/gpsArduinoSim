#include <TinyGPS++.h>
#include <SoftwareSerial.h> 
int RXPin = 3; 
int TXPin = 4; 

int GPSBaud = 9600;

TinyGPSPlus gps;

SoftwareSerial gpsSerial(RXPin, TXPin);

void setup()
{
  Serial.begin(9600);

  gpsSerial.begin(GPSBaud);  
}

void loop()
{

  while (gpsSerial.available() > 0)
    if (gps.encode(gpsSerial.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println("No GPS detected");
    while(true);
  }
}

void displayInfo()
{
  if (gps.location.isValid())
  {
    const double latitudDestino=-0.982648;//coordenadas de prueba
    const double longitudDestino=-80.716773;
    navegacion(latitudDestino,longitudDestino);
  }
  else
  {
    Serial.println("Location: Not Available");
  }

  Serial.println();
  Serial.println();
  delay(1000);
}

void navegacion(double latitDestino, double longitDestino){
  double distanciaKm = gps.distanceBetween(gps.location.lat(),gps.location.lng(),latitDestino,longitDestino);
  double rumbo = gps.courseTo(gps.location.lat(),gps.location.lng(),latitDestino,longitDestino);
  Serial.print("Distancia (metros): ");
  Serial.println(distanciaKm);
  Serial.print("Direccion Personal: ");
  Serial.println(gps.cardinal(rumbo));
}
