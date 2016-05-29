void checkInputs() { // Returns true if a photo button is pressed

  // First check the pushbutton:
  if (digitalRead(pin_pushButton) == LOW) {
    save_photo = true;
    analogWrite(pin_led_save, BRIGHTNESS_LED);
    return;
  }

  // get the state from the controller buttons
  state_button = nintendo.buttons();

  if (state_button & NES_A || state_button & NES_B) {
    save_photo = true;
    analogWrite(pin_led_save, BRIGHTNESS_LED);
    return;
  }

  // Check for enhanced mode:
  if (state_button & NES_SELECT && state_button & NES_START) {
    enable_enhanced_mode = true;
    for (int i = 0; i < 10; i++) {
      digitalWrite(pin_led_save, HIGH);
      delay(50);
      digitalWrite(pin_led_save, LOW);
      delay(50);
    }
    return;
  }

  // Change the values:
  if (enable_enhanced_mode && state_button & NES_START)
    set_mode = MODE_REGULAR;

  if (enable_enhanced_mode && state_button & NES_SELECT)
    set_mode = MODE_TEST;

  // Up/Down: C1
  if (enable_enhanced_mode && state_button & NES_UP)
    set_c1++;
  if (enable_enhanced_mode && state_button & NES_DOWN)
    set_c1--;
  if (set_c1 > 256) // Byte is 2 Bytes length!
    set_c1 = 0;
  if (set_c1 == 256)
    set_c1 = 255;

  // Left/Right: Gain
  if (enable_enhanced_mode && state_button & NES_LEFT)
    set_gain--;
  if (enable_enhanced_mode && state_button & NES_RIGHT)
    set_gain++;
  if (set_gain > 256)
    set_gain = 0;
  if (set_gain == 256)
    set_gain = 255;
}
