#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

hd44780_I2Cexp lcd;

// Button Pin Definitions 
// const int buttonCyclePage = 2;  // Changes what is displayed on screen
// const int buttonRestart   = 3;  // Resets the board and game state
// const int buttonEndTurn   = 4;  // Confirms human move and hands turn to AI

// Logic Variables [cite: 9]
bool playerTurn = true;
int currentPage = 0; 
const int totalPages = 3;

void lcdSetup() {
    // pinMode(buttonCyclePage, INPUT_PULLUP); 
    // pinMode(buttonRestart, INPUT_PULLUP);   
    // pinMode(buttonEndTurn, INPUT_PULLUP);    

    lcd.begin(16, 2); 
    lcd.backlight();
    
    lcd.print("ChessBot v1.0");
    //delay(1000);
    updateDisplay();
}

void updateDisplay() {
    lcd.clear();
    switch (currentPage) {
        case 0: // Page 1: Game Status 
            lcd.print("Status: Playing");
            lcd.setCursor(0, 1);
            lcd.print(playerTurn ? "> Your Turn" : "> AI Thinking");
            break;
            
        case 1: // Page 2: Piece Graveyard (Example) [cite: 13, 14]
            lcd.print("Last Capture:");
            lcd.setCursor(0, 1);
            lcd.print("None Yet"); 
            break;
            
        case 2: // Page 3: System Info
            lcd.print("Engine: Active");
            lcd.setCursor(0, 1);
            lcd.print("Depth: 3 Plies");
            break;
    }
}

void lcd_confirmMove(const char* move) {
    lcd.clear();
    lcd.print("Confirming Move:");
    lcd.setCursor(0, 1);
    lcd.print(move);
    delay(1000); // Display for 1 second
}

void pieceCaptured() {
    lcd.clear();
    lcd.print("Piece Captured!");
    delay(1000); // Display for 1 second
}