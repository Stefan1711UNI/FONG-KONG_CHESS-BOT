#include <AccelStepper.h>
#include <MultiStepper.h>


class CoreXYController {
  public:
    //Constructor
    CoreXYController(float squareSize, float stepsPerMM);

    void setUp(int pinStepX, int pinDirX, int pinStepY, int pinDirY, int pinMagnet, int pinLimitX, int pinLimitY, int pinEnable);

    //Calibrate the stepper motors
    void calibrate();

    //Updates the 8x8 piece map
    void updateBoardState(byte currentBoard[8][8]);

    //Call when moving a single piece, excl. Knights
    bool movePiece(String startSquare, String endSquare);

    //Call when moving a knight
    bool moveKnightPiece(String startSquare, String endSquare);

    //Call when capturing a piece
    bool capturePiece(String targetSquare, int graveyardSlot);

    ////Helper: Converts raw millimeters back to algebraic notation for debugging
    String mmToAlgebraic(float mmX, float mmY);

  
  private:
    AccelStepper motorA;
    AccelStepper motorB;
    MultiStepper steppers;

    //Board State Memory
    byte boardState[8][8];  //1 = piece present, 0 = empty
    float currentX;         //Curent X position mm 
    float currentY;         //Curent Y position mm

    //Physical Parameters
    float squareSizeMM;         //Size of square in mm
    float stepsPerMM;           //How many steps to travel 1 mm

    // Hardware Pins
    int stepA_Pin, dirA_Pin;
    int stepB_Pin, dirB_Pin;
    int magnet_Pin;
    int limitX_Pin, limitY_Pin;
    int enable_Pin;

    //HELPER FUNCTIONS

    //Translates "E4" into a grid coordinate (Column 4, Row 3)
    void parseSquare(String square, int &gridX, int &gridY);

    //Translates grid coordinates into physical millimeters
    void gridToMM(int gridX, int gridY, float &mmX, float &mmY);

    //The Pathfinder: Calculates waypoint routing to avoid collisions
    void routeAlongSeams(float startX, float startY, float targetX, float targetY);

    //Executes the CoreXY movement (Blocking)
    void executeCoreXYMovement(float targetX, float targetY);

    //Magnet Control
    void magnetON();
    void magnetOFF();

    //Disables/Enables motors for power saving
    void enableMotors();
    void disableMotors();

};
