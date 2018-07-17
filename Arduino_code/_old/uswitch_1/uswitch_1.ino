#define SWITCH 3
#define FORK 2

// we assume the key is at rest 
//when we start the sketch
bool status1, status2 = true;
bool prevStatus1 = status1;
bool prevStatus2 = status2;
bool sentVelocity = false;

long firstTime, secondTime, elapsedTime = 0;
int velocity = 0;


// SETUP /////////////////////////////////////////////////
void setup() 
{
  Serial.begin(9600);
  pinMode(SWITCH, INPUT);
  pinMode(FORK, INPUT);
  firstTime = millis();
}


// DRAW //////////////////////////////////////////////////
void loop() 
{
  status1 = digitalRead(SWITCH);
  status2 = digitalRead(FORK);
  
  if(!status1 && !prevStatus1 ) {
    firstTime = millis();
    prevStatus1 = true;
    //Serial.println("check 1");
  } else if(status1 && prevStatus1) {
    prevStatus1 = false;
    //Serial.println("UNcheck 1");
    firstTime = 0;
  }

  if(!status2 && !prevStatus2 ) {
    secondTime = millis();
    prevStatus2 = true;
    //Serial.println("check 2");
  } else if(status2 && prevStatus2) {
    prevStatus2 = false;
    //Serial.println("UNcheck 2");
    secondTime = 0;
  }
  
  if(firstTime>0 && secondTime>0 && !sentVelocity)
  {
    //Serial.println(firstTime);
    //Serial.println(secondTime);
    //Serial.println();
    elapsedTime = constrain( secondTime - firstTime, 0, 2000);
    velocity = map(elapsedTime, 0, 2000, 127, 0);
    Serial.println("NOTE ON: ");
    Serial.println(velocity);
    sentVelocity = true;
  } else if (firstTime == 0 && secondTime == 0 && sentVelocity) {
    Serial.println("NOTE OFF");
    sentVelocity = false;
  }
   
  delay(1);
}
