// Load Wi-Fi library
#include <Adafruit_NeoPixel.h>    // https://github.com/adafruit/Adafruit_NeoPixel
#include <ESP8266WiFi.h>

#define PIXEL_COUNT       30


// Replace with your network credentials
const char* ssid     = "YOUR_NETWORK_SSID";
const char* password = "YOUR_NETWORK_PASSWORD";

struct RGB {
  byte r;
  byte g;
  byte b;
};

RGB color;
RGB lastColor;
int scrolling;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, D4, NEO_GRB + NEO_KHZ800);

// Set web server port number to 80
WiFiServer server(80);

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

  // Initialize the colors
  lastColor = {0, 0, 0};
  color = {0, 0, 0};
  scrolling = 0;
  strip.begin();
  strip.show();
  
  setup_wifi();
  
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  
  if (WiFi.status() != WL_CONNECTED) {
    setup_wifi();
  }
  
  WiFiClient client = server.available();   // Listen for incoming clients

  if (scrolling > 0) { // If scrolling is greater than 0, render LEDs
    while(scrolling > 0) {
      renderLED(strip.numPixels() - scrolling);
      delay(20);
      scrolling--;  
    }
    lastColor = color;
    scrolling = strip.numPixels();
    while(scrolling > 0) {
      renderLED(strip.numPixels() - scrolling);
      delay(20);
      scrolling--;  
    }
  } else if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {            // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
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
              Serial.println(redString.toInt());
              Serial.println(greenString.toInt());
              Serial.println(blueString.toInt());
               // Update colors
               lastColor = color;
               color = {redString.toInt(), greenString.toInt(), blueString.toInt()};

               // Start scrolling animation
               scrolling = strip.numPixels();
              }
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
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

void setup_wifi() {
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    renderLEDColor(0, {255, 0, 0});
    delay(500);
    renderLEDColor(0, {0, 0, 0});
    delay(500);
  }
  renderLEDColor(0, {0, 255, 0});
}

double diffAbs(byte last, byte current) {
  return (double) (last > current ? last - current : current - last);
}

byte computeValueAt(byte last, byte current, int i) {
  double ratio = (double) i / (double) strip.numPixels();
  return (current + (last > current ? 1 : -1) * (byte) (diffAbs(last, current) * ratio));
}

void renderLED(int i) {
  byte r = computeValueAt(lastColor.r, color.r, i);
  byte g = computeValueAt(lastColor.g, color.g, i);
  byte b = computeValueAt(lastColor.b, color.b, i);
  strip.setPixelColor(i, r, g, b);
  strip.show();
}

void renderLEDColor(int i, RGB color) {
  strip.setPixelColor(i, color.r, color.g, color.b);
  strip.show();
}
