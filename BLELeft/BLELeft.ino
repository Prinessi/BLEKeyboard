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
  {KEY_ESC, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, 0},
  {'`', '1', '2', '3', '4', '5', '-', 0},
  {KEY_TAB, 'q', 'w', 'e', 'r', 't', '[', 0},
  {KEY_LEFT_CTRL, 'a', 's', 'd', 'f', 'g', 0, 0},
  {KEY_LEFT_SHIFT, 'z', 'x', 'c', 'v', 'b', 0, 0},
  {KEY_LEFT_GUI, KEY_LEFT_ALT, KEY_PRINT_SCREEN, KEY_DELETE, KEY_F13, KEY_MEDIA_MUTE, KEY_MEDIA_VOLUME_DOWN, KEY_MEDIA_VOLUME_UP},
  {0, 0, 0, 0, 0, ' ', KEY_RETURN, KEY_CAPS_LOCK}
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
