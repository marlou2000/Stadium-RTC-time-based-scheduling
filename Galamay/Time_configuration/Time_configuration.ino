 #include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <ESP8266HTTPClient.h>

#define WIFI_SSID "Galaxy A01 Core9067"                                  
#define WIFI_PASSWORD "samsung01" 

HTTPClient http;

String receivedString;

String ip = "192.168.43.66/RTC-Website";

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                  
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
 
  Serial.println();
  Serial.print("Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
    if (Serial.available() > 0) {
      receivedString = Serial.readStringUntil('%');

      Serial.println("Recieved String : " + receivedString);
      
         http.begin("http://"+ ip + "/update.php");
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        
        int httpCodeTime = http.GET();
        
        if (httpCodeTime == HTTP_CODE_OK) {
          String responseTime = http.getString();
          Serial.println("Time Received: " + responseTime);
          Serial1.print(responseTime + "%");
        } 
        
        else {
          Serial.println("Failed to retrieve current time.");
        }
   }
}
