void checkInputs() {
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
}


void outputConfig() {
  // Output selected Buttons:
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
}

