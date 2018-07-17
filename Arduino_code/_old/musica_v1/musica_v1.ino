// here a simple sketch where we manage three
// micro switch with a simple structure that kepps track of:
// * pin number;
// * current status;
// * previous status;


// SWITCH STUFF /////////////////////////////////////////

const int pins[] = {18, 19, 20, 21, 22, 2, 3, 4, 5, 6};

#define NKEYS 10
struct key {
  uint8_t pin;
  uint8_t id;
  bool state;
  bool prevState;
} keys[ NKEYS ];


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
}


// LOOP ///////////////////////////////////////////////////////////////////////////////
void loop()
{

  // get Serial Data
  
  // read data from the switches
  
  
  for(int i=0; i<NKEYS; i++)
  {
    keys[i].state = digitalRead(keys[i].pin);
    if( keys[i].state != keys[i].prevState )
    {
      Serial.print(keys[i].id); Serial.print("-"); 
      Serial.print(keys[i].state); Serial.println(";"); 
      keys[i].prevState = keys[i].state;
    }
  }
  

  delay(25);
}
