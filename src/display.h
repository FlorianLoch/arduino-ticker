#include <MD_Parola.h>
#include <MD_MAX72xx.h>

#define MAX_DEVICES 12 // number of modules connected
#define CLK_PIN 14
#define DATA_PIN 13 // aka MOSI
#define CS_PIN 15   // aka SS

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

#define SCROLL_SPEED 25
#define PAUSE_TIME 1000
#define LED_INTENSITY 0 // value between 0 (dark) and 15 (bright)

class Display
{
private:
  MD_Parola parola = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
  String *currentMessage;
public:
  Display()
  {
    this->parola.begin();
    this->parola.setIntensity(LED_INTENSITY);

    this->parola.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    this->parola.setSpeed(SCROLL_SPEED);
  };

  void showConnectingMessage()
  {
    this->parola.displayText("Connecting to WiFi...", PA_LEFT, SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  }

  void showConnectedMessage(String ipAddress)
  {
    this->setCurrentMessage(new String("Connected! IP address: " + ipAddress));
    // this->parola.displayText(this->currentMessage->c_str(), PA_LEFT, SCROLL_SPEED, 2 * PAUSE_TIME, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  }

  void setCurrentMessage(String *currentMessage) {
    free(this->currentMessage);
    this->currentMessage = currentMessage;
    // TODO set this just once at the beginning...
    this->parola.setTextBuffer(this->currentMessage->c_str());
  };

  void animate()
  {
    if (this->parola.displayAnimate())
    {
      Serial.println("Reseting animation...");
      this->parola.displayReset();
    }
  }
};
