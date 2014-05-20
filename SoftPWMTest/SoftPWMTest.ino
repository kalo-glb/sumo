#include <SoftPWM.h>

void setup()
{
    SoftPWMBegin();
    SoftPWMSet(1, 127);
}

void loop()
{
    //SoftPWMSet(2, 127);
}
