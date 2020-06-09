#include "credentials.h"
#include "udp_receiver.h"
#include "caching_temp_reader.h"
#include "decoder.h"
#include "display.h"
#include <SPI.h>

// pins for display etc. are set in display.h

CachingTempReader tempSensor;
Decoder decoder(MESSAGING_SECRET);
Display display(&tempSensor);

UDPReceiver udp(47000, [&](uint8_t *buffer, size_t size, String remoteAddress, uint16_t remotePort) -> void {
  Serial.println("Received UDP package from " + remoteAddress + ":" + remotePort);

  message_t msg = decoder.decode(buffer, size);
  // marqueeTitle = Marquee(SCREEN_WIDTH, FONT_WIDTH, currentTrack.title);
  // marqueeArtist = Marquee(SCREEN_WIDTH, FONT_WIDTH, currentTrack.artist);
  // marqueeAlbum = Marquee(SCREEN_WIDTH, FONT_WIDTH, currentTrack.album);

  if (msg.type == "msg")
  {
    Serial.println("Going to display:");
    Serial.println(msg.payload);
    display.setMessage(msg.payload);
  }
  else if (msg.type == "err")
  {
    Serial.println("An error occurred during decoding of received message:");
    Serial.println(msg.payload);
  }
});

void setup()
{
  Serial.begin(9600);

  display.showConnectingMessage();
  display.animate();

  if (!tempSensor.begin())
  {
    Serial.println("Cannot access SI7021 temperature sensor!");
  }

  WiFi.begin(WIFI_SSID, WIFI_KEY);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(80);

    Serial.print(".");
    yield();
  }

  Serial.println("IP address: " + WiFi.localIP().toString());

  display.showConnectedMessage(WiFi.localIP().toString());

  udp.sendRequestPacket(WiFi.subnetMask(), WiFi.localIP());
}

void loop()
{
  udp.check();
  yield();

  display.animate();
}