#define MAXDIST 300
#define MINDIST 20

const int triggerPin = 9;
const int ecoPin = 8;
const int motorPin = 10;

long duracao;
float dist;

void setup() {
  pinMode(triggerPin, OUTPUT); // Configura o pino trigger como saída
  pinMode(ecoPin, INPUT); // Configura o pino eco como entrada.
  pinMode(motorPin, OUTPUT);
  Serial.begin(9600); // Inicia a comunicação serial
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
  duracao = pulseIn(ecoPin, HIGH);

  // Calcular a distância
  dist = duracao * 0.034 / 2;

  // Mostrar a distância no monitor serial
  if (dist >= MAXDIST) dist = MAXDIST;
  if (dist <= MINDIST) dist = 0;
  

  Serial.print("Distancia em cm: ");
  Serial.println(dist);

  analogWrite(motorPin, 255 - ((dist*240)/MAXDIST));

  // Aguardar 100ms antes da próxima leitura para evitar interferência
  delay(100);
}
