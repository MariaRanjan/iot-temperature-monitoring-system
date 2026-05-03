#define BLYNK_TEMPLATE_ID "YOUR ID"
#define BLYNK_TEMPLATE_NAME "YOUR TEMPLATE NAME"
#define BLYNK_AUTH_TOKEN "YOUR TOKEN"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

// WiFi Credentials
char ssid[] = "POCO M2 Pro";
char pass[] = "12345678";

// DHT Setup
#define DHTPIN D4
#define DHTTYPE DHT11

// Motor Driver Pins
#define IN1 D5
#define IN2 D6
#define ENA D7

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// Global Variables
float threshold = 33;   // 🔥 Updated threshold
bool manualMode = false;
int speedValue = 1023;

// 🔘 Manual ON/OFF Button (V1)
BLYNK_WRITE(V1) {
  if (manualMode) {
    int state = param.asInt();

    Serial.print("Manual Button: ");
    Serial.println(state);

    if (state == 1) {
      Serial.println("Fan ON (Manual)");
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      analogWrite(ENA, speedValue);
    } else {
      Serial.println("Fan OFF (Manual)");
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      analogWrite(ENA, 0);
    }
  }
}

// 🎚️ Speed Control (V2)
BLYNK_WRITE(V2) {
  speedValue = constrain(param.asInt(), 0, 1023);
  Serial.print("Speed: ");
  Serial.println(speedValue);
}

// 🔁 Mode Switch (V3)
BLYNK_WRITE(V3) {
  manualMode = param.asInt();  // 1 = Manual, 0 = Auto

  Serial.print("Mode: ");
  Serial.println(manualMode ? "Manual" : "Auto");
}

// 🌡️ Sensor + Auto Control
void sendSensor() {
  float temp = dht.readTemperature();

  if (isnan(temp)) {
    Serial.println("DHT Error!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.println(temp);

  Blynk.virtualWrite(V0, temp);

  // 🤖 Auto Mode
  if (!manualMode) {
    if (temp > threshold) {
      Serial.println("Fan ON (Auto)");
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      analogWrite(ENA, speedValue);
    } else {
      Serial.println("Fan OFF (Auto)");
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      analogWrite(ENA, 0);
    }
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  // PWM setup (important for ESP8266)
  analogWriteRange(1023);
  analogWriteFreq(1000);

  dht.begin();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Serial.println("System Started...");

  timer.setInterval(2000L, sendSensor);
}

void loop() {
  Blynk.run();
  timer.run();
}
