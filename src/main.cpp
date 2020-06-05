#include "credentials.h"
#include "udp_receiver.h"
#include "decoder.h"
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define MAX_DEVICES 12 // number of modules connected
#define CLK_PIN 14
#define DATA_PIN 13 // aka MOSI
#define CS_PIN 15   // aka SS

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

#define SCROLL_SPEED 20
#define PAUSE_TIME 1000

MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

trackInformation_t currentTrack;
String noInfoYet("No track information received yet... Waiting...");

Decoder decoder("secret");

String *textToDisplay = new String();

UDPReceiver udp(47000, [&](uint8_t *buffer, size_t size, String remoteAddress, uint16_t remotePort) -> void {
  Serial.println("Received UDP package from " + remoteAddress + ":" + remotePort);

  currentTrack = decoder.decode(buffer, size);
  // marqueeTitle = Marquee(SCREEN_WIDTH, FONT_WIDTH, currentTrack.title);
  // marqueeArtist = Marquee(SCREEN_WIDTH, FONT_WIDTH, currentTrack.artist);
  // marqueeAlbum = Marquee(SCREEN_WIDTH, FONT_WIDTH, currentTrack.album);

  textToDisplay = new String("Playing '" + currentTrack.title + "' from '" + currentTrack.artist + "' on '" + currentTrack.album + "'");

  Serial.println(*textToDisplay);
});

void setup()
{
  Serial.begin(9600);

  P.begin();
  P.setIntensity(0); // set to lowest possible intensity

  P.displayText("Connecting to WiFi...", PA_LEFT, SCROLL_SPEED, 2 * PAUSE_TIME, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  P.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  P.setSpeed(SCROLL_SPEED);

  WiFi.begin(WIFI_SSID, WIFI_KEY);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(80);
    Serial.print(".");
    yield();
  }

  // We need to put this message on the heap, otherwise it's gone as soon as we leave setup().
  // This indeed causes a small memory leak, but as we want to keep showing this message until
  // actual content arrives it should be fine and not worth messing up the code...
  textToDisplay = new String("Connected! IP address: " + WiFi.localIP().toString());

  Serial.println("");
  Serial.println(*textToDisplay);

  P.displayText(textToDisplay->c_str(), PA_LEFT, SCROLL_SPEED, 2 * PAUSE_TIME, PA_SCROLL_LEFT, PA_SCROLL_LEFT);

  udp.sendRequestPacket(WiFi.subnetMask(), WiFi.localIP());
}

void loop()
{
  udp.check();
  yield();

  if (P.displayAnimate())
  {
    Serial.println("Reseting animation...");
    P.displayText(textToDisplay->c_str(), PA_LEFT, SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    P.displayReset();
  }
}