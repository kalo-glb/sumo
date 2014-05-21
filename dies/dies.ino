#include <Servo.h>
#include <SumoBase.h>
#include <SumoSensors.h>

#define SERDEBUG  // NoDebug, SensorStateRead, SensorValueRead, PrintLnDataAge, bla

Servo servor;
Servo servol;
Servo turn_servo;
Servo push_rear_left;
Servo push_rear_right;

//----------------------------------------- Sensors ----------------------------------------------

extern bool OptSensorData[OptDataMaxAge][OptSensorCount];
extern unsigned long OptDataAge[OptDataMaxAge];

//----------------------------------------- Motors ----------------------------------------------

void SetMotorSpeed(U16 leftBig, U16 rightBig, U8 directionServo, U16 leftSmall, U16 rightSmall)
{
    servol.writeMicroseconds(leftBig);
    servor.writeMicroseconds(rightBig);

    turn_servo.write(directionServo);
    push_rear_left.writeMicroseconds(leftSmall);
    push_rear_right.writeMicroseconds(rightSmall);
}

#define Forward()      SetMotorSpeed(2000, 1000, TurnForward,  1000, 2000)
#define Back()         SetMotorSpeed(1000, 2000, TurnForward,  2000, 1000)
#define SpinLeft()     SetMotorSpeed(1000, 1000, TurnLeft,     1000, 2000)
#define SpinRight()    SetMotorSpeed(2000, 2000, TurnRight,    1000, 2000)
#define ArkLeft()      SetMotorSpeed(2000, 1000, SemiTurnLeft, 1000, 2000)
#define ArkRight()     SetMotorSpeed(2000, 1000, SemiTurnRight,1000, 2000)
#define ArkBackLeft()  SetMotorSpeed(1000, 2000, SemiTurnLeft, 2000, 1000)
#define ArkBackRight() SetMotorSpeed(1000, 2000, SemiTurnRight,2000, 1000)
#define Stop()         SetMotorSpeed(1500, 1500, TurnForward,  1500, 1500)

bool executingDecision = false;

void ExcuteDecision(Decision decision)
{
    //Serial.println('a');
    //if((HasLnTimeoutEpired()) || (false == executingDecision))
    //{
        //Serial.println('a');
        switch(decision)
        {
        case dTurnLeft:
            SpinLeft();
            break;
        case dTurnRight:
            SpinRight();
            break;
        case dGoForward:
            Forward();
            break;
        case dArkLeft:
            ArkLeft();
            break;
        case dArkRight:
            ArkRight();
            break;
        case dGoBack:
            Back();
            break;
        case dNoDecision:
            Stop();
            break;
        case DecisionCount:
            Stop();
            break;
        default:
            Stop();
            break;
        }
        
        //executingDecision = true;
    //}
}

//----------------------------------------- Logic ----------------------------------------------

//DecisionHistory decisionHistory;
Decision decision = dGoForward;
Decision prevDecision = dGoForward;
DecisionPack* lastDecision;
DecisionHistory history;
unsigned long decisionTimeout = 0;
unsigned long lastDecisionTime = 0;

Decision DecisionBasedOnLine()
{
    Decision result = dNoDecision;
    // first level decision
    if((False != OptSensorData[0][LnBackRight]) && 
       (False != OptSensorData[0][LnBackLeft]))
    {
        result = dGoForward;
    }
    else if((False != OptSensorData[0][LnForRight]) && 
            (False != OptSensorData[0][LnForLeft]))
    {
        result = dGoBack;
    }
    else if((False != OptSensorData[0][LnForLeft]) && 
            (False != OptSensorData[0][LnBackLeft]))
    {
        result = dTurnRight;
    }
    else if((False != OptSensorData[0][LnForRight]) && 
            (False != OptSensorData[0][LnBackRight]))
    {
        result = dTurnLeft;
    }
    else if(False != OptSensorData[0][LnForLeft])
    {
        result = dTurnRight;
    }
    else if(False != OptSensorData[0][LnBackLeft])
    {
        if(((dArkRight == prevDecision) ||
            (dTurnRight == prevDecision)) &&
           ((lastDecisionTime + 2000) < Now()))
        {
            result = dGoForward;
        }
        else
        {
            result = dTurnLeft;
        }
    }
    else if(False != OptSensorData[0][LnForRight])
    {
        result = dTurnLeft;
    }
    else if(False != OptSensorData[0][LnBackRight])
    {
        if(((dArkLeft == prevDecision) ||
            (dTurnLeft == prevDecision)) &&
           ((lastDecisionTime + 2000) < Now()))
        {
            result = dGoForward;
        }
        else
        {
            result = dTurnRight;
        }
    } 
    else
    {
        result = dGoForward;
    }
    
    prevDecision = result;
    lastDecisionTime = Now();

    //SetLnDecisionTimeout(500);
    
    return result;
}

void SetLnDecisionTimeout(unsigned int time)
{
    if(false == executingDecision)
    {
        decisionTimeout = (Now() + time);
    }
}

bool HasLnTimeoutEpired()
{
    if(decisionTimeout < Now())
    {
        executingDecision = false;
        return true;
    }
    else
    {
        return false;
    }
}

Decision DecisionBasedOnOponent()
{
    
}

void setup()
{ 
    servol.attach(5);
    servor.attach(6);
    turn_servo.attach(TurningServo);
    push_rear_left.attach(3);
    push_rear_right.attach(11);

    push_rear_left.writeMicroseconds(1369);  // sreda1 
    push_rear_right.writeMicroseconds(1381); // sreda2

    // oponent
    pinMode(front_left, INPUT);
    pinMode(front_right, INPUT);
    pinMode(front_center, INPUT);
    pinMode(rear_left, INPUT);
    pinMode(rear_right, INPUT);
    pinMode(rear_center, INPUT);
    
    // line
    pinMode(FLeft, INPUT);
    pinMode(RLeft, INPUT);
    pinMode(RRight, INPUT);
    pinMode(FRight, INPUT);
    
    memset((void *)&OptSensorData, 0, sizeof(OptSensorData));
    
    //Debugging
    //Serial.begin(9600);
}

void loop()
{
    //delay(5);
    if(False != (ReadSensors()))
    {
        //Serial.println('a');
        decision = DecisionBasedOnLine();
        //decision = dArkLeft;
        ExcuteDecision(decision);
        AgeSensorData();
        delay(400);
    }
}
