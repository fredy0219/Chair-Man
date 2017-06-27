#ifndef AccurateMaxsonar_h
#define AccurateMaxsonar_h
#include <Arduino.h>
#endif

class AccurateMaxsonar
{
	public:
		AccurateMaxsonar();
		AccurateMaxsonar(uint8_t pin, uint8_t sample_size = 9, int threadhold = 50);
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
		int threadhold;
    	uint8_t ad_sample_delay = 10;
    	int* sample;
    	int* sonar_stack;


    	int readSensor();

    	static float toCentimeters(float inches)
		{
        	return 2.54 * inches;
   		};



};	