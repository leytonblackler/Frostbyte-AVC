/*========================IMPORTS======================*/
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
int processed_camera_output_columns[239];
int processed_camera_output_rows[239];		//Used to store the processed camera output (1 for line detected, 0 for white).
int offset_position[239];					//Used to determine the position of offset of the line detected.
int error_code_array[239];					//Used to find error code.

//Define numerical variables.
double error_code;							//Used to store the final calculated error code.
double proportional_signal;					//Used to determine the dispacement based on error code.
int maximum_error_code = 7140;				//Constant value used to find the motor speed ratio.
int left_motor_pin = 2;						//Pin assigned to left motor.
int right_motor_pin = 1;					//Pin assigned to right motor.
int left_motor_speed = 0;					//Used to store the calcualted speed for the left motor.
int right_motor_speed = 0;					//Used to store the calcualted speed for the right motor.
int left = 0;								//Used to store the processed camera output sum for the left column.
int front = 0;								//Used to store the processed camera output sum for the top row.

//Define tuning values.
float kP = 0.005;							//Constant used to tune the proportional signal.
//float kI = 1;								//Constant used to tune the integral signal.
//float kD = 1;								//Constant used to tune the derivative signal.

//Define booleans.
bool run = true;							//Used to continuously loop a main method.

//Define methods;
void calculate_motor_speeds();
void process_picture();
void open_gate();
void ir_read();

//variables for ir sensors
int wall_dist= 300; //distance from wall by ir value that we want the bot to stay
int ir_error=0;
double ir_proportional=0;
double kpir= 0.5;
int ir_left=0;
int ir_front=0;
int wall=0;
/*=====================================================*/


int main() {

	//Initialise hardware.
	init(0);

	//Opens the network gate.
	open_gate();

	//Connect the camera to the screen.
	open_screen_stream();

	//Waits for 2 seconds.
	Sleep(2, 0);

	//Main program loop, will constantly repeat.
	while (run) {

		//Sets the error code back to 0.
		error_code = 0;

		//Stores the current display on the camera.
		take_picture();

		//Processes the image and stores line detection in the processed camera output arrays.
		process_picture();
                
		//reads the ir sensors
		ir_read();
		//Checks if there is a line in front or to the left based on processed camera outputs.
		front = 0;
		left = 0;

		for (int i = 0; i < 239; i++) {
			left = left + processed_camera_output_rows[i];
		}

		for (int i = 0; i < 239; i++) {
			front = front + processed_camera_output_columns[i];
		}

		//Sets values from -160 to 159 in an array (includes 0).
		int value = -119;
		for (int i = 0; i < 239; i++) {
			offset_position[i] = value;
			value++;
		}

		//Finds error code array based on offset position and processed camera output..
		for (int i = 0; i < 239; i++) {
			error_code_array[i] = processed_camera_output_columns[i] * offset_position[i];
		}

		//Adds all the values in the error code array.
		for (int i = 0; i < 239; i++) {
			error_code = error_code + error_code_array[i];
		}

		//Calculate the proportional signal from the error code and the tuning value of kP.
		proportional_signal = error_code * kP;

		calculate_motor_speeds();

		//Print the values for the motor speeds.
		printf("left %d\n", left);
		printf("%d\n", front);

		//Sets the motors to the calculated speeds.
		set_motor(left_motor_pin, left_motor_speed);
		set_motor(right_motor_pin, right_motor_speed);

		//Waits for 0.5 seconds.
		Sleep(0, 500);
	}
	return 0;
}


/*
* Sets the values in the processed_camera_output array to either 1 or 0 depending on the colour detected.
*/
void process_picture() {

	//Reads and processes a column line to the left of the picture.
	for (int row = 0; row < 240; row++) {

		//Reads the RGB values for the pixel in the given row.
		int red = get_pixel(1, row, 0);
		int green = get_pixel(1, row, 1);
		int blue = get_pixel(1, row, 2);

		//Finds average colour from RGB values.
		int average_colour_rows = (red + green + blue) / 3;

		//Stores either a 1 or 0 in the array at the position of the pixel.
		if (average_colour_rows >= 127) { //--Light colour detected.

			//Set the pixel index value to 1 to indicate light colour detected.
			processed_camera_output_rows[row] = 1;
		}

		else { //--Dark colour detected.

			//Set the pixel index value to 0 to indicate dark colour detected.
			processed_camera_output_rows[row] = 0;
		}
	}

	//Reads and processes a row line at the top of the picture.
	for (int column = 0; column < 239; column++) {

		//Reads the RGB values for the pixel in the given column.
		int red = get_pixel(column, 1, 0);
		int green = get_pixel(column, 1, 1);
		int blue = get_pixel(column, 1, 2);

		//Finds average colour from RGB values.
		int average_colour_columns = (red + green + blue) / 3;

		//Stores either a 1 or 0 in the array at the position of the pixel.
		if (average_colour_columns >= 127) { //--Light colour detected.

			//Set the pixel index value to 1 to indicate light colour detected.
			processed_camera_output_columns[column] = 1;
		}

		else { //--Dark colour detected.

			//Set the pixel index value to 0 to indicate dark colour detected.
			processed_camera_output_columns[column] = 0;
		}
	}
}

/*
* Proportionally changes motor speed values depending on proportional_signal.
*/
void calculate_motor_speeds() {
if(wall=0){
	if (front > 50 && left < 50) { //--Line in front and no line to the left.

		if (proportional_signal < 0) { //--Too far right, need to turn left.

			//Left motor is set to a proportional faster speed than the right motor.
			right_motor_speed = (50 - (proportional_signal / (119)) * 175); //(80+((175 / maximum_error_code*kP)*(proportional_signal*-1)));
			left_motor_speed = (50 + (proportional_signal / (119)) * 175);
		}

		else if (proportional_signal > 0) { //--Too far left, need to turn right.

			//Right motor is set to a proportional faster speed than the left motor.
			right_motor_speed = (50 - (proportional_signal / (119)) * 175);
			left_motor_speed = (50 + (proportional_signal / (119)) * 175); //((175 / maximum_error_code*kP)*proportional_signal));
		}
	}
	else if (left > 50 && front > 50) { //--Line to the left and line in front.

		//Turn left.
		left_motor_speed = -20;
		right_motor_speed = 80;
	}
	else if (left < 50 && front < 50) { //--No line to the left and no line in front.

		//Turn right.
		left_motor_speed = 80;
		right_motor_speed = -20;
	}
}else if (wall=1){


}
}
/*reads the ir sensors and makes calculations for the error
code and proportional signal
*/
void ir_read(){
	ir_left=read_analog(6);
	ir_front= read_analog(7);
	ir_error= ir_left-wall_dist;
// this will give us a error >0 if the robot is too close to the wall, and 
//and an error<0 if the bot is too far from the wall

       ir_proportional= ir_error*kpir;
//our proportional signal
      
}

/*
* Connects to the gate server and issues the request to open the gate.
*/
void open_gate() {
	//Connects to the server using the IP address.
	connect_to_server("130.195.6.196", 1024);

	//Sends the message to the server.
	send_to_server("Please");
	char message[24];

	//Recevies a response message from the server and returns it back to the server.
	receive_from_server(message);
	send_to_server(message);

	//Prints the message from the server.
	printf("%s", message);
}
