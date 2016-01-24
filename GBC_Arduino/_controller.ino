void checkInputs() {
  // get the state from the buttons
  state_button = nintendo.buttons();

  // Check for enhanced mode:
  if (state_button & NES_SELECT && state_button & NES_START && state_button & NES_A && state_button & NES_B){
    enable_enhanced_mode = true;
    Serial.println("OK");
    return;
  }

  // Switch resolution/colordepth:
  if (enable_enhanced_mode && state_button & NES_SELECT) {
    if (state_button & NES_START) { // 1
      set_colordepth = COLORDEPTH_8BIT;
      set_resolution = RESOLUTION_32PX;
    } else if (state_button & NES_A) { // 2
      set_colordepth = COLORDEPTH_8BIT;
      set_resolution = RESOLUTION_128PX;
    } else if (state_button & NES_B) { // 3
      set_colordepth = COLORDEPTH_2BIT;
      set_resolution = RESOLUTION_128PX;
    }
    return;
  }

  // Change the values:
  if (enable_enhanced_mode && state_button & NES_START) {
    if (state_button & NES_A)
      set_mode = MODE_REGULAR;
    if (state_button & NES_B)
      set_mode = MODE_TEST;
    return;
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
    return;

  if (state_button & NES_A)
    take_photo = true;

  if (state_button & NES_B)
    take_photo = true;
}
