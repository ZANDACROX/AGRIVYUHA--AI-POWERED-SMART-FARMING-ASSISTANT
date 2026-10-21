#include <SoftwareSerial.h>
#include <ModbusMaster.h>
#include <SoftwareSerial.h>
#include <ModbusMaster.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// DHT22
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
// DS18B20
#define ONE_WIRE_BUS 7

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature soilTemp(&oneWire);

// IR Flame Sensor
#define FLAME_PIN 8

// Soil moisture sensor
#define SOIL_PIN A0
// MAX485 Connections (Arduino Nano)
#define MAX485_DI 6    // TX to MAX485 DI
#define MAX485_RO 5    // RX from MAX485 RO
#define MAX485_DE 4    // Driver Enable
#define MAX485_RE 3    // Receiver Enable

SoftwareSerial mod(MAX485_RO, MAX485_DI); // RX, TX

ModbusMaster node;

// Enable transmit mode
void preTransmission()
{
  digitalWrite(MAX485_RE, HIGH);
  digitalWrite(MAX485_DE, HIGH);
}

// Enable receive mode
void postTransmission()
{
  digitalWrite(MAX485_RE, LOW);
  digitalWrite(MAX485_DE, LOW);
}

void setup()
{
  Serial.begin(115200);

  pinMode(MAX485_RE, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);

  digitalWrite(MAX485_RE, LOW);
  digitalWrite(MAX485_DE, LOW);

  mod.begin(9600);

  node.begin(1, mod);   // Slave ID = 1

  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  Serial.println();
  Serial.println("NPK Sensor Started");
  dht.begin();
  soilTemp.begin();

pinMode(FLAME_PIN, INPUT);
}

void loop()
{
  uint8_t result;
  uint16_t nitrogen = 0;
  uint16_t phosphorus = 0;
  uint16_t potassium = 0;
  // DHT22 readings
float airTemp = dht.readTemperature();
float humidity = dht.readHumidity();

// Soil moisture reading
int soilRaw = analogRead(SOIL_PIN);
int soilPercent = map(soilRaw, 870, 300, 0, 100);
soilPercent = constrain(soilPercent, 0, 100);

// Read Soil Temperature
soilTemp.requestTemperatures();
float soilTemperature = soilTemp.getTempCByIndex(0);

// Read Flame Sensor
int flameState = digitalRead(FLAME_PIN);

  // Nitrogen
  result = node.readHoldingRegisters(0x001E, 1);
  if (result == node.ku8MBSuccess)
  {
    nitrogen = node.getResponseBuffer(0);
  }

  delay(100);

  // Phosphorus
  result = node.readHoldingRegisters(0x001F, 1);
  if (result == node.ku8MBSuccess)
  {
    phosphorus = node.getResponseBuffer(0);
  }

  delay(100);

  // Potassium
  result = node.readHoldingRegisters(0x0020, 1);
  if (result == node.ku8MBSuccess)
  {
    potassium = node.getResponseBuffer(0);
  }

  Serial.println("--------------------------------");

  Serial.print("Nitrogen   : ");
  Serial.print(nitrogen);
  Serial.println(" mg/kg");

  Serial.print("Phosphorus : ");
  Serial.print(phosphorus);
  Serial.println(" mg/kg");

  Serial.print("Potassium  : ");
  Serial.print(potassium);
  Serial.println(" mg/kg");
  // DHT22
if (isnan(airTemp) || isnan(humidity)) {
  Serial.println("DHT22 Error");
} else {
  Serial.print("Air Temp   : ");
  Serial.print(airTemp);
  Serial.println(" C");

  Serial.print("Humidity   : ");
  Serial.print(humidity);
  Serial.println(" %");
}

// Soil moisture
Serial.print("Soil Moisture : ");
Serial.print(soilPercent);
Serial.println(" %");


Serial.print("Soil Temperature : ");
Serial.print(soilTemperature);
Serial.println(" °C");

Serial.print("Flame Detected   : ");

if (flameState == LOW)
{
  Serial.println("YES");
}
else
{
  Serial.println("NO");
}

  Serial.println("--------------------------------");

  delay(2000);
}