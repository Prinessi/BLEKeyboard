#include <BleKeyboard.h>
#define KEY_PRINT_SCREEN 0x46
#define KEY_DELETE 0x4C
#define KEY_F13 0x68
#define KEY_MEDIA_MUTE 0x7F
#define KEY_MEDIA_VOLUME_DOWN 0x81
#define KEY_MEDIA_VOLUME_UP 0x80
#define KEY_CAPS_LOCK 0x39

BleKeyboard bleKeyboard("MyBLEKeyboard", "Nathan", 100);

const int ROWS = 7;
const int COLS = 8;

const int rowPins[ROWS] = {15, 2, 4, 16, 17, 5, 18};
const int colPins[COLS] = {13, 12, 14, 27, 26, 25, 33, 32};

uint8_t keymap[ROWS][COLS] = {
  {0, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, 0},
  {0, '=', '6', '7', '8', '9', '0', '\'},
  {0, 'y', 'u', 'i', 'o', 'p', 't',  KEY_BACKSPACE},
  {0, 'h', 'j', 'k', 'l', ';', ''', KEY_RETURN},
  {0, 0, 'n', 'm', ',', '.', '/', KEY_SHIFT},
  {KEY_MEDIA_PLAY, KEY_MEDIA_PREVIOUS, KEY_MEDIA_NEXT, KEY_ARROW_LEFT, KEY_ARROW_DOWN, KEY_ARROW_UP, KEY_ARROW_RIGHT, 0},
  {KEY_PAGE_DOWN, KEY_PAGE_UP, KEY_BACKSPACE ,0, 0, 0, 0, 0}
};


// const int ROWS = 2;
// const int COLS = 2;

// const int rowPins[ROWS] = {2, 3};
// const int colPins[COLS] = {4, 5};

// uint8_t keymap[ROWS][COLS] = {
//   {'a', 'b'},
//   {'c', 'd'}
// };

bool keyState[ROWS][COLS];
bool lastKeyState[ROWS][COLS];

void setup() {
  Serial.begin(115200);
  bleKeyboard.begin();

  // Setup rows
  for (int r = 0; r < ROWS; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], HIGH);
  }

  // Setup columns
  for (int c = 0; c < COLS; c++) {
    pinMode(colPins[c], INPUT_PULLUP);
  }
}

unsigned long lastKeyTime = 0;

void loop() {
  if (bleKeyboard.isConnected()) {
    unsigned long now = millis();
    if (now - lastKeyTime > 20) { // scan/send every 20ms
      scanMatrix();
      sendKeys();
      lastKeyTime = now;
    }
  }
}

void scanMatrix() {
  for (int r = 0; r < ROWS; r++) {
    digitalWrite(rowPins[r], LOW);
    for (int c = 0; c < COLS; c++) {
      keyState[r][c] = digitalRead(colPins[c]) == LOW;
    }
    digitalWrite(rowPins[r], HIGH);
  }
}

void sendKeys() {
  for (int r = 0; r < ROWS; r++) {
    for (int c = 0; c < COLS; c++) {
      if (keyState[r][c] && !lastKeyState[r][c]) {
        if (keymap[r][c] != 0) {
          bleKeyboard.press(keymap[r][c]);
        }
      } else if (!keyState[r][c] && lastKeyState[r][c]) {
        if (keymap[r][c] != 0) {
          bleKeyboard.release(keymap[r][c]);
        }
      }
      lastKeyState[r][c] = keyState[r][c];
    }
  }
}
