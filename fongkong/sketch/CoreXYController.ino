#include "CoreXYController.h"

//Constructor
CoreXYController::CoreXYController(float sqSize, float steps) {
  squareSizeMM = sqSize;
  stepsPerMM = steps;
}

//Setup
void CoreXYController::setUp(int pinStepA, int pinDirA, int pinStepB, int pinDirB, int pinMagnet, int pinLimitX, int pinLimitY, int pinEnable) {
  Monitor.println("BOARD: Hardware pins assigned.");
  stepA_Pin = pinStepA;
  dirA_Pin = pinDirA;
  stepB_Pin = pinStepB;
  dirB_Pin = pinDirB;
  magnet_Pin = pinMagnet;
  limitX_Pin = pinLimitX;
  limitY_Pin = pinLimitY;
  enable_Pin = pinEnable;   //Disables/Enables motors

  //Set up magnet as output, and turn it off at startup
  pinMode(magnet_Pin, OUTPUT);
  digitalWrite(magnet_Pin, LOW);

  //Set up micro switches 
  pinMode(limitX_Pin, INPUT_PULLUP);
  pinMode(limitY_Pin, INPUT_PULLUP);

  //Set up enable pin and turn off motors, HIGH = OFF
  pinMode(enable_Pin, OUTPUT);
  disableMotors();

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
  Monitor.println("BOARD: Homing complete. At (0,0).");
}

//Move Piece, returns TRUE when action completed
 bool CoreXYController::movePiece(String startSquare, String endSquare) {
  Monitor.print("BOARD: Initiating move from ");
  Monitor.print(startSquare);
  Monitor.print(" to ");
  Monitor.println(endSquare);
  //Grid coords
  int startGridX, startGridY;
  int endGridX, endGridY;
  //Actual mm coords
  float startMM_X, startMM_Y;
  float endMM_X, endMM_Y;
  //Convert string to grid
  parseSquare(startSquare, startGridX, startGridY);
  parseSquare(endSquare, endGridX, endGridY);
  //Convert grid to mm coords
  gridToMM(startGridX, startGridY, startMM_X, startMM_Y);
  gridToMM(endGridX, endGridY, endMM_X, endMM_Y);

  //---Move end effector to start position---
  Monitor.println("BOARD: Moving to start position...");
  //Turns motor on
  enableMotors();
  //Turn magnet off
  magnetOFF();
  //Move to mm coords of the start position
  executeCoreXYMovement(startMM_X, startMM_Y);

  //---COLLISION CHECK---
  Monitor.println("BOARD: Analyzing path for collisions...");
  bool pathClear = true;

  int dx = endGridX - startGridX;
  int dy = endGridY - startGridY;

  //Raycast
  // Normalize the direction to just -1, 0, or 1 
  int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
  int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);
  //We start the check by 1 grid ahead of the start square
  int checkX = startGridX + stepX;
  int checkY = startGridY + stepY;

  //Now we check if every square in the path is clear
  while (checkX != endGridX || checkY != endGridY) {
    if (boardState[checkX][checkY] == 1) {
      Monitor.println("BOARD: Obstacle detected mid-path! Path blocked.");
      pathClear = false;
      break; // Stop looking
    }
    checkX += stepX;
    checkY += stepY;
  }

  //Checks if the end square is actually clear
  if (boardState[endGridX][endGridY] == 1) {
    Monitor.println("BOARD: ERROR - Target square is occupied!");
    pathClear = false;
  }

  //---MOVE THE PIECE---
  Monitor.println("BOARD: Magnet ON.");
  magnetON();
  delay(200); //Allow the magnet to fully engage

  //If the path is clear we can move to the end square, if its not clear we must run "routeAlongSeams"
  if (pathClear == true) {
    Monitor.println("BOARD: Path clear. Executing direct (straight) move.");
    executeCoreXYMovement(endMM_X, endMM_Y);
  } else {
    Monitor.println("BOARD: Path blocked. Routing along physical seams.");
    routeAlongSeams(startMM_X, startMM_Y, endMM_X, endMM_Y);
  }

  //---MOVE FINISHED---
  Monitor.println("BOARD: Magnet OFF..");
  magnetOFF();
  delay(200);

  //Turn motors OFF to save power
  disableMotors();

  Monitor.println("BOARD: Piece delivered to target square.");
  return true; 
}


