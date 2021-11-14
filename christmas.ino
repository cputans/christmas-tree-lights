#include <FastLED.h>

#define LED_PIN 5
#define BUTTON_PIN 3
#define NUM_LEDS 80
#define BRIGHTNESS 64
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND 100
#define CHANGETIME 60
CRGB leds[NUM_LEDS];


boolean oSwitchState = LOW;
boolean nSwitchState = LOW;
boolean ledStatus = LOW;
int currentPaletteIndex = 0;

class SuperPalette {
    private:
        CRGBPalette16 palette;
        TBlendType blendType;
        uint8_t indexStepping;
        uint8_t delay;
        uint8_t motionSpeed;
    public:
        SuperPalette(CRGBPalette16 palette, TBlendType blendType, uint8_t indexStepping, uint8_t delay, uint8_t motionSpeed) {
            this->palette = palette;
            this->blendType = blendType;
            this->indexStepping = indexStepping;
            this->delay = delay;
            this->motionSpeed = motionSpeed;
        }

        CRGBPalette16 getPalette() {
            return this->palette;
        }

        TBlendType getBlendType() {
            return this->blendType;
        }

        uint8_t getIndexStepping() {
            return this->indexStepping;
        }

        uint8_t getDelay() {
            return this->delay;
        }

        uint8_t getMotionSpeed() {
            return this->motionSpeed;
        }
};


/* Custom palettes */
extern const TProgmemPalette16 redGreenWhiteChasing PROGMEM;
extern const TProgmemPalette16 redGreen PROGMEM;
extern const TProgmemPalette16 redGreenAlternating PROGMEM;

SuperPalette paletteArray[] = {
    SuperPalette(redGreenWhiteChasing, LINEARBLEND, 10, 10, 1), /* Red green white chasing */
    SuperPalette(redGreenAlternating, NOBLEND, 16, 0, 0), /* Red green alternating solid */
    SuperPalette(redGreen, LINEARBLEND, 1, 10, 1), /* All red or green alternating */
    SuperPalette(CRGBPalette16(CRGB::Red, CRGB::Green, CRGB::White, CRGB::Red), LINEARBLEND, 0, 50, 1) /* All Red green white blending */
};

void setup()
{
    delay(3000); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);

    Serial.begin(9600);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop()
{
    SuperPalette currentPalette = paletteArray[currentPaletteIndex];

    static uint8_t startIndex = 0;
    startIndex = startIndex + currentPalette.getMotionSpeed();
    FillLEDsFromPaletteColors(startIndex);

    nSwitchState = digitalRead(BUTTON_PIN);
    if(nSwitchState == LOW) {
        if((currentPaletteIndex+1) == (sizeof(paletteArray) / sizeof(paletteArray[0]))) {
            currentPaletteIndex = -1;
            Serial.println("TURNING OFF");
            FastLED.setBrightness(0);
        }
        else if(currentPaletteIndex == -1) {
            currentPaletteIndex = 0;
        }
        else {
            currentPaletteIndex++;
        }

        if(currentPaletteIndex >= 0) {
            CRGBPalette16 newPalette = paletteArray[currentPaletteIndex].getPalette();
            CRGBPalette16 oldPalette = currentPalette.getPalette();
            FastLED.setBrightness(BRIGHTNESS);
            Serial.println("ROTATING COLOR");
            uint8_t maxChanges = 24; 
            nblendPaletteTowardPalette(oldPalette, newPalette, maxChanges);
        }

        delay(250);
    }
}

void FillLEDsFromPaletteColors(uint8_t colorIndex)
{
    uint8_t brightness = 255;

    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = ColorFromPalette(paletteArray[currentPaletteIndex].getPalette(), colorIndex, brightness, paletteArray[currentPaletteIndex].getBlendType());
        colorIndex += paletteArray[currentPaletteIndex].getIndexStepping();
    }

    FastLED.show();
    FastLED.delay(paletteArray[currentPaletteIndex].getDelay());
}

const TProgmemPalette16 redGreenWhiteChasing PROGMEM =
    {
        CRGB::Red,
        CRGB::Red,
        CRGB::White,
        CRGB::Black,

        CRGB::Green,
        CRGB::Green,
        CRGB::White,
        CRGB::Black,

        CRGB::Red,
        CRGB::Red,
        CRGB::White,
        CRGB::Green,
        CRGB::Green,
        CRGB::White,
        CRGB::Black,
        CRGB::Black
    };

const TProgmemPalette16 redGreen PROGMEM =
    {
        CRGB::Green,
        CRGB::Green,
        CRGB::Green,
        CRGB::Green,

        CRGB::Green,
        CRGB::Green,
        CRGB::Green,
        CRGB::Green,

        CRGB::Red,
        CRGB::Red,
        CRGB::Red,
        CRGB::Red,
        CRGB::Red,
        CRGB::Red,
        CRGB::Red,
        CRGB::Red
    };

const TProgmemPalette16 redGreenAlternating PROGMEM =
    {
        CRGB::Red,
        CRGB::Green,
        CRGB::Red,
        CRGB::Green,
        CRGB::Red,
        CRGB::Green,
        CRGB::Red,
        CRGB::Green,
        CRGB::Red,
        CRGB::Green,
        CRGB::Red,
        CRGB::Green,
        CRGB::Red,
        CRGB::Green,
        CRGB::Red,
        CRGB::Green,   
    };