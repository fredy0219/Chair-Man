#ifndef AccurateMaxsonar_h
#define AccurateMaxsonar_h
#include <Arduino.h>
#endif

class AccurateMaxsonar
{
	public:
		AccurateMaxsonar();
		AccurateMaxsonar(uint8_t pin, uint8_t sample_size = 9, uint8_t threadhold = 300);
		~AccurateMaxsonar();
		void initStack();

		bool isApproaching();
		int getRange();
		int getSample();
		void readSample();
		void sortSmaple();

	private:
		uint8_t pin;
		uint8_t sample_size;
		uint8_t threadhold;
    	uint8_t ad_sample_delay = 10;
    	int* sample;
    	int* sonar_stack;


    	int readSensor();

    	static float toCentimeters(float inches)
		{
        	return 2.54 * inches;
   		};



};	