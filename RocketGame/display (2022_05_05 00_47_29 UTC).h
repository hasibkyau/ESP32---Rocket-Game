#ifndef H_A
#define H_A

#include <Arduino.h> //needed for Serial.println
//#include "weatherStation.h"

#include <Wire.h>
#include <Adafruit_GFX.h> //
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4 // not used nicht genutzt bei diesem Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

//Adafruit_SSD1306 display(OLED_RESET); Treiber vor Version1.2
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/*******************************************************************************************************************/

int esp32LED = 1;//Blue LED on board at TX0 = Pin 1

int count = 0;
int dir = 0;

const int unit = 200;

int threshold = 30;
bool touch0detected = false;
bool touch2detected = false;

const int debounceDelay = 20;

void gotTouch0() {
  touch0detected = true;
}

void gotTouch2() {
  touch2detected = true;
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  touchAttachInterrupt(T3, gotTouch0, threshold);
  touchAttachInterrupt(T4, gotTouch2, threshold);
  // pinMode(32, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)

  display.display();  //showing the Adafruit logo
  //do not forget display.display(); otherwise the picture will not be visible
  delay(100); //waiting 10ms

  display.clearDisplay();

  //loading screen
  display.setTextColor(WHITE);

  char title[] = "CrappyBird";
  char title2[] = "Xtreme";


  display.setCursor(0, 50);
  display.print("Modified by Hasib");
  display.setTextSize(2);
  display.setCursor(0, 5);

  for (int i = 0; i < 11; i++) {
    display.print(title[i]);
    display.display();
    delay(50);
  }
  display.setCursor(0, 24);

  for (int i = 0; i < 7; i++) {
    display.print(title2[i]);
    display.display();
    delay(50);
  }
  display.setTextSize(1);
  delay(2000);

  const int buttonPin = 14;     //the number of the pushbutton pin
  pinMode(buttonPin, INPUT_PULLUP);

  int score = 0;
  int gapPos = 5;  //upper ycoord of the obstacle gap size, can be randomized for increased game difficulty
  int gapSize = 15; //obstacle gap size, can be randomized for increased game difficulty
  int xcoord = display.width(); //Position of the obstacle (Init: 128)
  int ycoord = display.height() / 2; //Altitude of the Bird (Init: 16 = 32/2)

  while (true) { //could as well be put in void loop(){}... see explanation below
    //drawing the picture


    display.clearDisplay();
    display.setCursor(0, 0); //Score position
    display.println(score); //Score
    display.drawRect(15, ycoord, 10, 5, WHITE);  //Bird
    display.fillRect(xcoord, 0, 5, gapPos, WHITE);  //Upper obstacle
    display.fillRect(xcoord, gapPos + gapSize, 5, display.height() - (gapPos + gapSize), WHITE);  //Lower obstacle

    /*else if(gapPos == 0)
      {
      dir = (dir ? 0 : 1);
      }*/
    display.display();
    count++;
    if (count == 2)
    {
      count = 0;
    }

    if (gapPos == (display.height() - 1 - 20)) {
      dir = 1;
    }
    if (gapPos == 0) {
      dir = 0;
    }

    if (dir == 0)
    {
      if (count == 0)
      {
        gapPos++;
      }
    }

    if (dir == 1)
    {
      if (count == 0)
      {
        gapPos--;
      }
    }



    //calculating obstacle hit
    if ((xcoord > 15) && (xcoord < 25)) //Bird and obstacles have the same x-position
    {
      if ((ycoord < gapPos) || (ycoord + 5) > (gapPos + gapSize)) //Bird is not in the gap
      {
        display.setCursor(30, 12);
        display.print("Game Over");
        display.display();
        delay(1000);
        score = 0;
        //Resetting the Birds initial position
        xcoord = display.width();
        ycoord = display.height() / 2;
      }
    }

    //calculating obstacle
    xcoord--;
    if (xcoord < 0)
    {
      xcoord = 128; //resetting the obstacles' position to the far right
      score++;  //increasing score, when obstacle passed
    }

    //calculating Bird
    /*bool buttonState = digitalRead(buttonPin);  //reading input
      if (buttonState == true) {
      ycoord--;
      } else {
      ycoord++;
      }*/

    if (touch0detected) {
      touch0detected = false;

      ycoord--;
    }

    if (touch2detected) {
      touch2detected = false;
      ycoord++;
    }


    //Serial.print("ycoord");
    // Serial.println(ycoord);

    //preventing Bird from escaping
    if (ycoord < 1) {
      ycoord = 1;
    }
    if (ycoord >= display.height() - 5)
    {
      ycoord = display.height() - 5;

    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //Personally, i do not really like the concept of separated setup and loop (JM)

}
