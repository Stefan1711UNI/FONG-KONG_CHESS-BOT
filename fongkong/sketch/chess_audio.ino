#include "DFRobotDFPlayerMini.h"

DFRobotDFPlayerMini myDFPlayer;

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600); // Hardware Serial for DFPlayer on Uno R4

  Serial.println(F("Initializing DFPlayer..."));

  if (!myDFPlayer.begin(Serial1)) {
    Serial.println(F("Unable to begin. Check SD card and wiring."));
    while(true);
  }

  myDFPlayer.volume(25); // Set default volume [cite: 6]
  Serial.println(F("DFPlayer Mini online."));
}

void loop() {
  // Example usage:
  playCapture();
  delay(5000);
}

// --- Custom Audio Functions ---

// Plays a standard move sound (Folder 01, File 001)
void playMove() {
  Serial.println(F("Playing: Move"));
  myDFPlayer.playFolder(1, 1); [cite: 7]
}

// Plays a capture sound (Folder 01, File 002)
void playCapture() {
  Serial.println(F("Playing: Capture"));
  myDFPlayer.playFolder(1, 2);
}

// Plays a notification for Check (Folder 01, File 003)
void playCheck() {
  Serial.println(F("Playing: Check"));
  myDFPlayer.playFolder(1, 3);
}

// Plays game over/checkmate sound (Folder 01, File 004)
void playGameOver() {
  Serial.println(F("Playing: Game Over"));
  myDFPlayer.playFolder(1, 4);
}

// General function to play any specific file
void playEffect(int folder, int file) {
  myDFPlayer.playFolder(folder, file);
}