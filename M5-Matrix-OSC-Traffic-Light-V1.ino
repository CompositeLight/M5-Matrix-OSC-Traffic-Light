/* M5 Atom Matrix QLab/OSC Traffic Light.
Large sections of this code (the good bits) have been borrowed/stolen/magpied from the work of genius Joseph Adams */

#include <M5Atom.h>
#include <WiFi.h>
#include <Arduino_JSON.h>
#include <PinButton.h>
#include <stdint.h>
#include <Arduino.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#define DATA_PIN_LED 27
using namespace std;


// --------------------------------------------------------------------

/* USER CONFIG VARIABLES
    Change the following variables before compiling and sending the code to your device.
*/

//Wifi SSID and password
const char * networkSSID = "your_SSID_here";
const char * networkPass = "password";

//Wifi manual IP settings
IPAddress subnet(255, 255, 255, 0);    // Subnet Mask
IPAddress gateway(192, 168, 1, 1);     // Default Gateway
IPAddress ip(192, 168, 1, 184);        // Manual IP Address (must match the Destination IP setting in QLab Network Patch)

//OSC Settings
const unsigned int localPort = 54000;   // local port to listen for OSC messages (must match the Destination Port setting in QLab Network Patch)


// --------------------------------------------------------------------


//General Variables
bool networkConnected = false;    // State of wifi connection
uint8_t FSM = 0;


OSCErrorCode error;

//M5 variables
PinButton btnAction(39); //the "Action" button on the device - not currently used


//default color values
int GRB_COLOR_WHITE = 0xffffff;
int GRB_COLOR_BLACK = 0x000000;
int GRB_COLOR_RED = 0x00ff00;
int GRB_COLOR_ORANGE = 0xffa500;
int GRB_COLOR_YELLOW = 0xffff00;
int GRB_COLOR_GREEN = 0xff0000;
int GRB_COLOR_BLUE = 0x0000ff;
int GRB_COLOR_PURPLE = 0xa500ff;

int greenColour[] = {GRB_COLOR_RED, GRB_COLOR_BLACK};
int redColour[] = {GRB_COLOR_GREEN, GRB_COLOR_BLACK};
int yellowColour[] = {GRB_COLOR_YELLOW, GRB_COLOR_BLACK};
int orangeColour[] = {GRB_COLOR_ORANGE, GRB_COLOR_BLACK};
int purpleColour[] = {GRB_COLOR_PURPLE, GRB_COLOR_BLACK};
int blueColour[] = {GRB_COLOR_BLUE, GRB_COLOR_BLACK};


int blackColour[] = {GRB_COLOR_BLACK, GRB_COLOR_YELLOW};
int alloffcolor[] = {GRB_COLOR_BLACK, GRB_COLOR_BLACK};
int wificolor[] = {GRB_COLOR_BLUE, GRB_COLOR_BLACK};

int currentBrightness = 20;

//this is the array that stores the different LED looks
int number[19][25] = {{
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0
  },
  { 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1
  },
  { 1, 1, 1, 1, 1,
    1, 0, 0, 0, 1,
    1, 0, 1, 0, 1,
    1, 0, 0, 0, 1,
    1, 1, 1, 1, 1
  },
  { 0, 1, 1, 1, 0,
    1, 0, 1, 0, 1,
    1, 1, 0, 1, 1,
    1, 0, 1, 0, 1,
    0, 1, 1, 1, 0
  },
  { 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 1, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0
  },







};


WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("Network connected!");
      // Serial.println(String(WiFi.localIP()));
      Serial.println(WiFi.localIP().toString());
      networkConnected = true;
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("Network connection lost!");
      networkConnected = false;
      break;
  }
}

//---------------------------------------------------------------
// HERE IS THE MAIN LED DRAWING ROUTINE aka drawNumber
void drawNumber(int arr[], int colors[])
{
  for (int i = 0; i < 25; i++)
  {
    M5.dis.drawpix(i, colors[arr[i]]);
  }
}
//---------------------------------------------------------------
//Change colour of LEDs

void greensquare(){
  M5.dis.clear();
  drawNumber(number[0], greenColour);
}

