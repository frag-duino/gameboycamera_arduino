/*
    GameBoy Camera Arduino
    www.frag-duino.de
*/

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

// Constants
const int COLORDEPTH_8BIT = 8;
const int COLORDEPTH_2BIT = 2;
const int RESOLUTION_128PX = 1;
const int RESOLUTION_32PX = 3;
const int MODE_REGULAR = 0;
const int MODE_TEST = 1;

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
unsigned int set_c1   = 23; // C1=23
unsigned int set_c0   = 27; // C0=27
unsigned int set_p    = 1;  // P=1
unsigned int set_m    = 0;  // M=0
unsigned int set_x    = 1;  // X=1
unsigned int set_vref = 3;  // Vref=3
unsigned int set_i    = 0;  // I=0
unsigned int set_edge = 0;  // Edge=0
unsigned int set_offset  = 0;  // Offset=0V
unsigned int set_z    = 2;  // Z="10"

// Variables:
byte outBuffer[128];
byte data;
unsigned int temp;
char c;
String input;
boolean take_photo = false;
// unsigned long last_send_ready = 0;
// unsigned long currentMillis = 0;

void setConfig() {
  // Print the current settings
  Serial.println("Settings:");
  Serial.print("Mode=");
  Serial.print(set_mode);
  Serial.print(" set_colordepth=");
  Serial.print(set_colordepth);
  Serial.print(" set_resolution=");
  Serial.print(set_resolution);
  Serial.print(" Gain=");
  Serial.print(set_gain);
  Serial.print(" VH=");
  Serial.print(set_vh);
  Serial.print(" N=");
  Serial.print(set_n);
  Serial.print(" C1=");
  Serial.print(set_c1);
  Serial.print(" C0=");
  Serial.print(set_c0);
  Serial.print(" P=");
  Serial.print(set_p);
  Serial.print(" M=");
  Serial.print(set_m);
  Serial.print(" X=");
  Serial.print(set_x);
  Serial.print(" Vref=");
  Serial.print(set_vref);
  Serial.print(" I=");
  Serial.print(set_i);
  Serial.print(" Edge=");
  Serial.print(set_edge);
  Serial.print(" Offset=");
  Serial.print(set_offset);
  Serial.print(" Z=");
  Serial.println(set_z);

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
  // Serial.begin(115200);
  Serial.begin(9600);
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

  Serial.println("Initialized");
}

void loop() {
  // currentMillis = millis();

  // Send ready every x seconds
  //if ((!take_photo) && (last_send_ready + INTERVAL_READY < currentMillis)) {
  //  Serial.println("!READY!");
  //  last_send_ready = currentMillis;
  //}

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

  if (take_photo) {
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
          if (set_mode == MODE_TEST) {
            outBuffer[column] = random(256);
          } else {
            for (int pixel = 0; pixel < 4; pixel++) {
              temp = analogRead(pin_vout);

              if (temp < 256)
                data = B00000000;
              else if (temp < 512)
                data = B00000001;
              else if (temp < 768)
                data = B00000010;
              else
                data = B00000011;

              if (pixel == 0)
                outBuffer[column] = data;
              else
                outBuffer[column] = (outBuffer[column] << 2) | data;

              // Clock to get the next one:
              digitalWrite(pin_xck, LOW);
              digitalWrite(pin_xck, HIGH);
            }
          }
        }

        // Send complete row:
        Serial.write(outBuffer, 32); // Send complete buffer
      }
    } else if (set_colordepth == COLORDEPTH_8BIT && set_resolution == RESOLUTION_32PX) { // 8Bit, 32x32
      for (int row = 0; row < 32; row++) {
        for (int column = 0; column < 32; column++) {
          if (set_mode == MODE_TEST) {
            outBuffer[column] = random(256);
          }
          else
          {
            // Read current voltage
            data = analogRead(pin_vout) / 4; // 0-1024 --> 0-255
            outBuffer[column] = data;

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
        }

        // Send row:
        Serial.write(outBuffer, 32); // Send complete buffer
      }
    } else if (set_colordepth == COLORDEPTH_8BIT && set_resolution == RESOLUTION_128PX) { // 8Bit, 128x128
      for (int row = 0; row < 128; row++) {
        for (int column = 0; column < 128; column++) {
          if (set_mode == MODE_TEST) {
            outBuffer[column] = random(256);
          }
          else {
            // Read current voltage
            data = analogRead(pin_vout) / 4; // 0-1024 --> 0-255
            outBuffer[column] = data;

            // Clock to get the next one:
            digitalWrite(pin_xck, LOW);
            digitalWrite(pin_xck, HIGH);
          }
        }

        // Send row:
        Serial.write(outBuffer, 128); // Send complete buffer
      }
    }

    // Send end-bytes
    Serial.print(B00110011);
    Serial.print(B00110011);
    Serial.print(B00110011);
    Serial.println("!END!");
    Serial.println("Next");
    // last_send_ready = 0; // Force to send the next Ready
    take_photo = false;
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

