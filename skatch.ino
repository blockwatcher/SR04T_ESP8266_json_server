#include <stdio.h>
#include <ESP8266WebServer.h> //https://github.com/mancusoa74/Simple_REST_ESP8266
#include <ArduinoJson.h>
//https://github.com/jshaw/NewPingESP8266
#include <NewPing.h> //https://bbuseruploads.s3.amazonaws.com/21e40fee-c723-4f1e-ad97-534c9c7a130a/downloads/ebcb83c6-d408-4bf7-bd7c-74bcd35bb7ea/NewPing_v1.9.0.zip?Signature=gKb%2BZp1PfUvThMIemhkXjiNws2I%3D&Expires=1525597091&AWSAccessKeyId=AKIAIQWXW6WLXMB5QZAQ&versionId=SInFmuww8EJ.tUuWHkwludmZsH4mywY7&response-content-disposition=attachment%3B%20filename%3D%22NewPing_v1.9.0.zip%22

//curl -i http://192.168.178.123:8080/distance -> change to your IP -> json 
//  {
//    "count": 1,
//    "distance": 251
//  }

#define HTTP_REST_PORT 8080
#define WIFI_RETRY_DELAY 500
#define MAX_WIFI_INIT_RETRY 50

// defines pins numbers
#define TRIGGER_PIN  2   //D4 Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     14  //D5 Arduino pin tied to echo pin on the ultrasonic sensor.

// defines variables

long count;
#define MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

//wifi setup
const char* wifi_ssid = "your_ssid";
const char* wifi_passwd = "your_password";

ESP8266WebServer http_rest_server(HTTP_REST_PORT);

int init_wifi() {
    int retries = 0;

    Serial.println("Connecting to WiFi AP..........");

    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid, wifi_passwd);
    // check the status of WiFi connection to be WL_CONNECTED
    while ((WiFi.status() != WL_CONNECTED) && (retries < MAX_WIFI_INIT_RETRY)) {
        retries++;
        delay(WIFI_RETRY_DELAY);
        Serial.print("#");
    }
    return WiFi.status(); // return the WiFi connection status
}

void get_distance() {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& jsonObj = jsonBuffer.createObject();
    char JSONmessageBuffer[200];

        jsonObj["count"] = count;
        jsonObj["distance"] = sonar.ping_cm();
        jsonObj.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
        http_rest_server.send(200, "application/json", JSONmessageBuffer);
        Serial.print(JSONmessageBuffer);
        count+=1;
}

void config_rest_server_routing() {
    http_rest_server.on("/", HTTP_GET, []() {
        http_rest_server.send(200, "text/html",
            "Welcome to the ESP8266 REST Web Server");
    });
    http_rest_server.on("/distance", HTTP_GET, get_distance);
}



void setup(void) {
    count = 0;
    Serial.begin(115200);

    if (init_wifi() == WL_CONNECTED) {
        Serial.print("Connetted to ");
        Serial.print(wifi_ssid);
        Serial.print("--- IP: ");
        Serial.println(WiFi.localIP());
    }
    else {
        Serial.print("Error connecting to: ");
        Serial.println(wifi_ssid);
    }

    config_rest_server_routing();

    http_rest_server.begin();
    Serial.println("HTTP REST Server Started");
}

void loop(void) {
    http_rest_server.handleClient();
    delay(500);
}
