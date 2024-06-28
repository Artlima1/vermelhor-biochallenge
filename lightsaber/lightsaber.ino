#include <math.h>

enum {
  INTENSITY_MODE_LINEAR,
  INTENSITY_MODE_LOG,
};

#define MAX_DIST 300
#define MIN_DIST 20
#define MIN_VIB 20
#define MAX_VIB 255

#define MOVING_AVG_LEN 10

const int triggerPin = 10;
const int ecoPin = 9;
const int motorPin = 6;

long duration;
float dist;
int motor_intensity;
int intensity_mode;
float measures[MOVING_AVG_LEN];
int curr_pos;

void setup() {
  pinMode(triggerPin, OUTPUT); // Configura o pino trigger como saída
  pinMode(ecoPin, INPUT); // Configura o pino eco como entrada.
  pinMode(motorPin, OUTPUT);
  // Serial.begin(9600); // Inicia a comunicação serial

  intensity_mode = INTENSITY_MODE_LOG;
  curr_pos = 0;
}

float moving_avg();

void loop() {
  // Limpa o trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(5);

  // Configurar o trigger para nível alto para transmissão de sinais
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10); // tempo para envio do sinal
  digitalWrite(triggerPin, LOW);

  // Inicia contagem de tempo e lê o pino de eco
  duration = pulseIn(ecoPin, HIGH);

  // Calcular a distância
  measures[curr_pos] = duration * 0.034 / 2;
  curr_pos = (curr_pos + 1) % MOVING_AVG_LEN;

  dist = moving_avg();

  // Mostrar a distância no monitor serial
  if (dist > MAX_DIST) dist = MAX_DIST;
  if (dist <= MIN_DIST) dist = 0;

  if(intensity_mode == INTENSITY_MODE_LINEAR) motor_intensity = MAX_VIB - ((dist*(MAX_VIB-MIN_VIB))/MAX_DIST);
  else if(intensity_mode == INTENSITY_MODE_LOG) motor_intensity = MAX_VIB - (log10(1 + dist*9.0/MAX_DIST))*(MAX_VIB-MIN_VIB);

  analogWrite(motorPin, motor_intensity);

  // Serial.print("Medição: ");
  // Serial.print(measures[curr_pos-1]);
  // Serial.print(" Distancia Média: ");
  // Serial.print(dist);
  // Serial.print(" Intensidade: ");
  // Serial.print(motor_intensity);
  // Serial.print("\n");

  // Aguardar 100ms antes da próxima leitura para evitar interferência
  delay(100);
}

float moving_avg(){
  int i;
  float avg;
  for(i=0, avg=0; i<MOVING_AVG_LEN; i++){
    avg += measures[i];
  }
  avg /= MOVING_AVG_LEN;

  return avg;
}
