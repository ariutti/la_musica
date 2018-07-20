#include "Limulo_Animator.h"


// Set attack and release time in millisecond
// default are attack time (t_1) = 10ms and
// release time (t_2) = 500ms
void Animator_ASR::init( int t_1, int t_2 ) {
	t1 = t_1>0.0?t_1:10;
	t2 = t_2>0.0?t_2:500;
	state = QUIET;
}

void Animator_ASR::update()
{
	if(state == ATTACK)
	{
		dt = millis() - st;
    // TODO: insert a check here in order to prevent
    // cases where 'dt' become grater than t1.
    // Update st accordingly.
    //if( dt < 0 )
    //  Serial.println("ERROR");
		y =  (1.0*dt)/t1;
		if(y > 1.0)
		{
			y = 1.0;
			state = SUSTAIN;
		}
	}
	else if( state == RELEASE )
	{
		dt = millis() - st;
    // TODO: insert a check here in order to prevent
    // cases where 'dt' become grater than t2.
    // Update st accordingly.
		y =  1.0 - ( (1.0*dt)/t2 );
    if(y < 0.0)
		{
			y = 0.0;
			state = QUIET;
		}
	}
}

void Animator_ASR::triggerAttack()
{
	if(state == SUSTAIN)
	{
		return;
	}
	else if(state == QUIET)
	{
		st = millis();
	}
//	TODO: consider an attack trigger in the released phase.
//  this is not necessary. The release phase can be interrupted 
//  abruptly when an new attack comes in.
//	else if(state == RELEASE)
//	{
//	}
	

	state = ATTACK;
	st = millis();
}


void Animator_ASR::triggerRelease()
{
	if(state == QUIET)
	{
		return;
	}
	else if(state == SUSTAIN)
	{
		st = millis();
	}
	else if (state == ATTACK)
	{
    // mind the fact that st must be a 'long' in order not to 
    // fill the data container too fast.
		st = long( millis()*(1+((1.0*t2)/t1)) - t2*(1 + ((1.0*st)/t1)) );
	}
	state = RELEASE;
}

float Animator_ASR::getY()
{
	return y;
}

int Animator_ASR::getState()
{
	return state;
}


// remember:
//const int QUIET = -1;
//const int ATTACK  = 0;
//const int SUSTAIN  = 1;
//const int RELEASE = 2;
void Animator_ASR::printState()
{
  switch(state) {
    case QUIET:
      Serial.print("QUIET");
      break;
    case ATTACK:
      Serial.print("ATTACK");
      break;
    case SUSTAIN:
      Serial.print("SUSTAIN");
      break;
    case RELEASE:
      Serial.print("RELEASE");
      break;
    default:
      break;
  }
}
