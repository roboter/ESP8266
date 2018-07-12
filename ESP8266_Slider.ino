
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // legacy include: `#include "SSD1306.h"`
#include <A4988.h>
// using a 200-step motor (most common)
#define MOTOR_STEPS 200
#define MICROSTEPS 1
// configure the pins connected
#define DIR D4
#define STEP D0
A4988 stepper(MOTOR_STEPS, DIR, STEP);
SSD1306Wire  display(0x3c, D6, D5, GEOMETRY_128_32);

void setup() {
  stepper.begin(200, MICROSTEPS);
  ///pinMode(D0, OUTPUT); // ON ESP
  pinMode(D1, INPUT_PULLUP);
  pinMode(D2, INPUT_PULLUP);
  pinMode(D3, INPUT_PULLUP);
  //pinMode(D4, OUTPUT);// ON NODEMCU Board
  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setContrast(255);
}
float del = 1;
float i = 1;
int button1 = HIGH;
int button2 = HIGH;
int button3 = HIGH;
bool running = false;

void drawDisplay(int pos) {
  // put your main code here, to run repeatedly:

  display.clear();
  // Print to the screen
  // digitalWrite(D0,HIGH);
  // digitalWrite(D4,HIGH);

  display.drawRect(0, 12, 128, 10);
  display.fillRect(0, 12, pos, 10);

  if (running) {
    display.drawString(0, 20, "stop ");
    display.drawString(0, 0, "Delay: " + String((int)i));
  } else {
    display.drawString(0, 0, "Delay: " + String((int)pos));
  }
  display.display();
}

void loop() {
  button1 = digitalRead(D1);
  button2 = digitalRead(D2);
  button3 = digitalRead(D3);
  float one = 128 / (del + 1);
  int pos =  one * i;
  drawDisplay(running ? pos : (int)del % 128);

  digitalWrite(D0, LOW);
  digitalWrite(D4, LOW);
  if (button1 == LOW)
  {
    del--;
    i--;
    if (del < 1)del = 1;
    delay(10);
  }
  if (button2 == LOW)
  {
    del++;
    i++;
    delay(10);
  }

  if (button3 == LOW)
  {
    i = 0;
    running = !running;
    delay(1000);
    //stepper.dir_pin = HIGH;
    //stepper.rotate(360);
    //stepper.dir_pin = LOW;
    //stepper.rotate(-360);

  }
  if (i <= 0 && running)
  {
    i = del;
    stepper.move(1);
    stepper.disable();
  }
  if (running)
  {
    i = i - 0.1;
  }

}