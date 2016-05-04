//NOTES:
//Camera dimensions: 320 width, 240 height

#include <stdio.h>
#include <time.h>

extern "C" int init_hardware();
extern "C" int init(int d_lev);

extern "C" int take_picture();
extern "C" char get_pixel(int row, int col, int color);
extern "C" void set_pixel(int col, int row, char red,char green,char blue);

extern "C" int open_screen_stream();
extern "C" int close_screen_stream();
extern "C" int update_screen();
//extern "C" void GetLine(int row,int threshold);
extern "C" int display_picture(int delay_sec,int delay_usec);
extern "C" int save_picture(char filename[5]);

extern "C" int set_motor(int motor,int speed);

extern "C" int read_analog(int ch_adc);
extern "C" int Sleep(int sec, int usec);
extern "C" int select_IO(int chan, int direct);
extern "C" int write_digital(int chan,char level);
extern "C" int read_digital(int chan);
extern "C" int set_PWM(int chan, int value);

extern "C" int connect_to_server( char server_addr[15],int port);
extern "C" int send_to_server(char message[24]);
extern "C" int receive_from_server(char message[24]);

int main () {

   /**INITIAL SETUP*/
   
   init(0);
   // connect camera to the screen
   open_screen_stream();
   
   //Array used to store the processed camera output (1 for line detected, 0 for white).
   int processedCameraOutput[ 320 ];
   //Array used to determine the position of offset of the line detected.
   int offsetPosition[ 320 ];
   //Array used to find error code.
   int errorCodeArray[ 320 ];
   //Variable usedto store the error code.
   int errorCode = 0;

   //Stores the current display on the camera.
   take_picture();

   //Processes the image and stores line detection in the processedCameraOutput array.
   for (int column = 0; column < 320; column++) {
        //Reads the RGB values for the given pixel.
        int red = get_pixel(120, column, 0);
        int green = get_pixel(120, column, 1);
        int blue = get_pixel(120, column, 2);

        //Finds average colour from RGB values.
        int averageColour = (red+green+blue)/3;
        printf("Average colour: %d\n", errorCode);

        //Stores eithe a 1 or 0 in the array at the position of the pixel.
        if (averageColour >= 180) {
            //Line detected.
            processedCameraOutput[column] = 1;
        } else {
            //No line detected.
            processedCameraOutput[column] = 0;
        }
   }

   //Sets values from -160 to 159 in an array (includes 0).
   int value = -160;
   for (int i = 0; i < 320; i++) {
        offsetPosition[i] = value;
        value++;
   }

   //Finds error code based on arrays.
   for (int i = 0; i < 320; i++) {
        errorCodeArray[i] = processedCameraOutput[i] * offsetPosition[i];
   }
   //Adds all the values in the errorCode array.
   for (int i = 0; i < 320; i++) {
        errorCode = errorCode + errorCodeArray[i];
        printf("Final error code: %d\n", errorCode);
    }
    
    //Waits for 5 seconds.
    Sleep(5,0);
}
