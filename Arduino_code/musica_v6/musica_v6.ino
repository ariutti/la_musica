/*
 * Use an Arduino Leonardo.
 * This sketch is meant to be used with an Adafruit DotStarLED strip.
 * TODO: documentation here.
 * TODO: adda method to turn off all LEDs when a standby message is received
 */

// SWITCH STUFF /////////////////////////////////////////
#define NKEYS 10
#include "ButtonDebounce.h"

const int pins[] = {18, 19, 20, 21, 22, 2, 3, 4, 5, 6};

struct key 
{
  uint8_t pin;
  uint8_t id;
  // debouncing utility
  unsigned long delay;
  unsigned long lastDebounceTime;
  int lastStateBtn;
} keys[ NKEYS ];


String temp;
uint8_t key_index = 0;
uint8_t c_red   = 0;
uint8_t c_green = 0;
uint8_t c_blue  = 0;
bool bSendSerial = false;
bool bReadyToReleaseAll = true;

// LEDs /////////////////////////////////////////////////
#include "Adafruit_DotStar.h"
#include <SPI.h>

#define NUMPIXELS 100 // Number of LEDs in strip
const int PPK = 10; //PIXELS PER KEY

// Here's how to control the LEDs from any two pins:
#define DATAPIN    8
#define CLOCKPIN   7
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);
// GREEN, RED, BLUE
uint8_t r, g, b = 0;

// ANIMATORS ////////////////////////////////////////////
#include "Limulo_Animator.h"

const int FADEIN_TIME = 250;
const int FADEOUT_TIME = 100;
// the following is a factor to correct the overall brightness
const float BRIGHTNESS = 250;

struct ledblock 
{
  Animator_ASR asr;
  float rawAmp;
  uint8_t intAmp;
  uint8_t head, tail; // indexes of first/last pixel of the block
  uint8_t in, out; // indexes of the first/last pixel of the raised cosine animation
  uint8_t pivot;
  uint8_t r;
  uint8_t g;
  uint8_t b;
};
ledblock blocks[NKEYS];

// DEBUG STUFF //////////////////////////////////////////
bool DEBUG = false;

// DEBOUNCING UTILITY ///////////////////////////////////////////////////////////////
void button_update( int i )
{
  // if the debounce time has not been passed, return
  if( (millis() - keys[i].lastDebounceTime) < keys[i].delay ) return;
  keys[i].lastDebounceTime = millis();
  // get the button status
  int btnState = digitalRead( keys[i].pin );
  // if the status has not changed, return
  if(btnState == keys[i].lastStateBtn) return;
  // if we are here it means the current status of the button 
  // (after the debouncing time is changed!
  keys[i].lastStateBtn = btnState;
  
  if( !btnState )
  {
    if( bReadyToReleaseAll ){ releaseAll(); }; 
    blocks[i].asr.triggerAttack();
    if( bSendSerial )
    {
      printAllKeys();
      bReadyToReleaseAll = false;
      //Serial.print(keys[i].id); Serial.print("-"); 
      //Serial.print(keys[i].lastStateBtn); Serial.println(";");
    }
  }
  else
  {
    blocks[i].asr.triggerRelease(); 
    if( bSendSerial) 
    {
      printAllKeys();
      //Serial.print(keys[i].id); Serial.print("-"); 
      //Serial.print(keys[i].lastStateBtn); Serial.println(";");
    }
  }
}


// SETUP //////////////////////////////////////////////////////////////////////////////
void setup() 
{
  Serial.begin(9600);

  // SWITCHES STUFF ///////////////////////////////////////////////////////////////////
  // initilise the switches
  for(int i=0; i<NKEYS; i++)
  {
    keys[i].pin = pins[i];
    pinMode( keys[i].pin, INPUT );
    keys[i].id = i;
    keys[i].delay = 50;
    keys[i].lastDebounceTime = 0;
    keys[i].lastStateBtn = HIGH;
    
  }
  
  // ANIMATORS STUFF //////////////////////////////////////////////////////////////////
  for(int i=0; i<NKEYS; i++)
  {
    blocks[i].asr.init(FADEIN_TIME, FADEOUT_TIME);
    blocks[i].tail = i*PPK;
    blocks[i].head = blocks[i].tail + PPK - 1;   
    blocks[i].rawAmp = 0.0;
    blocks[i].intAmp = 0;
    setColor(i, 0x00, 0x00, 0xFF);   
  }

  // LEDs STUFF ///////////////////////////////////////////////////////////////////////
  strip.begin(); // Initialize pins for output


  // set colors 
//  setColor(0,   0,  27, 229);
//  setColor(1,   0, 127, 321);
//  setColor(2,   0, 230, 234);
//  setColor(3,   0, 237, 139);
//  setColor(4,   0, 240,  37);
//  setColor(5,  66, 243,   0);
//  setColor(6, 173, 246,   0);
//  setColor(7, 249, 215,   0);
//  setColor(8, 252, 108,   0);
//  setColor(9, 255,   0,   0);

  for(int i=0; i<NKEYS; i++) 
  {
    setColor(i, 0,0, 255);
  }

  
  // set every pixel to sleep
  for(int i=0; i<NKEYS; i++) 
  {
    for(int j=0; j<PPK; j++)
    {
      strip.setPixelColor(blocks[i].tail+j, r, g, b);
    }
  }
  strip.setBrightness( BRIGHTNESS );
  // Turn all LEDs off ASAP
  strip.show();  
}


