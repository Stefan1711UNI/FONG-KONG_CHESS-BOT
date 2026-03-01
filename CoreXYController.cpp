#include "CoreXYController.h"

//Constructor
CoreXYController::CoreXYController(float sqSize, float steps) {
  squareSizeMM = sqSize;
  stepsPerMM = steps;
}

//Setup
void CoreXYController::setUp(int pinStepA, int pinDirA, int pinStepB, int pinDirB, int pinMagnet, int pinLimitX, int pinLimitY) {
  Serial.println("BOARD: Hardware pins assigned.");
  //ASSIGN pins TODO
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


//Empty funcs so the compliler doesnt crash
void CoreXYController::updateBoardState(byte currentBoard[8][8]) {}
bool CoreXYController::capturePiece(String targetSquare, int graveyardSlot) { return true; }
void CoreXYController::parseSquare(String square, int &gridX, int &gridY) {}
void CoreXYController::gridToMM(int gridX, int gridY, float &mmX, float &mmY) {}
void CoreXYController::routeAlongSeams(float startX, float startY, float targetX, float targetY) {}
void CoreXYController::magnetON() {}
void CoreXYController::magnetOFF() {}