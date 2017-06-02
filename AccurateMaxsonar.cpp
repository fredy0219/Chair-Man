#include "AccurateMaxsonar.h"

AccurateMaxsonar::AccurateMaxsonar(){

}

AccurateMaxsonar::AccurateMaxsonar(uint8_t pin , uint8_t sample_size ,uint8_t threadhold):pin(pin),sample_size(sample_size),threadhold(threadhold)
{
	pinMode(pin, INPUT);
    sample = new int[sample_size];
    sonar_stack = new int[5];
}
AccurateMaxsonar::~AccurateMaxsonar(){
	
}

void AccurateMaxsonar::initStack()
{
	for(uint8_t i = 0 ; i < 5 ; i++)
		getRange();
}

bool AccurateMaxsonar::isApproaching()
{
	return (getRange() < threadhold);
}

int AccurateMaxsonar::getRange()
{
	readSample();

	for(uint8_t i = 0 ; i < 5-1 ; i++)
		sonar_stack[i] = sonar_stack[i+1];
	sonar_stack[5] = getSample();

	int average = 0;
	for(uint8_t i = 0 ; i < 5 ; i++)
		average += sonar_stack[i];

	return average / 5;
}

int AccurateMaxsonar::getSample()
{
	return sample[ sample_size /2 ];
}
void AccurateMaxsonar::readSample()
{
	// read
	for (int i = 0; i < sample_size; i++) {
        sample[i] = readSensor();

        delay(ad_sample_delay);
    }

    // sort
    for (int i = 1; i < sample_size; i++) {
        float j = sample[i];
        int k;

        for (k = i - 1; (k >= 0) && (j < sample[k]); k--)
            sample[k + 1] = sample[k];

        sample[k + 1] = j;
    }
}

int AccurateMaxsonar::readSensor()
{
    return (int)toCentimeters(analogRead(pin) / 2.0);;
}