//-- Asus Qcode Card --
//
// - Firmware by: Sciguy429
//
// - This is the firmware source for the 1X Qcode card. It is very basic, there is little
// - reason to rebuild this firmware yourself.

//7-segment library
#include "7Segment.h"

//Debounce library
#include "Debounce.h"

//Echo flag
// - If enabled, any QCodes received will be echoed back on the serial port. Allowing for
// - another device to receive the data in tandem.
#define ECHO_SERIAL_DATA true

//Board led
const uint8_t boardLED = 13;

//Button pin
const uint8_t buttonPin = 12;

//Button debouncer
Debounce buttonDebounce = Debounce(buttonPin);

//Total number of displays
#define MAX_DSP 2

//Mux pins
const uint8_t muxPins[] = {7, 6, 3, 4, 5, 8, 9, 2}; //A, B, C, D, E, F, G, DP

//Dsp pins
const uint8_t dspPins[] = {10, 11}; //Left to right

//7-segment object
SegmentDSP segmentDSP(MAX_DSP, muxPins, dspPins);

//Past qcodes
#define MAX_PAST_QCODES 256
uint8_t pastQCodes[MAX_PAST_QCODES];
int pastQcodeHead = 0;
int pastQcodeCount = 0;
int curQCode = 0x00;

//Called ~100 times per second
SIGNAL(TIMER2_COMPA_vect) 
{
  //Advance The COMPA Register
  OCR2A += 156;

  //Update the 7-segment displays
  segmentDSP.dspUpdate();
}

void setup()
{
  //Board LED
	pinMode(boardLED, OUTPUT);
  digitalWrite(boardLED, LOW);

  //Button pin
  pinMode(buttonPin, INPUT_PULLUP);

  //Setup all mux pins
  for (int i = 0; i < 8; i++)
  {
    pinMode(muxPins[i], OUTPUT);
    digitalWrite(muxPins[i], HIGH); //Active LOW
  }

  //Setup the dsp pins
  for (int i = 0; i < MAX_DSP; i++)
  {
    pinMode(dspPins[i], OUTPUT);
    digitalWrite(dspPins[i], LOW); //Active HIGH
  }

  //Open serial connection to the COM_DEBUG port
  Serial.begin(115200);

  //Set starting code to 0x00
  segmentDSP.setHex(0, 0x00);

  //Clear decimal points
  segmentDSP.setAllDP(false);

  //Clear the list of past qcodes
  for (int i = 0; i < MAX_PAST_QCODES; i++)
  {
    pastQCodes[i] = 0x00;
  }

  //Setup interrupt
  // - Timer 2 COMP-A
  // - 100hz fire rate
  // - 50hz flash for each dsp
  cli();
  TCCR2A = 0;           //Init Timer2A
  TCCR2B = 0;           //Init Timer2B
  TCCR2B |= B00000111;  //Prescaler = 1024
  OCR2A = 156;          //Timer Compare2A Register
  TIMSK2 |= B00000010;
  sei();
}

void loop()
{
  //Update the button state
  ButtonState buttonState = buttonDebounce.read();

  //Check our button state
  if (buttonState == ButtonState::BS_PRESSED)
  {
    //Print our saved codes
    if (pastQcodeCount > 0)
    {
      //Find starting pos
      int curPos = pastQcodeHead - pastQcodeCount;
      if (curPos < 0)
      {
        curPos = curPos + MAX_PAST_QCODES;
      }

      //Print codes
      bool dp = false;
      for (int i = 0; i < pastQcodeCount; i++)
      {
        //Set the display
        segmentDSP.setHex(0, pastQCodes[curPos]);
        segmentDSP.setDP(0, dp);
        segmentDSP.setDP(1, !dp);
        dp = !dp;

        //Push the position forward
        curPos++;
        if (curPos >= MAX_PAST_QCODES)
        {
          curPos = 0;
        }

        //Wait
        delay(250);
      }
    }

    //Reset the display to the last code
    segmentDSP.setHex(0, curQCode);
    segmentDSP.setAllDP(false);
  }

  //Wait for serial data
  if (Serial.available())
  {
    //Flash LED
    digitalWrite(boardLED, HIGH);

    //Grab our new code and update the display
    curQCode = Serial.read();
    segmentDSP.setHex(0, curQCode);

    //Echo the serial data if need be
    if (ECHO_SERIAL_DATA)
    {
      Serial.write(curQCode);
    }

    //Save our code into the buffer
    pastQCodes[pastQcodeHead] = curQCode;
    pastQcodeHead++;
    if (pastQcodeHead >= MAX_PAST_QCODES)
    {
      pastQcodeHead = 0;
    }

    //Add up our buffer size
    if (pastQcodeCount < MAX_PAST_QCODES)
    {
      pastQcodeCount++;
    }
  }
  else
  {
    //Flash LED
    digitalWrite(boardLED, LOW);
  }
}
