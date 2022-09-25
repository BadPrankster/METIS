// Code to recognise NFC stickers
// August-September 2022
// Lucas Wirz-Vitiuk

// Load NFC libraries
#include "PN7150Interface.h"            // NCI protocol runs over a hardware interface, in this case an I2C with 2 extra handshaking signals
#include "NCI.h"                        // Talking to the NFC module is done in an NCI language
#include "NFCReaderWriter.h"            // Implementing a NFC Reader/Writer application
#include "Tag.h"

// Define NFC pins
const int IRQ_Pin = 27;
const int VEN_Pin = 25;

// Create a global NFC device (using the default I2C address 0x28)
PN7150Interface theInterface = PN7150Interface(IRQ_Pin, VEN_Pin);
NCI theNCI(theInterface);
NFCReaderWriter theReaderWriter(theNCI);

// Define NFC-variables (including unique ID)
uint8_t theValue[4];
uint8_t theID[4] = {0x33, 0x74, 0xE4, 0xBB};
bool verification = false;
String state = "none";

void setup_NFC() {

  // Setup NFC recognition
  theReaderWriter.initialize();
}

String getNFCstate() {
  theNCI.run();
  switch (theReaderWriter.theState) {

    case ReaderWriterState::initializing:
      {
        switch (theNCI.getState())
        {
          case NciState::RfIdleCmd:
            {
              theNCI.activate();
              theReaderWriter.theState = ReaderWriterState::noTagPresent;
            }
            break;
          default:
            break;
        }
      }
      break;

    case ReaderWriterState::noTagPresent:
      {
        switch (theNCI.getState())
        {

          case NciState::RfPollActive:
            {
              theReaderWriter.theState = ReaderWriterState::singleTagPresent;
              for (uint8_t index = 0; index < 4; index++)
              {
                theValue[index] = theNCI.rxBuffer[index + 13];
                if (theValue[index] != theID[index]) {
                  state = "wrong";
                  break;
                }
                if (index == 3) {
                  state = "right";
                }
              }
            }
            break;
        }
      }
      break;

    case ReaderWriterState::singleTagPresent:
      {
        switch (theNCI.getState())
        {
          case NciState::RfIdleCmd:
            theNCI.activate();
            break;

          case NciState::RfDiscovery:
            if (theNCI.getTagsPresentStatus() == TagsPresentStatus::noTagsPresent)
            {
              theReaderWriter.theState = ReaderWriterState::noTagPresent;
              state = "none";
            }
            break;

          case NciState::RfWaitForHostSelect:
            theNCI.deActivate(NciRfDeAcivationMode::IdleMode);
            break;

          case NciState::RfPollActive:
            theNCI.deActivate(NciRfDeAcivationMode::IdleMode);
            break;
        }
      }
      break;
  }
  return state;
}
