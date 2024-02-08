#include <FastLED.h>
#include <ArduinoJson.h>

#define SEC_0 24
#define SEC_1 9
#define NUM_LEDS 33

CRGB main_leds[SEC_0];
int main_mode = 0;
CRGB main_color_0;
CRGB main_color_1;
int main_brightness = 255;
bool enable_main = false;
CRGB front_leds[SEC_1];
int front_mode = 0;
CRGB front_color_0;
CRGB front_color_1;
int front_brightness = 255;
bool enable_front = false;

CRGB leds[NUM_LEDS];

const int SERIAL_BUFFER_SIZE = 128;
char serialBuffer[SERIAL_BUFFER_SIZE];
bool newDataAvailable = false;

void setup() {
  Serial.begin(9600);
  main_color_0.setRGB(0, 0, 0);
  main_color_1.setRGB(0, 0, 0);
  front_color_0.setRGB(0, 0, 0);
  front_color_1.setRGB(0, 0, 0);
  FastLED.addLeds<WS2812, 3, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);;
  FastLED.setBrightness(255);
}


void loop() {
  uint8_t thisSpeed = 30;
  uint8_t deltaHue= 10;
  uint8_t thisHue = beat8(thisSpeed, 255); 

  if (main_mode == 0) {
    fill_solid(main_leds, SEC_0, main_color_0);
  } else if (main_mode == 1) {
    fill_gradient_RGB(main_leds, SEC_0, main_color_0, main_color_1);
  } else if (main_mode == 5) {
    fill_rainbow(main_leds, SEC_0, thisHue, deltaHue);
  }
  if (front_mode == 0) {
    fill_solid(front_leds, SEC_1, front_color_0);
  } else if (front_mode == 1) {
    fill_gradient_RGB(front_leds, SEC_1, front_color_0, front_color_1);
  } else if (front_mode == 5) {
    fill_rainbow(front_leds, SEC_1, thisHue, deltaHue);
  }
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i < SEC_0) {
      if (enable_main) {
        leds[i] = main_leds[i];
        leds[i].fadeToBlackBy(map(main_brightness, 0, 100, 255, 0));
      } else {
        leds[i] = CRGB::Black;
      }
    } else {
      if (enable_front) {
        leds[i] = front_leds[i - SEC_0];
        leds[i].fadeToBlackBy(map(front_brightness, 0, 100, 255, 0));
      } else {
        leds[i] = CRGB::Black;
      }
    }
  }
  FastLED.show();

  if (Serial.available() > 0) {
    // Read the incoming data until a newline character is received
    Serial.readBytesUntil('\n', serialBuffer, SERIAL_BUFFER_SIZE);
    newDataAvailable = true;
  }

  if (newDataAvailable) {
    // Parse the received JSON
    StaticJsonDocument<200> doc; // Adjust the size as necessary
    DeserializationError error = deserializeJson(doc, serialBuffer);
    
    // Check for parsing errors
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    
    if (doc.containsKey("main")) {
      if (doc["main"].containsKey("enable")) {
        enable_main = doc["main"]["enable"];
      } else {
        enable_main = false;
      }
      if (doc["main"].containsKey("brightness")) {
        main_brightness = doc["main"]["brightness"];
      }
      if (doc["main"].containsKey("mode")) {
        if (doc["main"]["mode"] == 0) {
          if (doc["main"]["color"].size() == 0) {
            main_mode = 0;
            main_color_0 = CRGB::Black;
            main_color_1 = CRGB::Black;
          } else if (doc["main"]["color"].size() == 1) {
            main_mode = 0;
            main_color_0.setRGB(doc["main"]["color"][0][0], doc["main"]["color"][0][1], doc["main"]["color"][0][2]);
          } else if (doc["main"]["color"].size() == 2) {
            main_mode = 1;
            main_color_0.setRGB(doc["main"]["color"][0][0], doc["main"]["color"][0][1], doc["main"]["color"][0][2]);
            main_color_0.setRGB(doc["main"]["color"][1][0], doc["main"]["color"][1][1], doc["main"]["color"][1][2]);
          }
        } else if (doc["main"]["mode"] == 1) {
          main_mode = 5;
        }
      }
    }
    if (doc.containsKey("front")) {
      if (doc["front"].containsKey("enable")) {
        enable_front = doc["front"]["enable"];
      } else {
        enable_front = false;
      }
      if (doc["front"].containsKey("brightness")) {
        front_brightness = doc["front"]["brightness"];
      }
      if (doc["front"].containsKey("mode")) {
        if (doc["front"]["mode"] == 0) {
          if (doc["front"]["color"].size() == 0) {
            front_mode = 0;
            front_color_0 = CRGB::Black;
            front_color_1 = CRGB::Black;
          } else if (doc["front"]["color"].size() == 1) {
            front_mode = 0;
            front_color_0.setRGB(doc["front"]["color"][0][0], doc["front"]["color"][0][1], doc["front"]["color"][0][2]);
          } else if (doc["front"]["color"].size() == 2) {
            front_mode = 1;
            front_color_0.setRGB(doc["front"]["color"][0][0], doc["front"]["color"][0][1], doc["front"]["color"][0][2]);
            front_color_0.setRGB(doc["front"]["color"][1][0], doc["front"]["color"][1][1], doc["front"]["color"][1][2]);
          }
        } else if (doc["front"]["mode"] == 1) {
          front_mode = 5;
        }
      }
    }
  }
}
