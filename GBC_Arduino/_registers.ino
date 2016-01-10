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

// Sets the registers to the camera:
void setReg( unsigned char adr, unsigned char data )
{
  // Adresse Transfer (3bit)
  if ((adr & 0x04) == 0x04)
    digitalWrite(pin_sin, HIGH);
  else
    digitalWrite(pin_sin, LOW);

  xckHIGHTtoLOW();

  if ((adr & 0x02) == 0x02)
    digitalWrite(pin_sin, HIGH);
  else
    digitalWrite(pin_sin, LOW);

  xckHIGHTtoLOW();

  if ((adr & 0x01) == 0x01)
    digitalWrite(pin_sin, HIGH);
  else
    digitalWrite(pin_sin, LOW);

  xckHIGHTtoLOW();

  // Datenübertragung (8bit)
  if ((data & 0x80) == 0x80)
    digitalWrite(pin_sin, HIGH);
  else
    digitalWrite(pin_sin, LOW);

  xckHIGHTtoLOW();

  if ((data & 0x40) == 0x40)
    digitalWrite(pin_sin, HIGH);
  else
    digitalWrite(pin_sin, LOW);

  xckHIGHTtoLOW();

  if ((data & 0x20) == 0x20)
    digitalWrite(pin_sin, HIGH);
  else
    digitalWrite(pin_sin, LOW);

  xckHIGHTtoLOW();

  if ((data & 0x10) == 0x10)
    digitalWrite(pin_sin, HIGH);
  else
    digitalWrite(pin_sin, LOW);

  xckHIGHTtoLOW();

  if ((data & 0x08) == 0x08)
    digitalWrite(pin_sin, HIGH);
  else
    digitalWrite(pin_sin, LOW);

  xckHIGHTtoLOW();

  if ((data & 0x04) == 0x04)
    digitalWrite(pin_sin, HIGH);
  else
    digitalWrite(pin_sin, LOW);

  xckHIGHTtoLOW();

  if ((data & 0x02) == 0x02)
    digitalWrite(pin_sin, HIGH);
  else
    digitalWrite(pin_sin, LOW);

  xckHIGHTtoLOW();

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
void xckHIGHTtoLOW() {
  // digitalWrite(pin_xck, HIGH);
  // digitalWrite(pin_xck, LOW);
  PORTH |= _BV(PH6);
  //camStepDelay();
  //camStepDelay();
  PORTH &= ~_BV(PH6);
  //camStepDelay();
  //camStepDelay();
}

void xckLOWtoHIGH() {
  // digitalWrite(pin_xck, LOW);
  // digitalWrite(pin_xck, HIGH);
  PORTH &= ~_BV(PH6);
  PORTH |= _BV(PH6);
  //camStepDelay();
  //camStepDelay();
}

// Delay used between each signal sent to the AR (four per xck cycle).
void camStepDelay() {
  unsigned char u = 0x07;
  while (u--) {
    __asm__ __volatile__ ("nop");
  }
}

