#line 2 "DisplayTests.ino"

#include <Arduino.h>
#include <AUnit.h>
#include <WString.h>
#include <Display.h>
#include <U8G2Display.h>

using aunit::TestRunner;
using aunit::Verbosity;
using namespace Display;
using namespace Display::Monochrome;

test(U8g2DisplayTests, Constructor)
{
    U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
    IDisplay *display = new U8G2Display(u8g2);

    assertNotEqual(nullptr, display);
}

test(U8g2DisplayTests, WriteLines)
{
    U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
    IDisplay *display = new U8G2Display(u8g2);

    String testLine1 = "A";
    String testLine2 = "B";
    String testLine3 = "C";

    int result = display->WriteLines(testLine1, testLine2, testLine3);
    
    // false indicates success
    assertEqual(0, result);
}

void setup()
{
#if ! defined(EPOXY_DUINO)
    delay(1000); // wait to prevent garbage on SERIAL_PORT_MONITOR
#endif
    SERIAL_PORT_MONITOR.begin(115200);
    while (!SERIAL_PORT_MONITOR); // needed for Leonardo/Micro

    TestRunner::setVerbosity(Verbosity::kAll);
}

void loop()
{
    TestRunner::run();
}
