
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#ifndef STASSID
#define STASSID "DDAP"
#define STAPSK  "34343434"
#endif

unsigned int localPort = 8266;      // local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1]; //buffer to hold incoming packet,
String  ReplyBuffer = "";     // a string to send back
byte mac[6];
IPAddress ip;

WiFiUDP Udp;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  //  Serial.print("Connected! IP address: ");
  //  Serial.println(WiFi.localIP());
  digitalWrite(LED_BUILTIN, LOW);
  WiFi.macAddress(mac);
  ip = WiFi.localIP();
  Serial.printf("UDP server on port %d\n", localPort);
  //sprintf(ReplyBuffer, "ack from %d.%d.%d.%d, %2X:%2X:%2X:%2X:%2X:%2X\r\n", ip[0], ip[1], ip[2], ip[3], mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  Udp.begin(localPort);
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

    if (strcmp(packetBuffer, "L0") == 0) {
      digitalWrite(LED_BUILTIN, HIGH);
    }
    if (strcmp(packetBuffer, "L1") == 0) {
      digitalWrite(LED_BUILTIN, LOW);
    }
    if (strcmp(packetBuffer, "FB") == 0) {
      ledFastBlinkTwice();
    }

    if (strcmp(packetBuffer, "Anyone?") == 0) {//compare eq0
      // send a reply, to the IP address and port that sent us the packet we received
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      ReplyBuffer = "Ack From" + ip.toString() + " MAC:" + mac2String(mac);
      Udp.write(ReplyBuffer.c_str());
      Udp.endPacket();
      digitalWrite(LED_BUILTIN, LOW);
      delay(300);
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }

}

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
