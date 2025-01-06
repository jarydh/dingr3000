/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
*********/

// Import required libraries
#include "WiFi.h"
#include "WifiManager.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

// Set LED GPIO
const int motorPin1 = 12;
const int motorPin2 = 14;

int motorSpeed = 255;

// Stores LED state
String ledState;

// Set stepper motor pins

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Replaces placeholder with LED state value
String processor(const String &var)
{
  Serial.println(var);
  if (var == "STATE")
  {
    if (digitalRead(motorPin1))
    {
      ledState = "ON";
    }
    else
    {
      ledState = "OFF";
    }
    Serial.print(ledState);
    return ledState;
  }
  return String();
}

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);

  // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // wifi setup
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  // WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;

  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  // wm.resetSettings();

  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("AutoConnectAP", "password"); // password protected ap

  if (!res)
  {
    Serial.println("Failed to connect");
    // ESP.restart();
  }
  else
  {
    // if you get here you have connected to the WiFi
    Serial.println("Connected to wifi");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String(), false, processor); });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/style.css", "text/css"); });

  server.on("/api/motorspeed", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    String param = request->getParam("speed", true)->value();

    if (request->hasParam("speed", true)) {
      String param = request->getParam("speed", true)->value();
      Serial.println(param);
    } else {
      Serial.println("no speed param");
      Serial.println(request->params());
    }

    Serial.println("Received data:");
    Serial.println("speed: " + param);

    motorSpeed = param.toInt();
    analogWrite(motorPin1, motorSpeed);    

    // Send a response back to the client
    request->send(200, "text/plain", "Data received successfully"); });

  server.on("/sachon.jpg", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/sachon.jpg", "img/jpg"); });

  server.on("/sachoff.jpg", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/sachoff.jpg", "img/jpg"); });

  // Route to set GPIO to HIGH
  server.on("/api/on", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    analogWrite(motorPin1, motorSpeed);    
    request->send(SPIFFS, "/index.html", String(), false, processor); });

  // Route to set GPIO to LOW
  server.on("/api/off", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    analogWrite(motorPin1, 0);    

    request->send(SPIFFS, "/index.html", String(), false, processor); });

  // Start server
  server.begin();

  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin2, LOW);
}

void loop()
{
}