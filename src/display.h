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
  char tempHumid[20];
  float temperature;
  float humidity;

public:
  Display()
  {
    this->parola.begin(2);

    this->parola.setZone(0, 0, MAX_DEVICES - 3);
    this->parola.setZone(1, MAX_DEVICES - 2, MAX_DEVICES - 1);
    // this->parola.setFont(1, numeric7Seg);
    this->parola.setIntensity(LED_INTENSITY);

    this->parola.setTextEffect(1, PA_PRINT, PA_PRINT);

    this->parola.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    this->parola.setSpeed(SCROLL_SPEED);
  };

  void showConnectingMessage()
  {
    this->parola.displayZoneText(0, "Connecting to WiFi...", PA_LEFT, SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  }

  void showConnectedMessage(String ipAddress)
  {
    this->setMessage(new String("Connected! IP address: " + ipAddress));
    // this->parola.displayText(this->currentMessage->c_str(), PA_LEFT, SCROLL_SPEED, 2 * PAUSE_TIME, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  }

  void setMessage(String currentMessage) {
    this->setMessage(new String(currentMessage));
  }

  void setMessage(String *currentMessage)
  {
    free(this->currentMessage);
    this->currentMessage = currentMessage;
    // TODO set this just once at the beginning...
    this->parola.setTextBuffer(0, this->currentMessage->c_str());
  };

  void setTemperature(float temperature)
  {
    this->temperature = temperature;

    // char buffer[7];
    // sprintf(buffer, "%.2f °", this->temperature);
    // // this->parola.setTextBuffer(1, buffer);
    // // this->parola.displayReset(1);
    // Serial.println(buffer);
    // this->parola.displayZoneText(1, buffer, PA_LEFT, SCROLL_SPEED, 0, PA_PRINT);
  };

  void setHumidity(float humidity)
  {
    this->humidity = humidity;
  };

  void animate()
  {
    if (this->parola.displayAnimate())
    {
      if (this->parola.getZoneStatus(0))
      {
        this->parola.displayReset(0);
      }
      if (this->parola.getZoneStatus(1))
      {
        sprintf(this->tempHumid, "%.2f", this->temperature);
        // this->parola.setTextBuffer(1, buffer);
        this->parola.displayZoneText(1, this->tempHumid, PA_LEFT, SCROLL_SPEED, 1000, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        this->parola.displayReset(1);
        Serial.println("Resetting Zone 1");
      }
    }
  }
};
