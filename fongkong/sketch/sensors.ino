#include <array>
#include <cstring>
#include <Wire.h>
using namespace std;
#define DEBUG

const byte PCF_ADDRS[8] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x38};

bool board[8][8];


char* detect_player_move() {

    char* lastMove = nullptr;
    while (stillDetectingMove()) {
        lastMove = get_changed_position();
    }   
    return lastMove;
}

char* get_changed_position() {
    static char pos[3]; // e.g., "E2"
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (board[row][col]) {
                pos[0] = 'A' + col; // Column A-H
                pos[1] = '1' + row; // Row 1-8
                pos[2] = '\0'; // Null terminator
                return pos;
            }
        }
    }
    return nullptr;
}

bool stillDetectingMove() {
    bool oldBoard[8][8] = board;
    delay(2000); // Debounce delay
    readSensors();
    return !std::equal(&oldBoard[0][0], &oldBoard[0][0] + 64, &board[0][0]);
}

void readSensors() {
  for (int row = 0; row < 8; row++) {
    byte rowData = readPCF(PCF_ADDRS[row]);

    for (int col = 0; col < 8; col++) {
      board[row][col] = !(rowData & (1 << col));
    }
  }


  #ifdef DEBUG
  Serial.println("------------------------");
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      Serial.print("[");
      Serial.print((char)('A' + col));   // A to H
      Serial.print(row + 1);             // 1 to 8
      Serial.print(": ");
      Serial.print(board[row][col] ? "X" : ".");
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
