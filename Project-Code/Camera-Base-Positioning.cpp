/*=======================IMPORTS=======================*/
#include <stdio.h>
#include <time.h>

extern "C" int init_hardware();
extern "C" int init(int d_lev);

extern "C" int take_picture();
extern "C" char get_pixel(int row, int col, int color);
extern "C" void set_pixel(int col, int row, char red, char green, char blue);

extern "C" int open_screen_stream();
extern "C" int close_screen_stream();
extern "C" int update_screen();
extern "C" int display_picture(int delay_sec, int delay_usec);
extern "C" int save_picture(char filename[5]);

extern "C" int set_motor(int motor, int speed);

extern "C" int read_analog(int ch_adc);
extern "C" int Sleep(int sec, int usec);
extern "C" int select_IO(int chan, int direct);
extern "C" int write_digital(int chan, char level);
extern "C" int read_digital(int chan);
extern "C" int set_PWM(int chan, int value);

extern "C" int connect_to_server(char server_addr[15], int port);
extern "C" int send_to_server(char message[24]);
extern "C" int receive_from_server(char message[24]);
/*=====================================================*/


/*==================VARIABLE DEFINTIONS================*/
//Define array variables.
int processed_camera_output[239];		//Used to store the processed camera output (1 for line detected, 0 for white).
int offset_position[239];               //Used to determine the position of offset of the line detected.
int error_code_array[239];              //Used to find error code.

//Define numerical variables.
double error_code;                      //Used to store the final calculated error code.
int maximum_error_code = 7140;			//Constant value used to find the motor speed ratio.
double proportional_signal;             //Used to determine the dispacement based on error code.
double absolute_proportional_signal;    //Used to store the abolsute value of the proportional_signal.
int left_motor_pin = 2;					//Pin assigned to left motor.
int right_motor_pin = 1;				//Pin assigned to right motor.
int left_motor_speed = 0;				//Used to store the calcualted speed for the left motor.
int right_motor_speed = 0;				//Used to store the calcualted speed for the right motor.


//Define tuning values.
double kP = 1;							//Constant used to tune the proportional signal.
//float kI = 1;							//Constant used to tune the integral signal.
//float kD = 1;							//Constant used to tune the derivative signal.

//Define booleans.
bool run = true;						//Used to continuously loop a main method.

//Define methods;
void set_motor_speeds();
void process_picture();
/*=====================================================*/



int main() {

	//Initialise hardware.
	init(0);

	//Connect the camera to the screen.
	open_screen_stream();

	//Main program loop, will constantly repeat.
	while (run) {

		//Sets the error code back to 0.
		error_code = 0;

		//Stores the current display on the camera.
		take_picture();

		//Processes the image and stores line detection in the processed_camera_output array.
		process_picture();

		//Sets values from -160 to 159 in an array (includes 0).
		int value = -119;
		for (int i = 0; i < 239; i++) {
			offset_position[i] = value;
			value++;
		}

		//Finds error code array based on offset position and processed camera output..
		for (int i = 0; i < 239; i++) {
			error_code_array[i] = processed_camera_output[i] * offset_position[i];
		}

		//Adds all the values in the error code array.
		for (int i = 0; i < 239; i++) {
			error_code = error_code + error_code_array[i];
		}

		//Calculate the proportional signal from the error code and the tuning value of kP.
		proportional_signal = error_code * kP;

		//Convert the proportional signal to an absolute value.
		if (proportional_signal < 0) {
			absolute_proportional_signal = absolute_proportional_signal * -1;
		}

		calculate_motor_speeds();

		//Sets the motors to the calculated speeds.
		set_motor(left_motor_pin, left_motor_speed);
		set_motor(right_motor_pin, right_motor_speed);

		//Waits for 0.5 seconds.
		Sleep(0, 500);
	}
	return 0;
}



void process_picture() {

	for (int column = 0; column < 239; column++) {

		//Reads the RGB values for the pixel in the given column.
		int red = get_pixel(column, 160, 0);
		int green = get_pixel(column, 160, 1);
		int blue = get_pixel(column, 160, 2);

		//Finds average colour from RGB values.
		int average_colour = (red + green + blue) / 3;

		//Stores either a 1 or 0 in the array at the position of the pixel.
		if (average_colour >= 127) { //--Light colour detected.

			//Set the pixel index value to 1 to indicate light colour detected.
			processed_camera_output[column] = 1;
		}

		else { //--Dark colour detected.

			//Set the pixel index value to 0 to indicate dark colour detected.
			processed_camera_output[column] = 0;
		}
	}
}



void calculate_motor_speeds() {

	//Proportionally changes motor speed values depending on proportional_signal. 
	if (proportional_signal < 0) { //--Too far right, need to turn left.

		//Left motor is set to a proportional faster speed than the right motor.
		left_motor_speed = (80 + ((175 / maximum_error_code)*absolute_proportional_signal));
		right_motor_speed = (80);

	}

	else if (proportional_signal > 0) { //--Too far left, need to turn right.

		//Right motor is set to a proportional faster speed than the left motor.
		left_motor_speed = (80);
		right_motor_speed = (80 + ((175 / maximum_error_code)*absolute_proportional_signal));

	}

	else { //--Centred, no need to turn.

		//Both motors are set to equal speeds.
		left_motor_speed = 80;
		right_motor_speed = 80;
	}
}