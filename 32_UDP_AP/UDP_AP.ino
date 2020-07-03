//UDP broadcast Server & AP

#include "WiFi.h"
#include <WiFiAP.h>
#include "AsyncUDP.h"

int listening_port = 8266;

const char * ap_ssid = "aa32p";
const char * ap_password = "34343434";

AsyncUDP udp;

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);
  IPAddress myIP = WiFi.softAPIP();
  //  IPAddress subnet = WiFi.subnetMask();
  //  IPAddress gateway = WiFi.gatewayIP();

  Serial.print("SOFT IP: ");
  Serial.println(myIP);
  //  Serial.print("NETMASK: ");
  //  Serial.println(subnet);
  //  Serial.print("GATEWAY: ");
  //  Serial.println(gateway);


  if (udp.listen(8266)) {
    Serial.print("UDP Listening on port: ");
    Serial.println(listening_port);
    udp.onPacket([](AsyncUDPPacket packet) {
      Serial.print("UDP Packet Type: ");
      Serial.print(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast" : "Unicast");
      Serial.print(", From: ");
      Serial.print(packet.remoteIP());
      Serial.print(":");
      Serial.print(packet.remotePort());
      Serial.print(", To: ");
      Serial.print(packet.localIP());
      Serial.print(":");
      Serial.print(packet.localPort());
      Serial.print(", Length: ");
      Serial.print(packet.length());
      Serial.print(", Data: ");
      Serial.write(packet.data(), packet.length());
      Serial.println();
      //reply to the client
      packet.printf("Got %u bytes of data", packet.length());
    });

  }
}

void loop()
{
  delay(10000);
  Serial.println();
  udp.broadcast("Anyone there? from32");
}
