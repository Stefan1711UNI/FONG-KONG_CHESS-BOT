#include "CoreXYController.h"

//Constructor
CoreXYController::CoreXYController(float sqSize, float steps) {
  squareSizeMM = sqSize;
  stepsPerMM = steps;
}

//Setup
void CoreXYController::setUp(int pinStepA, int pinDirA, int pinStepB, int pinDirB, int pinMagnet, int pinLimitX, int pinLimitY) {
  Serial.println("BOARD: Hardware pins assigned.");
  stepA_Pin = pinStepA;
  dirA_Pin = pinDirA;
  stepB_Pin = pinStepB;
  dirB_Pin = pinDirB;
  magnet_Pin = pinMagnet;
  limitX_Pin = pinLimitX;
  limitY_Pin = pinLimitY;

  //Set up magnet as output, and turn it off at startup
  pinMode(magnet_Pin, OUTPUT);
  digitalWrite(magnet_Pin, LOW);

  //Set up micro switches 
  pinMode(limitX_Pin, INPUT_PULLUP);
  pinMode(limitY_Pin, INPUT_PULLUP);

  //Configure AccelStepper
  motorA = AccelStepper(1, stepA_Pin, dirA_Pin);
  motorB = AccelStepper(1, stepB_Pin, dirB_Pin);

  //Set max speed of motors, #TODO find this value
  motorA.setMaxSpeed(2000.0);
  motorB.setMaxSpeed(2000.0);

  steppers.addStepper(motorA);
  steppers.addStepper(motorB);
}

//Calibrate the stepper motors and sets current pos to 0,0
void CoreXYController::calibrate() {
  //LOGIC TODO
  currentX = 0;
  currentY = 0;
  Serial.println("BOARD: Homing complete. At (0,0).");
}

//Move Piece, returns TRUE when action completed
bool CoreXYController::movePiece(String startSquare, String endSquare) {
  //TODO
  Serial.println("BOARD: Move complete.");
  return true; 
}

//Executes the CoreXY movement
void CoreXYController::executeCoreXYMovement(float targetX, float targetY){
  //Calculate coreXY target mm
  float motorA_mm = targetX + targetY;
  float motorB_mm = targetX - targetY;
  //Convert mm to steps for motor
  long targetStepsA = round(motorA_mm * stepsPerMM);
  long targetStepsB = round(motorB_mm * stepsPerMM);
  //Put into array for multiStepper
  long positions[2];
  positions[0] = targetStepsA; // Motor A
  positions[1] = targetStepsB; // Motor 
  //Call motors
  steppers.moveTo(positions);
  //Execute the move (BLOCKING)
  steppers.runSpeedToPosition();
  //Update new current position
  currentX = targetX;
  currentY = targetY;
}


void CoreXYController::updateBoardState(byte currentBoard[8][8]) {
  memcpy(boardState, currentBoard, sizeof(boardState));
}

//!!!! THIS CODE ASSUMES "HOME" (0,0) IS AT "A1", CHANGE ASCII MATH IS THIS IS NOT THE CASE!!!!!!
void CoreXYController::parseSquare(String square, int &gridX, int &gridY) {
  //Check if string is 2 char, exit if garbage values
  if(square.length() < 2){
    gridX = 0;
    gridY = 0;
    return;
  }

  //Seperate the letter and number
  char row = square.charAt(0);
  char column = square.charAt(1);

  //Calculates the X value using ASCII
  //'E' will be turned into '4', a more manageble value than 'E'
  if(row >= 'a' && row <= 'h'){
    gridX = row - 'a';
  } else if(row >= 'A' && row <= 'H'){
    gridX = row - 'A';
  }else{
    gridX = 0;
  }
  
  //Calculates the Y value
  if(column >= '1' && column <= '8'){
    gridY = column - '1';
  }else{
    gridY = 0;
  }

}

void CoreXYController::gridToMM(int gridX, int gridY, float &mmX, float &mmY) {
  //With our grid values we can just multiply them by the size of the square since (0,0) is at A1
  mmX = gridX * squareSizeMM;
  mmY = gridY * squareSizeMM;
}

void CoreXYController::magnetON() {
  
}
void CoreXYController::magnetOFF() {}

//Empty funcs so the compliler doesnt crash

bool CoreXYController::capturePiece(String targetSquare, int graveyardSlot) { return true; }
void CoreXYController::routeAlongSeams(float startX, float startY, float targetX, float targetY) {}
