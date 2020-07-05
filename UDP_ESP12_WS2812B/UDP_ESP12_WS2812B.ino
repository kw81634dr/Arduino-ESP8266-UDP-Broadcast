//--------Headers_Wi-Fi--------
#include"credential.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

//--------Headers_LED--------
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER //Must be defined before including <FastLED.h>
#include <FastLED.h>


//--------Define_LED--------
#define NUM_LEDS 1 // How many leds in your strip?
#define DATA_PIN 2
CRGB leds[NUM_LEDS];// Define the array of leds

//--------Define_UDP--------
unsigned int Port = 8266;      // local port to listen on
// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1]; //buffer to hold incoming packet,
String  ReplyBuffer = "";     // a string to send back
byte mac[6];
IPAddress ip;
WiFiUDP Udp;

//-------var--------
byte modeNum = 0;


//---------Setup------------
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);//LED_BUILTIN = D4 on WeMos D1 Mini
  digitalWrite(LED_BUILTIN, HIGH);

  LEDS.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  LEDS.setBrightness(64);//0~255
  leds[0] = CRGB::Black;
  FastLED.show();

  Serial.begin(115200);

  // Wi-Fi Init
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  WiFi.macAddress(mac);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
  }
  ip = WiFi.localIP();
  digitalWrite(LED_BUILTIN, LOW);
  Serial.print("Connected! IP address: ");
  Serial.println(ip);
  delay(3000);
  digitalWrite(LED_BUILTIN, HIGH);

  // UDP Init
  Serial.printf("UDP server on port %d\n", Port);
  while (! Udp.begin(Port) ) {     // UDP protocol connected to localPort which is a variable
    Serial.println('+');
    yield();                       // Play nicely with RTOS (alias = delay(0))
  }
}

void loop() {
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.printf("Received packet of size %d from %s:%d\n    (to %s:%d, free heap = %d B)\n",
                  packetSize,
                  Udp.remoteIP().toString().c_str(), Udp.remotePort(),
                  Udp.destinationIP().toString().c_str(), Udp.localPort(),
                  ESP.getFreeHeap());

    // read the packet into packetBufffer
    int n = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    packetBuffer[n] = 0;
    Serial.println("Contents:");
    Serial.println(packetBuffer);

    if (strcmp(packetBuffer, "RST") == 0) {  //restart ESP8266
      ESP.restart();
    }
    if (strcmp(packetBuffer, "blink") == 0) {  //led blink
      digitalWrite(LED_BUILTIN, LOW);
      delay(20);
      digitalWrite(LED_BUILTIN, HIGH);
    }
    if (strcmp(packetBuffer, "Anyone?") == 0) { //got discover command
      // send a reply, to the IP address and port that sent us the packet we received
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      ReplyBuffer = "Ack From" + ip.toString() + " MAC:" + mac2String(mac);
      Udp.write(ReplyBuffer.c_str());
      Udp.endPacket();
      digitalWrite(LED_BUILTIN, LOW);
      delay(300);
      digitalWrite(LED_BUILTIN, HIGH);
    }
    if (strcmp(packetBuffer, "d") == 0) modeNum = 0;
    if (strcmp(packetBuffer, "rgb") == 0) modeNum = 1;
    if (strcmp(packetBuffer, "r") == 0) modeNum = 2;
    if (strcmp(packetBuffer, "g") == 0) modeNum = 3;
    if (strcmp(packetBuffer, "b") == 0) modeNum = 4;
  }

  switch (modeNum) {
    case 0:
      leds[0] = CRGB::Black;
      FastLED.show();
      break;
    case 1:
      static uint8_t hue = 0;
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(hue++, 255, 255);
        FastLED.show();
        fadeall();
        delay(10);
      }
      break;
    case 2:
      leds[0] = CRGB::Red;
      FastLED.show();
      break;
    case 3:
      leds[0] = CRGB::Green;
      FastLED.show();
      break;
    case 4:
      leds[0] = CRGB::Blue;
      FastLED.show();
      break;
    default:
      modeNum = 0;
      break;
  }

}//------END OF LOOP------



//-----------Func----------
String mac2String(byte ar[]) {
  String s;
  for (byte i = 0; i < 6; ++i)
  {
    char buf[3];
    sprintf(buf, "%2X", ar[i]);
    s += buf;
    if (i < 5) s += ':';
  }
  return s;
}

void fadeall() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(250);
  }
}

void ledFastBlinkTwice(void) {
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(LED_BUILTIN, LOW);
  delay(25);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(150);
  digitalWrite(LED_BUILTIN, LOW);
  delay(25);
  digitalWrite(LED_BUILTIN, HIGH);
}
