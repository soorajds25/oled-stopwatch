#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Button
const int buttonPin = 2;

int buttonState = LOW;
int lastButtonState = LOW;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

unsigned long pressStartTime = 0;
const unsigned long longPressTime = 1000;

bool longPressTriggered = false;

// Stopwatch variables
bool running = false;
unsigned long startTime = 0;
unsigned long elapsedTime = 0;

void setup() {
  pinMode(buttonPin, INPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
}

void loop() {
  int reading = digitalRead(buttonPin);

  // Debounce
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {

    if (reading != buttonState) {
      buttonState = reading;

      // Button pressed
      if (buttonState == HIGH) {
        pressStartTime = millis();
        longPressTriggered = false;
      }

      // Button released
      if (buttonState == LOW) {
        if (!longPressTriggered) {
          // SHORT PRESS → start/pause
          running = !running;

          if (running) {
            startTime = millis() - elapsedTime;
          } else {
            elapsedTime = millis() - startTime;
          }
        }
      }
    }

    // LONG PRESS (while holding)
    if (buttonState == HIGH && !longPressTriggered) {
      if (millis() - pressStartTime >= longPressTime) {
        // RESET
        running = false;
        elapsedTime = 0;
        longPressTriggered = true;
      }
    }
  }

  // Update elapsed time
  if (running) {
    elapsedTime = millis() - startTime;
  }

  // Display
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);

  int seconds = (elapsedTime / 1000) % 60;
  int minutes = (elapsedTime / 60000);

  display.setCursor(20, 25);
  if (minutes < 10) display.print("0");
  display.print(minutes);
  display.print(":");
  if (seconds < 10) display.print("0");
  display.print(seconds);

  display.display();

  lastButtonState = reading;
}