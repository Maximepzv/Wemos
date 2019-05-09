#include <Adafruit_NeoPixel.h>    // https://github.com/adafruit/Adafruit_NeoPixel
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager
#include <ESP8266WiFi.h>

// Number of leds on the led strip
#define PIXEL_COUNT       30

// Set web server port number to 3042
WiFiServer server(3042);

// Structure to define the RGB color system 
struct RGB {
  byte r;
  byte g;
  byte b;
};

// Our RGB variable
RGB color;
// Iterator for the led strup
int scrolling;

// Using D3 instead of D4 to avoid that the small blue led of the wemos remains lit
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, D3, NEO_GRB + NEO_KHZ800);

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();

// Previous time
unsigned long previousTime = 0;

// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(9600);

  /********** WiFiManager **********/
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  
  // Uncomment the wifiManager.resetSettings() line; to force the reset of the board's WIFI parameters.
  //wifiManager.resetSettings();

  // TODO : CONFIGURE STATIC IP

  // Tries to connect to last known settings
  // If it does not connect it starts an access point 
  // and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("Wemos_LEDs")) {
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  /********** WiFiManager **********/

  // Initialize the colors
  color = {0, 0, 0};
  scrolling = 0;
  strip.begin();
  strip.show();
  
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){  
  // Listen for incoming clients
  WiFiClient client = server.available();   

  // If scrolling is greater than 0, render LEDs
  if (scrolling > 0) { 
    renderLEDColor(strip.numPixels() - scrolling, color);
    delay(40);
    scrolling--;  

    // If a new client connects,
  } else if (client) {                             
    currentTime = millis();
    previousTime = currentTime;
    // print a message out in the serial port
    Serial.println("New Client.");          
    // make a String to hold incoming data from the client
    String currentLine = "";                
    // loop while the client's connected
    while (client.connected() && currentTime - previousTime <= timeoutTime) {            
      currentTime = millis();
      // if there's bytes to read from the client
      if (client.available()) {             
        // read a byte
        char c = client.read();            
        // then print it out the serial monitor 
        Serial.write(c);                   
        header += c;
        // if the byte is a newline character
        if (c == '\n') {
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
                   
            // Display the HTML web page
            client.println("<!DOCTYPE html> <html> <head> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> <link rel=\"icon\" href=\"data:,\"> <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\"> <script src=\"https://cdn.jsdelivr.net/npm/@jaames/iro/dist/iro.min.js\"></script> </head> <body style=\"display: flex; align-items: center; justify-content: center; height: 100vh;\"> <div id=\"color-picker-container\"></div> <script> var urlParams = new URLSearchParams(window.location.search); var r = urlParams.get('r'); var g = urlParams.get('g'); var b = urlParams.get('b'); var colorPicker = new iro.ColorPicker('#color-picker-container', { color: { r: r, g: g, b: b } }); var hex = colorPicker.color.hexString; colorPicker.on('input:end', onColorChange); function onColorChange(color, changes) { window.location.href = \"?r=\" + Math.round(color.rgb.r) + \"&g=\" + Math.round(color.rgb.g) + \"&b=\" + Math.round(color.rgb.b) + \"&\"; } </script> </body> </html>");
            
            // The HTTP response ends with another blank line
            client.println();

            // Request sample: /?r201g32b255&
            // Red = 201 | Green = 32 | Blue = 255
            if(header.indexOf("GET /?r") >= 0) {
  
              // When receiving data, process it and start scrolling animation
              // Expected format is: RRRGGGBBB (as in 255000000, 035127078, ...)
              int pos1 = header.indexOf("r=") + 1;
              int pos2 = header.indexOf("g=") + 1;
              int pos3 = header.indexOf("b=") + 1;
              int pos4 = header.indexOf("&");
              String redString = header.substring(pos1+1, pos2);
              String greenString = header.substring(pos2+1, pos3);
              String blueString = header.substring(pos3+1, pos3+4);
  
              // Print a message out in the serial port
              Serial.println(redString.toInt());
              Serial.println(greenString.toInt());
              Serial.println(blueString.toInt());

               // Update colors
               color = {redString.toInt(), greenString.toInt(), blueString.toInt()};

               // Start scrolling animation
               scrolling = strip.numPixels();
              }
            // Break out of the while loop
            break;
          } else { 
            // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  
          // if you got anything else but a carriage return character,
          // add it to the end of the currentLine
          currentLine += c;      
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void renderLEDColor(int i, RGB color) {
  strip.setPixelColor(i, color.r, color.g, color.b);
  strip.show();
}
