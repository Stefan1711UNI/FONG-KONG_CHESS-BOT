#include "CoreXYController.h"

//Constructor, microstepping added x16 by steps_x + y
CoreXYController::CoreXYController(float sqSize, float steps_x) {
  squareSizeMM = sqSize;
  stepsPerMM_X = steps_x;
  stepsPerMM_Y = 40.0;
}

//Setup 
void CoreXYController::setUp(int pinStepX, int pinDirX, int pinStepY, int pinDirY, int pinMagnet, int pinLimitX, int pinLimitY, int pinEnable) {
  Serial.println("BOARD: Hardware pins assigned.");
  stepX_Pin = pinStepX; //X-axis
  dirX_Pin = pinDirX;
  stepY_Pin = pinStepY; //Y-axis
  dirY_Pin = pinDirY;
  magnet_Pin = pinMagnet;
  limitX_Pin = pinLimitX;
  limitY_Pin = pinLimitY;
  enable_Pin = pinEnable;   //Disables/Enables motors

  //Set up magnet as output, and turn it off at startup
  pinMode(magnet_Pin, OUTPUT);
  digitalWrite(magnet_Pin, HIGH);

  //Set up micro switches 
  pinMode(limitX_Pin, INPUT_PULLUP);
  pinMode(limitY_Pin, INPUT_PULLUP);

  //Set up enable pin and turn off motors, HIGH = OFF
  pinMode(enable_Pin, OUTPUT);
  disableMotors();

  //Configure AccelStepper
  motorX = AccelStepper(1, stepX_Pin, dirX_Pin);
  motorY = AccelStepper(1, stepY_Pin, dirY_Pin);

  //Set max speed of motors
  motorX.setMaxSpeed(2000.0);  //200 
  motorY.setMaxSpeed(2000.0);

  motorX.setAcceleration(2000.0); //800 
  motorY.setAcceleration(2000.0);

  steppers.addStepper(motorX);
  steppers.addStepper(motorY);
}

//Calibrate the stepper motors and sets current pos to 0,0
void CoreXYController::calibrate() {
  Serial.println("BOARD: Calibration starting:");

  //--SETUP--
  enableMotors();
  float homingSpeed = 4000.0;
  
  //Move X-axis motor
  Serial.println("BOARD: Moving towards X limit switch...");
  motorX.setSpeed(-homingSpeed); 


  //Keeps moving untill limit switch is hit 
  while (digitalRead(limitX_Pin) == HIGH) {
    motorX.runSpeed();
  }
  
  Serial.println("BOARD: X-axis switch HIT!");
  motorX.setSpeed(0); //Stop the motor

  //---BACK OFF X-AXIS---
  Serial.println("BOARD: Backing off X switch...");
  motorX.setCurrentPosition(0);
  motorX.moveTo(100);  // Move off the switch
  
  while (motorX.distanceToGo() != 0) {
    motorX.run();
  }


  //Home the Y-axis
  Serial.println("BOARD: Moving towards Y limit switch...");
  motorY.setSpeed(-homingSpeed);

  //Keeps going untill the switch is hit
  while (digitalRead(limitY_Pin) == HIGH) {
    motorY.runSpeed();
  }
  
  Serial.println("BOARD: Y-axis switch HIT!");
  motorY.setSpeed(0);

  //--- BACK OFF Y-AXIS---
  Serial.println("BOARD: Backing off Y switch...");
  motorY.setCurrentPosition(0);
  motorY.moveTo(200);  // Move off the switch
  while (motorY.distanceToGo() != 0) {
    motorY.run();
  }

  //---Move to A1---
  Serial.println("BOARD: Moving from switches to the center of Square A1...");

  //---!!!THIS IS THE DISTANCE FROM THE LIMIT SWITCHES TO THE CENTER OF A1!!!---
  float offsetA1_X_mm = 350.0;  //350
  float offsetA1_Y_mm = 10.0; 

  //Convert mm to steps
  long offsetStepsX = round(offsetA1_X_mm * stepsPerMM_X);
  long offsetStepsY = round(offsetA1_Y_mm * stepsPerMM_Y);

  //Command motors to move to A1
  motorX.moveTo(offsetStepsX);
  motorY.moveTo(offsetStepsY);

  //Execute the move simultaneously
  while (motorX.distanceToGo() != 0 || motorY.distanceToGo() != 0) {
    motorX.run();
    motorY.run();
  }

  //---SET HOME POSITION---
  motorX.setCurrentPosition(0);
  motorY.setCurrentPosition(0);
  currentX = 0; 
  currentY = 0;
  
  disableMotors();
  Serial.println("BOARD: Homing complete. A1 is now Absolute (0,0)!");
}

