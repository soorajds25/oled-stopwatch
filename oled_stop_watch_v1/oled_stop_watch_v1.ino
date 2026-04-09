#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ====== BUTTON ======
const int buttonPin = 2;

int buttonState = LOW;
int lastButtonState = LOW;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

unsigned long pressStartTime = 0;
const unsigned long longPressTime = 1000;

bool longPressTriggered = false;

// ====== STOPWATCH ======
bool running = false;
unsigned long startTime = 0;
unsigned long elapsedTime = 0;

// ====== LAP STORAGE ======
#define MAX_LAPS 3
unsigned long laps[MAX_LAPS] = {0};
int lapIndex = 0;

// ====== SETUP ======
void setup() {
  pinMode(buttonPin, INPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(20, 25);
  display.print("READY");
  display.display();
  delay(1000);
}

// ====== LOOP ======
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

          if (running) {
            if (lapIndex < MAX_LAPS) {
              laps[lapIndex++] = elapsedTime;
            }
          }

          // Toggle start/pause
          running = !running;

          if (running) {
            startTime = millis() - elapsedTime;
          } else {
            elapsedTime = millis() - startTime;
          }
        }
      }
    }

    // LONG PRESS → RESET
    if (buttonState == HIGH && !longPressTriggered) {
      if (millis() - pressStartTime >= longPressTime) {
        running = false;
        elapsedTime = 0;
        lapIndex = 0;
        for (int i = 0; i < MAX_LAPS; i++) laps[i] = 0;
        longPressTriggered = true;
      }
    }
  }

  // Update time
  if (running) {
    elapsedTime = millis() - startTime;
  }

  // ===== DISPLAY =====
  display.clearDisplay();
  display.setTextColor(WHITE);

  int minutes = (elapsedTime / 60000);
  int seconds = (elapsedTime / 1000) % 60;
  int ms = elapsedTime % 1000;

  // Main time
  display.setTextSize(2);
  display.setCursor(10, 0);

  if (minutes < 10) display.print("0");
  display.print(minutes);
  display.print(":");

  if (seconds < 10) display.print("0");
  display.print(seconds);
  display.print(":");

  if (ms < 100) display.print("0");
  if (ms < 10) display.print("0");
  display.print(ms);

  // ===== LAPS =====
  display.setTextSize(1);
  for (int i = 0; i < lapIndex; i++) {

    int m = laps[i] / 60000;
    int s = (laps[i] / 1000) % 60;
    int lms = laps[i] % 1000;

    display.setCursor(0, 30 + i * 10);
    display.print("L");
    display.print(i + 1);
    display.print(": ");

    if (m < 10) display.print("0");
    display.print(m);
    display.print(":");

    if (s < 10) display.print("0");
    display.print(s);
    display.print(":");

    if (lms < 100) display.print("0");
    if (lms < 10) display.print("0");
    display.print(lms);
  }

  display.display();  //VERY IMPORTANT

  lastButtonState = reading;
}