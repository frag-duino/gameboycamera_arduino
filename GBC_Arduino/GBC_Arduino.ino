/*
    GameBoy Camera Arduino
    www.frag-duino.de
*/

// Imports for the display and the controller:
#include <NESpad.h>

// Protocol:
const char TYPE_GAIN = 'G';
const char TYPE_VH = 'H';
const char TYPE_N = 'N';
const char TYPE_C1 = '1';
const char TYPE_C0 = '0';
const char TYPE_P = 'P';
const char TYPE_M = 'M';
const char TYPE_X = 'X';
const char TYPE_VREF = 'V';
const char TYPE_I = 'I';
const char TYPE_EDGE = 'E';
const char TYPE_OFFSET = 'O';
const char TYPE_Z = 'Z';
const char COMMAND_MODE = 'F'; // Regular or Test-mode

// Byte paddles:
const byte BYTE_PHOTO_BEGIN = B11001100; // dec 204
const byte BYTE_PHOTO_END_SHOW   = B00110011; // dec 51
const byte BYTE_PHOTO_END_SAVE = B11001110; // dec 206

// Pins
const int pin_start = 2;
const int pin_sin = 3;
const int pin_load = 4;
const int pin_reset = 5;
const int pin_xck = 6;   // Clock Arduino Pin
#define CLOCK_PORT PORTD // Ping for port
#define CLOCK_PIN PD6    // manipulation
const int pin_read = 7;
const int pin_vout = A5;
const int pin_led_send = 11;    // 100 Ohm + LED
const int pin_led_exposure = 9; // 100 Ohm + LED
const int pin_pushButton = A4;  // Only needed if no NES controller available
const int pin_analog_random = A3; // Leave it empty
const int pin_nes_clock = A0;
const int pin_nes_strobe = A1;
const int pin_nes_data = A2;

// Constants
const int MODE_REGULAR = 0;
const int MODE_TEST = 1;
const int offset_row = 0;
const int offset_column = 0;
const unsigned long INTERVAL_READY = 500;
const int BRIGHTNESS_LED = 10;

// Set regular or test-mode
int set_mode = MODE_REGULAR;

// Registers from datasheet
byte reg1, reg2, reg3, reg4, reg5, reg6, reg7, reg0;

// Image settings:
unsigned int set_gain = 0;  // Gain=0
unsigned int set_vh   = 3;  // VH=3
unsigned int set_n    = 0;  // N=1
unsigned int set_c1   = 8; // C1=8
unsigned int set_c0   = 0; // C0=0
unsigned int set_p    = 1;  // P=1
unsigned int set_m    = 0;  // M=0
unsigned int set_x    = 1;  // X=1
unsigned int set_vref = 3;  // Vref=3
unsigned int set_i    = 0;  // I=0
unsigned int set_edge = 0;  // Edge=0
unsigned int set_offset  = 0;  // Offset=0V
unsigned int set_z    = 2;  // Z="10"

// Variables:
NESpad nintendo = NESpad(pin_nes_strobe, pin_nes_clock, pin_nes_data);
byte outBuffer[128];
byte temp;
char c;
String input, tempString;
boolean take_photo = true;
boolean save_photo = false;
boolean receiving_commands = false;
byte state_button;
boolean enable_enhanced_mode = false;
int randomValue = 0;
unsigned long start = 0; // For time-measurements
char command;
byte value;

void setup() {
  // Initialize Serial
  Serial.begin(115200);

  setConfig(); // Set the config
  randomSeed(analogRead(pin_analog_random));

  // initialize pins:
  pinMode(pin_read, INPUT);
  pinMode(pin_xck, OUTPUT);
  pinMode(pin_reset, OUTPUT);
  pinMode(pin_load, OUTPUT);
  pinMode(pin_sin, OUTPUT);
  pinMode(pin_start, OUTPUT);
  pinMode(pin_led_exposure, OUTPUT);
  pinMode(pin_led_send, OUTPUT);
  digitalWrite(pin_led_exposure, HIGH);
  analogWrite(pin_led_send, BRIGHTNESS_LED);
  pinMode(pin_pushButton, INPUT);
  digitalWrite(pin_pushButton, HIGH); // Pullup the input
  delay(500);

  // Initialize Prescaler for faster analog read:
  ADCSRA &= ~(bit (ADPS0) | bit (ADPS1) | bit (ADPS2)); // clear prescaler bits
  //ADCSRA |= bit (ADPS0);                               //   2
  ADCSRA |= bit (ADPS1);                               //   4
  //ADCSRA |= bit (ADPS0) | bit (ADPS1);                 //   8
  //ADCSRA |= bit (ADPS2);                               //  16
  //ADCSRA |= bit (ADPS0) | bit (ADPS2);                 //  32
  //ADCSRA |= bit (ADPS1) | bit (ADPS2);                 //  64
  //ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2);   // 128

  Serial.print("Initialized");
  digitalWrite(pin_led_exposure, LOW);
  digitalWrite(pin_led_send, LOW);
}

