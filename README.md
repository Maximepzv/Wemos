
# Wemos LEDs Manager

I use a wemos mini D1 for this project.
The Wemos is connected to a LED strip. This strip is a gradient defined by two colors. When a new color is selected, the gradient goes from the new color to the old one.

## Installation
### Hardware
Connect the LED strip to the Wemos using the small cables:

 - Connect the red cable of the LED strip to the +5V of the Wemos
 - Connect the white cable of the LED strip to the GND (ground) of the Wemos
 - Connect the green cable of the LED strip to the D3 of the Wemos

Now, plug the Wemos into your computer.

### Software

On Linux, it should work out of the box.

For Windows and Mac OS, you need to download a driver:  [https://wiki.wemos.cc/downloads](https://wiki.wemos.cc/downloads)

For Mac OS, here is a detailed article on how to get started with Wemos:  [https://blog.rjdlee.com/getting-started-with-wemos-d1-on-mac-osx/](https://blog.rjdlee.com/getting-started-with-wemos-d1-on-mac-osx/)

### Arduino IDE

Install the Arduino IDE:  [https://www.arduino.cc/en/Main/Software](https://www.arduino.cc/en/Main/Software)

In order to interact with the Wemos, you have to configure the Arduino IDE. If you can't find the  `WeMos D1 R2 & mini`  board in  `Tools > Board`, do the following:

1.  Open preferences  `File > Preferences`.
2.  Add the line "`http://arduino.esp8266.com/stable/package_esp8266com_index.json`" in the  `Additional Boards Manager URLs`  field and close  `Preferences`.
3.  Open  `Tools > Board > Boards Manager`  and search  `ESP8266`.
4.  Install the  `ESP8266 by ESP8266 community`.
5.  Choose the  `WeMos D1 R2 & mini`  in  `Tools > Board`.

You also have to clone the two git repositories specified at the top of the file 
[https://github.com/Maximepzv/Wemos/blob/master/wemos.ino](https://github.com/Maximepzv/Wemos/blob/master/wemos.ino) into the `libraries` folder. (open `File > Preferences` and look at the Sketchbook location to find the `libraries` path).

You should also clone this repository under the path where your Arduino IDE is installed

### Upload to the board

1.  Select the right USB port in  `Tools > Port`.
2.  Change the SSID, Wi-Fi password and the hostname (your_app_id.cleverapps.io).
3.  Click on the  `Verify`  button.
4.  If there is no error,  `Upload`  it to the Wemos.

## How to connect the wemos to your network
-   when your ESP starts up, it sets it up in station mode and tries to connect to a previously saved Access Point
-   if this is unsuccessful (or no previous network saved) it moves the ESP into Access Point mode and spins up a DNS and WebServer (default IP 192.168.4.1)
-   using any Wi-Fi-enabled device with a browser (computer, phone, tablet) connect to the newly created Access Point
-   because of the Captive Portal and the DNS server you will either get a 'Join to network' type of popup or get any domain you try to access redirected to the configuration portal
-   choose one of the access points scanned, enter password, click save
-   ESP will try to connect. If successful, it relinquishes control back to your app. If not, reconnect to AP and reconfigure.

## Usage
Using any Wi-Fi-enabled device with a browser (computer, phone, tablet), connect your browser to the wemos local IP address on port 3042. It should look like: 192.168.0.42:3042
You can easily find it by using, for example, a mobile application that scans your network. Like "Fing" on Android. Don't forget the port at the end !
You should now see a color picker on your browser and be able to control your LEDs.

