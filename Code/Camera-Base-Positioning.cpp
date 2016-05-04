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
   //Array used to find error code.
   int errorCodeArray[ 320 ];
   //
   int errorCode = 0;

   //Stores the current dispaly on the camera.
   take_picture();

   //Processes the image and stores line detection in the processedCameraOutput array.
   for (int column = 0; column < 320; column++) {
        //Reads the RGB values for the given pixel.
        int red = get_pixel(120, column, 0);
        int green = get_pixel(120, column, 1);
        int blue = get_pixel(120, column, 2);

        //Finds average colour from RGB values.
        int averageColour = (red+green+blue)/3;

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
        printf("Array: %d\n", offsetPosition[i]);
        value++;
   }

   //Finds error code based on arrays.
   for (int i = 0; i < 320; i++) {
        errorCodeArray[i] = processedCameraOutput[i] * offsetPosition[i];
   }
   //Adds all the values in the errorCode array.
   for (int i = 0; i < 320; i++) {
        errorCode = errorCode + errorCodeArray[i];
    }
}
