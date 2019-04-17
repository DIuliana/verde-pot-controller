#define _TASK_SLEEP_ON_IDLE_RUN
#define _TASK_STATUS_REQUEST

#include <TaskScheduler.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

const char* ssid;
const char* pass;
ESP8266WebServer server(80);


void createSoftAccessPoint();
void checkConnectedStations();
void startServer();
void waitForRequest();

StatusRequest SAPconnectionStatus;
Scheduler scheduler;

Task createSoftAccessPointTask(0, TASK_ONCE, &createSoftAccessPoint, &scheduler, true);
Task waitForConnectionTask(500, TASK_FOREVER, &checkConnectedStations, &scheduler, true);
Task createHTTPServerTask(0 ,TASK_ONCE, &startServer, &scheduler);
Task waitForRequestTask(500, TASK_FOREVER, &waitForRequest, &scheduler);

void setup() {
  Serial.begin(9600);
  scheduler.startNow();

  SAPconnectionStatus.setWaiting();
  createHTTPServerTask.waitFor(&SAPconnectionStatus);
}


void loop() {
  scheduler.execute();
}


void createSoftAccessPoint() {
  Serial.println("Creating Soft Access Point...");
  boolean ready = WiFi.softAP("Verde_31529819901", "123456789");
  Serial.println(ready ? "SoftAP ready" : "SoftAP failed");
  Serial.print("SoftAP IP address: ");
  Serial.println(WiFi.softAPIP());
}

void checkConnectedStations(){
  int connectedStations =  WiFi.softAPgetStationNum();
  Serial.printf("Connected stations = %d\n", connectedStations);
  if(connectedStations > 0){
    Serial.println("Client just connected");

    //self disable task
    waitForConnectionTask.disable();
    SAPconnectionStatus.signalComplete();
  }
}

void startServer(){
  addServerEndpoints();
  server.begin();
  Serial.println("Server started...");
  //enable next task
  waitForRequestTask.enable();
}

void waitForRequest(){
  Serial.println("Waiting for TN credentials...");
  server.handleClient();
}

/*
 * Register server endpoint used for getting the TN credentials
 * Used in startServer() context
 */
void addServerEndpoints(){
  server.on("/target-network", handleTargetNetworkRequest);
  server.onNotFound(handleNotFound);
   Serial.println("Registered server endpoints");
}

/*
 * Handle HTTP 404 - Not Found response
 */
void handleNotFound(){
  server.send(404, "application/json", createResponse("Not found"));
}

/*
 * Handle response for HTTP GET '/target-network' request
 * Response HTTP 500 if the request can't be parsed
 * Response HTTP 204 if the password and ssid could be extracted
 */
void handleTargetNetworkRequest(){

  if (!server.hasArg("plain")) {
    server.send(500, "application/json", createResponse("Body not received"));
    return;
  }
  else {
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
