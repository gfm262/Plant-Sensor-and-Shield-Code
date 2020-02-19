// IoT Workshop
// Send temperature and humidity data to MQTT
//
// WiFiNINA https://www.arduino.cc/en/Reference/WiFiNINA (MKR WiFi 1010)
// Arduino MKR ENV https://www.arduino.cc/en/Reference/ArduinoMKRENV
// Arduino MQTT Client  https://github.com/arduino-libraries/ArduinoMqttClient

#include <WiFiNINA.h>
#include <Arduino_MKRENV.h>
#include <ArduinoMqttClient.h>

#include "config.h"

WiFiSSLClient net;
MqttClient mqtt(net);

String temperatureTopic = "itp/" + DEVICE_ID + "/temperature";
String humidityTopic = "itp/" + DEVICE_ID + "/humidity";
String soilTopic = "itp/" + DEVICE_ID + "/soil";
String pressureTopic = "itp/" + DEVICE_ID + "/pressure";
String illuminanceTopic = "itp/" + DEVICE_ID + "/illuminance";
String uvaTopic = "itp/" + DEVICE_ID + "/uva";
String uvbTopic = "itp/" + DEVICE_ID + "/uvb";


// Publish every X seconds for the workshop. Real world apps need this data every 5 or 10 minutes.
unsigned long publishInterval = 300 * 1000; //every 5 min (changed on 17 Feb 20 from 15 to 5 min)
unsigned long lastMillis = 0;

void setup() {
  Serial.begin(9600);

  // Wait for a serial connection
 // while (!Serial) { }

  // initialize the shield
  if (!ENV.begin()) {
    Serial.println("Failed to initialize MKR ENV shield!");
    while (1);
  }
 
  Serial.println("Connecting WiFi");
  connectWiFi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!mqtt.connected()) {
    connectMQTT();
  }
  
  // poll for new MQTT messages and send keep alives
  mqtt.poll();

  if (millis() - lastMillis > publishInterval) {
    lastMillis = millis();

    int soil = analogRead(A0);
    float temperature = ENV.readTemperature(FAHRENHEIT);
    float humidity = ENV.readHumidity();
    float pressure = ENV.readPressure();
    float illuminance = ENV.readIlluminance();
    float uva = ENV.readUVA();
    float uvb = ENV.readUVB();
    
    Serial.print(temperature);
    Serial.print("°F ");
    Serial.print(humidity);
    Serial.println("% RH");
    Serial.print("Soil Moisture: ");
    Serial.println(soil);
    Serial.print(pressure);
    Serial.println("kPa");
    Serial.print(illuminance);
    Serial.println("lux");
    Serial.print(uva);
    Serial.println("μW/cm2");
    Serial.print(uvb);
    Serial.println("μW/cm2");
    
    
    
    mqtt.beginMessage(temperatureTopic);
    mqtt.print(temperature); 
    mqtt.endMessage();

    mqtt.beginMessage(humidityTopic);
    mqtt.print(humidity); 
    mqtt.endMessage();

    mqtt.beginMessage(soilTopic);
    mqtt.print(soil); 
    mqtt.endMessage();

    mqtt.beginMessage(pressureTopic);
    mqtt.print(pressure); 
    mqtt.endMessage();

    mqtt.beginMessage(illuminanceTopic);
    mqtt.print(illuminance); 
    mqtt.endMessage();

    mqtt.beginMessage(uvaTopic);
    mqtt.print(uva); 
    mqtt.endMessage();

    mqtt.beginMessage(uvbTopic);
    mqtt.print(uvb); 
    mqtt.endMessage();
  }  
}

void connectWiFi() {
  // Check for the WiFi module
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  Serial.print("WiFi firmware version ");
  Serial.println(WiFi.firmwareVersion());
  
  Serial.print("Attempting to connect to SSID: ");
  Serial.print(WIFI_SSID);
  Serial.print(" ");

  while (WiFi.begin(WIFI_SSID, WIFI_PASSWORD) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(3000);
  }

  Serial.println("Connected to WiFi");
  printWiFiStatus();

}

void connectMQTT() {
  Serial.print("Connecting MQTT...");
  mqtt.setId(DEVICE_ID);
  mqtt.setUsernamePassword(MQTT_USER, MQTT_PASSWORD);

  while (!mqtt.connect(MQTT_BROKER, MQTT_PORT)) {
    Serial.print(".");
    delay(5000);
  }

  Serial.println("connected.");
}

void printWiFiStatus() {
  // print your WiFi IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}
