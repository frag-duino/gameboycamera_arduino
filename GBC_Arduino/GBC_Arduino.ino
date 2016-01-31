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

const char COMMAND_COLORDEPTH = 'D';
const char COMMAND_RESOLUTION = 'R';
const char COMMAND_MODE = 'F';

// Byte paddles:
const byte BYTE_PHOTO_BEGIN_SHOW = B11001100; // 204
const byte BYTE_PHOTO_BEGIN_SHOW_SAVE = B11001110; // 206
const byte BYTE_PHOTO_END   = B00110011; // 51

// Pins
const int pin_vout = A3;
const int pin_start = 7;
const int pin_read = 8;
const int pin_xck = 9; // PH6 on Mega; PB1 on Uno
const int pin_reset = 10;
const int pin_load = 11;
const int pin_sin = 12;
const int pin_led_send = 5; // 100 Ohm + LED
const int pin_led_exposure = 13; // Build in
const int pin_analog_random = A4;
const int pin_nes_strobe = A1; // Yellow
const int pin_nes_clock = A0;  // Blue
const int pin_nes_data = A2;   // Brown
// Pin nes 5V: Red
// Pin nes GND: White

// Constants
const int COLORDEPTH_8BIT = 8;
const int COLORDEPTH_2BIT = 2;
const int RESOLUTION_128PX = 1;
const int RESOLUTION_32PX = 3;
const int MODE_REGULAR = 0;
const int MODE_TEST = 1;
const int offset_row = 0;
const int offset_column = 0;
const unsigned long INTERVAL_READY = 500;
const boolean MEASURE_TIME = false;
const boolean ENABLE_DISPLAY = false;
const int BRIGHTNESS_LED = 10;

/*
  Set the color depth:
  4  --> 8 Bit (0-255) maximum depth
  64 --> 2 Bit (0-3)  like a Gameboy
  Length:
  2Bit, 128px: 128*32 = 4096 Bytes
  8Bit, 128px: 128*128 = 16384 Bytes
  8Bit, 32px: 32*32 = 1024 Bytes
  2Bit, 32px: not supported
*/
int set_colordepth = COLORDEPTH_2BIT;
int set_resolution = RESOLUTION_128PX;
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
          else if (command == COMMAND_COLORDEPTH)
            set_colordepth = value;
          else if (command == COMMAND_RESOLUTION)
            set_resolution = value;
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

    if (MEASURE_TIME)
      start = millis();

    checkInputs();

    if (take_photo && !save_photo){ // Button A
      Serial.write(BYTE_PHOTO_BEGIN_SHOW);
      digitalWrite(pin_led_send, LOW);
    }
    else if (take_photo && save_photo) { // Button B
      analogWrite(pin_led_send, BRIGHTNESS_LED);
      Serial.write(BYTE_PHOTO_BEGIN_SHOW_SAVE);
    }

    if (set_colordepth == COLORDEPTH_2BIT && set_resolution == RESOLUTION_128PX) { // 2Bit, 128x128
      for (int row = 0; row < 128; row++) {
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
          if (outBuffer[column] == BYTE_PHOTO_BEGIN_SHOW || outBuffer[column] == BYTE_PHOTO_BEGIN_SHOW_SAVE || outBuffer[column] == BYTE_PHOTO_END)
            outBuffer[column]++;
        }

        // Send complete row to serial:
        if (take_photo)
          Serial.write(outBuffer, 32); // Send complete buffer
      }
    } else if (set_colordepth == COLORDEPTH_8BIT && set_resolution == RESOLUTION_32PX) { // 8Bit, 32x32
      for (int row = 0; row < 32; row++) {
        for (int column = 0; column < 32; column++) {

          if (set_mode == MODE_REGULAR) // Regular mode, read voltage
            outBuffer[column] = analogRead(pin_vout) / 4; // 0-1024 --> 0-255
          else // Testmode
            outBuffer[column] = getNextValue();

          // Prevent to be mistaken with end or beginning
          if (outBuffer[column] == BYTE_PHOTO_BEGIN_SHOW || outBuffer[column] == BYTE_PHOTO_BEGIN_SHOW_SAVE ||  outBuffer[column] == BYTE_PHOTO_END)
            outBuffer[column]++;

          for (int i = 0; i < 4; i++) // 4times=128/4
            xckLOWtoHIGH(); // Clock to get the next one
        }

        // Send row:
        if (take_photo)
          Serial.write(outBuffer, 32); // Send complete buffer
      }
    } else if (set_colordepth == COLORDEPTH_8BIT && set_resolution == RESOLUTION_128PX) { // 8Bit, 128x128
      for (int row = 0; row < 128; row++) {
        for (int column = 0; column < 128; column++) {

          if (set_mode == MODE_REGULAR) // Regular mode, read voltage
            outBuffer[column] = analogRead(pin_vout) / 4; // 0-1024 --> 0-255
          else // Test mode
            outBuffer[column] = getNextValue();

          // Prevent to be mistaken with end or beginning
          if (outBuffer[column] == BYTE_PHOTO_BEGIN_SHOW || outBuffer[column] == BYTE_PHOTO_BEGIN_SHOW_SAVE ||  outBuffer[column] == BYTE_PHOTO_END)
            outBuffer[column]++;

          xckLOWtoHIGH(); // Clock to get the next one
        }

        // Send row:
        if (take_photo)
          Serial.write(outBuffer, 128); // Send complete buffer
      }
    }

    setConfig();// Apply the current inputs to the config

    if (MEASURE_TIME)
      Serial.println(millis() - start);

    // Send end-bytes
    if (take_photo) {
      Serial.write(BYTE_PHOTO_END);
      digitalWrite(pin_led_send, LOW);
      take_photo = true;
      save_photo = false;
    }
  }
}
