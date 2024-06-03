Hi everyone, I'm Liying.
Welcome to ESP8266 with EEPROM !

Features
AP Mode: Create a Wi-Fi network for configuration.
STA Mode: Connect to an existing Wi-Fi network.
EEPROM Storage: Save Wi-Fi credentials and relay status.
Web Interface: Simple web page to toggle the relay and configure Wi-Fi settings.

Components:
NodeMCU ESP8266
Relay Module
Breadboard and jumper wires

Code Explanation
The code is structured as follows:

Global Variables
ESP8266WebServer server(80);: Initializes the web server on port 80.
String ssid, password;: Stores Wi-Fi credentials.
bool relayStatus = false;: Stores the relay status (OFF by default).
const int relayPin = D1;: Defines the relay control pin.
setup()
Initializes serial communication and EEPROM.
Reads stored Wi-Fi credentials and relay status from EEPROM.
Sets the relay to the last known status.
Configures the NodeMCU in AP mode and launches the web server.
loop()
Handles incoming client requests.
launchWeb(int webtype)
Launches the web server based on the mode (AP or STA).
createWebServer(int webtype)
Defines web routes for the respective mode.
testWiFi()
Attempts to connect to a Wi-Fi network with the stored credentials.
writeData(String a, String b)
Writes Wi-Fi credentials to EEPROM.
writeRelayStatus()
Writes relay status to EEPROM.
readData()
Reads Wi-Fi credentials and relay status from EEPROM.

How to Use
AP Mode
Power up the NodeMCU.
Connect to the Wi-Fi network named NodeMCU-AP.
Open a web browser and go to http://192.168.4.1.
Configure the new Wi-Fi credentials.
STA Mode
After configuring the Wi-Fi credentials in AP mode, reboot the NodeMCU.
The NodeMCU will attempt to connect to the configured Wi-Fi network.
If connected, it will serve a web page with the relay control interface.
Access the web page via the IP address assigned by the router.

Web Interface
STA Mode Home Page
Displays the current Wi-Fi SSID.
Provides a button to toggle the relay.
Shows the current relay status (ON/OFF).
AP Mode Home Page
Displays the current Wi-Fi configuration.
Provides a form to input new Wi-Fi credentials.
