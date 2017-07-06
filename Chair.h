#ifndef Chair_h
#define Chair_h
#include <Arduino.h>
#include <HighSpeedStepper.h>
#include <AccurateMaxsonar.h>

#endif


//	Stepper define parameters
#define CM_STEP_SIZE 27 
#define STEPPER_NUMBER 4
#define STEPPER_0_UP 1
#define STEPPER_0_DOWN 0
#define STEPPER_1_UP 0
#define STEPPER_1_DOWN 1
#define STEPPER_2_UP 1
#define STEPPER_2_DOWN 0
#define STEPPER_3_UP 0
#define STEPPER_3_DOWN 1

#define STEPPER_0_DISTANCE 455
#define STEPPER_1_DISTANCE 462
#define STEPPER_2_DISTANCE 747
#define STEPPER_3_DISTANCE 749

//	Sonar define parameters
#define SONAR_TIGGER_PIN 2
#define SONAR_NUMBER 4
#define MOTION_NONE_TIGGER 0
#define MOTION_SINGEL_TIGGER_ONE 1                //0001
#define MOTION_SINGEL_TIGGER_TWO 2                //0010
#define MOTION_SINGEL_TIGGER_THREE 4              //0100
#define MOTION_SINGEL_TIGGER_FOUR 8               //1000
#define MOTION_TWIN_BEWTEEN_TIGGER_ONE_TWO 3      //0011
#define MOTION_TWIN_BEWTEEN_TIGGER_TWO_THERE 6    //0110
#define MOTION_TWIN_BEWTEEN_TIGGER_THREE_FOUR 12  //1100
#define MOTION_TWIN_BEWTEEN_TIGGER_FOUR_ONE 9     //1001
#define MOTION_TWIN_SYMMETRY_TIGGER_ONE_THREE 5   //0101
#define MOTION_TWIN_SYMMETRY_TIGGER_TWO_FOUR 10   //1010
#define MOTION_TRIPLE_TIGGER_ONE 14               //1110
#define MOTION_TRIPLE_TIGGER_TWO 7                //1101
#define MOTION_TRIPLE_TIGGER_THREE 11             //1011
#define MOTION_TRIPLE_TIGGER_FOUR 13              //0111
#define MOTION_QUADRUPLE_TIGGER 15                //1111

#define MOTION_NONE 0
#define MOTION_SIT 1
#define MOTION_STAND 2
#define MOTION_LEFT 3
#define MOTION_RIGHT 4
#define MOTION_FORWARD 5
#define MOTION_BACKWARD 6
#define MOTION_INIT 7

#define STATE_MACHINE_BLOCK_0 0	// init
#define STATE_MACHINE_BLOCK_1 1 // before standup
#define STATE_MACHINE_BLOCK_2 2 // after standup

class Chair{


	//-- Function
	public:
		Chair();
		void init();

		// Motion function
		bool isMoving();
		bool isMotioning();

		void motor_stop();
		void motion_correction();

		void motion_init();

		void motion_template();
		void motion_standup();
		void motion_sitdown();
		void motion_left( uint8_t number );
		void motion_right(  uint8_t number );
		void motion_forward(  uint8_t number );
		void motion_backward(  uint8_t number );

		void run();
		void run_kernel();
		void decide_motion_before_standup();
		void decide_motion_after_standup();

		// Stepper function
		void interrupt(uint8_t index);
		void stepper_all_reset();
		bool isEmergence();

		// Sonar function
		void activate_sonar();
		void update_sonar_table();



	private:
		//-- Define Variable

		//-- Variable
		HighSpeedStepper stepper[STEPPER_NUMBER];
		bool stepper_interrupt[STEPPER_NUMBER];
		uint8_t current_motion = MOTION_NONE;

		// State machine:
		//		block 0: init
		// 		block 1: before standup
		//		block 2: after standup
		uint8_t current_state = STATE_MACHINE_BLOCK_0;

		bool _isEmergence = false;

		// Sonar 
		AccurateMaxsonar sonar[SONAR_NUMBER];
		uint8_t sonar_approching_table;

		
};
