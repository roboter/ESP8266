#include <spi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#define UP_BUTTON D1
#define SELECT_BUTTON D2
#define DOWN_BUTTON D3
//#define RESET_BUTTON D4 //BUILDINLED ON ESPMODULE is in same PIN
#define LED D4 // LED On ESP8266 module

#define SHUTTER D8
#define FOCUS D0

boolean backlight = false;
int timer_delay = PWMRANGE;

int menuitem = 1;
int page = 1;
//#define  BACKLIGHT  D0
volatile boolean up = false;
volatile boolean down = false;
volatile boolean middle = false;

int downButtonState = 0;
int upButtonState = 0;
int resetButtonState = 0;
int selectButtonState = 0;
int lastDownButtonState = 0;
int lastSelectButtonState = 0;
int lastUpButtonState = 0;

Adafruit_PCD8544 display = Adafruit_PCD8544(D6, -1, -1); //48x84

void checkIfDownButtonIsPressed()
{
	if (downButtonState != lastDownButtonState)
	{
		if (downButtonState == 0)
		{
			down = true;
		}
		delay(50);
	}
	lastDownButtonState = downButtonState;
}

void checkIfUpButtonIsPressed()
{
	if (upButtonState != lastUpButtonState)
	{
		if (upButtonState == 0) {
			up = true;
		}
		delay(50);
	}
	lastUpButtonState = upButtonState;
}

void checkIfSelectButtonIsPressed()
{
	if (selectButtonState != lastSelectButtonState)
	{
		if (selectButtonState == 0) {
			middle = true;
		}
		delay(50);
	}
	lastSelectButtonState = selectButtonState;
}
void SetMenuColor(bool currentMenu)
{
	display.setTextColor(currentMenu ? WHITE : BLACK, currentMenu ? BLACK : WHITE);
}


void drawMenu()
{
	if (page == 1)
	{
		display.setTextSize(1);
		display.clearDisplay();
		display.setTextColor(BLACK, WHITE);
		display.setCursor(0, 0);
		display.print("ESP8266 Timer");
		display.drawFastHLine(0, 10, 83, BLACK);
		display.setCursor(0, 15);

		SetMenuColor(menuitem == 1);

		display.print("Delay: ");
		display.print(timer_delay);
		display.setCursor(0, 25);

		SetMenuColor(menuitem == 2);
		display.print("Light: ");
		display.print(backlight ? "ON" : "OFF");

		display.display();

		SetMenuColor(menuitem == 3);
		display.setCursor(0, 35);
		display.print("Start");
		display.display();
	}
	else if (page == 2)
	{
		display.setTextSize(1);
		display.clearDisplay();
		display.setTextColor(BLACK, WHITE);
		display.setCursor(15, 0);
		display.print("DELAY");
		display.drawFastHLine(0, 10, 83, BLACK);
		display.setCursor(5, 15);
		display.print("Value");
		display.setTextSize(2);
		display.setCursor(5, 25);
		display.print(timer_delay);

		display.setTextSize(2);
		display.display();
	}
}

void resetDefaults()
{
	timer_delay = PWMRANGE;
	backlight = false;
}

void setup() {

	pinMode(UP_BUTTON, INPUT_PULLUP);
	pinMode(SELECT_BUTTON, INPUT_PULLUP);
	pinMode(DOWN_BUTTON, INPUT_PULLUP);
//	pinMode(RESET_BUTTON, INPUT_PULLUP);
	pinMode(LED, OUTPUT);

	Serial.begin(115200);

	display.begin();
	display.setRotation(2);
	display.clearDisplay();
	display.display();
	drawMenu();
}

void loop() {
	downButtonState = digitalRead(DOWN_BUTTON);
	selectButtonState = digitalRead(SELECT_BUTTON);
	upButtonState = digitalRead(UP_BUTTON);
//	resetButtonState = digitalRead(RESET_BUTTON);

	checkIfDownButtonIsPressed();
	checkIfUpButtonIsPressed();
	checkIfSelectButtonIsPressed();

	if (up && page == 1) {
		up = false;
		menuitem--;
		if (menuitem == 0)
		{
			menuitem = 3;
		}
		drawMenu();
	}
	else if (up && page == 2) {
		up = false;
		timer_delay--;
		if (timer_delay < 0) timer_delay = 0;
		drawMenu();
	}

	if (down && page == 1) {
		down = false;
		menuitem++;
		if (menuitem == 4)
		{
			menuitem = 1;
		}
		drawMenu();
	}
	else if (down && page == 2) {
		down = false;
		timer_delay++;
		if (timer_delay > PWMRANGE) timer_delay = PWMRANGE;

		drawMenu();
	}
	// if (!resetButtonState) {
	// 	    resetDefaults();
	// 	    drawMenu();
	// }

	if (middle) {
		middle = false;

		if (page == 1 && menuitem == 2)
		{
			if (backlight)
			{
				backlight = false;
			}
			else
			{
				backlight = true;
			}
		}

		if (page == 1 && menuitem == 3)
		{
			resetDefaults();
		}
		else if (page == 1 && menuitem == 1) {
			page = 2;
		}
		else if (page == 2) {
			page = 1;
		}
		drawMenu();
	}
	digitalWrite(LED, backlight?HIGH:LOW);
}
