# Verde Pot Controller

VPC is an Arduino project created for bringing a flower pot into the IoT world using the NodeMCU devkit.



 ### Connecting to Internet by mobile phone
For connecting the VPC to Internet an Internet connected Wi-Fi network is needed. Throughout this document will call this Wi-Fi network **"target network"(TN)**.  

The Internet connection is done by using the *ESP8266* module in **Soft Access Point(SoftAP) mode** first and then switch to **Station mode**. Creating the SoftAP is needed because the SSID and password of the TN are not known upfront.

The ESP8266 first boots in SoftAP mode, so we can connect to it using the mobile phone and provide credentials to the TN.

![ESP8266 Soft Access Point mode](doc/images/SAP_mode.png "ESP8266 Soft Access Point mode")

By searching the Wi-Fi networks on your mobile you will find a network named something like: `Verde_xxxxxxxxxxx`. You will have to connect to it using the your pot password(you will be given this password).

Now, to be able to receive the credentials for the TN, a web sever will be created. It will be available at `http://<SoftAP IP>:80/target-network`.

 The server will listen to this endpoint, so to send the credentials for the TN you will need to call:
```json
HTTP POST http://<SoftAP IP>:80/target-network
 {
   "ssid":"TP-LINK_BE92",
   "pass":"53342xxx"
 }
```
If the call is successful, you should see the following HTTP response:
`204 No Content`



Dependencies:
1.  ESP8266WiFi Arduino library:

  * Source: https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi
  * Docs: https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html

2. ESP8266WebServer Arduino library:
    * Source: https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer
    * Docs: https://lastminuteengineers.com/creating-esp8266-web-server-arduino-ide/
      https://github.com/jeremypoulter/SmartPlug/blob/master/src/web_ui.cpp#L111:L151

3. ArduinoJson Arduino library:
    * Source/Docs: https://arduinojson.org/
