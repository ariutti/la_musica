/*
 * Use an Arduino Leonardo.
 * This sketch is meant to be used with an Adafruit DotStarLED strip.
 * TODO: documentation here.
 */

// LEDs /////////////////////////////////////////////////
#include <Adafruit_DotStar.h>
// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET
//#include <avr/power.h> // ENABLE THIS LINE FOR GEMMA OR TRINKET

#define NUMPIXELS 16 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN    6
#define CLOCKPIN   7
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
// GREEN, RED, BLUE

// ANIMATORS ////////////////////////////////////////////
#include <Limulo_Animator.h>

const int PPP = 4; //PIXELS PER PAD
const int NPADS = 4;
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
ledblock blocks[NPADS];


// utility variables 
int i=0, j=0;



// SWITCH STUFF /////////////////////////////////////////
#define NSWITCHES 3
struct uswitch {
  uint8_t pin;
  bool curState;
  bool prevState;
};
uswitch uswitches[NSWITCHES];


// DEBUG STUFF //////////////////////////////////////////
bool DEBUG = false;

// SETUP //////////////////////////////////////////////////////////////////////////////
void setup() 
{
  Serial.begin(9600);

  // SWITCHES //////////////////
  uswitches[0].pin = 8;
  uswitches[1].pin = 9;
  uswitches[2].pin = 10;
  for(int i=0; i<NSWITCHES; i++)
  {
    pinMode( uswitches[i].pin, INPUT);
    uswitches[i].curState = false;
    uswitches[i].prevState = false;
  }
  
  // LED & ANIMATORS
  for(i=0; i<NPADS; i++)
  {
    blocks[i].asr.init(FADEIN_TIME, FADEOUT_TIME);
    blocks[i].tail = i*PPP;
    blocks[i].head = blocks[i].tail + PPP - 1;   
    blocks[i].rawAmp = 0.0;
    blocks[i].intAmp = 0;
  }

  strip.begin(); // Initialize pins for output

  // set every pixel to sleep
  for(i=0; i<NPADS; i++) 
  {
    for(j=0; j<PPP; j++)
    {
      strip.setPixelColor(blocks[i].tail+j, 0x00, 0x00, 0x00);
    }
  }
  strip.show();  // Turn all LEDs off ASAP
}


// LOOP ///////////////////////////////////////////////////////////////////////////////
void loop() 
{

  // SERIAL COM stuff ////////////////////////////////////
  // is ther something waiting to be read on the serial port?
  if( Serial.available() >0 )
  {
    byte b = Serial.read();
    /*
    b = b - 48;
    //Serial.println( b );
    if(b < NPADS && b >= 0)
    {
      releaseAll();
      blocks[b].asr.triggerAttack();
    }*/
  }
  

  // SWITCHES stuff //////////////////////////////////////
  if(DEBUG) Serial.print("\n");
  for(int i=0; i<NSWITCHES; i++)
  {
    uswitches[i].curState = digitalRead(uswitches[i].pin);
    if( uswitches[i].curState != uswitches[i].prevState )
    {
      if(DEBUG) {Serial.print("["); Serial.print(uswitches[i].pin);}
      if(DEBUG) {Serial.print(" - "); Serial.print(uswitches[i].curState);}
      if(DEBUG) {Serial.print("]\t");}
      
      if(!uswitches[i].curState)
        blocks[i].asr.triggerAttack();
      else
        blocks[i].asr.triggerRelease();
        
      uswitches[i].prevState = uswitches[i].curState;
    }
  }
  if(DEBUG) Serial.print("\n");


  // LED stuff ///////////////////////////////////////////  
  if(DEBUG) Serial.println();
  for(i=0; i<NPADS; i++)
  {
    blocks[i].asr.update();
    blocks[i].rawAmp = blocks[i].asr.getY();
    // better to sqare the 0.0 - 1.0 value in order to get a smoother transition
    blocks[i].rawAmp = blocks[i].rawAmp * blocks[i].rawAmp;
    blocks[i].intAmp = 255.0*blocks[i].rawAmp * AMPLITUDE;
    if(DEBUG) {Serial.print( blocks[i].rawAmp );Serial.print(" ");}
    if(DEBUG) {Serial.print( blocks[i].intAmp );Serial.print(" ");}
    if(DEBUG) {Serial.print( blocks[i].asr.getState() );Serial.print("\t");}
    for(j=0; j<PPP; j++)
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
  for(int i=0; i<NPADS; i++)
  {
    blocks[i].asr.triggerRelease();
  }  
}

