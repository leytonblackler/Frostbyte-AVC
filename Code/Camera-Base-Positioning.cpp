//NOTES:
//Camera dimensions: 320 width, 240 height

#include <stdio.h>

extern "C" int take_picture();
extern "C" char get_pixel(int row, int col, int color);

int main () {

   /**INITIAL SETUP*/
   //Array used to store the processed camera output (1 for line detected, 0 for white).
   int processedCameraOutput[ 320 ];
   //Array used to determine the position of offset of the line detected.
   int offsetPosition[ 320 ];

   //Stores the current dispaly on the camera.
   take_picture();

   //Sets values from -160 to 159 in an array (includes 0).
   int value = -160;
   for (int i = 0; i < 320; i++) {
        offsetPosition[i] = value;
        printf("Array: %d\n", offsetPosition[i]);
        value++;
   }
}
