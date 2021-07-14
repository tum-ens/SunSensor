/*
	Source code for SunSensor in serial application with MATLAB
	Written by Patrick Buchenberg patrick.buchenberg@tum.de
	Version2 - 14.06.2021 
*/

#define NUMPINS 5
#define NUMMEAS 1

int LED1 = 15;
int Strom = A0;

int sensorPin[] = {22, 28, 26, 24, 30};
double sensorValue[NUMPINS] = {0};
double sensorOffset[NUMPINS] = {0};

unsigned long timeZero = millis(), zeit;

void setup() 
{

  Serial.begin(115200);

  pinMode(LED1, OUTPUT);

  for(int i =0; i<NUMPINS; i++)
  {
    pinMode(sensorPin[i], INPUT);
    digitalWrite(sensorPin[i], LOW);
  }

  //Messung(sensorOffset);
  
}

void loop() 
{
  zeit = millis()-timeZero;
  Messung(sensorValue);
  Serial.print(zeit);
  Serial.print("\t");

  for(int i =0; i<NUMPINS; i++)
  {
    if(i)
      Serial.print("\t");
    Serial.print(sensorValue[i]);
  }
  Serial.println(""); 
}

void Messung(double* array)
{
  for(int i =0; i<NUMPINS; i++)
  {
    pinMode(sensorPin[i], OUTPUT);
    digitalWrite(sensorPin[i], HIGH);
    double summe = 0;
    for(int n =0; n<NUMMEAS; n++)
    {
      summe += analogRead(Strom);
    }
    pinMode(sensorPin[i], INPUT);
    digitalWrite(sensorPin[i], LOW);
    array[i] = summe/NUMMEAS;
  }
}

// ---------------------------------------------

void Auswertung(double* array, int anzahl)
{
  int offset_x = 0;
  int offset_y = 0;
  int offset_z = 0;

  double lux[anzahl] = {};
  for(int i=0; i<anzahl; i++)
  {
    lux[i] = log(array[i]);
  }
  
  double x = lux[1]-lux[3]-offset_x;
  double y = lux[2]-lux[4]-offset_y;
  double z = lux[5]-offset_z;

  int winkel_ebene = atan2d(y,x);
  int winkel_hoehe = atan2d(z,sqrt(sq(x)+sq(y)));

  int betrag = sqrt(sq(x)+sq(y)+sq(z));

  Serial.print('Winkel-Ebene [°]:'); Serial.println(winkel_ebene);
  Serial.print('Winkel-Höhe [°]:'); Serial.println(winkel_hoehe);
  Serial.print('Betrag:'); Serial.println(betrag);
}


int atan2d(double a, double b)  
{
  double pos = atan(a/b);
  if (b < 0 && a > 0)
    pos += 180;
  if (b < 0 && a < 0)
    pos -= 180;
    
  return pos*360/(2*PI);
}
