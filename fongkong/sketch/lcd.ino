#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

hd44780_I2Cexp lcd;

const int buttonTurn = 2;
const int buttonCapture = 3;
const int buttonPower = 4;

bool playerTurn = true;
bool systemOn = true;

String pieces[] = {"Pawn", "Knight", "Bishop", "Rook", "Queen"};
int totalPieces = 5;

void powerOnScreen() {
    lcd.clear();
    lcd.print("Power ON");
    delay(1000);
    lcd.clear();
    lcd.print("ChessBot Ready");
    delay(1000);
}

void powerOffScreen() {
    lcd.clear();
    lcd.print("Power OFF");
}

void gameStartScreen() {
    lcd.clear();
    lcd.print("Game Starting...");
    delay(1500);
}

void showTurn(bool playerTurn) {
    lcd.clear();
    if (playerTurn)
        lcd.print("Your Turn");
    else
        lcd.print("AI Thinking...");
}

void pieceCaptured() {
    int randomIndex = random(totalPieces);
    lcd.clear();
    lcd.print("Captured:");
    lcd.setCursor(0,1);
    lcd.print(pieces[randomIndex]);
    delay(1500);
}

void lcdSetup() {
    pinMode(buttonTurn, INPUT_PULLUP);
    pinMode(buttonCapture, INPUT_PULLUP);
    pinMode(buttonPower, INPUT_PULLUP);

    lcd.begin(16,2);

    randomSeed(analogRead(A0));

    powerOnScreen();
    gameStartScreen();
    showTurn(playerTurn);
}

// void loop() {

//     if (digitalRead(buttonPower) == LOW) {
//         systemOn = !systemOn;

//         if (systemOn) {
//             powerOnScreen();
//             gameStartScreen();
//             showTurn(playerTurn);
//         } else {
//             powerOffScreen();
//         }

//         delay(400);
//     }

//     if (!systemOn) return;

//     if (digitalRead(buttonTurn) == LOW) {
//         playerTurn = !playerTurn;
//         showTurn(playerTurn);
//         delay(300);
//     }

//     if (digitalRead(buttonCapture) == LOW) {
//         pieceCaptured();
//         showTurn(playerTurn);
//         delay(300);
//     }
// }
