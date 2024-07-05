#include <math.h>
#include <Wire.h>
#include <VL53L0X.h>

enum {
  SENSOR_TYPE_LIGHT,
  SENSOR_TYPE_SOUND,
};

enum {
  INTENSITY_MODE_LINEAR,
  INTENSITY_MODE_LOG,
};

#define MAX_DIST 200
#define MIN_DIST 10
#define MIN_VIB 3
#define MAX_VIB 255

#define MOVING_AVG_LEN 5

const int lightSensoXShutPin = 7;
const int triggerPin = 10;
const int ecoPin = 9;
const int motorPin = 6;

int intensity_mode;
int sensor_type;

long duration;
float dist;
int motor_intensity;
float measures[MOVING_AVG_LEN];
int curr_pos;

VL53L0X LightSensor;

float moving_avg();
float read_sensor();

void setup() {
  // Configuracoes
  intensity_mode = INTENSITY_MODE_LOG;
  sensor_type = SENSOR_TYPE_LIGHT;

  if(sensor_type == SENSOR_TYPE_SOUND){
    // Configuracao pinos sensor ultrassonico e motor
    pinMode(triggerPin, OUTPUT); // Configura o pino trigger como saída
    pinMode(ecoPin, INPUT); // Configura o pino eco como entrada.
  }

  else if(sensor_type == SENSOR_TYPE_LIGHT){
    // Configuracao sensor de Luz
    pinMode(lightSensoXShutPin, OUTPUT);
    Wire.begin();
    pinMode(lightSensoXShutPin, INPUT);
    delay(10);
    LightSensor.init();
    LightSensor.setTimeout(500);
    LightSensor.startContinuous();
  }

  pinMode(motorPin, OUTPUT);

  curr_pos = 0;
  for (int i = 0; i < MOVING_AVG_LEN; i++) {
    measures[i] = 0;
  }
  Serial.begin(9600); // Inicia a comunicação serial
}

void loop() {
  measures[curr_pos] = read_sensor();
  curr_pos = (curr_pos + 1) % MOVING_AVG_LEN;

  dist = moving_avg();

  // Mostrar a distância no monitor serial
  if (dist > MAX_DIST) dist = MAX_DIST;
  if (dist <= MIN_DIST) dist = 0;

  if(intensity_mode == INTENSITY_MODE_LINEAR) motor_intensity = MAX_VIB - ((dist*(MAX_VIB-MIN_VIB))/MAX_DIST);
  else if(intensity_mode == INTENSITY_MODE_LOG) motor_intensity = MAX_VIB - (log10(1 + dist*9.0/MAX_DIST))*(MAX_VIB-MIN_VIB);

  analogWrite(motorPin, motor_intensity);

  Serial.print("Medição: ");
  Serial.print(measures[curr_pos-1]);
  Serial.print(" Distancia Média: ");
  Serial.print(dist);
  Serial.print(" Intensidade: ");
  Serial.print(motor_intensity);
  Serial.print("\n");

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

float read_sensor(){
  float measure=0;

  if(sensor_type == SENSOR_TYPE_SOUND){
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
    measure = duration * 0.034 / 2;
  }

  else if(sensor_type == SENSOR_TYPE_LIGHT){
    // Le sensor e converte para cm
    measure = LightSensor.readRangeContinuousMillimeters()*0.1;
    if (LightSensor.timeoutOccurred()) {
      Serial.println("Sensor timeout!");
      measure = MAX_DIST;
    }
  }

  return (measure < MAX_DIST) ? measure : MAX_DIST;
}
