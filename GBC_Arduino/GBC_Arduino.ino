/*
    GameBoy Camera Arduino
    www.frag-duino.de
*/

// Imports for the display and the controller:
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
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

const char COMMAND_TAKEPHOTO = 'T';
const char COMMAND_SETCONFIG = 'S';
const char COMMAND_COLORDEPTH = 'D';
const char COMMAND_RESOLUTION = 'R';
const char COMMAND_MODE = 'F';

// Pins
const int pin_vout = A3;
const int pin_read = 8;
const int pin_xck = 9;
const int pin_reset = 10;
const int pin_load = 11;
const int pin_sin = 12;
const int pin_start = 13;
// Hardware  SPI for OLED-Display:
const int OLED_RESET = 5; // > RES
const int OLED_DC = 6;    // > DC
const int OLED_CS = 7;    // > CS
// Hardware Mega MOSI: 51 > D1
// Hardware Mega SCK:  52 > D0
// Hardware Uno MOSI: 11 > D1
// Hardware Uno MISO: 12 > unused
// Hardware Uno SCK:  13 > D0

// Constants
const int COLORDEPTH_8BIT = 8;
const int COLORDEPTH_2BIT = 2;
const int RESOLUTION_128PX = 1;
const int RESOLUTION_32PX = 3;
const int MODE_REGULAR = 0;
const int MODE_TEST = 1;
const int offset_row = 16;
const int offset_column = 96;
const unsigned long INTERVAL_READY = 500;

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
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);
NESpad nintendo = NESpad(A4, A5, A6);
byte outBuffer[128];
byte temp;
char c;
String input, tempString;
boolean take_photo = false;
byte state_button;

void setConfig() {
  // Print the current settings
  //   Serial.println("Settings:");
  // Serial.print("Mode=");
  // Serial.print(set_mode);
  // Serial.print(" set_colordepth=");
  // Serial.print(set_colordepth);
  // Serial.print(" set_resolution=");
  // Serial.print(set_resolution);
  // Serial.print(" Gain=");
  // Serial.print(set_gain);
  // Serial.print(" VH=");
  // Serial.print(set_vh);
  // Serial.print(" N=");
  // Serial.print(set_n);
  // Serial.print(" C1=");
  // Serial.print(set_c1);
  // Serial.print(" C0=");
  // Serial.print(set_c0);
  // Serial.print(" P=");
  // Serial.print(set_p);
  // Serial.print(" M=");
  // Serial.print(set_m);
  // Serial.print(" X=");
  // Serial.print(set_x);
  // Serial.print(" Vref=");
  // Serial.print(set_vref);
  // Serial.print(" I=");
  // Serial.print(set_i);
  // Serial.print(" Edge=");
  // Serial.print(set_edge);
  // Serial.print(" Offset=");
  // Serial.print(set_offset);
  // Serial.print(" Z=");
  // Serial.println(set_z);

  reg1 = set_gain | (set_vh << 5) | (set_n << 7);
  reg2 = set_c1;
  reg3 = set_c0;
  reg4 = set_p;
  reg5 = set_m;
  reg6 = set_x;
  reg7 = set_vref | (set_i << 3) | (set_edge << 4);
  reg0 = set_offset | (set_z << 6);

  // Registers:
  // Serial.print("Reg1: ");
  // Serial.println(reg1, BIN);
  // Serial.print("Reg2: ");
  // Serial.println(reg2, BIN);
  // Serial.print("Reg3: ");
  // Serial.println(reg3, BIN);
  // Serial.print("Reg4: ");
  // Serial.println(reg4, BIN);
  // Serial.print("Reg5: ");
  // Serial.println(reg5, BIN);
  // Serial.print("Reg6: ");
  // Serial.println(reg6, BIN);
  // Serial.print("Reg7: ");
  // Serial.println(reg7, BIN);
  // Serial.print("Reg0: ");
  // Serial.println(reg0, BIN);
}

void setup() {
  // Initialize Serial
  // Serial.begin(9600);
  Serial.begin(115200);
  // Serial.begin(250000);
  delay(1000);
  setConfig(); // Set the config
  randomSeed(analogRead(0));

  // initialize pins:
  pinMode(pin_read, INPUT);
  pinMode(pin_xck, OUTPUT);
  pinMode(pin_reset, OUTPUT);
  pinMode(pin_load, OUTPUT);
  pinMode(pin_sin, OUTPUT);
  pinMode(pin_start, OUTPUT);

  // Initialize display
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Hello Gameboycamera!");
  display.display();

  Serial.println("Initialized");
}

