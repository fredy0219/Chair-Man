#ifndef HighSpeedStepper_h
#define HighSpeedStepper_h
#include <Arduino.h>
#endif

class HighSpeedStepper
{
	public:
		HighSpeedStepper();
		HighSpeedStepper(uint8_t step_pin , uint8_t direction_pin , uint8_t enable_pin);
		~HighSpeedStepper();

		bool isMoving();

		uint8_t getDirection();
		void setDirection(uint8_t _direction);

		void setSteps(int _steps , int _speed , uint8_t _direction);
		int getSteps();

		int getCurrentSteps();

		void setSpeed(int _speed);
		int getSpeed();

		void run();
		void stop();
		void reset();

	private:
		uint8_t clk_pin; // clk for steps
		uint8_t cw_pin;  // cw for direction
		uint8_t en_pin; // enable

		uint8_t direction = 0; // 1:backward ,  0:forward

		int speed = 0; // user set speed
		int currentSpeed = 0; // check current speed
		int steps = 0; // user set steps
		int currentSteps = 0; // check current steps
		bool _isMoving = false; // is on movement status or not

		unsigned long mircoTimer; // use for decal mircosecond each pulse
		unsigned long targetTimer; 
		bool nextPulse = true; // true: calculate next timer , false: calculating timer

		float decelerate = 1.5f;

		

		void init();

		float fast_tanh_wave( int index){

			//unsigned long waveTimer = micros();

			float ex = 2.7828;
			float m = 1;
			float mapIndex = 0;

			if(index < steps*0.5)
    			mapIndex = map(index,steps*0.05,steps*0.1,1,-1);
			else if(index >= steps*0.5)
    			mapIndex = map(index,steps*0.9,steps*0.95,-1,1);

    		float result = (((pow(ex,m * 2*mapIndex)-1) / (pow(ex, m* 2*mapIndex)+1) +1) *0.5);

    		//unsigned long waveEndTimer = micros() - waveTimer;
    		//Serial.println(waveEndTimer);

			return result;
		}

		void cwWrite(uint8_t pin, bool value){

			if(value){
				switch(pin){
					case 27:
						PORTA |= _BV(5); break;
					case 41:
						PORTG |= _BV(0); break;
					case 49:
						PORTL |= _BV(0); break;
					case 33:
						PORTC |= _BV(4); break;
				}
			}else{
				switch(pin){
					case 27:
						PORTA &= ~_BV(5); break;
					case 41:
						PORTG &= ~_BV(0); break;
					case 49:
						PORTL &= ~_BV(0); break;
					case 33:
						PORTC &= ~_BV(4); break;
				}
			}
		}

		void clkWrite(uint8_t pin, bool value){

			if(value){
				switch(pin){
					case 25:
						PORTA |= _BV(3); break;
					case 39:
						PORTG |= _BV(2); break;
					case 47:
						PORTL |= _BV(2); break;
					case 31:
						PORTC |= _BV(6); break;
				}
			}else{
				switch(pin){
					case 25:
						PORTA &= ~_BV(3); break;
					case 39:
						PORTG &= ~_BV(2); break;
					case 47:
						PORTL &= ~_BV(2); break;
					case 31:
						PORTC &= ~_BV(6); break;
				}
			}
		}

		

};