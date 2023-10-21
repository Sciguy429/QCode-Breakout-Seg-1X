#pragma once

#include <Arduino.h>

//Glyphs
#include "7SegmentGlyphs.h"

//DSP segment count
// - 7 segments + 1 dp
#define DSP_SEGMENTS 8

class SegmentDSP
{
public:
  //Constructor & destructor
  SegmentDSP(uint8_t dspCount, const uint8_t* muxPins, const uint8_t* dspPins);
  ~SegmentDSP();

  //Set a display glyph
  // - Uses the default hex font
  void setGlyph(uint8_t dspNumber, uint8_t glyphNumber);

  //Set a display glyph
  // - Uses the special font
  // - - Special font must be provided in the glyph header
  // - TODO
  //void setGlyphSpecial(uint8_t dspNumber, uint8_t glyphNumber);

  //Set a alpha glyph
  // - Uses the alpha font
  // - - Alpha font must be provided in the glyph header
  // - TODO
  //void setGlyphAlpha(uint8_t dspNumber, char alphaChar);

  //Set a pair of displays to a 8-bit hex value
  // - Start dsp and the subsequent will be used
  void setHex(uint8_t startDsp, uint8_t hexValue);

  //Set the decimal point on a specific display
  void setDP(uint8_t dspNumber, bool dp);

  //Set all decimal points
  void setAllDP(bool dp);

  //Update the next dsp in the mux
  // - This should be run within an interrupt of some kind
  void dspUpdate();

private:
  //DSP count
  uint8_t dspCount;

  //Current/last dsp being displayed
  uint8_t currentDsp;
  uint8_t lastDsp;

  //Mux pins
  const uint8_t* muxPins;

  //DSP pins
  const uint8_t* dspPins;

  //Mux output states
  // - Treated as a long 1D array for speed on an AVR chip
  volatile uint8_t* muxPinsOut;
};