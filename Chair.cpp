#include <Chair.h>

Chair::Chair(){
	stepper[0] = HighSpeedStepper(27,25,10);
	stepper[1] = HighSpeedStepper(41,39,10);
	stepper[2] = HighSpeedStepper(49,47,10);
	stepper[3] = HighSpeedStepper(33,31,10);

	sonar[0] = AccurateMaxsonar(A4);
	sonar[1] = AccurateMaxsonar(A7);
	sonar[2] = AccurateMaxsonar(A6);
	sonar[3] = AccurateMaxsonar(A5);

}

void Chair::init(){

	for(uint8_t i = 0 ; i < STEPPER_NUMBER ; i++)
		stepper_interrupt[i] = false;

	delay(5000);

	this->motion_init();

	delay(5000);

	this->activate_sonar();

}

bool Chair::isMoving(){

	if( !stepper[0].isMoving() && !stepper[1].isMoving() && !stepper[2].isMoving() && !stepper[3].isMoving())
		return false;
	else
		return true;
}

void Chair::run(){
	for(uint8_t i = 0 ; i < STEPPER_NUMBER ; i++)
		stepper[i].run();
}


bool Chair::isMotioning(){

	//Serial.println("isMotioning");
	if(current_motion != MOTION_NONE)
		return true;
	else
		return false;
}

void Chair::motion_correction(){

	Serial.print("motion_correction : ");
	Serial.println(current_motion);


	switch( current_motion ){
		case MOTION_SIT:      motion_sitdown();  break;
		case MOTION_STAND:    motion_standup();  break;
		case MOTION_LEFT:
		case MOTION_RIGHT:
		case MOTION_FORWARD:
		case MOTION_BACKWARD: motion_template(); break;
	}

	Serial.print(" current_motion ");
	Serial.println(current_motion);
}	

void Chair::run_kernel(){

	Serial.print("run_kernel");
	Serial.println(sonar_approching_table,BIN);

	switch(current_state){
		case STATE_MACHINE_BLOCK_1:
			decide_motion_before_standup(); break;
		case STATE_MACHINE_BLOCK_2:
			decide_motion_after_standup(); break;
	}
}

void Chair::decide_motion_before_standup(){

	//Serial.println("decide_motion_before_standup");
	//Serial.println(sonar_approching_table,BIN);
	update_sonar_table();

	if(sonar_approching_table != MOTION_NONE_TIGGER){
		current_motion = MOTION_STAND;
		current_state = STATE_MACHINE_BLOCK_2;
	}
}
void Chair::decide_motion_after_standup(){

	update_sonar_table();
	update_sonar_table();
	update_sonar_table();
	update_sonar_table();
	update_sonar_table();


	switch( sonar_approching_table ){

		case MOTION_NONE_TIGGER:
			current_motion = MOTION_SIT;
			current_state = STATE_MACHINE_BLOCK_1; // change state machine block to 0
			break;

		//--Normal motion decide

		case MOTION_SINGEL_TIGGER_ONE:
		case MOTION_TRIPLE_TIGGER_THREE:
			current_motion = MOTION_FORWARD; break;

		case MOTION_SINGEL_TIGGER_TWO:
		case MOTION_TRIPLE_TIGGER_FOUR:
			current_motion = MOTION_RIGHT; break;

		case MOTION_SINGEL_TIGGER_THREE:
		case MOTION_TRIPLE_TIGGER_TWO:
			current_motion = MOTION_BACKWARD; break;

		case MOTION_SINGEL_TIGGER_FOUR:
		case MOTION_TRIPLE_TIGGER_ONE:
			current_motion = MOTION_LEFT; break;
		//

		//--Special motion decide
			//  TWIN BEWTEEN
		case MOTION_TWIN_BEWTEEN_TIGGER_ONE_TWO:
			if(sonar[0].getSample() > sonar[1].getSample())
				current_motion = MOTION_RIGHT;
			else
				current_motion = MOTION_FORWARD;
			break;

		case MOTION_TWIN_BEWTEEN_TIGGER_TWO_THERE:
			if(sonar[1].getSample() > sonar[2].getSample())
				current_motion = MOTION_BACKWARD;
			else
				current_motion = MOTION_RIGHT;
			break;

		case MOTION_TWIN_BEWTEEN_TIGGER_THREE_FOUR:
			if(sonar[2].getSample() > sonar[3].getSample())
				current_motion = MOTION_LEFT;
			else
				current_motion = MOTION_BACKWARD;
			break;		

		case MOTION_TWIN_BEWTEEN_TIGGER_FOUR_ONE:
			if(sonar[3].getSample() > sonar[0].getSample())
				current_motion = MOTION_FORWARD;
			else
				current_motion = MOTION_LEFT;
			break;
			//

			//  TWIN SYMMETRY
		case MOTION_TWIN_SYMMETRY_TIGGER_ONE_THREE:
			if(sonar[1].getSample() > sonar[3].getSample())
				current_motion = MOTION_LEFT;
			else
				current_motion = MOTION_RIGHT;
			break;

		case MOTION_TWIN_SYMMETRY_TIGGER_TWO_FOUR:
			if(sonar[0].getSample() > sonar[2].getSample())
				current_motion = MOTION_BACKWARD;
			else
				current_motion = MOTION_FORWARD;
			break;	
			//	
		//	
	}
}

