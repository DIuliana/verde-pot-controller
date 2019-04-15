#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>


const char* ssid;
const char* pass;

ESP8266WebServer server(80);

void setup() {
  Serial.begin(9600);

  createSoftAccessPoint();
  startServer();
  
}

void loop() {
   if(areStationsConnected()){
    Serial.println("SoftAP connection ready, waiting for TN credentials...");
    delay(5000);
    
    server.handleClient();
    
   }
}


boolean createSoftAccessPoint() {
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

void startServer(){
  addServerEndpoints();
  server.begin();
  Serial.println("Server started...");
}


void addServerEndpoints(){
  server.on("/target-network", handleTargetNetworkRequest);
  server.onNotFound(handleNotFound);
  
}

void handleNotFound(){
  server.send(404, "application/json", createResponse("Not found"));
}

void handleTargetNetworkRequest(){

  
  if (!server.hasArg("plain")) {
    server.send(500, "application/json", createResponse("Body not received"));
    return;
  }
  else{
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(server.arg("plain"));
    ssid = root["ssid"];
    pass = root["pass"];
    Serial.println(ssid);
    Serial.println(pass);
    server.send(204, "application/json", createResponse(""));
  }

}

String createResponse(String respMessage){
  
  StaticJsonBuffer<200> jsonBufferResp;
  JsonObject& response = jsonBufferResp.createObject();
  
  response["resp"] = respMessage;  
  String json;
  response.prettyPrintTo(json);
  return json;
}
