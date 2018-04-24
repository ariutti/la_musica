#define SWITCH 3
#define FORK 2

// we assume the key is at rest 
//when we start the sketch
bool status1, prevStatus1 = true; //inverse logic
bool status2, prevStatus2 = true;


// SETUP /////////////////////////////////////////////////
void setup() 
{
  Serial.begin(9600);
  pinMode(SWITCH, INPUT);
  pinMode(FORK, INPUT);
}


// DRAW //////////////////////////////////////////////////
void loop() 
{
  status1 = digitalRead(SWITCH);
  status2 = digitalRead(FORK);
  
  if( (!status1 && prevStatus1) || (status1 && !prevStatus1) ){
    printSwitches();
    prevStatus1 = status1;
  }

  if( (!status2 && prevStatus2) || (status2 && !prevStatus2) ){
    printSwitches();
    prevStatus2 = status2;
  }
   
  delay(1);
}


void printSwitches()
{
  // remember: we have an inverse logic here.
  Serial.print(!status1);
  Serial.print(",");
  Serial.print(!status2);
  Serial.println();
}

