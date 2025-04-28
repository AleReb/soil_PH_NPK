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
  uint16_t data[3]; // Para Humedad, Temperatura y Conductividad
 
  // Leer registros (Humedad, Temperatura, Conductividad)
  result = node.readHoldingRegisters(0x0000, 3); // Direcciones: 0x0000 a 0x0002
  if (result == node.ku8MBSuccess) {
    data[0] = node.getResponseBuffer(0); // Humedad
    data[1] = node.getResponseBuffer(1); // Temperatura
    data[2] = node.getResponseBuffer(2); // Conductividad
 
    float humidity = data[0] * 0.1; // Convertir a %RH
    float temperature = (int16_t)data[1] * 0.1; // Convertir a °C (con signo)
    float conductivity = data[2]; // µS/cm
 
    // Imprimir resultados
    Serial.print("Humedad: ");
    Serial.print(humidity);
    Serial.println(" %RH");
 
    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.println(" °C");
 
    Serial.print("Conductividad: ");
    Serial.print(conductivity);
    Serial.println(" µS/cm");
  } else {
    Serial.print("Error al leer registros. Código: ");
    Serial.println(result, HEX);
  }
 
  delay(2000); // Esperar 2 segundos antes de la próxima lectura
}