/*
	Source code for SunSensor in standalone application
	Written by Patrick Buchenberg patrick.buchenberg@tum.de
	Version2 - 14.06.2021 
*/

#define NUMPINS 5
#define NUMMEAS 1000

#define RMESS 2e3
#define ADC_AUFLOESUNG 1024
#define A 125366
#define B -0.709

#define X 0
#define Y 1
#define Z 2

int LED1 = 13, TASTER1 = 8;
int Strom = A0;
int sensorPin[] = {22, 28, 26, 24, 30};

double sensorValue[NUMPINS] = {};
double Koordinaten[3];
int offset_x = 0;
int offset_y = 0;
int offset_z = 0;

void setup() {

  Serial.begin(115200);
  Serial.println("SunSensor starts ... v2");

  pinMode(LED1, OUTPUT);
  pinMode(TASTER1, INPUT);
  digitalWrite(TASTER1, HIGH);
  for(int i =0; i<NUMPINS; i++)
  {
    pinMode(sensorPin[i], INPUT);
    digitalWrite(sensorPin[i], LOW);
  }
}

void loop() 
{
  if(!digitalRead(TASTER1))
  {
    digitalWrite(LED1, HIGH);
    Messung(sensorValue,1000);
    Kalibrierung(sensorValue);
    delay(1000);  
    digitalWrite(LED1, LOW);
  }
  
  Messung(sensorValue, NUMMEAS);
  Auswertung(sensorValue);

  delay(500);  
}

void Messung(double* array, int Anzahl_Messungen)
{
  for(int i =0; i<NUMPINS; i++)
  {
    pinMode(sensorPin[i], OUTPUT);
    digitalWrite(sensorPin[i], HIGH);
    double summe = 0;
    for(int n =0; n<Anzahl_Messungen; n++)
    {
      summe += analogRead(Strom);
    }
    pinMode(sensorPin[i], INPUT);
    digitalWrite(sensorPin[i], LOW);
    array[i] = summe/Anzahl_Messungen;
  }
}

// ---------------------------------------------

void Auswertung(double* array)
{
  double* Position = XYZ(array);
  double x = Position[X];
  double y = Position[Y];
  double z = Position[Z];
  
  int winkel_ebene = atan2d(y,x);
  int winkel_hoehe = atan2d(z,sqrt(sq(x)+sq(y)));
  double betrag = sqrt(sq(x)+sq(y)+sq(z));

  Serial.print("Winkel-Ebene [°]:"); Serial.println(winkel_ebene);
  Serial.print("Winkel-Höhe [°]: "); Serial.println(winkel_hoehe);
  Serial.print("Betrag:          "); Serial.println(betrag);
}

double* XYZ(double* adc)
{
  double lux[NUMPINS] = {};
  for(int i=0; i<NUMPINS; i++)
  {
    double R_ldr = RMESS*(ADC_AUFLOESUNG/adc[i]-1);
    lux[i] = pow(R_ldr/A,1/B);
    //Serial.print("R:"); Serial.println(R_ldr);
    Serial.print("L:"); Serial.println(lux[i]);
  }
  
  Koordinaten[X] = lux[0]-lux[2]-offset_x;
  Koordinaten[Y] = lux[1]-lux[3]-offset_y;
  Koordinaten[Z] = lux[4]-offset_z;

  Serial.print("x:"); Serial.println(Koordinaten[X]);
  Serial.print("y:"); Serial.println(Koordinaten[Y]);
  Serial.print("z:"); Serial.println(Koordinaten[Z]);

  return Koordinaten;
}

int atan2d(double a, double b)  
{
  double winkel = atan(a/b);
  winkel *= 360/(2*PI);
  if (b < 0 && a > 0)
    winkel += 180;
  if (b < 0 && a <= 0)
    winkel -= 180;
    
  return winkel;
}

void Kalibrierung(double* array)
{
  Serial.println("Kalibrierung gestartet!...");
  double* Position = XYZ(array);
  offset_x += Position[X];
  offset_y += Position[Y];
  offset_z += Position[Z];

  Serial.print("Offset x:"); Serial.println(offset_x);
  Serial.print("Offset y:"); Serial.println(offset_y);
  Serial.print("Offset z:"); Serial.println(offset_z);
}