bool CoreXYController::moveKnightPiece(String startSquare, String endSquare) {
  Monitor.print("BOARD: Initiating KNIGHT move from ");
  Monitor.print(startSquare);
  Monitor.print(" to ");
  Monitor.println(endSquare);

  //Grid coords
  int startGridX, startGridY, endGridX, endGridY;
  //Actual mm coords
  float startMM_X, startMM_Y, endMM_X, endMM_Y;

  //Convert string to grid
  parseSquare(startSquare, startGridX, startGridY);
  parseSquare(endSquare, endGridX, endGridY);
  //Convert grid to mm coords
  gridToMM(startGridX, startGridY, startMM_X, startMM_Y);
  gridToMM(endGridX, endGridY, endMM_X, endMM_Y);

  //---Move end effector to start position---
  //Turns motor on
  enableMotors();
  //Turn magnet off
  magnetOFF(); 
  //Move to mm coords of the start position
  executeCoreXYMovement(startMM_X, startMM_Y);
  magnetON();
  delay(200); //Allow the magnet to fully engage

  //---CALCULATE THE PIVOTS---
  //A Knight cannot move in a straight diagonal line from the start to the end square, without clipping other pieces.
  //It must move in a 2-part "L" shape. This section calculates the two possible
  //intermediate "corner" squares (pivots) the Knight can step on to complete the move.

  //Find the total grid distance to move in X and Y
  int dx = endGridX - startGridX;
  int dy = endGridY - startGridY;
  
  //Determine the 1-square step direction (+1 or -1) for both axes
  int stepX = (dx > 0) ? 1 : -1;
  int stepY = (dy > 0) ? 1 : -1;

  //Calculate Pivot 1: Moving 1 square diagonally first
  int pivot1_X = startGridX + stepX;
  int pivot1_Y = startGridY + stepY;

  //Calculate Pivot 2: Moving 1 square orthogonally (straight) first
  int pivot2_X = startGridX;
  int pivot2_Y = startGridY;
  
  //Determine which axis the Knight moves 2 squares along (the long axis)
  if (abs(dx) > abs(dy)) {
    //X is the long axis, apply the step to X
    pivot2_X = startGridX + stepX;
  } else {
    //Y is the long axis, apply the step to Y
    pivot2_Y = startGridY + stepY;
  }

  //---COLLISION CHECK---
  //We check the board memory to see if either of the calculated L-shape paths are empty.
  //If both pivots are blocked by other pieces, the Knight is boxed in and we must 
  //fallback to the slower method of safely dragging the piece between the gridlines.
  
  float pivotMM_X, pivotMM_Y;

  //Tier 1: Check if Pivot 1 and the final target square are both empty
  if (boardState[pivot1_X][pivot1_Y] == 0 && boardState[endGridX][endGridY] == 0) {
    Monitor.println("BOARD: Knight Path 1 (Diagonal First) is clear.");
    //Convert pivot grid to mm coords
    gridToMM(pivot1_X, pivot1_Y, pivotMM_X, pivotMM_Y);
    
    //Execute the staircase move: drive to the pivot, then to the end position
    executeCoreXYMovement(pivotMM_X, pivotMM_Y);
    executeCoreXYMovement(endMM_X, endMM_Y);
  } 
  
  //Tier 2: Check if Pivot 2 and the final target square are both empty
  else if (boardState[pivot2_X][pivot2_Y] == 0 && boardState[endGridX][endGridY] == 0) {
    Monitor.println("BOARD: Knight Path 2 (Orthogonal First) is clear.");
    //Convert pivot grid to mm coords
    gridToMM(pivot2_X, pivot2_Y, pivotMM_X, pivotMM_Y);
    
    //Execute the staircase move: drive to the pivot, then to the end position
    executeCoreXYMovement(pivotMM_X, pivotMM_Y);
    executeCoreXYMovement(endMM_X, endMM_Y);
  } 
  
  //Tier 3: Fallback if both intermediate pivot squares are blocked by pieces
  else {
    Monitor.println("BOARD: Knight is boxed in. Routing along seams.");
    routeAlongSeams(startMM_X, startMM_Y, endMM_X, endMM_Y);
  }

  //---MOVE FINISHED---
  //Turn magnet off
  magnetOFF();
  delay(200);

  //Turn motors OFF to save power
  disableMotors();

  Monitor.println("BOARD: Knight move complete.");
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
  positions[1] = targetStepsB; // Motor B

  //DEBUGING
  Monitor.print("   -> Driving Motors to: [");
  Monitor.print(mmToAlgebraic(targetX, targetY));
  Monitor.print("]  (X=");
  Monitor.print(targetX);
  Monitor.print("mm, Y=");
  Monitor.print(targetY);
  Monitor.println("mm)");

  //Call motors
  steppers.moveTo(positions);

  //Execute the move (BLOCKING)
  steppers.runSpeedToPosition();

  //Update new current position
  currentX = targetX;
  currentY = targetY;
}


void CoreXYController::updateBoardState(uint8_t currentBoard[8][8]) {
  memcpy(boardState, currentBoard, sizeof(boardState));
}

//!!!! THIS CODE ASSUMES "HOME" (0,0) IS AT "A1", CHANGE ASCII MATH IF THIS IS NOT THE CASE!!!!!!
void CoreXYController::parseSquare(String square, int &gridX, int &gridY) {
  //Check if string is 2 char, exit if garbage values
  if(square.length() < 2){
    gridX = 0;
    gridY = 0;
    return;
  }

  //Separate the letter and number
  char file = square.charAt(0);  //Letter (Column on the board)
  char rank = square.charAt(1); //Number (Row on the board)

  //Calculates the X value using ASCII
  //'E' will be turned into '4', a more manageble value than 'E'
  if(file >= 'a' && file <= 'h'){
    gridX = file - 'a';
  } else if(file >= 'A' && file <= 'H'){
    gridX = file - 'A';
  }else{
    gridX = 0;
  }
  
  //Calculates the Y value
  if(rank >= '1' && rank <= '8'){
    gridY = rank - '1';
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
  digitalWrite(magnet_Pin, HIGH);
}
void CoreXYController::magnetOFF() {
  digitalWrite(magnet_Pin, LOW);
}


//DEBUG TESTING TEMP 
String CoreXYController::mmToAlgebraic(float mmX, float mmY) {
  int gridX = round(mmX / squareSizeMM);
  int gridY = round(mmY / squareSizeMM);
  
  char file = 'a' + gridX;
  char rank = '1' + gridY;
  
  String square = "";
  square += file;
  square += rank;
  return square;
}

//Powers the stepper coils, LOW = ON
void CoreXYController::enableMotors() {
  digitalWrite(enable_Pin, LOW);
  delay(50); //Allow time to power the motor
}

//Cuts power to the stepper coils, HIGH = OFF
void CoreXYController::disableMotors() {
  digitalWrite(enable_Pin, HIGH);
}


//Empty funcs so the compliler doesnt crash
bool CoreXYController::capturePiece(String targetSquare, int graveyardSlot) { return true; }
void CoreXYController::routeAlongSeams(float startX, float startY, float targetX, float targetY) {}
