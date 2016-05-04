//This is just a practice test just to make sure the components and general robot can move.

# include < stdio .h > 
# include < time .h > 

\\ These load specific methods from the ENGR101 library 
extern "C" int InitHardware ();
extern "C" int sleep ( int sec , int usec ); 
extern "C" int set_motor ( int motor , int speed );

int main (){ 
  // Sets up RPi hardware, makes sure it works correctly
	InitHardware ();

  //Sets motors to full speed for 3 seconds
	set_motor(1,255);
	set_motor(2,255);
	
	sleep(3,0);
	
	set_motor(1,0);
	set_motor(2,0);

	return 0;
}
