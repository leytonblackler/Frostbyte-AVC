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
//extern "C" void GetLine(int row,int threshold);
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


/*====================INITIAL SETUP====================*/


//Define arrays.
int processed_camera_output[239];       //Used to store the processed camera output (1 for line detected, 0 for white).
int offset_position[239];                       //Used to determine the position of offset of the line detected.
int error_code_array[239];                      //Used to find error code.

                                                                        //Define doubles.
double error_code;                                      //Used to store the final calculated error code.
double proportional_signal;                     //Used to determine the dispacement based on error code.
double absolute_proportional_signal;                    //Used to determine the dispacement based on error code.
int Mec=7140;
                                                                                                //Define tuning values.
double kP =0.5;
extern "C" int set_PWM(int chan, int value);

extern "C" int connect_to_server(char server_addr[15], int port);
extern "C" int send_to_server(char message[24]);
extern "C" int receive_from_server(char message[24]);


/*====================INITIAL SETUP====================*/


//Define arrays.
int processed_camera_output[239];       //Used to store the processed camera output (1 for line detected, 0 for white).
int offset_position[239];                       //Used to determine the position of offset of the line detected.
int error_code_array[239];                      //Used to find error code.

                                                                        //Define doubles.
double error_code;                                      //Used to store the final calculated error code.
double proportional_signal;                     //Used to determine the dispacement based on error code.
double absolute_proportional_signal;                    //Used to determine the dispacement based on error code.
int Mec=7140;
                                                                                                //Define tuning values.
double kP =0.5;
//float kI = 0.5;
//float kD = 0.5;

//Define booleans.
bool run = true;                                        //Used to continuously loop a main method.
                                                                        /*=====================================================*/




                                                                        //If big negative, too far right, need to turn left
                                                                        //If big positive, too far left, need to turn right



int main() {

        //Initialise hardware.
        init(0);

        //Connect the camera to the screen.
        open_screen_stream();

        while (run) {

//Define booleans.
bool run = true;                                        //Used to continuously loop a main method.
                                                                        /*=====================================================*/




                                                                        //If big negative, too far right, need to turn left
                                                                        //If big positive, too far left, need to turn right



int main() {

        //Initialise hardware.
        init(0);

        //Connect the camera to the screen.
        open_screen_stream();

        while (run) {

                //Sets the error code back to 0.
                error_code = 0;
                //Stores the current display on the camera.
                take_picture();

                //Processes the image and stores line detection in the processed_camera_output array.
                for (int column = 0; column < 239; column++) {
                        //Reads the RGB values for the given pixel.
                        int red = get_pixel(column, 160, 0);
                        int green = get_pixel(column, 160, 1);
                        int blue = get_pixel(column, 160, 2);

                        //Finds average colour from RGB values.
                        int average_colour = (red + green + blue) / 3;

                        //Stores either a 1 or 0 in the array at the position of the pixel.
                        if (average_colour >= 127) {
                                //Line detected.
                                processed_camera_output[column] = 1;
                        }
                        else {
                                //No line detected.
                                processed_camera_output[column] = 0;
   error_code = 0;
                //Stores the current display on the camera.
                take_picture();

                //Processes the image and stores line detection in the processed_camera_output array.
                for (int column = 0; column < 239; column++) {
                        //Reads the RGB values for the given pixel.
                        int red = get_pixel(column, 160, 0);
                        int green = get_pixel(column, 160, 1);
                        int blue = get_pixel(column, 160, 2);

                        //Finds average colour from RGB values.
                        int average_colour = (red + green + blue) / 3;

                        //Stores either a 1 or 0 in the array at the position of the pixel.
                        if (average_colour >= 127) {
                                //Line detected.
                                processed_camera_output[column] = 1;
                        }
                        else {
                                //No line detected.
                                processed_camera_output[column] = 0;
                        }
                }

                //Sets values from -160 to 159 in an array (includes 0).
                int value = -119;
                for (int i = 0; i < 239; i++) {
                        offset_position[i] = value;
                        value++;
                }

                //Finds error code based on arrays.
                for (int i = 0; i < 239; i++) {
                        error_code_array[i] = processed_camera_output[i] * offset_position[i];
                }

                //Adds all the values in the error_code array.
                for (int i = 0; i < 239; i++) {
                        error_code = error_code + error_code_array[i];
                        //printf("Final error code: %d\n", error_code);
                }
// 50+(205/max value times errorcode)*kp
//int prop = 255/error_code;
//if(error_code>0){set_motor(2,30+prop);
//                set_motor(1, 30);}

                //Sets values from -160 to 159 in an array (includes 0).
                int value = -119;
                for (int i = 0; i < 239; i++) {
                        offset_position[i] = value;
                        value++;
                }

                //Finds error code based on arrays.
                for (int i = 0; i < 239; i++) {
                        error_code_array[i] = processed_camera_output[i] * offset_position[i];
                }

                //Adds all the values in the error_code array.
                for (int i = 0; i < 239; i++) {
                        error_code = error_code + error_code_array[i];
                        //printf("Final error code: %d\n", error_code);
                }
// 50+(205/max value times errorcode)*kp
//int prop = 255/error_code;
//if(error_code>0){set_motor(2,30+prop);
//                set_motor(1, 30);}
//if(error_code<0){set_motor(2,30);
//               set_motor(1,30+prop);}
                //Print the final error code.
                //printf("Final error code: %d\n", error_code);

                //Runs the method used to set the speeds for the left and right motors.
                int left_motor_pin = 2;
                int right_motor_pin = 1;

                int left_motor_speed = 0;
                int right_motor_speed = 0;

                proportional_signal = error_code * kP;
                //printf("Proportional signal: %d", proportional_signal);

                //Converts to absolute:
                if (proportional_signal < 0) {          //Too far right, need to turn left.
                        absolute_proportional_signal = absolute_proportional_signal * -1;
                }
int asdf= error_code;
                 printf("%d\n",asdf);
                if (proportional_signal < 0) {          //Too far right, need to turn left.
                        left_motor_speed = (80 +((175/Mec)*absolute_proportional_signal) );
                        right_motor_speed = (80  );
      //Print the final error code.
                //printf("Final error code: %d\n", error_code);

                //Runs the method used to set the speeds for the left and right motors.
                int left_motor_pin = 2;
                int right_motor_pin = 1;

                int left_motor_speed = 0;
                int right_motor_speed = 0;

                proportional_signal = error_code * kP;
                //printf("Proportional signal: %d", proportional_signal);

                //Converts to absolute:
                if (proportional_signal < 0) {          //Too far right, need to turn left.
                        absolute_proportional_signal = absolute_proportional_signal * -1;
                }
int asdf= error_code;
                 printf("%d\n",asdf);
                if (proportional_signal < 0) {          //Too far right, need to turn left.
                        left_motor_speed = (80 +((175/Mec)*absolute_proportional_signal) );
                        right_motor_speed = (80  );
                }
                else if (proportional_signal > 0) { //Too far left, need to turn right.
                        left_motor_speed = (80 );
                        right_motor_speed = (80 + ((175/Mec)*absolute_proportional_signal ));
                }
                else {                                                          //Too far right, need to turn left.
                        left_motor_speed = 80;
                        right_motor_speed = 80;
                }

                set_motor(left_motor_pin, left_motor_speed);
                set_motor(right_motor_pin, right_motor_speed);

                //Waits for 0.5 seconds.
                Sleep(0,500);
        }
        return 0;
}