// LOOP ///////////////////////////////////////////////////////////////////////////////
void loop() 
{
  // get serial information
  getSerialData();  

  // read the buttons
  for(int i=0; i<NKEYS; i++) 
  {
    button_update(i);
  }
  // Animators & LEDs stuff
  if(DEBUG) Serial.println();
  for(int i=0; i<NKEYS; i++)
  {
    //r = g = b = 0x00;
    
    blocks[i].asr.update();
    blocks[i].rawAmp = blocks[i].asr.getY();
    // better to square the 0.0 - 1.0 value in order to get a smoother transition
    blocks[i].rawAmp = blocks[i].rawAmp * blocks[i].rawAmp;
    blocks[i].intAmp = 255.0*blocks[i].rawAmp;
    if(DEBUG) { Serial.print( blocks[i].rawAmp );Serial.print(" "); }
    if(DEBUG) { Serial.print( blocks[i].intAmp );Serial.print(" "); }
    //if(DEBUG) {Serial.print( blocks[i].asr.getState() );Serial.print("\t");}
    if(DEBUG) { blocks[i].asr.printState();Serial.print("\t"); }
    r = blocks[i].rawAmp * blocks[i].r;
    g = blocks[i].rawAmp * blocks[i].g;
    b = blocks[i].rawAmp * blocks[i].b;
    for(int j=0; j<PPK; j++)
    {
      strip.setPixelColor(blocks[i].tail+j, r, g, b );
    }
  }
  //if(DEBUG) Serial.println();
  strip.show();
  
  delay(10);
}


// OTHER STUFF //////////////////////////////////////////////////////////////////////
void releaseAll()
{
  for(int i=0; i<NKEYS; i++)
  {
    blocks[i].asr.triggerRelease();
  }  
}


// PRINT ALL KEYS STATUS ////////////////////////////////////////////////////////////
void printAllKeys()
{
  for(int i=0; i<NKEYS; i++)
  {
    Serial.print( keys[i].lastStateBtn );
  }  
  Serial.println(";");
}


// SET COLOR ////////////////////////////////////////////////////////////////////////
void setColor(uint16_t i, uint8_t r, uint8_t g, uint8_t b)
{
  blocks[i].r = r;
  blocks[i].g = g;
  blocks[i].b = b; 
}


// SERIAL DATA //////////////////////////////////////////////////////////////////////
void getSerialData()
{
  while (Serial.available() > 0) 
  {
    char c = Serial.read();
    // the message from VVVV is finished
    if (c == ';')
    {
      //Serial.print( temp );
      //Serial.print( " - " );
      //Serial.println( temp.length() );
      if (temp.length() == NKEYS )
      {
        bReadyToReleaseAll = true;
        for(int i=0; i<NKEYS; i++) 
        {
          if(temp[i] == '1')
          {
            //Serial.print(i);
            //Serial.println(" ATTACK;");
            blocks[i].asr.triggerAttack();
          }
          else if(temp[i] == '0')
          {
            //Serial.print(i);
            //Serial.println(" RELEASE;");
            blocks[i].asr.triggerRelease();
          }
        }
        temp = "";
      }
      else if( temp.length() == 1 )
      {
        if (temp[0] == '1')
        {
          bSendSerial = true;
          temp = "";
        }
        else if (temp[0] == '0')
        {
          bSendSerial = false;
          temp = "";
        }
        else
        {
          temp = "";
        } 
      }
      else
      {
        temp = "";
      }
    } 
    else
    {
      // if the message from VVVV is not finished yet,
      // append character to 'temp' string
      temp += c;
    }
  }
}

/*
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
*/

