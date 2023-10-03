#include <WString.h>
#include <Arduino.h>
#include "TestDI.h"

void TestDI::DoTheTest()
{
    Serial.println("In TestDI DoTheTest");
    String line1 = "hey";
    String line2 = "there";
    String line3 = "dude";
    
    display->WriteLines(line1, line2, line3);
}

TestDI::TestDI(IDisplay* display)
{
    Serial.println("In the testDI constructor");
    this->display = display;
}
