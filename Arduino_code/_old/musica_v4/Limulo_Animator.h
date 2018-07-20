/*
* Animator.h - a library to create smooth linear variation
* on a value according to a customizable ASR envelope.
* Created by Nicola Ariutti, March 06, 2018
* TODO: license
*/
#ifndef _LIMULO_ANIMATOR
#define _LIMULO_ANIMATOR
#include "Arduino.h"

class Animator_ASR
{
public:
	Animator_ASR() {};

	void init( int t_1, int t_2 );
  void update();
  void triggerAttack();
  void triggerRelease();
  float getY();
	int getState();
  void printState();
 

private:

  // note that time variables must be 
  // long in order to store great values.
	int t1, t2;  // t attack, t release
	long dt;      // delta time
	long st;      // start time

  // states
  enum {
    QUIET = -1,
    ATTACK,
    SUSTAIN,
    RELEASE
  } state;

  float y;
};
#endif
