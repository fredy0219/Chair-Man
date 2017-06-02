#include <HighSpeedStepper.h>

HighSpeedStepper::HighSpeedStepper()
{
}

HighSpeedStepper::HighSpeedStepper(uint8_t step_pin , uint8_t direction_pin ,uint8_t enable_pin)
{
	clk_pin = step_pin;
	cw_pin = direction_pin;
	en_pin = enable_pin;

	init();
}

HighSpeedStepper::~HighSpeedStepper()
{

}

void HighSpeedStepper::init()
{
	pinMode(clk_pin, OUTPUT);
	pinMode(cw_pin, OUTPUT);
	pinMode(en_pin, OUTPUT);

	digitalWrite(clk_pin, LOW);
	digitalWrite(cw_pin, LOW);
	digitalWrite(en_pin, LOW);
}

bool HighSpeedStepper::isMoving()
{
	return _isMoving;
}

uint8_t HighSpeedStepper::getDirection()
{
	return direction;
}

void HighSpeedStepper::setDirection(uint8_t _direction)
{
	if( direction != _direction)
		direction = _direction;

	digitalWrite(cw_pin , direction);
}

void HighSpeedStepper::setSteps(int _steps, int _speed, uint8_t _direction)
{

	steps = _steps;
	currentSteps = steps;
	speed = _speed;
	direction = _direction;

	if(direction == 0)
		clkWrite(cw_pin,LOW);
	if(direction == 1)
		clkWrite(cw_pin,HIGH);

	_isMoving = true;
}

int HighSpeedStepper::getCurrentSteps()
{
	return currentSteps;
}
int HighSpeedStepper::getSteps()
{
	return steps;
}

void HighSpeedStepper::setSpeed(int _speed)
{
	speed = _speed;
}

int HighSpeedStepper::getSpeed()
{
	return currentSpeed;
}


/*-- Stepper run as below digram
 *               ______________ 
 *    speed |   /              \
 *          |  /                \
 *          | /                  \
 *          |___________________________
 *                                  step
 */

void HighSpeedStepper::run()
{	
	if(currentSteps == 0 && _isMoving == true){
		_isMoving = false;
		currentSpeed = 0;
	}

	// if(isMoving){
	// 	if(nextPulse){
	// 		targetTimer = micros();

	// 		mircoTimer = fast_tanh_wave(currentSteps) * speed + speed;
	// 		//Serial.println(mircoTimer);
	// 		currentSpeed = mircoTimer;
	// 		nextPulse = false;

	// 		digitalWrite(clk_pin,HIGH);

	// 	}else{
	// 		if(micros() - mircoTimer > targetTimer){
	// 			digitalWrite(clk_pin,LOW);
	// 			nextPulse = true;
	// 			currentSteps--;
	// 		}
	// 	}
	// }

	if(_isMoving)
	{
		if(nextPulse){

			targetTimer = micros();

			unsigned long waveTimer = micros();

			if(currentSteps < steps*0.1)
				mircoTimer = map(currentSteps ,0 ,steps*0.1 ,speed * decelerate ,speed);
			else if(currentSteps > steps*0.9)
				mircoTimer = map(currentSteps ,steps*0.9 ,steps,speed ,speed * decelerate);
			else
				mircoTimer = speed;

			// unsigned long waveEndTimer = micros() - waveTimer;
   			// Serial.println(waveEndTimer);

			//Serial.println(mircoTimer);
			currentSpeed = mircoTimer;
			nextPulse = false;

			cwWrite(clk_pin , HIGH);

		}else{
			if(micros() - mircoTimer > targetTimer){ 

				cwWrite(clk_pin , LOW);

				nextPulse = true;
				currentSteps--;
			}
		}
	}

	
}

void HighSpeedStepper::reset(){
	_isMoving = false;
	nextPulse = true;
	speed = 0; // user set speed
	currentSpeed = 0; // check current speed
	steps = 0; // user set steps
	currentSteps = 0; // check current steps
}

void HighSpeedStepper::stop()
{

	digitalWrite(en_pin , HIGH);
	_isMoving = false;
	nextPulse = true;
	speed = 0; // user set speed
	currentSpeed = 0; // check current speed
	steps = 0; // user set steps
	currentSteps = 0; // check current steps
}