#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define MAX_DEVICES 12 // Number of modules connected
#define CLK_PIN 14 //5     // SPI SCK pin on UNO
#define DATA_PIN 13    // SPI MOSI pin on UNO
#define CS_PIN 15      // connected to pin 10 on UNO

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

#define SCROLL_SPEED 25
#define PAUSE_TIME 1000

// Hardware SPI connection
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// sets scrolling direction if slider in middle at start
textEffect_t scrollEffect = PA_SCROLL_LEFT;

textPosition_t scrollAlign = PA_LEFT; // how to aligh the text

const char *message = "HALLO RICARDO!"; // used to hold current message

void setup()
{
  P.begin(); // Start Parola

  // configure Parola
  P.displayText(message, scrollAlign, SCROLL_SPEED, PAUSE_TIME, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  P.setTextEffect(PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  P.setSpeed(SCROLL_SPEED);
}

void loop()
{
  if (P.displayAnimate()) // If finished displaying message
  {
    P.displayReset(); // Reset and display it again
  }
}