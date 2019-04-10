#include <ESP8266WiFi.h>


void setup() {
  Serial.begin(9600);

  isSoftAccessPointReady();
}

void loop() {
   if(areStationsConnected()){
    Serial.println("SoftAP connection ready, waiting for TN credentials...");
    delay(5000);
   }
}


boolean isSoftAccessPointReady() {
  Serial.println("Creating Soft Access Point...");
  boolean ready = WiFi.softAP("Verde_31529819901", "123456789");
  Serial.println(ready ? "SoftAP ready" : "SoftAP failed");
  Serial.print("SoftAP IP address: ");
  Serial.println(WiFi.softAPIP());

  return ready;
}

boolean areStationsConnected(){
  int connectedStations =  WiFi.softAPgetStationNum();
  Serial.printf("Connected stations = %d\n", connectedStations);
  return connectedStations > 0;
}
