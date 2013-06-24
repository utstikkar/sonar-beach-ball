#include <Wire.h>
#include <Adafruit_LSM303.h>
#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h> 
#include <SFEMP3Shield.h>
#include <SFE_TPA2016D2.h>

const int knockSensor = A0; // the piezo is connected to analog pin 0
const int threshold = 3;  // threshold value to decide when the detected vibration is a knock or not

const int EN_GPIO1 = A2; // Amp enable + MIDI/MP3 mode select

int sensorReading = 0;      // variable to store the value read from the sensor pin

SdFat sd;
SFEMP3Shield MP3player;

Adafruit_LSM303 lsm; // accelerometer/compass
int previousX = 0; // variable to store the previous value of x dimension
int previousY = 0; // variable to store the previous value of y dimension
int previousZ = 0; // variable to store the previous value of z dimension
int currentX; // variable to store the most recent value of x dimension
int currentY; // variable to store the most recent value of y dimension
int currentZ; // variable to store the most recent value of z dimension
boolean moving = false; // boolean 
int track; //number of the track currently playing

SFE_TPA2016D2 amp; // amplifier

void setup() 
{
  Serial.begin(9600);
  
  // Try to initialise and warn if we couldn't detect the chip
  if (!lsm.begin())
  {
    Serial.println("Oops ... unable to initialize the LSM303. Check your wiring!");
    while (1);
  }
  
  // The board uses a single I/O pin to select the
  // mode the MP3 chip will start up in (MP3 or MIDI),
  // and to enable/disable the amplifier chip:
  
  pinMode(EN_GPIO1,OUTPUT);
  digitalWrite(EN_GPIO1,LOW);  // MP3 mode / amp off

  // Turn on the Wire (I2C) library (amplifier control)
  Wire.begin();
  
  //start the shield
  sd.begin(SD_SEL, SPI_HALF_SPEED);
  MP3player.begin();
  
  //start playing a random track
  track = random(1, 7);
  digitalWrite(EN_GPIO1,HIGH); // amp on
  MP3player.playTrack(track);
  MP3player.setVolume(0,0); // set the volume on both channels to the highest value (0)
  
  
}

void loop() 
{
  // read the touch/knock sensor and store it in the variable sensorReading:
  sensorReading = analogRead(knockSensor);
  
  // if the sensor reading is greater than the threshold:
  if (sensorReading >= threshold) {
    // send the string "Knock!" back to the computer
    Serial.println("Knock!");
    // and play a random track
    track = random(1, 7);
    MP3player.stopTrack(); // stop the previous track first before playing a new one 
    digitalWrite(EN_GPIO1,HIGH);
    MP3player.playTrack(track);    
  }
  lsm.read();
  currentX = (int)lsm.accelData.x;
  currentY = (int)lsm.accelData.y;
  currentZ = (int)lsm.accelData.z;
  // if the difference between the previous and current value of the accelerometer on one of the dimensions at least is large enough
  if (abs(currentX - previousX) > 250 || abs(currentY - previousY) > 250 || abs(currentZ - previousZ) > 250)
  {
    moving = true; // then the beach ball is moving
    Serial.println("moving");
    MP3player.resumeMusic(); // play the music
  }else{
    MP3player.pauseMusic(); // otherwise pause
  }
  delay(500); //re-evaluate the sensors data every 500ms
  previousX = currentX;
  previousY = currentY;
  previousZ = currentZ;
  moving = false;
}