void loop() {

  if (Serial.available() > 0) {
    c = Serial.read();
    input += c;
    if (c == '\n') {
      // Serial.print("Received setting: ");
      // Serial.print(input[0]);
      // Serial.print('=');
      // Serial.print(input[1], BIN);
      // Serial.print(" (");
      // Serial.print(input[1], DEC);
      // Serial.println(")");

      if (input[0] == TYPE_GAIN)
        set_gain = input[1];
      else if (input[0] == TYPE_VH)
        set_vh = input[1];
      else if (input[0] == TYPE_N)
        set_n = input[1];
      else if (input[0] == TYPE_C0)
        set_c0 = input[1];
      else if (input[0] == TYPE_C1)
        set_c1 = input[1];
      else if (input[0] == TYPE_P)
        set_p = input[1];
      else if (input[0] == TYPE_M)
        set_m = input[1];
      else if (input[0] == TYPE_X)
        set_x = input[1];
      else if (input[0] == TYPE_VREF)
        set_vref = input[1];
      else if (input[0] == TYPE_I)
        set_i = input[1];
      else if (input[0] == TYPE_EDGE)
        set_edge = input[1];
      else if (input[0] == TYPE_OFFSET)
        set_offset = input[1];
      else if (input[0] == TYPE_Z)
        set_z = input[1];
      else if (input[0] == COMMAND_MODE)
        set_mode = input[1];
      else if (input[0] == COMMAND_COLORDEPTH)
        set_colordepth = input[1];
      else if (input[0] == COMMAND_RESOLUTION)
        set_resolution = input[1];
      else if (input[0] == COMMAND_TAKEPHOTO)
        take_photo = true;
      else if (input[0] == COMMAND_SETCONFIG)
        setConfig();
      else
        Serial.print("Unknown");

      input = "";
    }
  }

  if (!take_photo) {
    display.clearDisplay();

    // Reset camera
    // Serial.println("Reset camera:");
    digitalWrite(pin_reset, LOW); // RESET -> Low
    xck();
    xck();
    digitalWrite(pin_reset, HIGH); // RESET -> High
    xck();

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
    xck();
    digitalWrite(pin_start, HIGH); // Start High -> Kamerastart
    digitalWrite(pin_xck, HIGH);
    digitalWrite(pin_start, LOW); // Start L
    xck();

    // Wait until the images comes:
    // Serial.println("Waiting for image:");
    while (digitalRead(pin_read) == LOW)  // READ signal
      xck();

    Serial.println("!IMAGE!");

    if (set_colordepth == COLORDEPTH_2BIT && set_resolution == RESOLUTION_128PX) { // 2Bit, 128x128
      for (int row = 0; row < 128; row++) {
        for (int column = 0; column < 32; column++) {
          for (int pixel = 0; pixel < 4; pixel++) {

            if (set_mode == MODE_REGULAR) // Regular mode, read voltage
              temp = analogRead(pin_vout) / 4;
            else // Testmode
              temp = random(256);

            if (pixel == 0)
              outBuffer[column] = temp;
            else
              outBuffer[column] = (outBuffer[column] << 2) | temp;

            // Print it 1:4 on the display:
            if (row % 4 == 0 && pixel == 0) // Every fourth row/pixel
            {
              if (temp > 127)
                display.drawPixel(column + offset_column, row / 4 + offset_row, WHITE);
              else
                display.drawPixel(column + offset_column, row / 4 + offset_row , BLACK);
            }

            // Clock to get the next one:
            digitalWrite(pin_xck, LOW);
            digitalWrite(pin_xck, HIGH);
          }
        }

        // Send complete row:
        if (take_photo)
          Serial.write(outBuffer, 32); // Send complete buffer
      }
    } else if (set_colordepth == COLORDEPTH_8BIT && set_resolution == RESOLUTION_32PX) { // 8Bit, 32x32
      for (int row = 0; row < 32; row++) {
        for (int column = 0; column < 32; column++) {
          if (set_mode == MODE_REGULAR) // Regular mode, read voltage
            outBuffer[column] = analogRead(pin_vout) / 4; // 0-1024 --> 0-255
          else // Testmode
            outBuffer[column] = random(256);

          // Print it 1:1 on the display:
          if (outBuffer[column] > 127)
            display.drawPixel(column + offset_column, row + offset_row, WHITE);
          else
            display.drawPixel(column + offset_column, row + offset_row , BLACK);

          // Clock 4 times to get the fourth next one:
          digitalWrite(pin_xck, LOW);
          digitalWrite(pin_xck, HIGH);
          digitalWrite(pin_xck, LOW);
          digitalWrite(pin_xck, HIGH);
          digitalWrite(pin_xck, LOW);
          digitalWrite(pin_xck, HIGH);
          digitalWrite(pin_xck, LOW);
          digitalWrite(pin_xck, HIGH);
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
            outBuffer[column] = random(256);

          // Print it 1:4 on the display:
          if (row % 4 == 0 && column % 4 == 0) // Every fourth row/column
          {
            if (outBuffer[column] > 127)
              display.drawPixel(column / 4 + offset_column, row / 4 + offset_row, WHITE);
            else
              display.drawPixel(column / 4 + offset_column, row / 4 + offset_row , BLACK);
          }

          // Clock to get the next one:
          digitalWrite(pin_xck, LOW);
          digitalWrite(pin_xck, HIGH);
        }

        // Send row:
        if (take_photo)
          Serial.write(outBuffer, 128); // Send complete buffer
      }
    }

    // get the state from the buttons
    state_button = nintendo.buttons();

    // Change the values:
    if (state_button & NES_SELECT)
      if (set_mode == MODE_TEST )
        set_mode = MODE_REGULAR;
      else
        set_mode = MODE_TEST;

    // Switch resolution/colordepth:
    if (state_button & NES_START)
      if (set_colordepth == COLORDEPTH_2BIT && set_resolution == RESOLUTION_128PX) { // 1
        set_colordepth = COLORDEPTH_8BIT;
        set_resolution = RESOLUTION_32PX;
      } else if (set_colordepth == COLORDEPTH_8BIT && set_resolution == RESOLUTION_32PX) { // 2
        set_colordepth = COLORDEPTH_8BIT;
        set_resolution = RESOLUTION_128PX;
      } else if (set_colordepth == COLORDEPTH_8BIT && set_resolution == RESOLUTION_128PX) { // 3
        set_colordepth = COLORDEPTH_2BIT;
        set_resolution = RESOLUTION_128PX;
      }

    // Up/Down: C1
    if (state_button & NES_UP)
      set_c1++;
    if (state_button & NES_DOWN)
      set_c1--;

    // Left/Right: Gain
    if (state_button & NES_LEFT)
      set_gain--;
    if (state_button & NES_RIGHT)
      set_gain++;

    // Output Selected:
    display.setCursor(0, 0);
    tempString = "Buttons: ";
    if (state_button & NES_A)
      tempString += "A";
    if (state_button & NES_B)
      tempString += "B";
    if (state_button & NES_SELECT)
      tempString += "Se";
    if (state_button & NES_START)
      tempString += "St";
    if (state_button & NES_UP)
      tempString += "U";
    if (state_button & NES_DOWN)
      tempString += "D";
    if (state_button & NES_LEFT)
      tempString += "L";
    if (state_button & NES_RIGHT)
      tempString += "R";
    display.println(tempString);

    display.setCursor(0, 16);
    tempString = "Mode:";
    if (set_mode == MODE_REGULAR)
      tempString += "Reg";
    else
      tempString += "Test";
    tempString += "\r\nColor:";
    if (set_colordepth == COLORDEPTH_2BIT)
      tempString += "2Bit";
    else
      tempString += "8Bit";
    tempString += "\r\nRes:";
    if (set_resolution == RESOLUTION_128PX)
      tempString += "128x128";
    else
      tempString += "32x32";
    tempString += "\r\nC0:";
    tempString += set_c0;
    tempString += "\r\nC1:";
    tempString += set_c1;
    tempString += "\r\nGain:";
    tempString += set_gain;
    display.println(tempString);
    display.display();

    setConfig();

    // Send end-bytes
    if (take_photo) {
      Serial.print(B00110011);
      Serial.print(B00110011);
      Serial.print(B00110011);
      Serial.println("!END!");
      Serial.println("Next");
      take_photo = false;
    }
  }
}