void loop() {

  if (Serial.available() > 0) {
    receiving_commands = true;
    c = Serial.read();
    input += c;
    if (c == '\n') {
      //Serial.print("Received setting: ");
      //Serial.print(input);
      //Serial.print( "(");
      //Serial.print(input.length());
      //Serial.print("): ");

      if (input.length() % 2 == 1) // not even
        for (int pointer = 0; pointer < input.length() - 1; pointer += 2) {
          command = input[pointer];
          value = input[pointer + 1];

          //Serial.print("Processing command: ");
          Serial.print(command);
          Serial.print("-");
          Serial.print(value);
          Serial.print(" ");

          if (command == TYPE_GAIN)
            set_gain = value;
          else if (command == TYPE_VH)
            set_vh = value;
          else if (command == TYPE_N)
            set_n = value;
          else if (command == TYPE_C0)
            set_c0 = value;
          else if (command == TYPE_C1)
            set_c1 = value;
          else if (command == TYPE_P)
            set_p = value;
          else if (command == TYPE_M)
            set_m = value;
          else if (command == TYPE_X)
            set_x = value;
          else if (command == TYPE_VREF)
            set_vref = value;
          else if (command == TYPE_I)
            set_i = value;
          else if (command == TYPE_EDGE)
            set_edge = value;
          else if (command == TYPE_OFFSET)
            set_offset = value;
          else if (command == TYPE_Z)
            set_z = value;
          else if (command == COMMAND_MODE)
            set_mode = value;
          else
            Serial.print("U");
        }
      input = "";
      receiving_commands = false;
      Serial.println("OK");
    }
  }

  if (!receiving_commands) { // Smile, taking a photo :)

    // Reset camera
    // Serial.println("Reset camera:");
    digitalWrite(pin_reset, LOW); // RESET -> Low
    xckHIGHTtoLOW();
    xckHIGHTtoLOW();
    digitalWrite(pin_reset, HIGH); // RESET -> High
    xckHIGHTtoLOW();

    // Set the registers:
    // Serial.println("Set the registers");
    setReg(1, reg1);
    setReg(2, reg2);
    setReg(3, reg3);
    setReg(4, reg4);
    setReg(5, reg5);
    setReg(6, reg6);
    setReg(7, reg7);
    setReg(0, reg0);

    // Starting the camera:
    // Serial.println("Starting camera");
    digitalWrite(pin_led_exposure, HIGH);
    xckHIGHTtoLOW();
    digitalWrite(pin_start, HIGH); // Start High -> Kamerastart
    digitalWrite(pin_xck, HIGH);
    digitalWrite(pin_start, LOW); // Start L
    xckHIGHTtoLOW();

    // Wait until the images comes:
    // Serial.println("Waiting for image:");
    while (digitalRead(pin_read) == LOW)  // READ signal
      xckHIGHTtoLOW();
    digitalWrite(pin_led_exposure, LOW);

    checkInputs();

    if (take_photo && !save_photo) {
      Serial.write(BYTE_PHOTO_BEGIN);
      digitalWrite(pin_led_send, LOW);
    }
    else if (take_photo && save_photo) { // Button pressed
      analogWrite(pin_led_send, BRIGHTNESS_LED);
      Serial.write(BYTE_PHOTO_BEGIN);
    }

    for (int row = 0; row < 112; row++) {  // 2Bit, 128x112
      for (int column = 0; column < 32; column++) {
        for (int pixel = 0; pixel < 4; pixel++) {

          if (set_mode == MODE_REGULAR) // Regular mode, read voltage
            temp = analogRead(pin_vout) / 4; // --> make it a 8 Bit value (0-1024 --> 0-255)
          else // Testmode
            temp = getNextValue(); // Returns a 8 Bit value (0-255)

          // Reduce bitness to 2 Bit (0-3)
          if (temp < 64) temp = 0;
          else if (temp < 128) temp = 1;
          else if (temp < 192) temp = 2;
          else temp = 3;

          // And put it in the output byte
          if (pixel == 0)
            outBuffer[column] = temp; // 000000xx
          else
            outBuffer[column] = (outBuffer[column] << 2) | temp; // 0000xxyy

          xckLOWtoHIGH(); // Clock to get the next pixel
        }

        // Prevent to be mistaken with end or beginning
        if (outBuffer[column] == BYTE_PHOTO_BEGIN || outBuffer[column] == BYTE_PHOTO_END_SHOW || outBuffer[column] == BYTE_PHOTO_END_SAVE)
          outBuffer[column]++;
        if (!save_photo)
          checkInputs();
      }

      // Send complete row to serial:
      if (take_photo)
        Serial.write(outBuffer, 32); // Send complete buffer
    }

    setConfig();// Apply the current inputs to the config

    // Send end-bytes
    if (take_photo) {
      if (save_photo)
        Serial.write(BYTE_PHOTO_END_SAVE);
      else
        Serial.write(BYTE_PHOTO_END_SHOW);

      digitalWrite(pin_led_send, LOW);
      take_photo = true;
      save_photo = false;
    }
  }
}
