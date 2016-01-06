// Draws the buffer on the display
void drawBuffer() {
  if (graphicPointer == 128) {
    graphicPointer = 0;
    // Print the row on the screen (TODO: Optimization by higher buffer)
    tft.begin();
    for (int column = 0; column < 128; column++)
      tft.drawPixelFAST(tft.Color565(graphicBuffer[column], graphicBuffer[column], graphicBuffer[column]));
    tft.commit(); // Commit every row!
  }
}


// Returns a pseudo-random value
int getNextValue() {
  randomValue++;
  if (randomValue == 256)
    randomValue = random(256); // get one random value for test mode
  return randomValue;
}

