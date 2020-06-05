#include "credentials.h"
#include "udp_receiver.h"
#include "decoder.h"
#include "display.h"
#include <SPI.h>

// pins for display etc. are set in display.h

Decoder decoder("secret");
Display display;

UDPReceiver udp(47000, [&](uint8_t *buffer, size_t size, String remoteAddress, uint16_t remotePort) -> void {
  Serial.println("Received UDP package from " + remoteAddress + ":" + remotePort);

  trackInformation_t currentTrack = decoder.decode(buffer, size);
  // marqueeTitle = Marquee(SCREEN_WIDTH, FONT_WIDTH, currentTrack.title);
  // marqueeArtist = Marquee(SCREEN_WIDTH, FONT_WIDTH, currentTrack.artist);
  // marqueeAlbum = Marquee(SCREEN_WIDTH, FONT_WIDTH, currentTrack.album);

  display.setCurrentMessage(new String("Playing \"" + currentTrack.title + "\" from \"" + currentTrack.artist + "\" on \"" + currentTrack.album + "\""));
});

void setup()
{
  Serial.begin(9600);

  display.showConnectingMessage();

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