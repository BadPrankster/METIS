// Code of the Socket 'Porus'
// Diploma project at TEKO Basel
// August-September 2022
// Lucas Wirz-Vitiuk

// Load Wi-Fi library
#include <WiFi.h>
#include <WebServer.h>

// Set network credentials
const char* ssid = "Edipsos (METIS-Hotspot)";
const char* password = "Lucas6.0";

// Set IP of the associated WiFi-switch
String wifi_switch = "http://10.3.141.222/relay/0?turn=";
String wifi_switch_url;

// Set web server port number
WiFiServer server(80);

// Variables to store the HTTP request
String header;
String currentLine = "";
char c;

// Define time variables and timeout
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

// Define pin for current detection
const int Detection_Pin = 4;

// Variables to show status
String NFCstate;
String status = "Ready";
String color = "#32cd32";
String interlock = "undefined";

// Variables for temperature monitoring
float temperature;
const float warning_limit = 30;
const float maximum_temperature = 31.5;
const float threshold = 4;
bool is_cutout = false;

// Variable to store mating cycles
unsigned int mating_cycles;
bool is_connected = false;

void setup() {
  Serial.begin(115200);

  // Setup current detection
  pinMode(Detection_Pin, INPUT_PULLDOWN);

  // Setup and test LED-Outputs
  setup_LEDs();
  test_LEDs();

  // Setup temperature measurement
  setup_temperature_measurement();

  // Setup servo
  setup_servo();

  // Read mating cycles
  mating_cycles = readMemory();

  // Connect to network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();

  // Setup NFC recognition
  setup_NFC();
}

