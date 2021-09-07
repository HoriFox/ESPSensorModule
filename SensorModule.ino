
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include "Adafruit_Sensor.h"                            // Adafruit_Sensor
#include "DHT.h"                                        // Подключаем библиотеку DHT
#include "index.h"
#include "data.h"

//Для прошивки по WiFi
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h> // Библиотека для OTA-прошивки

const char* nameDevice = nameDeviceValue;
const char* passDevice = passDeviceValue;
const char* ssid = ssidValue;
const char* password = passwordValue;

//IPAddress ip(192,168,1,20);  // Статический IP
//IPAddress gateway(192,168,1,1);
//IPAddress subnet(255,255,255,0);

#define DHTPIN 2                                        // Пин к которому подключен датчик температуры/влажности
#define DHTTYPE DHT11                                   // Используемый датчик DHT 11
DHT dht(DHTPIN, DHTTYPE);                               // Инициализируем датчик
ESP8266WebServer server(80);

int tempErrBias = 0;
int humiErrBias = 0;
boolean pirActive = false;
String callbackAddress = "null";

int addressCountRestart = 0;
int addressTempErr = 2;
int addressHumiErr = 4;
int addressCallbackAddress = 6;

void update() {
  int activeStatus = digitalRead(0);
  if(activeStatus != pirActive){
    pirActive = activeStatus;
    if(callbackAddress != "null" && pirActive){
      Serial.print("Отправляем оповещение от " + String(nameDevice));
    }
  }
}

void serverRoot() {
  server.send(200, "text/html", MAIN_page);
}

void registrationPage() {
  callbackAddress = server.client().remoteIP().toString();
  EEPROM.write(addressCallbackAddress, 2);
  EEPROM.commit();
  server.send(200, "text/plain", "Successfully set callback to: " + callbackAddress);
}

void settingPage() {
  String message = "Request example: /setting?temp_err_bias=10&humi_err_bias=15\n";
  message += "Values cannot be negative!\n";
  String tempErrValue = server.arg("temp_err_bias");
  if(tempErrValue != "") {
      int value = tempErrValue.toInt();
      tempErrBias = value;
      EEPROM.write(addressTempErr, value);
      EEPROM.commit();
      message += "\ntemp err bias set " + String(value) + "\n";
  }
  String humiErrValue = server.arg("humi_err_bias");
  if(humiErrValue != "") {
      int value = humiErrValue.toInt();
      humiErrBias = value;
      EEPROM.write(addressHumiErr, value);
      EEPROM.commit();
      message += "\nhumi err bias set " + String(value) + "\n";
  }
  server.send(200, "text/plain", message);
}

void infoPage() {
  float temperature = dht.readTemperature() - tempErrBias;        // Запрос на считывание температуры
  float humidity = dht.readHumidity() - humiErrBias;
  
  String message = "{";
  message += "\"temperature\": \"" + String(temperature) + "\",";
  message += "\"temperature_err_mody\": \"" + String(tempErrBias) + "\",";
  message += "\"humidity\": \"" + String(humidity) + "\","; 
  message += "\"humidity_err_mody\": \"" + String(humiErrBias) + "\",";
  message += "\"pir_artive\": \"" + String(pirActive) + "\",";
  message += "\"callback\": \"" + callbackAddress + "\",";
  message += "\"flashChipSize\": \"" + String(ESP.getFlashChipSize()) + "\",";
  message += "\"real\": \"" + String(ESP.getFlashChipRealSize()) + "\"";
  message += "}";
  server.send(200, "application/json", message);
}

void metricsPage() {
  int countReboot = EEPROM.read(addressCountRestart);
  String message = "{\n";
  message += "  \"countReboot\": " + String(countReboot) + ",\n";
  message += "}";
  server.send(200, "application/json", message);
}

void restartPage() {
  server.send(200, "text/plain", "restart esp");
  ESP.restart();
}

void fileNotFound() {
  server.send(404, "text/plain", "File Not Found");
}

void setup(void) {
  pinMode(0, INPUT);

  Serial.begin(115200);
  Serial.println("Start");
  WiFi.mode(WIFI_STA); //Режим клиента (WIFI_AP - точка доступа, WIFI_AP_STA - оба режима)
  WiFi.begin(ssid, password);
  //WiFi.config(ip, gateway, subnet); //Ставим статический адрес
  Serial.println("");

  // Ждём соединения
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //Для прошивки по WiFi
  ArduinoOTA.setHostname(nameDevice);     // Задаем имя сетевого порта
  //ArduinoOTA.setPassword(passDevice);   // Задаем пароль доступа для удаленной прошивки
  ArduinoOTA.begin();

  server.on("/", serverRoot);
  server.on("/registration", registrationPage);
  server.on("/setting", settingPage);
  server.on("/info", infoPage);
  server.on("/restart", restartPage);
  server.on("/metrics", metricsPage);
  server.onNotFound(fileNotFound);
  server.begin();
  Serial.println("HTTP server started");

  // Инициализируем EEPROM
  EEPROM.begin(512);
  int countReboot = EEPROM.read(addressCountRestart); // Получаем количество перезапусков
  int tempErrBiasEep = EEPROM.read(addressTempErr);
  if(tempErrBiasEep != '\0') tempErrBias = tempErrBiasEep;
  int humiErrBiasEep = EEPROM.read(addressHumiErr);
  if(humiErrBiasEep != '\0') humiErrBias = humiErrBiasEep;
  int callbackAddressData = EEPROM.read(addressCallbackAddress);
  if(callbackAddressData != '\0') callbackAddress = callbackAddressData;
  EEPROM.write(addressCountRestart, countReboot + 1); // Записываем в память количество перезапусков модуля
  EEPROM.commit(); // Сохраняем в памяти
}

void loop(void) {
  ArduinoOTA.handle();                              // Всегда готовы к прошивке
  server.handleClient();
  update();
}