// -- Motion Function --
void Chair::motion_init(){
	// While power up, set to "SITTING"
	// Only call when init and emergence.

	delay(5000);

	if( !stepper_interrupt[0] )
		stepper[0].setSteps(500,3000,STEPPER_0_UP);

	if( !stepper_interrupt[1] )
		stepper[1].setSteps(500,3000,STEPPER_1_UP);

	if( !stepper_interrupt[2] )
		stepper[2].setSteps(800,3000,STEPPER_2_UP);

	if( !stepper_interrupt[3] )
		stepper[3].setSteps(800,3000,STEPPER_3_UP);


	while( !stepper_interrupt[0] || !stepper_interrupt[1] || !stepper_interrupt[2] || !stepper_interrupt[3]){
		this->run();
	}

	delay(1000);

	// move to init postion
	stepper[0].setSteps(15,3000,STEPPER_0_DOWN);
	stepper[1].setSteps(15,3000,STEPPER_1_DOWN);
	stepper[2].setSteps(15,3000,STEPPER_2_DOWN);
	stepper[3].setSteps(15,3000,STEPPER_3_DOWN);

	while( this->isMoving() ){
		this->run();
	}

	for(uint8_t i = 0 ; i < STEPPER_NUMBER ; i++)
		stepper_interrupt[i] = false;

	current_state = STATE_MACHINE_BLOCK_1; // switch to STATE_MACHINE_BLOCK_1
	_isEmergence = false;  // ensure _isEmergence reset
}
void Chair::motion_standup(){

	Serial.println("motion_standup");

	stepper[0].setSteps(440,3000,STEPPER_0_DOWN);
	stepper[1].setSteps(440,3000,STEPPER_1_DOWN);

	stepper[2].setSteps(180,3000,STEPPER_2_DOWN);
	stepper[3].setSteps(180,3000,STEPPER_3_DOWN);

	while(isMoving()){
		this->run();
	}

	current_motion = MOTION_NONE;
}
void Chair::motion_sitdown(){

	Serial.println("motion_sitdown");

	stepper[0].setSteps(440,3000,STEPPER_0_UP);
	stepper[1].setSteps(440,3000,STEPPER_1_UP);

	stepper[2].setSteps(180,3000,STEPPER_2_UP);
	stepper[3].setSteps(180,3000,STEPPER_3_UP);

	while( isMoving()){
		this->run();
	}

	current_motion = MOTION_NONE;
}
void Chair::motion_template(){

	Serial.println("motion_template");
	Serial.println(current_motion);

	static uint8_t max_split = 8;

	uint8_t split_motion_number = 0;
	unsigned long nextTimer = 0;

	int timer_table[] = {1000,1000,1000,1000,1000,1000,1000,1000};

	bool finish = false;
	while( !finish ){
		// Serial.println("motion_template");

		if( split_motion_number == max_split && !isMoving())
			finish = true;

		if( split_motion_number < max_split ){
			unsigned long t = millis() - nextTimer;
			
			if(t > timer_table[split_motion_number]){

				switch(current_motion){
					case MOTION_LEFT: motion_left( split_motion_number ); nextTimer = millis(); break;
					case MOTION_RIGHT: motion_right( split_motion_number ); nextTimer = millis(); break;
					case MOTION_FORWARD: motion_forward( split_motion_number ); nextTimer = millis(); break;
					case MOTION_BACKWARD: motion_backward( split_motion_number ); nextTimer = millis(); break;
				}
				split_motion_number++; 
			}
		}

		// Serial.print( "->");
		// Serial.print(current_motion);
		// Serial.print( "->");
		// Serial.print(split_motion_number);

		this->run();
	}

	Serial.println("template finish");

	current_motion = MOTION_NONE;

	return ;
}
void Chair::motion_left(uint8_t number){

	switch(number){
		case 0:
			stepper[0].setSteps(180,5000,STEPPER_0_UP);
			stepper[3].setSteps(180,5000,STEPPER_3_UP); break;
		case 1:
			stepper[1].setSteps(180,3000,STEPPER_1_UP);
			stepper[2].setSteps(180,3000,STEPPER_2_UP); break;
		case 2:
			stepper[0].setSteps(180,3000,STEPPER_0_DOWN);
			stepper[3].setSteps(180,3000,STEPPER_3_DOWN); break;
		case 3:
			stepper[1].setSteps(180,3000,STEPPER_1_DOWN);
			stepper[2].setSteps(180,3000,STEPPER_2_DOWN); break;
		case 4:
			stepper[0].setSteps(180,3000,STEPPER_0_UP);
			stepper[3].setSteps(180,3000,STEPPER_3_UP); break;
		case 5:
			stepper[1].setSteps(180,3000,STEPPER_1_UP);
			stepper[2].setSteps(180,3000,STEPPER_2_UP); break;
		case 6:
			stepper[0].setSteps(180,3000,STEPPER_0_DOWN);
			stepper[3].setSteps(180,3000,STEPPER_3_DOWN); break;
		case 7:
			stepper[1].setSteps(180,3000,STEPPER_1_DOWN);
			stepper[2].setSteps(180,3000,STEPPER_2_DOWN); break;
	}
}
void Chair::motion_right( uint8_t number ){

	switch(number){
		case 0:
			stepper[1].setSteps(180,5000,STEPPER_1_UP);
			stepper[2].setSteps(180,5000,STEPPER_2_UP); break;
		case 1:
			stepper[0].setSteps(180,3000,STEPPER_0_UP);
			stepper[3].setSteps(180,3000,STEPPER_3_UP); break;
		case 2:
			stepper[1].setSteps(180,3000,STEPPER_1_DOWN);
			stepper[2].setSteps(180,3000,STEPPER_2_DOWN); break;
		case 3:
			stepper[0].setSteps(180,3000,STEPPER_0_DOWN);
			stepper[3].setSteps(180,3000,STEPPER_3_DOWN); break;
		case 4:
			stepper[1].setSteps(180,3000,STEPPER_1_UP);
			stepper[2].setSteps(180,3000,STEPPER_2_UP); break;
		case 5:
			stepper[0].setSteps(180,3000,STEPPER_0_UP);
			stepper[3].setSteps(180,3000,STEPPER_3_UP); break;
		case 6:
			stepper[1].setSteps(180,3000,STEPPER_1_DOWN);
			stepper[2].setSteps(180,3000,STEPPER_2_DOWN); break;
		case 7:
			stepper[0].setSteps(180,3000,STEPPER_0_DOWN);
			stepper[3].setSteps(180,3000,STEPPER_3_DOWN); break;
	}
}
void Chair::motion_forward( uint8_t number ){

	switch(number){
		case 0:
			stepper[0].setSteps(220,5000,STEPPER_0_UP);
			stepper[1].setSteps(220,5000,STEPPER_1_UP); break;
		case 1:
			stepper[2].setSteps(180,3000,STEPPER_2_UP);
			stepper[3].setSteps(180,3000,STEPPER_3_UP); break;
		case 2:
			stepper[0].setSteps(220,3000,STEPPER_0_DOWN);
			stepper[1].setSteps(220,3000,STEPPER_1_DOWN); break;
		case 3:
			stepper[2].setSteps(180,3000,STEPPER_2_DOWN);
			stepper[3].setSteps(180,3000,STEPPER_3_DOWN); break;
		case 4:
			stepper[0].setSteps(220,3000,STEPPER_0_UP);
			stepper[1].setSteps(220,3000,STEPPER_1_UP); break;
		case 5:
			stepper[2].setSteps(180,3000,STEPPER_2_UP);
			stepper[3].setSteps(180,3000,STEPPER_3_UP); break;
		case 6:
			stepper[0].setSteps(220,3000,STEPPER_0_DOWN);
			stepper[1].setSteps(220,3000,STEPPER_1_DOWN); break;
		case 7:
			stepper[2].setSteps(180,3000,STEPPER_2_DOWN);
			stepper[3].setSteps(180,3000,STEPPER_3_DOWN); break;
	}
}
void Chair::motion_backward( uint8_t number ){

	switch(number){
		case 0:
			stepper[2].setSteps(180,5000,STEPPER_2_UP);
			stepper[3].setSteps(180,5000,STEPPER_3_UP); break;
		case 1:
			stepper[0].setSteps(220,3000,STEPPER_0_UP);
			stepper[1].setSteps(220,3000,STEPPER_1_UP); break;
		case 2:
			stepper[2].setSteps(180,3000,STEPPER_2_DOWN);
			stepper[3].setSteps(180,3000,STEPPER_3_DOWN); break;
		case 3:
			stepper[0].setSteps(220,3000,STEPPER_0_DOWN);
			stepper[1].setSteps(220,3000,STEPPER_1_DOWN); break;
		case 4:
			stepper[2].setSteps(180,3000,STEPPER_2_UP);
			stepper[3].setSteps(180,3000,STEPPER_3_UP); break;
		case 5:
			stepper[0].setSteps(220,3000,STEPPER_0_UP);
			stepper[1].setSteps(220,3000,STEPPER_1_UP); break;
		case 6:
			stepper[2].setSteps(180,3000,STEPPER_2_DOWN);
			stepper[3].setSteps(180,3000,STEPPER_3_DOWN); break;
		case 7:
			stepper[0].setSteps(220,3000,STEPPER_0_DOWN);
			stepper[1].setSteps(220,3000,STEPPER_1_DOWN); break;
	}
	
}

