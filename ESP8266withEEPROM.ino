#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>

// Global variables
ESP8266WebServer server(80); // Initialize ESP8266WebServer on port 80
String ssid, password; // Variables to store Wi-Fi credentials
String deviceID; // Variable to store device ID (not currently used)
bool relayStatus = false; // Variable to store relay status (false = OFF, true = ON)
const int relayPin = D1;  // Pin to control the relay

// Setup function, runs once at the start
void setup() {
  // Start serial communication
  Serial.begin(115200);
  EEPROM.begin(512); // Initialize EEPROM with 512 bytes
  delay(100); // Short delay to stabilize

  // Initialize relay pin as an output
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Ensure relay is off initially

  // Read stored Wi-Fi credentials and relay status from EEPROM
  readData();

  // Set relay to last known status from EEPROM
  digitalWrite(relayPin, relayStatus ? HIGH : LOW);

  //if (testWiFi()) {
    //launchWeb(0); // Launch STA mode web server
 // } else {
    // Set up Access Point mode
    const char* ssidap = "NodeMCU-AP";
    const char* passap = "";
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssidap, passap);
    Serial.print("AP mode - http://");
    Serial.println(WiFi.softAPIP());
    launchWeb(1); // Launch AP mode web server
  //}
}

// Main loop function, runs continuously
void loop() {
  server.handleClient(); // Handle incoming client requests
}

// Function to launch web server based on mode
void launchWeb(int webtype) {
  createWebServer(webtype); // Create web server routes
  server.begin(); // Start the server
}

// Function to create web server routes based on mode
void createWebServer(int webtype) {
  if (webtype == 0) {
    // STA Mode Web Server
    server.on("/", []() {
      // HTML content for STA mode home page
      String content = "<!DOCTYPE HTML><html>";
      content += "<head><title>NodeMCU Control</title>";
      content += "<style>";
      content += "body { font-family: Arial, sans-serif; text-align: center; background-color: #f0f0f0; }";
      content += "h1 { color: #333; }";
      content += "p { font-size: 1.2em; }";
      content += "form { margin: 20px; }";
      content += "input[type=submit] { background-color: #4CAF50; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; font-size: 1em; }";
      content += "input[type=submit]:hover { background-color: #45a049; }";
      content += ".relay-status-on { color: green; font-weight: bold; }";
      content += ".relay-status-off { color: red; font-weight: bold; }";
      content += "</style></head>";
      content += "<body>";
      content += "<h1>WiFi Mode</h1>";
      content += "<p>Welcome to Your NodeMCU</p>";
      content += "<p>Connected to SSID: " + ssid + "</p>";
      content += "<form method='get' action='toggle'>";
      content += "<input type='submit' value='Toggle Relay'></form>";
      String relayStatusString = digitalRead(relayPin) ? "<span class='relay-status-on'>ON</span>" : "<span class='relay-status-off'>OFF</span>";
      content += "<p>Relay is currently " + relayStatusString + "</p>";
      content += "</body></html>";
      server.send(200, "text/html", content); // Send the content to the client
    });

    server.on("/toggle", []() {
      // Toggle relay status and update EEPROM
      digitalWrite(relayPin, !digitalRead(relayPin)); // Toggle relay
      relayStatus = digitalRead(relayPin); // Update relay status
      writeRelayStatus(); // Save relay status to EEPROM
      // HTML content for relay status page
      String content = "<!DOCTYPE HTML><html>";
      content += "<head><title>NodeMCU Control</title>";
      content += "<style>";
      content += "body { font-family: Arial, sans-serif; text-align: center; background-color: #f0f0f0; }";
      content += "h1 { color: #333; }";
      content += "p { font-size: 1.2em; }";
      content += "a { display: inline-block; margin-top: 20px; padding: 10px 20px; background-color: #4CAF50; color: white; text-decoration: none; border-radius: 5px; }";
      content += "a:hover { background-color: #45a049; }";
      content += ".relay-status-on { color: green; font-weight: bold; }";
      content += ".relay-status-off { color: red; font-weight: bold; }";
      content += "</style></head>";
      content += "<body>";
      content += "<h1>Relay Status</h1>";
      String relayStatusString = relayStatus ? "<span class='relay-status-on'>ON</span>" : "<span class='relay-status-off'>OFF</span>";
      content += "<p>Relay is now " + relayStatusString + "</p>";
      content += "<a href='/'>Go Back</a>";
      content += "</body></html>";
      server.send(200, "text/html", content); // Send the content to the client
    });
  }

  if (webtype == 1) {
    // AP Mode Web Server
    server.on("/", []() {
      // HTML content for AP mode home page
      String content = "<!DOCTYPE HTML><html>";
      content += "<head><title>NodeMCU Configuration</title>";
      content += "<style>";
      content += "body { font-family: Arial, sans-serif; text-align: center; background-color: #f0f0f0; }";
      content += "h1, h2 { color: #333; }";
      content += "form { margin: 20px auto; max-width: 300px; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }";
      content += "input[type=text] { width: 100%; padding: 10px; margin: 10px 0; border: 1px solid #ccc; border-radius: 5px; }";
      content += "input[type=submit] { background-color: #4CAF50; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; font-size: 1em; }";
      content += "input[type=submit]:hover { background-color: #45a049; }";
      content += "</style></head>";
      content += "<body>";
      content += "<h1>Access Point Mode</h1>";
      content += "<h2>NodeMCU WiFi Configuration</h2>";
      content += "<p><b>Current Configuration:</b></p>";
      content += "<p>SSID: " + ssid + "</p>";
      content += "<p>Password: " + password + "</p>";
      content += "<form method='get' action='setting'>";
      content += "<p><b>New Configuration:</b></p>";
      content += "<label>SSID: </label><br><input type='text' name='ssid' maxlength='32'><br>";
      content += "<label>Password: </label><br><input type='text' name='password' maxlength='32'><br>";
      content += "<br><input type='submit' value='Save'></form>";
      content += "</body></html>";
      server.send(200, "text/html", content); // Send the content to the client
    });

    server.on("/setting", []() {
      // Save new Wi-Fi credentials and display success message
      ssid = server.arg("ssid");
      password = server.arg("password");
      writeData(ssid, password); // Save new credentials to EEPROM
      // HTML content for success page
      String content = "<!DOCTYPE HTML><html>";
      content += "<head><title>NodeMCU Configuration</title>";
      content += "<style>";
      content += "body { font-family: Arial, sans-serif; text-align: center; background-color: #f0f0f0; }";
      content += "h1 { color: #333; }";
      content += "</style></head>";
      content += "<body>";
      content += "<h1>Configuration Saved</h1>";
      content += "<p>Success. Please reboot to apply the new settings.</p>";
      content += "</body></html>";
      server.send(200, "text/html", content); // Send the content to the client
    });
  }
}

