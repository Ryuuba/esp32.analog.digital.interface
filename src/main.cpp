#include <Arduino.h>
#include <LiquidCrystal.h>
#include <PubSubClient.h>
#include "WiFiSetup.h"

// Note: ADC2 pins cannot be used when Wi-Fi is used. So, if you’re using Wi-Fi
// and you’re having trouble getting the value from an ADC2 GPIO, you may
// consider using an ADC1 GPIO instead. That should solve your problem.

// Matches A1602 LCD pins
// #define RS 4
// #define E  5
// #define D4 15
// #define D5 16
// #define D6 17
// #define D7 18
#define TEST_LED  21
#define CTRL_LED  22
#define SENSOR_INPUT 33

// Global variables used in this program
LiquidCrystal lcd(4, 5, 15, 16, 17, 18);
bool on_off;
bool first_it;
const char* mqttServer = "10.0.0.11"; // RPi3
// const char* mqttServer = "10.0.0.15"; // Oztoatl
const int mqttPort = 1883;
const char* subscriptionTopic = "esp32/ctrl";
const char* publishTopic = "esp32/pot";
const char* clientID = "nodeMCU";
WiFiClient espClient;
PubSubClient client(espClient);
long lastBlink = 0;
long lastMsg = 0;
char msg[50];
char lcdBuffer[16];
int value = 0;

WiFiSetup wifiSetup(home);

// This function is called when a MQTT message is received
void callback(char* topic, byte* message, unsigned int length) {
  // sprintf(lcdBuffer, "%s, %s", topic, message);
  // lcd.print(lcdBuffer); // Prints data about a received MQTT
  // sprintf(lcdBuffer, "%s", message);;
  String messageText;
  String topicStr(topic);
  for (int i = 0; i < length; i++)
    messageText += (char)message[i];
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Msg arrives");
  lcd.setCursor(0, 1);
  lcd.print(topicStr + ' ' + messageText);
  if (topicStr == subscriptionTopic) {
    if (messageText == "on")
      digitalWrite(CTRL_LED, HIGH); // Turns the CTRL_LED on
    else
      digitalWrite(CTRL_LED, LOW);  // Turns the CTRL_LED off
  }
  delay(2000);
  lcd.clear();
}

void reconnect() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CNT MQTT BKR");
  lcd.setCursor(0, 1);
  while (1) {                              // Loops until we're reconnected
    if (client.connect(clientID)) {        // Attempts to connect
      lcd.print("Status: OK");
      client.subscribe(subscriptionTopic); // Subscribes MQTT client
      delay(1000);
      break;
    }
    else {
      sprintf(lcdBuffer, "Status: %d", client.state());
      lcd.print(lcdBuffer);
      delay(5000);
    }
  }
}

void setup() {
  first_it = true;
  on_off = true;  // Flag that turn on or turn of the test led
  pinMode(SENSOR_INPUT, INPUT); // Analog pin emulating a sensor
  pinMode(TEST_LED, OUTPUT);    // Digital pin blinking a LED
  pinMode(CTRL_LED, OUTPUT);    // Digital pin controlling a LED
  lcd.begin(16,2);  // Initializes the LCD
  wifiSetup(lcd);      // Sets the WiFi state
  client.setServer(mqttServer, mqttPort);   // Sets MQTT client
  client.setCallback(callback);             // Sets MQTT callback's client
}

void loop() {
  long now = millis();
  if (now - lastMsg > 4095) {
    if (!client.connected())
      reconnect();
    client.loop();
    lcd.setCursor(10, 1);
    lastMsg = now;
    sprintf(lcdBuffer, "%d", value);
    client.publish(publishTopic, lcdBuffer);
  }
  else if (now - lastBlink > 511) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MQTT TEST");
    lcd.setCursor(0, 1);
    value = analogRead(SENSOR_INPUT);
    sprintf(lcdBuffer, "Value: %d", value);
    lcd.print(lcdBuffer);
    lastBlink = now;
    on_off = !on_off;
    digitalWrite(TEST_LED,on_off);
  }
}

// void loop() {
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("ADC TEST");
//   lcd.setCursor(0, 1);
//   lcd.print("Value : ");
//   lcd.setCursor(10, 1);
//   lcd.print(analogRead(SENSOR_INPUT));
//   on_off = !on_off;
//   digitalWrite(TEST_LED,on_off);
//   delay(500);
// }