//Move Piece, returns TRUE when action completed
bool CoreXYController::movePiece(String startSquare, String endSquare) {
  Serial.print("BOARD: Initiating move from ");
  Serial.print(startSquare);
  Serial.print(" to ");
  Serial.println(endSquare);

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
  Serial.println("BOARD: Moving to start position...");
  enableMotors();
  magnetOFF();
  //Move to mm coords of the start position
  executeCoreXYMovement(startMM_X, startMM_Y);

  //---COLLISION CHECK---
  Serial.println("BOARD: Analyzing path for collisions...");
  bool pathClear = true;

  int dx = endGridX - startGridX;
  int dy = endGridY - startGridY;

  //Raycast
  // Normalize the direction to just -1, 0, or 1 
  int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
  int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);
  //We start the check 1 grid ahead of the start square
  int checkX = startGridX + stepX;
  int checkY = startGridY + stepY;

  //Now we check if every square in the path is clear
  while (checkX != endGridX || checkY != endGridY) {
    if (boardState[checkY][checkX] != nullptr) {
      Serial.println("BOARD: Obstacle detected mid-path! Path blocked.");
      pathClear = false;
      break; // Stop looking
    }
    checkX += stepX;
    checkY += stepY;
  }

  //Checks if the end square is occupied 
  if (boardState[endGridY][endGridX] != nullptr) {
    Serial.println("BOARD: ERROR - Target square is occupied!");
    pathClear = false;
  }

  //---MOVE THE PIECE---
  Serial.println("BOARD: Magnet ON.");
  magnetON();
  delay(200); //Allow the magnet to fully engage

  //If the path is clear we can move to the end square, if not we must run "routeAlongSeams"
  if (pathClear == true) {
    Serial.println("BOARD: Path clear. Executing direct (straight) move.");
    executeCoreXYMovement(endMM_X, endMM_Y);
  } else {
    Serial.println("BOARD: Path blocked. Routing along physical seams.");
    routeAlongSeams(startMM_X, startMM_Y, endMM_X, endMM_Y);
  }

  //---MOVE FINISHED---
  Serial.println("BOARD: Magnet OFF..");
  magnetOFF();
  delay(200);
  disableMotors();

  Serial.println("BOARD: Piece delivered to target square.");
  return true; 
}

//Move knight, L-shape pathing
bool CoreXYController::moveKnightPiece(String startSquare, String endSquare) {
  Serial.print("BOARD: Initiating KNIGHT move from ");
  Serial.print(startSquare);
  Serial.print(" to ");
  Serial.println(endSquare);

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
  enableMotors();
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
  if (boardState[pivot1_Y][pivot1_X] == nullptr && boardState[endGridY][endGridX] == nullptr) {
    Serial.println("BOARD: Knight Path 1 (Diagonal First) is clear.");
    //Convert pivot grid to mm coords
    gridToMM(pivot1_X, pivot1_Y, pivotMM_X, pivotMM_Y);
    
    //Execute the staircase move: drive to the pivot, then to the end position
    executeCoreXYMovement(pivotMM_X, pivotMM_Y);
    executeCoreXYMovement(endMM_X, endMM_Y);
  } 
  
  //Tier 2: Check if Pivot 2 and the final target square are both empty
  else if (boardState[pivot2_Y][pivot2_X] == nullptr && boardState[endGridY][endGridX] == nullptr) {
    Serial.println("BOARD: Knight Path 2 (Orthogonal First) is clear.");
    //Convert pivot grid to mm coords
    gridToMM(pivot2_X, pivot2_Y, pivotMM_X, pivotMM_Y);
    
    //Execute the staircase move: drive to the pivot, then to the end position
    executeCoreXYMovement(pivotMM_X, pivotMM_Y);
    executeCoreXYMovement(endMM_X, endMM_Y);
  } 
  
  //Tier 3: Fallback if both intermediate pivot squares are blocked by pieces
  else {
    Serial.println("BOARD: Knight is boxed in. Routing along seams.");
    routeAlongSeams(startMM_X, startMM_Y, endMM_X, endMM_Y);
  }

  //---MOVE FINISHED---
  magnetOFF();
  delay(200);
  disableMotors();

  Serial.println("BOARD: Knight move complete.");
  return true;
}

//Executes the CoreXY movement
void CoreXYController::executeCoreXYMovement(float targetX, float targetY){
  //Calculate coreXY target mm
  float motorX_mm = targetX;
  float motorY_mm = targetY;

  //if (targetY > currentY) {
  //  motorY_mm += squareSizeMM; 
  //}

  //Convert mm to steps for motor
  long targetStepsA = round(motorX_mm * stepsPerMM_X);
  long targetStepsB = round(motorY_mm * stepsPerMM_Y);

  //Put into array for multiStepper
  long positions[2];
  positions[0] = targetStepsA; // Motor X
  positions[1] = targetStepsB; // Motor Y

  //DEBUGING
  Serial.print("   -> Driving Motors to: [");
  Serial.print(mmToAlgebraic(targetX, targetY));
  Serial.print("]  (X=");
  Serial.print(targetX);
  Serial.print("mm, Y=");
  Serial.print(targetY);
  Serial.println("mm)");

  //Call motors
  steppers.moveTo(positions);
  //Execute the move (BLOCKING)
  steppers.runSpeedToPosition();

  //Update new current position
  currentX = targetX;
  currentY = targetY;
}


