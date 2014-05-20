#include <Servo.h>
#include <SumoBase.h>

#define SERDEBUG NoDebug // SensorStateRead, SensorValueRead

Servo servor;
Servo servol;
Servo turn_servo;
Servo push_rear_left;
Servo push_rear_right;

//----------------------------------------- Sensors ----------------------------------------------
// line sensors
U16 f_left;
U16 f_right;
U16 r_left;
U16 r_right;

// line detection
#define detect_line_fl() (f_left >= 100)//((f_left >= 100)? True : False)
#define detect_line_fr() (f_right >= 100)//((f_right >= 100)? True : False)
#define detect_line_rl() (r_left >= 100)//((r_left >= 100)? True : False)
#define detect_line_rr() (r_right >= 100)//((r_right >= 100)? True : False)

// oponent sensors
U8 detect_front_left   = 1;
U8 detect_front_center = 1;
U8 detect_front_right  = 1;
U8 detect_rear_left    = 1;
U8 detect_rear_center  = 1;
U8 detect_rear_right   = 1;

// Arrays of sensor detection state
bool SensorData[SensorCount];
bool SensorDataOld[SensorCount];

Boolean ReadSensors()
{
    Boolean changeInSensorData = False;
    unsigned int i = 0;
    
    f_right = analogRead(FRight); // white: >100, black:20<
    f_left  = analogRead(FLeft);  // white: >400, black:40<
    r_left  = analogRead(RLeft);  // white: >100, black:20<
    r_right = analogRead(RRight); // white: >100, black:20<

    detect_front_left   = digitalRead(front_left);
    detect_front_center = digitalRead(front_center);
    detect_front_right  = digitalRead(front_right);
    detect_rear_left    = digitalRead(rear_left);
    detect_rear_center  = digitalRead(rear_center);
    detect_rear_right   = digitalRead(rear_right);
    
    // set sensor detection state
    SensorData[LnForLeft]   = detect_line_fl();
    SensorData[LnBackLeft]  = detect_line_rl();
    SensorData[LnBackRight] = detect_line_rr();
    SensorData[LnForRight]  = detect_line_fr();
    SensorData[OpForCenter] = ((0 == detect_front_center)? True : False);
    SensorData[OpForLeft]   = ((0 == detect_front_left)? True : False);
    SensorData[OpLeft]      = ((0 == detect_rear_left)? True : False);
    SensorData[OpBack]      = ((0 == detect_rear_center)? True : False);
    SensorData[OpRight]     = ((0 == detect_rear_right)? True : False);
    SensorData[OpForRight]  = ((0 == detect_front_right)? True : False);
    
    for(i = 0; i < sizeof(SensorData); i++)
    {
        if(SensorData[i] != SensorDataOld[i])
        {
            changeInSensorData = True;
            break;
        }
    }
    
    // debug code
    #if SERDEBUG == SensorStateRead
    for(i = 0;i < (SensorCount - 1); i++)
    {
        Serial.print(SensorData[i]);
        Serial.print(" ");
    }
    Serial.println(SensorData[i]);
    #endif
    
    // debug code
    #if SERDEBUG == SensorValueRead
    Serial.print(f_right);
    Serial.print(" ");
    Serial.print(f_left);
    Serial.print(" ");
    Serial.print(r_left);
    Serial.print(" ");
    Serial.println(r_right);
    #endif
    
    return changeInSensorData;    
}

void CopyToOldSensorData()
{
    for(i = 0; i < sizeof(SensorData); i++)
    {
        SensorDataOld[i] = SensorData[i];
    }
}

//----------------------------------------- Motors ----------------------------------------------

void SetMotorSpeed(U16 leftBig, U16 rightBig, U8 directionServo, U16 leftSmall, U16 rightSmall)
{
    servol.writeMicroseconds(leftBig);
    servor.writeMicroseconds(rightBig);

    turn_servo.write(directionServo);
    push_rear_left.writeMicroseconds(leftSmall);
    push_rear_right.writeMicroseconds(rightSmall);
}

#define Forward()    SetMotorSpeed(2000, 1000, TurnForward,  1000, 2000)
#define Back()       SetMotorSpeed(1000, 2000, TurnForward,  2000, 1000)
#define SpinLeft()   SetMotorSpeed(1000, 1000, TurnLeft,     1000, 2000)
#define SpinRight()  SetMotorSpeed(2000, 2000, TurnRight,    1000, 2000)
#define ArkLeft()    SetMotorSpeed(2000, 1000, SemiTurnLeft, 1000, 2000)
#define ArkRight()   SetMotorSpeed(2000, 1000, SemiTurnRight,1000, 2000)
#define Stop()       SetMotorSpeed(1500, 1500, SemiTurnRight,1500, 1500)

void ExcuteDecision(Decision decision)
{
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
}

//----------------------------------------- Logic ----------------------------------------------

//DecisionHistory decisionHistory;
Decision decision = dNoDecision;
DecisionPack* lastDecision;
DecisionHistory history;
unsigned long time = 0;

Decision DecisionBasedOnLine()
{
    Decision result = dNoDecision;
    // first level decision
    if(False != SensorData[LnForLeft])
    {
        result = dTurnRight;
    }
    else if(False != SensorData[LnBackLeft])
    {
        result = dTurnLeft;
    }
    else if(False != SensorData[LnBackRight])
    {
        result = dTurnRight;
    }
    else if(False != SensorData[LnForRight])
    {
        result = dTurnLeft;
    }
    else if((False != SensorData[LnBackRight]) && 
            (False != SensorData[LnBackLeft]))
    {
        result = dGoForward;
    }
    else if((False != SensorData[LnForRight]) && 
            (False != SensorData[LnForLeft]))
    {
        result = dGoBack;
    }
    else
    {
        result = dGoForward;
    }
    
    // the index of the history array always points to the next free cell
    lastDecision = GetDecision(1, &history);
    AddDecision(result, &history);
    time = millis();
    
    // seckond level decision
    if((time - lastDecision->timeStamp) < 2000)
    {
        switch(lastDecision->decision)
        {
        case dTurnLeft:
            //if(SensorDataOld[
        }
    }
    
    // AddDecisionToHistory(result, &decisionHistory);
    
    return result;
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
    
    /*
    servol.writeMicroseconds(1375); // sreda1 
    servor.writeMicroseconds(1361); // sreda2
    */

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
    
    memset((void *)&SensorData, 0, sizeof(SensorData));
    memset((void *)&SensorData, 0, sizeof(SensorData));
    memset((void *)&history, 0, sizeof(SensorData));
    
    //Debugging
    #if SERDEBUG != NoDebug
    Serial.begin(9600);
    #endif
}

/*
void StartTimer(U32 *timerAddr, U32 timerDelay)
{
    *timerAddr = ((millis()) + timerDelay);
}

U8 IsTimerExpired(U32 timer)
{
    if((False != IsTimerStarted(timer)) && (timer < (millis())))
    {
        return True;
    }
    
    return False;
}

U8 IsTimerStarted(U32 timerAddr)
{
    if(0 != timerAddr)
    {
        return True;
    }
    else
    {
        return False;
    }
}
*/

void loop()
{
    if(False != (ReadSensors()))
    {
        decision = DecisionBasedOnLine();
        ExcuteDecision(decision);
        CopyToOldSensorData();
    }
}