void Chair::interrupt(uint8_t index){

	if( current_state == STATE_MACHINE_BLOCK_0 ){
		if( !stepper_interrupt[index] ){
			Serial.println("init reset");
			stepper[index].reset(); // stop stepper
			stepper_interrupt[index] = true;

		}
	}else{
		if( !stepper_interrupt[index] ){

			Serial.println("all reset");
			stepper_all_reset();
			stepper_interrupt[index] = true;
			_isEmergence = true;
		}

	}
}

void Chair::stepper_all_reset(){

	for(uint8_t i = 0 ; i < STEPPER_NUMBER ; i++)
		stepper[i].reset();
}

bool Chair::isEmergence(){
	return _isEmergence;
}

void Chair::update_sonar_table()
{
	
	for(uint8_t i = 0 ; i < SONAR_NUMBER ; i++)
		if(sonar[i].isApproaching())
			bitWrite(sonar_approching_table , i ,1);
		else
			bitWrite(sonar_approching_table , i ,0);

	//Serial.println(sonar_approching_table,BIN);

}

void Chair::activate_sonar(){

	// Activate all sonar sensor
	pinMode(SONAR_TIGGER_PIN,OUTPUT);
	delay(200);
	digitalWrite(SONAR_TIGGER_PIN,HIGH);
	delayMicroseconds(25);
	digitalWrite(SONAR_TIGGER_PIN,LOW);
	pinMode(SONAR_TIGGER_PIN,INPUT);
	delay(50);

	//
	for (uint8_t i = 0; i < SONAR_NUMBER; ++i)
		sonar[i].initStack();
	
}