// Sets the registers to the camera:
void setReg( unsigned char adr, unsigned char data )
{
  // Adresse Transfer (3bit)
  if ((adr & 0x04) == 0x04)
    digitalWrite(pin_sin, HIGH);
  else
    digitalWrite(pin_sin, LOW);

  xck();

  if ((adr & 0x02) == 0x02)
    digitalWrite(pin_sin, HIGH);
  else
    digitalWrite(pin_sin, LOW);

  xck();

  if ((adr & 0x01) == 0x01)
    digitalWrite(pin_sin, HIGH);
  else
    digitalWrite(pin_sin, LOW);

  xck();

  // Datenübertragung (8bit)
  if ((data & 0x80) == 0x80)
    digitalWrite(pin_sin, HIGH);
  else
    digitalWrite(pin_sin, LOW);

  xck();

  if ((data & 0x40) == 0x40)
    digitalWrite(pin_sin, HIGH);
  else
    digitalWrite(pin_sin, LOW);

  xck();

  if ((data & 0x20) == 0x20)
    digitalWrite(pin_sin, HIGH);
  else
    digitalWrite(pin_sin, LOW);

  xck();

  if ((data & 0x10) == 0x10)
    digitalWrite(pin_sin, HIGH);
  else
    digitalWrite(pin_sin, LOW);

  xck();

  if ((data & 0x08) == 0x08)
    digitalWrite(pin_sin, HIGH);
  else
    digitalWrite(pin_sin, LOW);

  xck();

  if ((data & 0x04) == 0x04)
    digitalWrite(pin_sin, HIGH);
  else
    digitalWrite(pin_sin, LOW);

  xck();

  if ((data & 0x02) == 0x02)
    digitalWrite(pin_sin, HIGH);
  else
    digitalWrite(pin_sin, LOW);

  xck();

  if ((data & 0x01) == 0x01)
    digitalWrite(pin_sin, HIGH);
  else
    digitalWrite(pin_sin, LOW);

  // Confirm load:
  digitalWrite(pin_xck, HIGH);
  digitalWrite(pin_load, HIGH);
  digitalWrite(pin_sin, LOW);
  digitalWrite(pin_xck, LOW);
  digitalWrite(pin_load, LOW);
}

// Send some clocks:
void xck() {
  digitalWrite(pin_xck, HIGH);
  digitalWrite(pin_xck, LOW);
}
