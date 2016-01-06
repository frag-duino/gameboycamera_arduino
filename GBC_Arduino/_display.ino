// Draws the buffer on the display
void drawBuffer() {
  if (!ENABLE_DISPLAY)
    graphicPointer = 0;

  if (graphicPointer == size_graphicbuffer) {
    graphicPointer = 0;
    // Print the row on the screen (TODO: Optimization by higher buffer)
    tft.begin();
    for (graphicPointer = 0; graphicPointer < size_graphicbuffer; graphicPointer++)
      tft.drawPixelFAST(tft.Color565(graphicBuffer[graphicPointer], graphicBuffer[graphicPointer], graphicBuffer[graphicPointer]));
    tft.commit(); // Commit every row!

    graphicPointer = 0;
  }
}


// Returns a pseudo-random value
int getNextValue() {
  randomValue++;
  if (randomValue == 256)
    randomValue = random(256); // get one random value for test mode
  return randomValue;
}

