#include <array>
#include <cstring>
#include <Wire.h>
using namespace std;
#define DEBUG

const byte PCF_ADDRS[8] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x38};

bool sensorBoard[8][8];
bool initialBoard[8][8];

void wait_for_pawns() {
    Serial.println("Waiting for all pawns to be placed in starting positions...");
    bool all_pawns_placed = false;
    unsigned long last_print_time = 0; // Timer to prevent Serial spam

    while (!all_pawns_placed) {
        readSensors(); // Grab the latest board state
        all_pawns_placed = true; // Assume true until proven otherwise
        
        // Check if 1 second has passed since the last print
        bool should_print = (millis() - last_print_time > 1000);

        if (should_print) {
            Serial.print("Missing pawns: ");
        }

        // Check White pawns (Row 1 -> Chess Rank 2)
        for (int col = 0; col < 8; col++) {
            if (sensorBoard[1][col] == false) {
                all_pawns_placed = false;
                if (should_print) {
                    Serial.print((char)('A' + col));
                    Serial.print("2 "); // White pawns are on rank 2
                }
            }
        }

        // Check Black pawns (Row 6 -> Chess Rank 7)
        for (int col = 0; col < 8; col++) {
            if (sensorBoard[6][col] == false) {
                all_pawns_placed = false;
                if (should_print) {
                    Serial.print((char)('A' + col));
                    Serial.print("7 "); // Black pawns are on rank 7
                }
            }
        }

        // Finish the printed line and reset the timer
        if (should_print) {
            if (!all_pawns_placed) {
                Serial.println(); // Move to the next line for the next readout
            }
            last_print_time = millis();
        }

        // Short delay to prevent overwhelming the I2C communication
        if (!all_pawns_placed) {
            delay(100); 
        }
    }

    Serial.println("\nAll pawns detected in starting positions!");
}

char* detect_player_move(bool is_first_turn, volatile bool* playerEndedTurn = nullptr) {
  if (is_first_turn) {
    //for (int row = 0; row < 8; row++) {
      //for (int col = 0; col < 8; col++) {
        // Pieces start on rows 0, 1 (White) and rows 6, 7 (Black). 
        // Rows 2, 3, 4, 5 are empty.
        //initialBoard[row][col] = (row <= 1 || row >= 6);
      //}
    if (is_first_turn || playerEndedTurn != nullptr) {
    readSensors();
    std::copy(&sensorBoard[0][0], &sensorBoard[0][0] + 64, &initialBoard[0][0]);
    }
  }
    //}
  //}
  else {
    readSensors();
    std::copy(&sensorBoard[0][0], &sensorBoard[0][0] + 64, &initialBoard[0][0]);
  }
    

    #ifdef DEBUG
    Serial.println("------------------------");
    Serial.println("Initial Board State:");
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      Serial.print("[");
      Serial.print((char)('A' + col));   // A to H
      Serial.print(row + 1);             // 1 to 8
      Serial.print(": ");
      Serial.print(initialBoard[row][col] ? "X" : ".");
      Serial.print("] ");
    }
    Serial.println();
  }
  #endif


  char* lastMove = nullptr;
  if (playerEndedTurn == nullptr) {
    while (stillDetectingMove()) {
        lastMove = get_changed_position();
    }   
  }
  

  while (!*playerEndedTurn) {
    readSensors();
    lastMove = get_changed_position();  
    #ifdef DEBUG
    if (lastMove != nullptr) { // ONLY print if a move was actually returned
        Serial.print("Detected move: ");
        Serial.println(lastMove);
    }
    #endif
  }
  return lastMove;
}

char* get_changed_position() {
    static char pos[5]; // Array to hold "a2e4\0"
    
    // Set variables to -1 so we know if they haven't been found yet
    int from_col = -1, from_row = -1;
    int to_col = -1, to_row = -1;

    // Scan the entire 8x8 board for changes
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (sensorBoard[row][col] != initialBoard[row][col]) {
                
                // If the sensor is now false, the piece was picked up (From)
                if (sensorBoard[row][col] == false) {
                  if (from_col == -1) {
                    from_col = col;
                    from_row = row;
                  }
                }
                 
                // If the sensor is now true, a piece was placed down (To)
                else if (sensorBoard[row][col] == true) {
                  if (to_col == -1) {
                    to_col = col;
                    to_row = row;
                  }
                  
                }
                
            }
        }
    }

    // Only construct and return the string if BOTH squares were found
    // (This prevents crashes if the player is still holding the piece in the air)
    if (from_col != -1 && to_col != -1) {
        pos[0] = 'a' + from_col;
        pos[1] = '1' + from_row;
        pos[2] = 'a' + to_col;
        pos[3] = '1' + to_row;
        pos[4] = '\0'; // Null terminator to end the string
        return pos;
    }

    // Return null if the full move hasn't been completed yet
    return nullptr;
}

bool stillDetectingMove() {
    bool oldBoard[8][8] ;
    std::copy(&sensorBoard[0][0], &sensorBoard[0][0] + 64, &oldBoard[0][0]);
    delay(2000); // Debounce delay
    readSensors();
    return !std::equal(&oldBoard[0][0], &oldBoard[0][0] + 64, &sensorBoard[0][0]);
}

bool stillMoving() {}

void readSensors() {
  for (int row = 0; row < 8; row++) {
    byte rowData = readPCF(PCF_ADDRS[row]);

    for (int col = 0; col < 8; col++) {
      sensorBoard[row][col] = !(rowData & (1 << col));
    }
  }


  #ifdef DEBUGG
  Serial.println("------------------------");
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      Serial.print("[");
      Serial.print((char)('A' + col));   // A to H
      Serial.print(row + 1);             // 1 to 8
      Serial.print(": ");
      Serial.print(sensorBoard[row][col] ? "X" : ".");
      Serial.print("] ");
    }
    Serial.println();
  }
  #endif
}


void writePCF(byte addr, byte value) {
  Wire.beginTransmission(addr);
  Wire.write(value);
  Wire.endTransmission();
}

byte readPCF(byte addr) {
  Wire.requestFrom(addr, (byte)1);
  if (Wire.available()) return Wire.read();
  return 0xFF;
}

void sensorSetup() {
  Wire.begin();
  Serial.begin(9600);
  delay(2000);

  for (int i = 0; i < 8; i++) {
    writePCF(PCF_ADDRS[i], 0xFF);
  }

  Serial.println("Sensors initialized.");
}
