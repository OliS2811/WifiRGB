#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include "effects_handler.h"
#include "sinricpro_handler.h"
#include "names.h"
#include "web_interface.h"
#include "web_admin.h"

ESP8266WebServer server(80);
Adafruit_NeoPixel np(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

String currentScene = "";
int brightness = MAX_BRIGHTNESS;

// Funktion zur Einstellung der Farbe
void setColor(uint8_t red, uint8_t green, uint8_t blue) {
  for (int i = 0; i < NUMPIXELS; i++) {
    np.setPixelColor(i, np.Color(red, green, blue));
  }
  np.show();
}

// Funktion zur Verarbeitung von JSON-Anfragen
void handleApiStateRequest() {
  if (!server.hasArg("plain")) {
    server.send(400, "application/json", "{\"error\":\"Kein Body empfangen\"}");
    return;
  }

  DynamicJsonDocument jsonDoc(512);
  DeserializationError error = deserializeJson(jsonDoc, server.arg("plain"));
  if (error) {
    server.send(400, "application/json", "{\"error\":\"Ungültiges JSON\"}");
    return;
  }

  const char* state = jsonDoc["state"];
  if (strcmp(state, "OFF") == 0) {
    currentScene = "";
    setColor(0, 0, 0); // LEDs ausschalten
    server.send(200, "application/json", "{\"state\":\"OFF\"}");
    return;
  }

  if (strcmp(state, "ON") == 0) {
    currentScene = "";
    if (jsonDoc.containsKey("color")) {
      JsonObject color = jsonDoc["color"];
      uint8_t r = color["r"];
      uint8_t g = color["g"];
      uint8_t b = color["b"];
      setColor(r, g, b);
    }
    if (jsonDoc.containsKey("brightness")) {
      brightness = jsonDoc["brightness"];
      np.setBrightness(brightness);
    }
    server.send(200, "application/json", "{\"state\":\"ON\"}");
    return;
  }

  if (jsonDoc.containsKey("scene")) {
    String scene = jsonDoc["scene"];
    if (scene == "party") {
      currentScene = "party";
    } else if (scene == "romantic") {
      currentScene = "romantic";
    } else if (scene == "reading") {
      currentScene = "reading";
    } else if (scene == "night") {
      currentScene = "night";
    } else {
      server.send(400, "application/json", "{\"error\":\"Unbekannte Szene\"}");
      return;
    }
    server.send(200, "application/json", "{\"scene\":\"" + scene + "\"}");
    return;
  }

  server.send(400, "application/json", "{\"error\":\"Ungültige Anfrage\"}");
}

void setup() {
  Serial.begin(115200);

  // WLAN-Verbindung herstellen
  WiFi.hostname("RGB-Strip-Test");
  WiFi.begin("FRITZ!BoxSA", "Raissa2200!");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nVerbunden mit WLAN");
  Serial.print("IP-Adresse: ");
  Serial.println(WiFi.localIP());

  // mDNS-Setup
  if (MDNS.begin("RGB-Strip-Test")) {
    Serial.println("mDNS responder gestartet: RGB-Strip-Test.local");
  } else {
    Serial.println("mDNS responder konnte nicht gestartet werden");
  }

  // OTA-Setup
    ArduinoOTA.setHostname("RGB-Strip-Test");
    

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "Sketch";
        } else {
            type = "Filesystem";
        }
        Serial.println("Start updating " + type);
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nUpdate abgeschlossen");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Fortschritt: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Fehler[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth-Fehler");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Start-Fehler");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Verbindungs-Fehler");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Empfangs-Fehler");
        else if (error == OTA_END_ERROR) Serial.println("End-Fehler");
    });

    // OTA starten
    ArduinoOTA.begin();
    Serial.println("OTA ist bereit");
    Serial.print("Hostname: ");
    Serial.println(ArduinoOTA.getHostname());

  // NeoPixel initialisieren
  np.begin();
  np.setBrightness(brightness);
  np.show();

  // SinricPro-Integration
  setupSinricPro(appKey, appSecret, lightId);

  // HTTP-Server starten
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/plain", "NeoPixel WiFi RGB Controller ist aktiv");
  });
  server.on("/ui", HTTP_GET, []() {
    server.send_P(200, "text/html", WEBINTERFACE);
  });
  server.on("/admin", HTTP_GET, []() {
    server.send_P(200, "text/html", WEBADMIN);
  });
  server.on("/api/v1/state", HTTP_POST, handleApiStateRequest);
  server.begin();
  Serial.println("HTTP-Server gestartet");
}

void loop() {
  MDNS.update();
  ArduinoOTA.handle();
  SinricPro.handle();
  server.handleClient();

  if (currentScene == "party") {
    partyMode();
  } else if (currentScene == "romantic") {
    romanticMode();
  } else if (currentScene == "reading") {
    readingMode();
  } else if (currentScene == "night") {
    nightMode();
  }
}
