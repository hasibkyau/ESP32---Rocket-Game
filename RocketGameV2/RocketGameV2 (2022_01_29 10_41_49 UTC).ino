// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

//display section
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


void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case MPU6050_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case MPU6050_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case MPU6050_RANGE_16_G:
      Serial.println("+-16G");
      break;
  }


  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
      Serial.println("260 Hz");
      break;
    case MPU6050_BAND_184_HZ:
      Serial.println("184 Hz");
      break;
    case MPU6050_BAND_94_HZ:
      Serial.println("94 Hz");
      break;
    case MPU6050_BAND_44_HZ:
      Serial.println("44 Hz");
      break;
    case MPU6050_BAND_21_HZ:
      Serial.println("21 Hz");
      break;
    case MPU6050_BAND_10_HZ:
      Serial.println("10 Hz");
      break;
    case MPU6050_BAND_5_HZ:
      Serial.println("5 Hz");
      break;
  }

  Serial.println("");
  delay(100);

  //display section
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)

  display.display();  //showing the Adafruit logo
  //do not forget display.display(); otherwise the picture will not be visible
  delay(100); //waiting 10ms

  display.clearDisplay();

  //loading screen
  display.setTextColor(WHITE);

  char title[] = "Journey to";
  char title2[] = "The March";
  display.setCursor(0, 50);
  display.print("By Hasib:Robo Squad");
  delay(2000);
  display.setTextSize(2);
  display.setCursor(0, 5);

  for (int i = 0; i < 11; i++) {
    display.print(title[i]);
    display.display();
    delay(50);
  }
  display.setCursor(0, 24);

  for (int i = 0; i < 9; i++) {
    display.print(title2[i]);
    display.display();
    delay(50);
  }
  display.setTextSize(1);
  delay(2000);
  display.clearDisplay();
  display.setCursor(30, 12);
  display.print("Level 1");
  display.display();
  delay(1000);


  const int buttonPin = 14;     //the number of the pushbutton pin
  pinMode(buttonPin, INPUT_PULLUP);

  int level = 1;
  int score = 0;
  int gapPos = 5;  //upper ycoord of the obstacle gap size, can be randomized for increased game difficulty
  int gapSize = 15; //obstacle gap size, can be randomized for increased game difficulty
  int xcoord = display.width(); //Position of the obstacle (Init: 128)
  int ycoord = display.height() / 2; //Altitude of the Bird (Init: 16 = 32/2)

  while (true) { //could as well be put in void loop(){}... see explanation below
    /* Get new sensor events with the readings */
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    /* Print out the values */
    float joystickX = a.acceleration.x;
    int joystickY = a.acceleration.y;
    float joystickZ = a.acceleration.z;

    //Serial.print("JoyStick: ");
    //Serial.print(joystickX);
    //Serial.print(" Y: ");
    //Serial.print(joystickY);
    //Serial.print(" Z: ");
    //Serial.print(joystickZ);

    //drawing the picture


    display.clearDisplay();
    display.setCursor(0, 0); //Score position
    display.println(score); //Score
     display.setCursor(100, 0); //Score position
      display.print("L:"); //Score
      display.println(level);
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

    if (score < 10) {
      xcoord--;
    }
    if (score >= 10)
    {
      level = 2;
      xcoord = xcoord - 2;
    }
    if (score >= 20)
    {
      level = 3;
      xcoord = xcoord - 3;
    }
    if (score >= 30)
    {
      level = 4;
      xcoord = xcoord - 4;
    }
     if (score >= 40)
    {
      level = 5;
      xcoord = xcoord - 5;
    }
    if (score == 50)
    {
      display.setCursor(15, 6);
      display.print("Mission Completed");
      display.setCursor(40, 64);
      display.print("Landed on March");
      display.display();
      delay(1000);
      score = 0;
      //Resetting the Birds initial position
      xcoord = display.width();
      ycoord = display.height() / 2;
    }
    if (xcoord < 0)
    {
      xcoord = 128; //resetting the obstacles' position to the far right
      score++;  //increasing score, when obstacle passed
      gapPos = random(0, 32);
    }

    if ( joystickZ < 0.0 && joystickZ > -0.5)
    {
      Serial.println(" Rocket: steady");
    }
    if ( joystickZ > 0.0)
    {
      Serial.println(" Rocket: Slowly up");
      ycoord++;
    }
    if ( joystickZ < -0.5)
    {
      Serial.println(" Rocket: Slowly down");
      ycoord--;
    }

    if (touch0detected) {
      touch0detected = false;
    }

    if (touch2detected) {
      touch2detected = false;
    }

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

}
