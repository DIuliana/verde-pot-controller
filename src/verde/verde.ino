#define _TASK_SLEEP_ON_IDLE_RUN
#define _TASK_STATUS_REQUEST

#include <TaskScheduler.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

const char* ssid;
const char* pass;
int count = 0;
ESP8266WebServer server(80);


void createSoftAccessPoint();
void checkConnectedStations();
void startServer();
void serverListen();
void wifiConnect();
void checkWifiStatus();
void printWifiStatus();

StatusRequest SAPconnection;
Scheduler scheduler;

Task createSoftAccessPointTask(0, TASK_ONCE, &createSoftAccessPoint, &scheduler, true);
Task waitForConnectionTask(500, TASK_FOREVER, &checkConnectedStations, &scheduler, true);
Task createHTTPServerTask(0 ,TASK_ONCE, &startServer, &scheduler);
Task serverListenTask(500, TASK_FOREVER, &serverListen, &scheduler);
Task wifiConnectTask(20*1000, TASK_FOREVER, &wifiConnect, &scheduler);
Task checkWifiStatusTask(500, TASK_FOREVER, &checkWifiStatus, &scheduler);
Task printWifiStatusTask(30*5000, TASK_FOREVER, &printWifiStatus, &scheduler);

void setup() {
  Serial.begin(9600);
  scheduler.startNow();

  SAPconnection.setWaiting();
  createHTTPServerTask.waitFor(&SAPconnection);

}


void loop() {
  scheduler.execute();
}


void createSoftAccessPoint() {
  Serial.println("Creating Soft Access Point...");
  WiFi.mode(WIFI_AP_STA);
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
    SAPconnection.signalComplete();
  }
}

void startServer(){
  addServerEndpoints();
  server.begin();
  Serial.println("Server started...");
  //enable next task
  serverListenTask.enable();
}

void serverListen(){
  server.handleClient();
}

// entry point for an already existing network
void wifiConnect(){
  Serial.println("Wifi begin...");
  WiFi.begin(ssid, pass);

  //check for wifi status task
  checkWifiStatusTask.enable();
  wifiConnectTask.disable();

}

//try to connect for 20 seconds
void checkWifiStatus(){

  count++;
  if (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
  }
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("WiFi connected");
    checkWifiStatusTask.disable();
  }
  // TODO handle here && serverNotStarted
  if(count == 20) {
    wifiConnectTask.enable();
    count = 0;
  }

}

void printWifiStatus(){
   Serial.print("WiFi status: ");
   Serial.println(WiFi.status());
}

/*
 * Register server endpoint used for getting the TN credentials
 * Used in startServer() context
 */
void addServerEndpoints(){
  server.on("/target-network", handleTargetNetworkRequest);
  server.on("/target-network/status", handleTargetNetworkStatusRequest);
  server.onNotFound(handleNotFound);
  Serial.println("Registered server endpoints");
}

/*
 * Handle HTTP 404 - Not Found response
 */
void handleNotFound(){
  server.send(404, "application/json", createResponse("Not found"));
   Serial.println("Returned 400 - Not found");
}

/*
 * Handle response for HTTP GET '/target-network' request
 * Response HTTP 500 if the request can't be parsed
 * Response HTTP 204 if the password and ssid could be extracted
 */
void handleTargetNetworkRequest(){

  if (!server.hasArg("plain")) {
    server.send(500, "application/json", createResponse("Body not received"));
    Serial.println("Returned 500 - Body not received");
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
    Serial.println("Returned 204");
    //enable wifi connect
    wifiConnectTask.enable();
  }
}

void handleTargetNetworkStatusRequest() {
  String connectionStatus = String(WiFi.status());
  server.send(200, "application/json", createResponse(connectionStatus));
  Serial.println("Returned 200, connection status: "+ connectionStatus);
  if(WiFi.status() == WL_CONNECTED) {
    serverListenTask.disable();
    delay(500);
    WiFi.softAPdisconnect(true);
    printWifiStatusTask.enable();
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
