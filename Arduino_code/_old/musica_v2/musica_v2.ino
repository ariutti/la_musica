/*
 * Use an Arduino Leonardo.
 * This sketch is meant to be used with an Adafruit DotStarLED strip.
 * TODO: documentation here.
 * TODO: adda method to turn off all LEDs when a standby message is received
 */


// SWITCH STUFF /////////////////////////////////////////
#define NKEYS 10

const int pins[] = {18, 19, 20, 21, 22, 2, 3, 4, 5, 6};

struct key {
  uint8_t pin;
  uint8_t id;
  bool state;
  bool prevState;
} keys[ NKEYS ];


bool bSendSerial = true;

// LEDs /////////////////////////////////////////////////
#include <Adafruit_DotStar.h>
// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET
//#include <avr/power.h> // ENABLE THIS LINE FOR GEMMA OR TRINKET

#define NUMPIXELS 100 // Number of LEDs in strip
const int PPK = 10; //PIXELS PER KEY

// Here's how to control the LEDs from any two pins:
#define DATAPIN    8
#define CLOCKPIN   7
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
// GREEN, RED, BLUE

// ANIMATORS ////////////////////////////////////////////
#include <Limulo_Animator.h>

const int FADEIN_TIME = 250;
const int FADEOUT_TIME = 100;
// the following is a factor to correct the overall brightness
const float AMPLITUDE = 0.5;

struct ledblock {
  Animator_ASR asr;
  float rawAmp;
  uint8_t intAmp;
  uint8_t head, tail; // indexes of first/last pixel of the block
  uint8_t in, out; // indexes of the first/last pixel of the raised cosine animation
  uint8_t pivot;
};
ledblock blocks[NKEYS];


// utility variables 
int i=0, j=0;


// DEBUG STUFF //////////////////////////////////////////
bool DEBUG = false;



// SETUP //////////////////////////////////////////////////////////////////////////////
void setup() 
{
  Serial.begin(9600);

  // initilise the switches
  for(int i=0; i<NKEYS; i++)
  {
    
    keys[i].pin = pins[i];
    pinMode( keys[i].pin, INPUT );
    keys[i].id = i;
    keys[i].state = true;
    keys[i].prevState = true;
    
  }
  
  // LED & ANIMATORS
  for(i=0; i<NKEYS; i++)
  {
    blocks[i].asr.init(FADEIN_TIME, FADEOUT_TIME);
    blocks[i].tail = i*PPK;
    blocks[i].head = blocks[i].tail + PPK - 1;   
    blocks[i].rawAmp = 0.0;
    blocks[i].intAmp = 0;
  }

  strip.begin(); // Initialize pins for output

  // set every pixel to sleep
  for(i=0; i<NKEYS; i++) 
  {
    for(j=0; j<PPK; j++)
    {
      strip.setPixelColor(blocks[i].tail+j, 0x00, 0x00, 0x00);
    }
  }
  strip.show();  // Turn all LEDs off ASAP
}


// LOOP ///////////////////////////////////////////////////////////////////////////////
void loop() 
{
  getSerialData();  

  // SWITCHES stuff //////////////////////////////////////
  for(int i=0; i<NKEYS; i++)
  {
    keys[i].state = digitalRead(keys[i].pin);
    if( keys[i].state != keys[i].prevState )
    {
      Serial.print(keys[i].id); Serial.print("-"); 
      Serial.print(keys[i].state); Serial.println(";"); 
      keys[i].prevState = keys[i].state;

      if( !keys[i].state )
        blocks[i].asr.triggerAttack();
      else
        blocks[i].asr.triggerRelease();
      
    }
  }


  // LED stuff ///////////////////////////////////////////  
  if(DEBUG) Serial.println();
  for(i=0; i<NKEYS; i++)
  {
    blocks[i].asr.update();
    blocks[i].rawAmp = blocks[i].asr.getY();
    // better to sqare the 0.0 - 1.0 value in order to get a smoother transition
    blocks[i].rawAmp = blocks[i].rawAmp * blocks[i].rawAmp;
    blocks[i].intAmp = 255.0*blocks[i].rawAmp * AMPLITUDE;
    if(DEBUG) {Serial.print( blocks[i].rawAmp );Serial.print(" ");}
    if(DEBUG) {Serial.print( blocks[i].intAmp );Serial.print(" ");}
    if(DEBUG) {Serial.print( blocks[i].asr.getState() );Serial.print("\t");}
    for(j=0; j<PPK; j++)
    {
      strip.setPixelColor(blocks[i].tail+j, 0x00, 0x00, blocks[i].intAmp);
    }
  }
  if(DEBUG) Serial.println();
  strip.show();
  
  delay(10);
}


void releaseAll()
{
  for(int i=0; i<NKEYS; i++)
  {
    blocks[i].asr.triggerRelease();
  }  
}

// SERIAL UTILITY FUNCTIONS /////////////////////////////////////////////////////////
void getSerialData()
{
  if(Serial.available()) {
    char user_input = Serial.read(); // Read user input and trigger appropriate function
    
    if( user_input == '1' )
    {
      bSendSerial = true;
    }
    else if( user_input == '0' )
    {
      bSendSerial = false;
    }
  }
}

