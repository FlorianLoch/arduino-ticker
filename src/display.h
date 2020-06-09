#include <MD_Parola.h>
#include <MD_MAX72xx.h>

#define MAX_DEVICES 12 // number of modules connected
#define CLK_PIN 14
#define DATA_PIN 13 // aka MOSI
#define CS_PIN 15   // aka SS

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

#define SCROLL_SPEED 30
#define PAUSE_TIME 0
#define LED_INTENSITY 0 // value between 0 (dark) and 15 (bright)

class Display
{
private:
  MD_Parola parola = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
  String *currentMessage;
  float temperature;
  float humidity;
  uint8_t displayIteration = 0;
  char tempHumid[20];

public:
  Display()
  {
    this->parola.begin();

    this->parola.setIntensity(LED_INTENSITY);

    this->parola.setSpeed(SCROLL_SPEED);
  };

  void showConnectingMessage()
  {
    this->parola.setTextEffect(PA_PRINT, PA_NO_EFFECT);
    this->setMessage("Connecting to WiFi...");
    // this->parola.displayReset();
    // this->parola.displayAnimate();
    this->parola.displayText(this->currentMessage->c_str(), PA_CENTER, SCROLL_SPEED, 2 * PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);
  }

  void showConnectedMessage(String ipAddress)
  {
    this->parola.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    this->setMessage("Connected! IP address: " + ipAddress);
    // this->parola.displayReset();
    // this->parola.displayAnimate();
    this->parola.displayText(this->currentMessage->c_str(), PA_LEFT, SCROLL_SPEED, 2 * PAUSE_TIME, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  }

  void setMessage(String currentMessage)
  {
    this->setMessage(new String(currentMessage));
  }

  void setMessage(String *currentMessage)
  {
    free(this->currentMessage);
    this->currentMessage = currentMessage;
  };

  void setTemperature(float temperature)
  {
    this->temperature = temperature;
  };

  void setHumidity(float humidity)
  {
    this->humidity = humidity;
  };

  void animate()
  {
    if (this->parola.displayAnimate())
    {
      if (this->displayIteration % 4 == 0)
      {
        sprintf(this->tempHumid, "%.2f *C | %.2f %%\0", this->temperature, this->humidity);
        this->parola.setTextBuffer(this->tempHumid);
      }
      else
      {
        this->parola.setTextBuffer(this->currentMessage->c_str());
      }

      this->parola.displayReset();

      this->displayIteration++;
    }
  }
};
