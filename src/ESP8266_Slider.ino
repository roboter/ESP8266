#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Arduino.h>
#include <SPI.h>
#include <menu.h>
#include <menuIO/adafruitGfxOut.h>
#include <menuIO/chainStream.h>
#include <menuIO/keyIn.h>

using namespace Menu;

#define BTN_SEL D2  // Select button
#define BTN_UP D1   // Up
#define BTN_DOWN D3 // Down

Adafruit_PCD8544 gfx(D6, -1, -1); // 48x84
//#define LEDPIN D4

result showEvent(eventMask e, navNode &nav, prompt &item) { return proceed; }

int test = 55;
int ledCtrl = LOW;

result myLedOn() {
  ledCtrl = HIGH;
  return proceed;
}
result myLedOff() {
  ledCtrl = LOW;
  return proceed;
}

result alert(menuOut &o, idleEvent e);
result doAlert(eventMask e, prompt &item);

TOGGLE(ledCtrl, setLed, "Led: ", doNothing, noEvent,
       noStyle //,doExit,enterEvent,noStyle
       ,
       VALUE("On", HIGH, doNothing, noEvent),
       VALUE("Off", LOW, doNothing, noEvent));

char *constMEM hexDigit MEMMODE = "0123456789ABCDEF";
char *constMEM hexNr[] MEMMODE = {"0", "x", hexDigit, hexDigit};
char buf1[] = "0x11";

MENU(mainMenu, "ESP8266 Timer", doNothing, noEvent, wrapStyle,
     // FIELD(test, "Test", "%", 0, 100, 10, 1, doNothing, noEvent, wrapStyle),
     SUBMENU(setLed), OP("LED On", myLedOn, enterEvent),
     OP("LED Off", myLedOff, enterEvent),
     // OP("Alert test", doAlert, enterEvent),
     EDIT("Hex", buf1, hexNr, doNothing, noEvent, noStyle), EXIT("<Back"));

// define menu colors --------------------------------------------------------
//  {{disabled normal,disabled selected},{enabled normal,enabled selected,
//  enabled editing}}
// monochromatic color table
const colorDef<uint16_t> colors[] MEMMODE = {
    {{WHITE, BLACK}, {WHITE, BLACK, BLACK}}, // bgColor
    {{BLACK, WHITE}, {BLACK, WHITE, WHITE}}, // fgColor
    {{BLACK, WHITE}, {BLACK, WHITE, WHITE}}, // valColor
    {{BLACK, WHITE}, {BLACK, WHITE, WHITE}}, // unitColor
    {{BLACK, WHITE}, {WHITE, WHITE, WHITE}}, // cursorColor
    {{BLACK, WHITE}, {WHITE, BLACK, BLACK}}, // titleColor
};

#define gfxWidth 84
#define gfxHeight 48
#define fontX 6
// 5
#define fontY 9
#define MAX_DEPTH 2

// encoderIn<encA, encB> encoder; // simple quad encoder driver
// encoderInStream<encA, encB> encStream(encoder,4); // simple quad encoder fake
// Stream

// a keyboard with only one key as the encoder button
// keyMap encBtn_map[] = {{-encBtn,defaultNavCodes[enterCmd].ch}}; // negative
// pin numbers use internal
// pull-up, this is on when low
// keyIn<1> encButton(encBtn_map);      // 1 is the number of keys

keyMap joystickBtn_map[] = {
    {-BTN_SEL, defaultNavCodes[enterCmd].ch},
    {-BTN_UP, defaultNavCodes[upCmd].ch},
    {-BTN_DOWN, defaultNavCodes[downCmd].ch},
};
keyIn<3> joystickBtns(joystickBtn_map);

MENU_INPUTS(in, &joystickBtns);

#define MAX_DEPTH 2
#define textScale 1
MENU_OUTPUTS(out, MAX_DEPTH,
             ADAGFX_OUT(gfx, colors, fontX, fontY,
                        {0, 0, gfxWidth / fontX, gfxHeight / fontY}),
             NONE);

NAVROOT(nav, mainMenu, MAX_DEPTH, in, out);

result alert(menuOut &o, idleEvent e) {
  if (e == idling) {
    o.setCursor(0, 0);
    o.print(F("alert test"));
    o.setCursor(0, 1);
    o.print(F("press [select]"));
    o.setCursor(0, 2);
    o.print(F("to continue..."));
  }
  return proceed;
}

result doAlert(eventMask e, prompt &item) {
  nav.idleOn(alert);
  return proceed;
}

// when menu is suspended
result idle(menuOut &o, idleEvent e) {
  o.setCursor(0, 0);
  o.print(F("suspended..."));
  o.setCursor(0, 1);
  o.print(F("press [select]"));
  o.setCursor(0, 2);
  o.print(F("to continue"));
  return proceed;
}

void setup() {
  joystickBtns.begin();
  nav.idleTask = idle; // point a function to be used when menu is suspended
  SPI.begin();
  gfx.begin();
  gfx.setRotation(2);
  gfx.clearDisplay();
  gfx.display(); // show splashscreen
}

void loop() {
  nav.doInput();
  if (nav.changed(0)) { // only draw if changed
    nav.doOutput();
    gfx.display();
  }
}