void CoreXYController::updateBoardState(std::array<std::array<chessbot::piece*, 8>, 8> currentBoard) {
  boardState = currentBoard;
}

//Translates algebraic notation to grid indices (Assumes Home 0,0 is A1)
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

//Translates grid coordinates to raw physical millimeters
void CoreXYController::gridToMM(int gridX, int gridY, float &mmX, float &mmY) {
  //---AXIS INVERSION---
  //Set to -1 since the motor drives away from the board, relative to the A1 limit switch
  int xDirection = -1;
  int yDirection = 1;

  mmX = gridX * squareSizeMM * xDirection;
  mmY = gridY * squareSizeMM * yDirection;
}

//Helper: Reconstructs Algebraic notation for Debugging
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

//--- CAPTURE PIECE LOGIC ---
//Takes the targeted piece off the board to a fixed graveyard coordinate.
bool CoreXYController::capturePiece(String targetSquare) {
  Serial.print("BOARD: Executing CAPTURE on ");
  Serial.print(targetSquare);

  int targetGridX, targetGridY;
  float targetMM_X, targetMM_Y;

  //Get the coordinates of the piece we are capturing
  parseSquare(targetSquare, targetGridX, targetGridY);
  gridToMM(targetGridX, targetGridY, targetMM_X, targetMM_Y);

  //Define the graveyard coordinate
  float graveMM_X = -380.0; 
  float graveMM_Y = 350.0;

  //Move to the target piece
  enableMotors();
  magnetOFF();
  executeCoreXYMovement(targetMM_X, targetMM_Y);

  //Grab and move to graveyard
  Serial.println("BOARD: Magnet ON. Grabbing captured piece...");
  magnetON();
  delay(300);

  //Move along x axis towards graveyard
  //TODO: Implement raycasting here to ensure no other pieces are blocking the path to the X-edge.
  Serial.println("BOARD: Dragging piece off the board edge...");
  executeCoreXYMovement(graveMM_X, targetMM_Y);

  //Move into graveyard
  Serial.println("BOARD: Moving to graveyard...");
  executeCoreXYMovement(graveMM_X, graveMM_Y);

  //Drop piece
  Serial.println("BOARD: Magnet OFF. Piece eliminated.");
  magnetOFF();
  delay(200);

  disableMotors();
  return true;
}

//SEAM ROUTING, avoids collisions
void CoreXYController::routeAlongSeams(float startX, float startY, float targetX, float targetY) {
  Serial.println("BOARD: Calculating safe seam route...");

  float halfSquare = squareSizeMM / 2.0; 
  float seamStartX, seamStartY, seamEndX, seamEndY;

  //Determine the direction of travel
  int dirX = (targetX > startX) ? 1 : -1;
  int dirY = (targetY > startY) ? 1 : -1;

  //Moving straight along the Y-axis
  if (targetX == startX) {
    seamStartX = startX + halfSquare; //Move right to get on the line
    seamEndX = targetX + halfSquare;  //Stay on the line
    
    seamStartY = startY + (dirY * halfSquare); //Move towards target
    seamEndY = targetY - (dirY * halfSquare);  //Stop short of target center
  }

  //Moving straight along the X-axis
  else if (targetY == startY) {
    seamStartX = startX + (dirX * halfSquare); 
    seamEndX = targetX - (dirX * halfSquare);  
    
    seamStartY = startY + halfSquare; 
    seamEndY = targetY + halfSquare;  
  }

  //Moving Diagonally
  else {
    seamStartX = startX + (dirX * halfSquare);
    seamEndX = targetX - (dirX * halfSquare);
    seamStartY = startY + (dirY * halfSquare);
    seamEndY = targetY - (dirY * halfSquare);
  }

  //---START ROUTING ALONG SEAMS---

  //First diagonal shift to the starting square's corner
  Serial.println("BOARD: Routing Step 1 (To Grid Corner)");
  executeCoreXYMovement(seamStartX, seamStartY);
  delay(2000);
  //Next slide down the X-axis gridline
  Serial.println("BOARD: Routing Step 2 (Sliding X Seam)");
  delay(2000);
  executeCoreXYMovement(seamEndX, seamStartY);

  //THen move down the Y-axis gridline
  Serial.println("BOARD: Routing Step 3 (Sliding Y Seam)");
  delay(2000);
  executeCoreXYMovement(seamEndX, seamEndY);

  //Finaly a diagonal shift into the center of the target square
  Serial.println("BOARD: Routing Step 4 (Entering Target Center)");
  delay(2000);
  executeCoreXYMovement(targetX, targetY);
}  

//---HARDWARE CONTROL---

void CoreXYController::magnetON() {
  digitalWrite(magnet_Pin, LOW);  //Active Low
}
void CoreXYController::magnetOFF() {
  digitalWrite(magnet_Pin, HIGH);
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

