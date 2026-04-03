// #include "DFRobotDFPlayerMini.h"

// DFRobotDFPlayerMini myDFPlayer;

//void audioSetup() {
    // Monitor.begin(115200);
//   // Monitor1.begin(9600); // Hardware Monitor for DFPlayer on Uno R4

//   Monitor.println(F("Initializing DFPlayer..."));

//   if (!myDFPlayer.begin()) {
//     Monitor.println(F("Unable to begin. Check SD card and wiring."));
//     while(true);
//   }

//   myDFPlayer.volume(25); // Set default volume [cite: 6]
//   Monitor.println(F("DFPlayer Mini online."));
// }

// // void loop() {
// //   // Example usage:
// //   playCapture();
// //   delay(5000);
// // }

// // --- Custom Audio Functions ---

// // Plays a standard move sound (Folder 01, File 001)
// void playMove() {
//   Monitor.println(F("Playing: Move"));
//   myDFPlayer.playFolder(1, 1);
// }

// // Plays a capture sound (Folder 01, File 003)
// void playCapture() {
//   Monitor.println(F("Playing: Capture"));
//   myDFPlayer.playFolder(1, 3);
// }

// // Plays a notification for Check (Folder 01, File 002)
// void playCheck() {
//   Monitor.println(F("Playing: Check"));
//   myDFPlayer.playFolder(1, 2);
// }

// // Plays game over/checkmate sound (Folder 01, File 004)
// void playGameOver() {
//   Monitor.println(F("Playing: Game Over"));
//   myDFPlayer.playFolder(1, 4);
// }

// // General function to play any specific file
// void playEffect(int folder, int file) {
//   myDFPlayer.playFolder(folder, file);
// }