void redsquare(){
  M5.dis.clear();
  drawNumber(number[0], redColour);
}

void yellowsquare(){
  M5.dis.clear();
  drawNumber(number[0], yellowColour);
}

void orangesquare(){
  M5.dis.clear();
  drawNumber(number[0], orangeColour);
}

void bluesquare(){
  M5.dis.clear();
  drawNumber(number[0], blueColour);
}

void purplesquare(){
  M5.dis.clear();
  drawNumber(number[0], purpleColour);
}


void blacksquare(){
  M5.dis.clear();
  drawNumber(number[4], blackColour);
}






void wifiErrorSquare(){
  M5.dis.clear();
  drawNumber(number[3], wificolor);
}

void connectToNetwork() {
  Serial.println("");
  Serial.println("Connecting to SSID: " + String(networkSSID));

  WiFi.disconnect(true);
  WiFi.onEvent(WiFiEvent);

  WiFi.mode(WIFI_STA); //station
  WiFi.setSleep(false);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(networkSSID, networkPass);
}



// --------------------------------------------------------------------------------------------------------------------
// Setup is the pre-loop running program

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Initializing M5-Atom.");

  M5.begin(true, false, true);
  delay(50);
  M5.dis.drawpix(0, 0xf00000);

  // blanks out the screen
  // drawNumber(number[17], alloffcolor);
  wifiErrorSquare();
  delay(100); //wait 100ms before moving on

  connectToNetwork(); //starts Wifi connection
  delay(5000);
  while (!networkConnected) {
    connectToNetwork(); //starts Wifi connection
    delay(5000);
  }
  // UDP connect for OSC
  Udp.begin(localPort);
  // Flash screen if connected to wifi.
  drawNumber(number[0], alloffcolor);
  delay(100);
  drawNumber(number[2], wificolor);
  delay(400);
  drawNumber(number[0], alloffcolor);
  delay(200);
  drawNumber(number[2], wificolor);
  delay(400);
  drawNumber(number[0], alloffcolor);
  delay(200);
  drawNumber(number[2], wificolor);
  delay(400);
  drawNumber(number[0], alloffcolor);
  delay(100);
  blacksquare();
}

void greenON(OSCMessage &rxmsg) {
  greensquare();
  Serial.println("GREEN!");
}

void redON(OSCMessage &rxmsg) {
  redsquare();
  Serial.println("RED!");
}

void yellowON(OSCMessage &rxmsg) {
  yellowsquare();
  Serial.println("YELLOW!");
}

void orangeON(OSCMessage &rxmsg) {
  orangesquare();
  Serial.println("ORANGE!");
}

void purpleON(OSCMessage &rxmsg) {
  purplesquare();
  Serial.println("PURPLE!");
}

void blueON(OSCMessage &rxmsg) {
  bluesquare();
  Serial.println("BLUE!");
}

void blackON(OSCMessage &rxmsg) {
  blacksquare();
  Serial.println("BLACK!");
}








// --------------------------------------------------------------------------------------------------------------------
// This is the main program loop

void loop()
{
  OSCMessage msg;
  int size = Udp.parsePacket();
  if (size > 0) {
    while (size--) {
      msg.fill(Udp.read());
    }
    if (!msg.hasError()) {
      msg.dispatch("/green", greenON);
      msg.dispatch("/red", redON);
      msg.dispatch("/yellow", yellowON);
      msg.dispatch("/orange", orangeON);
      msg.dispatch("/blue", blueON);
      msg.dispatch("/purple", purpleON);
      msg.dispatch("/black", blackON);
      delay (100);
    } else {
      error = msg.getError();
      Serial.println("error: ");
      Serial.println(error);
    }
  }

  if (!networkConnected){
    // Lost Network Connection
    wifiErrorSquare();
    connectToNetwork(); //starts Wifi connection
    while (!networkConnected) {
      delay(200);
    }
     // UDP connect for OSC
     Udp.begin(localPort);
     redsquare();
     
   }
  
  delay(50);
  M5.update();
}

// --------------------------------------------------------------------------------------------------------------------
