#include "DFRobotDFPlayerMini.h"

// On Uno R4 WiFi, we use Serial1 (Pins 0 and 1)
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  // Serial Monitor 
  Serial.begin(115200);
  
  // Hardware Serial for DFPlayer
  Serial1.begin(9600);

  Serial.println(F("Initializing DFPlayer..."));

  // Check if the module responds
  if (!myDFPlayer.begin(Serial1)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1. Check if the SD card is FAT32."));
    Serial.println(F("2. Check if RX/TX are swapped (try swapping Pin 0 and 1)."));
    Serial.println(F("3. Check if the module has power (5V)."));
    while(true);
  }

  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.volume(25);  // Set volume (0 to 30)
  
  // Play the file: /mp3/0001.mp3
  Serial.println(F("Playing 0001.mp3..."));
  myDFPlayer.playFolder(1, 1); // Specifically plays folder 01, file 001
}

void loop() {
  // Keep it simple for now
  myDFPlayer.play(1);
  delay(10000);
}