// Function to test WiFi connection
boolean testWiFi() {
  WiFi.begin(ssid.c_str(), password.c_str()); // Begin WiFi connection
  int c = 0;
  while (c < 20) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Connected to WiFi");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      return true;
    }
    Serial.print(".");
    delay(500);
    c++;
  }
  Serial.println("Connection timed out");
  return false;
}

// Function to write data to EEPROM
void writeData(String a, String b) {
  Serial.println("Writing to EEPROM");
  for (int i = 0; i < 32; i++) {
    EEPROM.write(i, i < a.length() ? a[i] : 0);
  }
  for (int i = 32; i < 64; i++) {
    EEPROM.write(i, i - 32 < b.length() ? b[i - 32] : 0);
  }
  EEPROM.commit();
  Serial.println("Write successful");
}

// Function to write relay status to EEPROM
void writeRelayStatus() {
  EEPROM.write(128, relayStatus ? 1 : 0);
  EEPROM.commit();
}

// Function to read data from EEPROM
void readData() {
  Serial.println("Reading from EEPROM...");
  ssid = "";
  password = "";
  for (int i = 0; i < 32; i++) {
    ssid += char(EEPROM.read(i));
  }
  for (int i = 32; i < 64; i++) {
    password += char(EEPROM.read(i));
  }
  relayStatus = EEPROM.read(128) == 1;
  Serial.println("WiFi SSID from EEPROM: " + ssid);
  Serial.println("WiFi password from EEPROM: " + password);
  Serial.println("Relay status from EEPROM: " + String(relayStatus ? "ON" : "OFF"));
  Serial.println("Reading successful.");
}


