#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include <ESP8266WiFi.h>

const char DEVICE_LOGIN_NAME[] = ""; 
const char SSID[]              = "";    
const char PASS[]              = "";    
const char DEVICE_KEY[]        = "";    

// Relay and Switch Pins
#define RelayPin1 5   // D1
#define RelayPin2 4   // D2
#define RelayPin3 0  // D3
#define RelayPin4 2  // D4

#define SwitchPin1 14  // D5
#define SwitchPin2 12   // D6 
#define SwitchPin3 13  // D7
#define SwitchPin4 3  // Rx

#define wifiLed    16  // D0

// States
bool relayState1 = false;
bool relayState2 = false;
bool relayState3 = false;
bool relayState4 = false;

bool lastSwitchPin1 = HIGH;
bool lastSwitchPin2 = HIGH;
bool lastSwitchPin3 = HIGH;
bool lastSwitchPin4 = HIGH;

// Cloud Switches
CloudSwitch switch1;
CloudSwitch switch2;
CloudSwitch switch3;
CloudSwitch switch4;

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);

void initProperties() {
  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);

  ArduinoCloud.addProperty(switch1, READWRITE, ON_CHANGE, onSwitch1Change);
  ArduinoCloud.addProperty(switch2, READWRITE, ON_CHANGE, onSwitch2Change);
  ArduinoCloud.addProperty(switch3, READWRITE, ON_CHANGE, onSwitch3Change);
  ArduinoCloud.addProperty(switch4, READWRITE, ON_CHANGE, onSwitch4Change);
}

// Apply state to relay (LOW = ON)
void applyRelayState(int relayPin, bool state) {
  digitalWrite(relayPin, !state); // active LOW
}

// Manual Toggle Logic
void checkManualSwitches() {
  bool current1 = digitalRead(SwitchPin1);
  if (current1 != lastSwitchPin1) {
    lastSwitchPin1 = current1;
    relayState1 = (current1 == LOW); // LOW = ON
    switch1 = relayState1;
    applyRelayState(RelayPin1, relayState1);
  }

  bool current2 = digitalRead(SwitchPin2);
  if (current2 != lastSwitchPin2) {
    lastSwitchPin2 = current2;
    relayState2 = (current2 == LOW);
    switch2 = relayState2;
    applyRelayState(RelayPin2, relayState2);
  }

  bool current3 = digitalRead(SwitchPin3);
  if (current3 != lastSwitchPin3) {
    lastSwitchPin3 = current3;
    relayState3 = (current3 == LOW);
    switch3 = relayState3;
    applyRelayState(RelayPin3, relayState3);
  }

  bool current4 = digitalRead(SwitchPin4);
  if (current4 != lastSwitchPin4) {
    lastSwitchPin4 = current4;
    relayState4 = (current4 == LOW);
    switch4 = relayState4;
    applyRelayState(RelayPin4, relayState4);
  }
}

// Cloud change handlers
void onSwitch1Change() {
  relayState1 = switch1;
  applyRelayState(RelayPin1, relayState1);
}

void onSwitch2Change() {
  relayState2 = switch2;
  applyRelayState(RelayPin2, relayState2);
}

void onSwitch3Change() {
  relayState3 = switch3;
  applyRelayState(RelayPin3, relayState3);
}

void onSwitch4Change() {
  relayState4 = switch4;
  applyRelayState(RelayPin4, relayState4);
}

void setup() {
  Serial.begin(9600);
  delay(1500);

  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);

  pinMode(SwitchPin1, INPUT_PULLUP);
  pinMode(SwitchPin2, INPUT_PULLUP);
  pinMode(SwitchPin3, INPUT_PULLUP);
  pinMode(SwitchPin4, INPUT_PULLUP);

  pinMode(wifiLed, OUTPUT);

  // Initialize all to OFF
  applyRelayState(RelayPin1, false);
  applyRelayState(RelayPin2, false);
  applyRelayState(RelayPin3, false);
  applyRelayState(RelayPin4, false);

  digitalWrite(wifiLed, HIGH); // LED off
}

void loop() {
  ArduinoCloud.update();
  checkManualSwitches();

  digitalWrite(wifiLed, WiFi.status() == WL_CONNECTED ? LOW : HIGH);
}
