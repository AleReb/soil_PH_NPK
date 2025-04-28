#include <ModbusMaster.h>
#include <SoftwareSerial.h>
 
// Pines para el control RS485

 
// Pines para SoftwareSerial (elige pines disponibles y que no interfieran con otras funciones) esto funciona en esp32 c3
#define RS485_RX 5
#define RS485_TX 18
// Inicializar SoftwareSerial para RS485
SoftwareSerial RS485Serial(RS485_RX, RS485_TX); // (RX, TX)
#define DE_RE_PIN 0 // Pines DE y RE conectados juntos al GPIO0

ModbusMaster node;

// Funciones para controlar la transmisión RS485
void preTransmission() {
  digitalWrite(DE_RE_PIN, HIGH);
}

void postTransmission() {
  digitalWrite(DE_RE_PIN, LOW);
}

void setup() {
  Serial.begin(115200); // Comunicación serial para monitoreo
    RS485Serial.begin(4800);  // Inicializar SoftwareSerial para RS485

  pinMode(DE_RE_PIN, OUTPUT);
  digitalWrite(DE_RE_PIN, LOW);

  node.begin(1, RS485Serial); // Dirección esclavo Modbus = 1
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}

void loop() {
  uint8_t result;

  // Solicita lectura desde Humedad hasta Potasio (7 registros desde 0x0000)
  result = node.readHoldingRegisters(0x0000, 7);

  if (result == node.ku8MBSuccess) {
    float humedad = node.getResponseBuffer(0) / 10.0;
    float temperatura = node.getResponseBuffer(1) / 10.0;
    uint16_t conductividad = node.getResponseBuffer(2);
    float pH = node.getResponseBuffer(3) / 10.0;
    uint16_t nitrogeno = node.getResponseBuffer(4);
    uint16_t fosforo = node.getResponseBuffer(5);
    uint16_t potasio = node.getResponseBuffer(6);

    Serial.print("Humedad: "); Serial.print(humedad); Serial.println(" %");
    Serial.print("Temperatura: "); Serial.print(temperatura); Serial.println(" °C");
    Serial.print("Conductividad: "); Serial.print(conductividad); Serial.println(" us/cm");
    Serial.print("pH: "); Serial.println(pH);
    Serial.print("Nitrógeno: "); Serial.print(nitrogeno); Serial.println(" mg/kg");
    Serial.print("Fósforo: "); Serial.print(fosforo); Serial.println(" mg/kg");
    Serial.print("Potasio: "); Serial.print(potasio); Serial.println(" mg/kg");

  } else {
    Serial.print("Error en lectura Modbus, código: ");
    Serial.println(result, HEX);
  }

  delay(2000); // Espera 2 segundos entre lecturas
}