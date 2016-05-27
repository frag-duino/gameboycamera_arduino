boolean checkInputs() { // Returns true if a photo button is pressed

  // First check the pushbutton:
  if (digitalRead(pin_pushButton) == LOW) {
    save_photo = true;
    return true;
  }


  // get the state from the buttons
  state_button = nintendo.buttons();

  if (state_button & NES_A || state_button & NES_B) {
    save_photo = true;
    return true;

    // Check for enhanced mode:
    if (state_button & NES_SELECT && state_button & NES_START && state_button & NES_A && state_button & NES_B) {
      enable_enhanced_mode = true;
      Serial.println("OK");
      return false;
    }

    // Switch Select (unused)
    if (enable_enhanced_mode && state_button & NES_SELECT) {
      if (state_button & NES_START) // 1
        ;
      else if (state_button & NES_A) // 2
        ;
      else if (state_button & NES_B) // 3
        ;
      return false;
    }

    // Change the values:
    if (enable_enhanced_mode && state_button & NES_START) {
      if (state_button & NES_A)
        set_mode = MODE_REGULAR;
      if (state_button & NES_B)
        set_mode = MODE_TEST;
      return false;
    }

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

    if (state_button & NES_SELECT && state_button & NES_START)
      return false;

  }
}
