
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#ifndef STASSID
#define STASSID "DAP"
#define STAPSK  "12345678"
#endif

unsigned int Port = 8266;      // local port to listen on

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
  WiFi.macAddress(mac);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
  }
  ip = WiFi.localIP();
  digitalWrite(LED_BUILTIN, LOW);
  Serial.print("Connected! IP address: ");
  Serial.println(ip);
  Serial.printf("UDP server on port %d\n", Port);
  while (! Udp.begin(Port) ) {                             // UDP protocol connected to localPort which is a variable
    Serial.println('+');
    yield();                                                    // Play nicely with RTOS (alias = delay(0))
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

    if (strcmp(packetBuffer, "L0") == 0) {  //turn led off
      digitalWrite(LED_BUILTIN, HIGH);
    }
    if (strcmp(packetBuffer, "L1") == 0) {  //turn led ON
      digitalWrite(LED_BUILTIN, LOW);
    }
    if (strcmp(packetBuffer, "FB") == 0) {  //led fast blink twice
      ledFastBlinkTwice();
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
