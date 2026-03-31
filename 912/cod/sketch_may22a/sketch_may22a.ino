#include <Arduino.h>

// Definirea segmentelor și cifrelor
const int caractere[11][8] = {
  {1, 1, 1, 1, 1, 1, 0, 0},  // 0
  {0, 1, 1, 0, 0, 0, 0, 0},  // 1
  {1, 1, 0, 1, 1, 0, 1, 0},  // 2
  {1, 1, 1, 1, 0, 0, 1, 0},  // 3
  {0, 1, 1, 0, 0, 1, 1, 0},  // 4
  {1, 0, 1, 1, 0, 1, 1, 0},  // 5
  {1, 0, 1, 1, 1, 1, 1, 0},  // 6
  {1, 1, 1, 0, 0, 0, 0, 0},  // 7
  {1, 1, 1, 1, 1, 1, 1, 0},  // 8
  {1, 1, 1, 1, 0, 1, 1, 0},  // 9
  {0, 0, 0, 0, 0, 0, 0, 0},
};

// Pinii pentru fiecare segment din display
int pin_A = 32;
int pin_B = 33;
int pin_C = 25;
int pin_D = 26;
int pin_E = 27;
int pin_F = 14;
int pin_G = 21;
int pin_DP = 22;

// Pinii pentru fiecare cifră
int pin_Cifra1 = 2;
int pin_Cifra2 = 15;

const int sensorPin = 34; // Pinul analogic la care este conectat NTC-ul
const float referenceResistance = 10000; // Rezistența de referință (în ohmi)
const float nominalResistance = 10000; // Rezistența nominală a NTC-ului la temperatura de referință (în ohmi)
const float nominalTemperature = 25; // Temperatura de referință a NTC-ului (în grade Celsius)
const float betaCoefficient = 3950; // Coeficientul beta al NTC-ului

void setup() {
  Serial.begin(115200);
  
  // Configurarea pinii pentru segmente ca output
  pinMode(pin_A, OUTPUT);
  pinMode(pin_B, OUTPUT);
  pinMode(pin_C, OUTPUT);
  pinMode(pin_D, OUTPUT);
  pinMode(pin_E, OUTPUT);
  pinMode(pin_F, OUTPUT);
  pinMode(pin_G, OUTPUT);
  pinMode(pin_DP, OUTPUT);
  
  // Configurarea pinii pentru cifre ca output
  pinMode(pin_Cifra1, OUTPUT);
  pinMode(pin_Cifra2, OUTPUT);
  
  // Dezactivarea inițială a cifrelor
  digitalWrite(pin_Cifra1, LOW);
  digitalWrite(pin_Cifra2, LOW);
}

// Funcția pentru setarea pinilor segmentelor
void setSegmentPins(int character) {
  digitalWrite(pin_A, caractere[character][0]);
  digitalWrite(pin_B, caractere[character][1]);
  digitalWrite(pin_C, caractere[character][2]);
  digitalWrite(pin_D, caractere[character][3]);
  digitalWrite(pin_E, caractere[character][4]);
  digitalWrite(pin_F, caractere[character][5]);
  digitalWrite(pin_G, caractere[character][6]);
  digitalWrite(pin_DP, caractere[character][7]);
}

void displayCharacter(int ch, int digit) {
  // Dezactivează ambele cifre
  digitalWrite(pin_Cifra1, LOW);
  digitalWrite(pin_Cifra2, LOW);
  
  // Setează segmentele pentru cifra specificată
  setSegmentPins(10);
  setSegmentPins(ch);
  
  // Activează cifra corespunzătoare
  if (digit == 1) {
    digitalWrite(pin_Cifra1, HIGH);
  } else {
    digitalWrite(pin_Cifra2, HIGH);
  }
}

void loop() {
  // Citirea valorii analogice de la senzor
  int sensorValue = analogRead(sensorPin);
  
  // Convertirea valorii analogice la tensiune (0-3.3V pentru ESP32)
  float voltage = sensorValue / 4095.0 * 3.3;

  // Calcularea rezistenței NTC-ului
  float resistance = (voltage * referenceResistance) / (3.3 - voltage);

  // Calcularea temperaturii în Kelvin folosind formula Steinhart-Hart
  float temperatureK = 1.0 / (log(resistance / nominalResistance) / betaCoefficient + 1.0 / (nominalTemperature + 273.15));
  
  // Convertirea temperaturii din Kelvin în Celsius
  float temperatureC = temperatureK - 273.15;
  
  // Afișarea temperaturii pe monitorul serial
  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.println(" C");

  // Convertirea temperaturii la intregi pentru afișare
  int temperatureInt = (int)temperatureC;

  // Extrage zecile și unitățile
  int zecile = temperatureInt / 10;
  int unitatile = temperatureInt % 10;

  // Multiplexare cifre pe display
  for (int i = 0; i < 500; i++) {  // Rulează ciclul de multiplexare de 500 ori pentru a crea efectul de persistență vizuală
    displayCharacter(zecile, 1);
   
    
    delay(5);  // Pauză scurtă pentru a permite cifra să fie vizibilă
    
    displayCharacter(unitatile, 2);
    delay(5);  // Pauză scurtă pentru a permite cifra să fie vizibilă
  }
  
  delay(1000); // Pauză de 1 secundă între măsurători
}
