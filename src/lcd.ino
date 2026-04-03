#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// #include <hd44780.h>
// #include <hd44780ioClass/hd44780_I2Cexp.h>

//hd44780_I2Cexp lcd;

// Button Pin Definitions 
// const int buttonCyclePage = 2;  // Changes what is displayed on screen
// const int buttonRestart   = 3;  // Resets the board and game state
// const int buttonEndTurn   = 4;  // Confirms human move and hands turn to AI

// Logic Variables [cite: 9]
bool playerTurn = true;
int currentPage = 0; 
const int totalPages = 3;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void lcdSetup() {
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Hello");

    // pinMode(buttonCyclePage, INPUT_PULLUP); 
    // pinMode(buttonRestart, INPUT_PULLUP);   
    // pinMode(buttonEndTurn, INPUT_PULLUP);    

    //lcd.begin(16, 2); 
    //lcd.backlight();
    
    //lcd.print("ChessBot v1.0");
    //delay(1000);
    //updateDisplay();
}

void updateDisplay() {
    //lcd.clear();
    //switch (currentPage) {
      //  case 0: // Page 1: Game Status 
        //    lcd.print("Status: Playing");
          //  lcd.setCursor(0, 1);
            //lcd.print(playerTurn ? "> Your Turn" : "> AI Thinking");
            //break;
            
        //case 1: // Page 2: Piece Graveyard (Example) [cite: 13, 14]
         //   lcd.print("Last Capture:");
           // lcd.setCursor(0, 1);
            //lcd.print("None Yet"); 
            //break;
            
        //case 2: // Page 3: System Info
          //  lcd.print("Engine: Active");
           // lcd.setCursor(0, 1);
           // lcd.print("Depth: 3 Plies");
           // break;
    //}
}

void lcd_confirmMove(const char* move, bool is_ai_move) {
    lcd.clear();
    
    if (is_ai_move) {
        lcd.print("confirm ai move:");
    } else {
        lcd.print("confirm player move:");
    }

    lcd.setCursor(0, 1);
    lcd.print(move);
    delay(1000); // Display for 1 second
}

void lcd_aiMove() {
  lcd.clear();
  lcd.print("AI is thinking...");

}

void lcd_playerMove() {
  lcd.clear();
  lcd.print("Your turn!");
}

void lcd_wrongMove() {
  lcd.clear();
  lcd.print("Error: Move was incomplete.");

}

void lcd_moveRejected() {
  lcd.clear();
  lcd.print("Move Rejected!");
}

void pieceCaptured() {
    //lcd.clear();
    //lcd.print("Piece Captured!");
    //delay(1000); // Display for 1 second
}