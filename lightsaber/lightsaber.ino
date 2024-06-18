#include <math.h>

enum {
  INTENSITY_MODE_LINEAR,
  INTENSITY_MODE_LOG,
};

#define MAX_DIST 300
#define MIN_DIST 20
#define MIN_VIB 20
#define MAX_VIB 255

const int triggerPin = 9;
const int ecoPin = 8;
const int motorPin = 10;

long duration;
float dist;
int motor_intensity;
int intensity_mode;

void setup() {
  pinMode(triggerPin, OUTPUT); // Configura o pino trigger como saída
  pinMode(ecoPin, INPUT); // Configura o pino eco como entrada.
  pinMode(motorPin, OUTPUT);
  Serial.begin(9600); // Inicia a comunicação serial

  intensity_mode = INTENSITY_MODE_LOG;
}

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
  dist = duration * 0.034 / 2;

  // Mostrar a distância no monitor serial
  if (dist > MAX_DIST) dist = MAX_DIST;
  if (dist <= MIN_DIST) dist = 0;

  if(intensity_mode == INTENSITY_MODE_LINEAR) motor_intensity = MAX_VIB - ((dist*(MAX_VIB-MIN_VIB))/MAX_DIST);
  else if(intensity_mode == INTENSITY_MODE_LINEAR) motor_intensity = MAX_VIB - (np.log10(dist*10.0/MAX_DIST))*(MAX_VIB-MIN_VIB);

  analogWrite(motorPin, motor_intensity);

  Serial.print("Distancia: ");
  Serial.print(dist);
  Serial.print(" Intensity: ");
  Serial.print(motor_intensity);
  Serial.print("\n");

  // Aguardar 100ms antes da próxima leitura para evitar interferência
  delay(100);
}
