#define SWITCH A0
int value = 0;
void setup() 
{
  Serial.begin(9600);  
}

void loop() 
{
  value = analogRead(SWITCH);
  Serial.println(value);
  delay(1);

}