void loop() {

  if (!is_cutout) {

    if (!digitalRead(Detection_Pin)) {

      // Open interlock
      unlock();
      interlock = "open";

      // Check NFC state
      NFCstate = getNFCstate();

      // Actuate LEDs according to state
      if (NFCstate == "none") {
        status = "Ready";
        color = "#32cd32";
        green_Signal();
        is_connected = false;
        wifi_switch_url = wifi_switch + "off";
      }
      else if ((NFCstate != "none")) {
        if (!is_connected) {
          mating_cycles++;
          writeMemory("cycles", mating_cycles);
          is_connected = true;
        }
        if ((NFCstate == "right")) {
          status = "Connected";
          color = "#ffd700";
          yellow_Signal();
          wifi_switch_url = wifi_switch + "on";
        }
        else if ((NFCstate == "wrong")) {
          status = "Not Connected";
          color = "#ffd700";
          alarm("yellow");
          wifi_switch_url = wifi_switch + "off";
        }
      }
    } else {

      // Close interlock
      lock();
      interlock = "closed";

      // Get temperature and check limits
      temperature = measure_temperature(0);
      status = "Current";
      color = "#D61D23";
      red_Signal();
      if (temperature >= warning_limit) {
        status = "Overheat";
        alarm("red");
        if (temperature >= maximum_temperature) {
          status = "Cutout";
          is_cutout = true;
        }
      }
    }
  } else {

    // Cutout conditions

    unlock();
    interlock = "open";
    
    Serial.println("Am I not here?");
    status == "Cutout";
    color = "#D61D23";
    alarm("red");
    wifi_switch_url = wifi_switch + "off";

    temperature = measure_temperature(0);

    // Requirement to revoke cutout
    if (temperature <= (maximum_temperature - threshold)) {
      is_cutout = false;
    }
  }

  WiFiClient client = server.available();

  if (client) {

    currentTime = millis();
    previousTime = currentTime;
    currentLine = "";

    // loop while the client is connected
    while (client.connected() && currentTime - previousTime <= timeoutTime) {

      currentTime = millis();
      if (client.available()) {

        c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {

          if (currentLine.length() == 0) {

            client.println("HTTP/1.1 200 OK");

            // Allow ajax request
            client.println("Access-Control-Allow-Origin:*");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Display the HTML web page
            client.println("<!DOCTYPE html>\n<html>");

            // Header and CSS style
            client.println("\t<head>\n\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("\t\t<meta http-equiv=\"refresh\" content=\"1\" />\n\t\t<meta name=\"author\" content=\"Lucas Wirz-Vitiuk\" />");
            client.println("\t\t<title>Porus</title>");
            client.println("\t\t<style>\n\t\t\thtml {\n\t\t\t\tfont-family: Helvetica;\n\t\t\t\tfont-weight: bold;\n\t\t\t\ttext-align: center\n\t\t\t}\n");
            client.println("\t\t\th1 {\n\t\t\t\tmargin-top: 1.3em;\n\t\t\t}\n");
            client.println("\t\t\ttable {\n\t\t\t\twidth: 84%;\n\t\t\t\tmargin: 0 auto;\n\t\t\t\tborder-spacing: 2pt;\n\t\t\t}\n");
            client.println("\t\t\ttr:nth-child(odd) {\n\t\t\t\tbackground-color: #C7CDD7;\n\t\t\t}\n");
            client.println("\t\t\ttr:nth-child(even) {\n\t\t\t\tbackground-color: #E3E5EB;\n\t\t\t}\n");
            client.println("\t\t\ttd {\n\t\t\t\twidth: 50%;\n\t\t\t\theight: 3em;\n\t\t\t\tpadding: 0 1em;\n\t\t\t}\n");
            client.println("\t\t\ttd:first-child {\n\t\t\t\ttext-align: left;\n\t\t\t}\n");
            client.println("\t\t\ttd:last-child {\n\t\t\t\ttext-align: right;\n\t\t\t}\n");
            client.println("\t\t\t.blink {\n\t\t\t\tanimation: blink 1.75s;\n\t\t\t}\n");
            client.println("\t\t\t@keyframes blink {\n\t\t\t\t0% {opacity: 1;}\n\t\t\t\t50%{opacity: 0;}\n\t\t\t\t100%{opacity: 1;}\n\t\t\t}\n");
            client.println("\t\t\t#warning {\n\t\t\t\twidth: 74%;\n\t\t\t\tborder: 2px solid black;\n\t\t\t\tborder-radius: 12px;\n\t\t\t\tmargin-left: auto;\n\t\t\t\tmargin-right: auto;\n\t\t\t}\n");
            client.println("\t\t</style>\n\t</head>\n");

            // Web page content
            client.println("\t<body>\n\t\t<h1>Porus</h1>\n");

            client.print("\t\t<table>\n\t\t\t<tr>\n\t\t\t\t<td>Status:</td>\n\t\t\t\t<td><p style=\"-webkit-text-stroke: 0.5px black; color:");
            client.print(color);
            client.print("\">");
            client.print(status);
            client.println("</p></td>\n\t\t\t</tr>\n");

            client.println("\t\t\t<tr>\n\t\t\t\t<td>Mating Cycles:</td>");
            client.println("\t\t\t\t<td id=\"matingCycles\"></td>\n\t\t\t</tr>\n");

            client.print("\t\t\t<tr>\n\t\t\t\t<td>Interlock:</td>\n\t\t\t\t<td>");
            client.print(interlock);
            client.println("</td>\n\t\t\t</tr>\n");

            client.println("\t\t\t<tr>\n\t\t\t\t<td>Temperature:</td>");
            client.print("\t\t\t\t<td>");

            if ((digitalRead(Detection_Pin)) || (is_cutout)) {

              // Display temperature
              client.print(String(temperature, 1));
              client.print("&nbsp;&deg;C");

            } else {
              client.print("&ndash;");
            }
            client.println("</td>\n\t\t\t</tr>");
            client.println("\t\t</table>\n");

            // Output warning message
            if (status == "Not Connected") {
              client.println("<br><p id=\"warning\" style=\"background-color: #fef3cb\"><br><span class=\"blink\">&#x26A0; Warning!</span><br>Wrong connector plugged in<br><br></p>");
            }

            if ((status == "Overheat") || (status == "Cutout")) {
              client.print("<br><p id=\"warning\" style=\"background-color: #f5d7da\"><br><span class=\"blink\">&#x26A0; Warning!</span><br>");

              if (status == "Overheat") {
                client.println("Temperature-Limit exceeded<br><br></p>");
              }
              if (status == "Cutout") {
                client.println("Power turned off automatically<br><br></p>");
              }
            }

            // Toggle the WiFi switch
            client.print("\t\t<iframe src=\"");
            client.print(wifi_switch_url);
            client.println("\" hidden></iframe>\n");

            // Add separatorts to the number of mating cycles
            client.print("\t<script>\n\t\tnumber = ");
            client.print(mating_cycles);
            client.println("\n\t\telem = document.getElementById(\"matingCycles\");\n\t\telem.innerText = number.toLocaleString('de-CH').replace('.', \"'\");\n\t</script>\n");

            client.println("\t</body>\n</html>");

            // End HTTP response with another blank line
            client.println();
            break;
          }
        }
      }
    }
    // Clear the header variable
    header = "";

    // Close the connection
    client.stop();
  }
}
