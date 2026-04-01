#include <array>
#include <cstring>
#include <Wire.h>
using namespace std;
#define DEBUG

const uint8_t PCF_ADDRS[8] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x38};

bool sensorBoard[8][8];
bool initialBoard[8][8];


char* detect_player_move(bool* playerEndedTurn = nullptr) {
    readSensors();
    std::copy(&sensorBoard[0][0], &sensorBoard[0][0] + 64, &initialBoard[0][0]);

    #ifdef DEBUG
    Monitor.println("------------------------");
    Monitor.println("Initial Board State:");
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      Monitor.print("[");
      Monitor.print((char)('A' + col));   // A to H
      Monitor.print(row + 1);             // 1 to 8
      Monitor.print(": ");
      Monitor.print(initialBoard[row][col] ? "X" : ".");
      Monitor.print("] ");
    }
    Monitor.println();
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
    Monitor.print("Detected move: ");
    Monitor.println(lastMove);
    #endif
  }
  return lastMove;
}

char* get_changed_position() {
    static char pos[5]; // e.g., "A2E4"
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (sensorBoard[row][col] != initialBoard[row][col]) {
                pos[0] = 'A' + col;
                pos[1] = '1' + row;
                pos[2] = '\0';
                return pos;
            }
        }
    }
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
    uint8_t rowData = readPCF(PCF_ADDRS[row]);

    for (int col = 0; col < 8; col++) {
      sensorBoard[row][col] = !(rowData & (1 << col));
    }
  }


  #ifdef DEBUG
  Monitor.println("------------------------");
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      Monitor.print("[");
      Monitor.print((char)('A' + col));   // A to H
      Monitor.print(row + 1);             // 1 to 8
      Monitor.print(": ");
      Monitor.print(sensorBoard[row][col] ? "X" : ".");
      Monitor.print("] ");
    }
    Monitor.println();
  }
  #endif
}


void writePCF(uint8_t addr, uint8_t value) {
  Wire.beginTransmission(addr);
  Wire.write(value);
  Wire.endTransmission();
}

uint8_t readPCF(uint8_t addr) {
  Wire.requestFrom(addr, (uint8_t)1);
  if (Wire.available()) return Wire.read();
  return 0xFF;
}

void sensorSetup() {
  Wire.begin();
  Monitor.begin(9600);
  delay(2000);

  for (int i = 0; i < 8; i++) {
    writePCF(PCF_ADDRS[i], 0xFF);
  }

  Monitor.println("Sensors initialized.");
}
