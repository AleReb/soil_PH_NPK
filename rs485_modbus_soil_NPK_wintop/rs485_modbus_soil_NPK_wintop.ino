#include <ModbusMaster.h>
#include <SoftwareSerial.h>

// Pines RS485 para ESP32-C3 (SoftwareSerial)
#define RS485_RX 5   // RO
#define RS485_TX 18  // DI
#define DE_RE_PIN 0  // DE y RE en GPIO0

SoftwareSerial RS485Serial(RS485_RX, RS485_TX);
ModbusMaster node;

// Control de transmisión RS485
void preTransmission() { digitalWrite(DE_RE_PIN, HIGH); }
void postTransmission()    { digitalWrite(DE_RE_PIN, LOW);  }

void setup() {
  Serial.begin(115200);
  RS485Serial.begin(4800); // Configuración: 4800,n,8,1

  pinMode(DE_RE_PIN, OUTPUT);
  digitalWrite(DE_RE_PIN, LOW);

  node.begin(1, RS485Serial); // Dirección Modbus esclavo = 1
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  Serial.println("=== Lectura Soil NPK-S Sensor ===");
}

void loop() {
  // Leer Nitrogeno, Fosforo y Potasio juntos
  // Registros: 0x001E (N), 0x001F (P), 0x0020 (K)
  uint8_t result = node.readHoldingRegisters(0x001E, 3);
  if (result == node.ku8MBSuccess) {
    uint16_t nitrogen   = node.getResponseBuffer(0); // mg/kg
    uint16_t phosphorus = node.getResponseBuffer(1); // mg/kg
    uint16_t potassium  = node.getResponseBuffer(2); // mg/kg

    Serial.printf("Nitrógeno: %u mg/kg, Fósforo: %u mg/kg, Potasio: %u mg/kg\n",
                  nitrogen, phosphorus, potassium);
  } else {
    Serial.printf("Error lectura NPK (start 0x001E): 0x%02X\n", result);
  }

  delay(2000); // Intervalo entre lecturas
}
