# Verde Pot Controller

VPC is an Arduino project created for bringing a flower pot into the IoT world using the NodeMCU devkit.



 ### Connecting to Internet by mobile phone
For connecting the VPC to Internet an Internet connected Wi-Fi network is needed. Throughout this document will call this Wi-Fi network **"target network"(TN)**.  

The Internet connection is done by using the *ESP8266* module in **Soft Access Point(SoftAP) mode** first and then switch to **Station mode**. Creating the SoftAP is needed because the SSID and password of the TN are not known upfront.

The ESP8266 first boots in SoftAP mode, so we can connect to it using the mobile phone and provide credentials to the TN.

![ESP8266 Soft Access Point mode](doc/images/SAP_mode.png "ESP8266 Soft Access Point mode")

By searching the Wi-Fi networks on your mobile you will find a network named something like: `Verde_xxxxxxxxxxx`. You will have to connect to it using the your pot password(you will be given this password).



Dependencies:
1.  ESP8266WiFi Arduino library:

  * Source: https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi
  * Docs: https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html
