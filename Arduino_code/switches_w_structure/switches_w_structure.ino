// here a simple sketch where we manage three
// micro switch with a simple structure that kepps track of:
// * pin number;
// * current status;
// * previous status;


// SWITCH STUFF /////////////////////////////////////////
#define NSWITCHES 3
struct uswitch {
  uint8_t pin;
  bool curState;
  bool prevState;
};
uswitch uswitches[NSWITCHES];

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
}


// LOOP ///////////////////////////////////////////////////////////////////////////////
void loop() 
{
  // SWITCHES
  Serial.print("\n");
  for(int i=0; i<NSWITCHES; i++)
  {
    uswitches[i].curState = digitalRead(uswitches[i].pin);
    if( uswitches[i].curState != uswitches[i].prevState )
    {
      Serial.print("["); Serial.print(uswitches[i].pin);
      Serial.print(" - "); Serial.print(uswitches[i].curState);
      Serial.print("]\t");
      uswitches[i].prevState = uswitches[i].curState;
    }
  }
  Serial.print("\n");

  delay(100);
}
