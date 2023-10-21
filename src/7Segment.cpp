#include "7Segment.h"

SegmentDSP::SegmentDSP(uint8_t dspCount, const uint8_t* muxPins, const uint8_t* dspPins)
{
  //Save the dspCount
  this->dspCount = dspCount;

  //Reset our current/last display
  this->currentDsp = 0;
  this->lastDsp = 0;

  //Save the pointers to our pin lists
  // - We assume they are valid
  this->muxPins = muxPins;
  this->dspPins = dspPins;

  //Allocate our mux output list
  int listSize = dspCount * DSP_SEGMENTS;
  this->muxPinsOut = new uint8_t[listSize];

  //Zero the output list
  // - The displays are inverted, so set them all to '1'
  for (int i = 0; i < listSize; i++)
  {
    this->muxPinsOut[i] = 1;
  }
}

SegmentDSP::~SegmentDSP()
{
  //Clean up our mux outputs
  delete[] muxPinsOut;
}

void SegmentDSP::setGlyph(uint8_t dspNumber, uint8_t glyphNumber)
{
  //Make sure the dsp number is valid
  if (dspNumber < dspCount)
  {
    //Get the starting position in our mux outputs
    int start = dspNumber * DSP_SEGMENTS;

    //Set the data
    for (uint8_t i = 0; i < GLYPH_SIZE; i++)
    {
      muxPinsOut[start + i] = hexNumbers[glyphNumber][i];
    }
  }
}

//Set special
// - TODO

void SegmentDSP::setHex(uint8_t startDsp, uint8_t hexValue)
{
  //Split up our hex
  uint8_t hexBottom = hexValue & 0xf;
  uint8_t hexTop = hexValue >> 4;

  //Set the mux pins
  setGlyph(startDsp, hexBottom);
  setGlyph(startDsp + 1, hexTop);
}

void SegmentDSP::setDP(uint8_t dspNumber, bool dp)
{
  //Get the mux output location
  int index = (dspNumber * DSP_SEGMENTS) + GLYPH_SIZE;

  //Set it
  muxPinsOut[index] = !dp;
}

void SegmentDSP::setAllDP(bool dp)
{
  for (int i = 0; i < dspCount; i++)
  {
    setDP(i, dp);
  }
}

void SegmentDSP::dspUpdate()
{
  //Turn off the last display
  digitalWrite(dspPins[lastDsp], LOW);

  //Update the mux output with the next display
  int start = currentDsp * DSP_SEGMENTS;
  for (int i = 0; i < DSP_SEGMENTS; i ++) {
    digitalWrite(muxPins[i], muxPinsOut[start + i]);
  }

  //Activate the next display
  digitalWrite(dspPins[currentDsp], HIGH);

  //Move our tracking variables forward
  lastDsp = currentDsp;
  currentDsp++;
  if (currentDsp >= dspCount) {
    currentDsp = 0;
  }
}