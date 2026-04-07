#include <Arduino.h>
#include <DHT.h>
// Pin assignments
#define PIN_DHT           4
#define PIN_LDR           34   // ADC1_CH6 — input-only, no internal pull-up
#define PIN_POTENTIOMETER 35   // ADC1_CH7 — input-only
#define PIN_PIR           13
#define PIN_LED_RED       25
#define PIN_LED_GREEN     26
#define PIN_LED_YELLOW    27

// DHT constants
#define DHT_TYPE DHT22
DHT dht(PIN_DHT, DHT_TYPE);

// Thresholds
// ──────────────────────────────────────
#define LDR_DARK_THRESHOLD    1500  // ADC 0-4095: below = dark
#define TEMP_MIN              15.0  // °C — potentiometer range
#define TEMP_MAX              40.0

// ──────────────────────────────────────
// Global variables
// ──────────────────────────────────────
float temperature          = 0.0;
float humidity             = 0.0;
float temperatureSetpoint  = 22.0;
bool  motionAlarm         = false;
bool  isDark              = false;

// DHT reading timing (min 2 s)
unsigned long lastDHTRead = 0;
#define DHT_INTERVAL_MS 2500

// Serial display timing
unsigned long lastDisplayTime = 0;
#define DISPLAY_INTERVAL_MS 1000

// PIR alert timing
unsigned long motionAlertTime = 0;
#define PIR_ALERT_DURATION_MS 5000   // ms

// ──────────────────────────────────────
// Function prototypes
// ──────────────────────────────────────
void readDHT();
void readLDR();
void readPotentiometer();
void handlePIR();
void updateLEDs();
void displayStatus();
void printSeparator(char c = '-', int n = 50);

// ============================================================
//  SETUP
// ============================================================
void setup() {
  Serial.begin(115200);
  delay(500);

  dht.begin();

  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);

  digitalWrite(PIN_LED_RED, LOW);
  digitalWrite(PIN_LED_GREEN, LOW);
  digitalWrite(PIN_LED_YELLOW, LOW);

  Serial.println();
  printSeparator('=');
  Serial.println("  SMART HOME - ESP32 NodeMCU-32S");
  Serial.println("  PlatformIO / Arduino Framework");
  printSeparator('=');
  Serial.println(" GPIO4  -> DHT22            (temperature + humidity)");
  Serial.println(" GPIO34 -> LDR              (light sensor)");
  Serial.println(" GPIO35 -> Potentiometer     (temperature setpoint)");
  Serial.println(" GPIO13 -> PIR              (motion sensor)");
  Serial.println(" GPIO25 -> RED LED          (thermostat indicator)");
  Serial.println(" GPIO26 -> GREEN LED        (PIR alarm indicator)");
  Serial.println(" GPIO27 -> YELLOW LED       (darkness indicator)");
  printSeparator('=');
  Serial.println();
}

// ============================================================
//  LOOP
// ============================================================
void loop() {
  unsigned long currentMillis = millis();

  readDHT();
  readLDR();
  readPotentiometer();
  handlePIR();
  updateLEDs();

  if (currentMillis - lastDisplayTime >= DISPLAY_INTERVAL_MS) {
    lastDisplayTime = currentMillis;
    displayStatus();
  }
}

// ============================================================
// Read DHT22 sensor
// ============================================================
void readDHT() {
  if (millis() - lastDHTRead < DHT_INTERVAL_MS) return;
  lastDHTRead = millis();

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (!isnan(t)) temperature = t;
  if (!isnan(h)) humidity    = h;
}

// ============================================================
// Read LDR sensor
// ============================================================
void readLDR() {
  int ldrValue = analogRead(PIN_LDR);
  isDark = (ldrValue < LDR_DARK_THRESHOLD);
}

// ============================================================
// Read potentiometer for temperature setpoint
// ============================================================
void readPotentiometer() {
  int potValue = analogRead(PIN_POTENTIOMETER);
  temperatureSetpoint = TEMP_MIN + ((float)potValue / 4095.0) * (TEMP_MAX - TEMP_MIN);
}

// ============================================================
// PIR — motion detection
// ============================================================
void handlePIR() {
  bool motionDetected = digitalRead(PIN_PIR);

  if (motionDetected) {
    if (!motionAlarm) {
      motionAlarm = true;
      motionAlertTime = millis();
      Serial.println();
      printSeparator('!');
      Serial.println("  *** ALERT: MOTION DETECTED! ***");
      printSeparator('!');
      Serial.println();
    } else {
      motionAlertTime = millis();
    }
  }

  if (motionAlarm && (millis() - motionAlertTime > PIR_ALERT_DURATION_MS)) {
    motionAlarm = false;
  }
}

// ============================================================
// LEDs
// ============================================================
void updateLEDs() {
  digitalWrite(PIN_LED_RED, (temperature > temperatureSetpoint) ? HIGH : LOW);
  digitalWrite(PIN_LED_GREEN, motionAlarm ? HIGH : LOW);
  digitalWrite(PIN_LED_YELLOW, isDark ? HIGH : LOW);
}

// ============================================================
// Serial monitor display
// ============================================================
void displayStatus() {
  printSeparator();

  // Temperature & humidity
  Serial.print("  Temperature : ");
  Serial.print(isnan(temperature) ? 0.0 : temperature, 1);
  Serial.println(" °C");

  Serial.print("  Humidity    : ");
  Serial.print(isnan(humidity) ? 0.0 : humidity, 1);
  Serial.println(" %");

  Serial.print("  Setpoint    : ");
  Serial.print(temperatureSetpoint, 1);
  Serial.println(" °C");

  Serial.print("  Thermostat  : ");
  Serial.println((temperature > temperatureSetpoint) ? "HEATING — RED LED ON" : "OK — RED LED OFF");

  printSeparator();

  // Alarm & light
  Serial.print("  PIR Alarm   : ");
  Serial.println(motionAlarm ? "ACTIVE — MOTION DETECTED" : "No motion");

  Serial.print("  Light Level : ");
  Serial.print(analogRead(PIN_LDR));
  Serial.print(" / 4095  — ");
  Serial.println(isDark ? "DARK — YELLOW LED ON" : "BRIGHT — YELLOW LED OFF");

  printSeparator();

  // LED states
  Serial.print("  RED LED     : ");
  Serial.println((temperature > temperatureSetpoint) ? "ON" : "OFF");
  Serial.print("  GREEN LED   : ");
  Serial.println(motionAlarm ? "ON" : "OFF");
  Serial.print("  YELLOW LED  : ");
  Serial.println(isDark ? "ON" : "OFF");
}

// ============================================================
// Utility: separator
// ============================================================
void printSeparator(char c, int n) {
  for (int i = 0; i < n; i++) Serial.print(c);
  Serial.println();
}