void checkInputs() {
  // get the state from the buttons
  state_button = nintendo.buttons();

  // Check for enhanced mode:
  if (state_button & NES_SELECT && state_button & NES_START && state_button & NES_A && state_button & NES_B)
    enable_enhanced_mode = true;

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
  if (state_button & NES_UP)
    set_c1++;
  if (state_button & NES_DOWN)
    set_c1--;
  if (set_c1 > 256) // Byte is 2 Bytes length!
    set_c1 = 0;
  if (set_c1 == 256)
    set_c1 = 255;

  // Left/Right: Gain
  if (state_button & NES_LEFT)
    set_gain--;
  if (state_button & NES_RIGHT)
    set_gain++;
  if (set_gain > 256)
    set_gain = 0;
  if (set_gain == 256)
    set_gain = 255;
}

void outputConfig() {
  checkInputs();
  // Clear the values by drawing a rectangle over them:
  //display.fillRect(0, 0, display.width(), 16, 0);
  //display.fillRect(0, 16, 95, display.height(), 0);

  // Output selected Buttons:
  //display.setCursor(0, 0);
  tft.setCursor(0, 30);
  tft.setTextColor(ST7735_RED);
  tft.setTextSize(1);
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
  tft.print(tempString);
  //display.println(tempString);

  //display.setCursor(0, 16);
  tft.setCursor(0, 30);
  tft.setTextColor(ST7735_RED);
  tft.setTextSize(1);
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
  tft.print(tempString);
  // display.display();
}

