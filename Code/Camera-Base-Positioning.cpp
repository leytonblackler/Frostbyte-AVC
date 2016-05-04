//NOTES:
//Camera dimensions: 320 width, 240 height

#include <stdio.h>

int main () {

   /**INITIAL SETUP*/
   //Array used to store the processed camera output (1 for line detected, 0 for white)
   int processedCameraOutput[ 320 ];

   //Array used to determine the position of offset of the line detected.
   int offsetPosition[ 320 ];

   //Sets values from -160 to 159 in an array (includes 0).
   int value = -160;
   for (int i = 0; i < 320; i++) {
        offsetPosition[i] = value;
        printf("Array: %d\n", offsetPosition[i]);
        value++;
   }
}
