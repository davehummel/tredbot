#include <quaternion.h>
#include <dh_logger.h>
#include <dh_arduino_logger.h>

uint16_t flash=40;

void setup(){ 
	pinMode(13, OUTPUT);
	Serial1.begin(115200); 
	Serial1.print("Test Start\n");

  double dX = 21;
  double dY = 1;
  double dZ = -72;

  double axisAngle = sqrt(dX*dX+dY*dY+dZ*dZ);

  dX = dX/axisAngle;
  dY = dY/axisAngle;
  dZ = dZ/axisAngle;

  double normalized =dX*dX+dY*dY+dZ*dZ;

  if ( normalized >1.01 || normalized < .99 ){
  	Serial1.print ("This is not right way to normalize ");
  	Serial1.println(dX*dX+dY*dY+dZ*dZ);
  	return;
  }
	
	Quaternion <double>a(0,0,90); //90 degrees on Z

	normalized = sqrt(.5);

	if (a.w > normalized+.01 || a.w< normalized-.01){
		Serial1.print ("Expected aW:");
		Serial1.print (a.w);
		Serial1.print (" == ");
  		Serial1.println(normalized);
  		return;
	}

	if (a.z > normalized+.01 || a.z< normalized-.01){
		Serial1.print ("Expected aZ:");
		Serial1.print (a.z);
		Serial1.print (" == ");
  		Serial1.println(normalized);
  		return;
	}

	if (a.x > .01 || a.x< -.01){
		Serial1.print ("Expected aX:");
		Serial1.print (a.x);
		Serial1.print (" == "); 
  		Serial1.println(0);
  		return;
	}

	if (a.y > .01 || a.y< -.01){
		Serial1.print ("Expected aY:");
		Serial1.print (a.y);
		Serial1.print (" == ");
  		Serial1.println(0);
  		return;
	}

	double out[3]{0,0,0};
	a.getEuler(out); 


	if (out[2]*57.2957795131 > 90.01 || out[2]*57.2957795131< 89.99){
		Serial1.print ("Expected heading of ");
		Serial1.print (out[2]*57.2957795131);
		Serial1.print (" == ");
  		Serial1.println(90);
  		return;
	}

	a = Quaternion<double>(45,0,45); //90 degrees on x


	Serial1.print(a.x,6);

	Serial1.print(",");
	Serial1.print(a.y,6);

	Serial1.print(",");
	Serial1.println(a.z,6);

		a.getEuler(out); 

	Serial1.print(out[0]*57.2957795131,6);
	Serial1.print(",");
	Serial1.print(out[1]*57.2957795131,6);
	Serial1.print(",");
	Serial1.println(out[2]*57.2957795131,6);

	Serial1.println("Testing a series of rotations");

	a = Quaternion<double>(0,0,360); //90 degrees on x

	Serial1.print(a.w,6);
	Serial1.print(",");
	Serial1.print(a.x,6);
	Serial1.print(",");
	Serial1.print(a.y,6);
	Serial1.print(",");
	Serial1.println(a.z,6);

	Quaternion<double> b(0,0,1);
		Serial1.print(b.w,6);
	Serial1.print(",");
	Serial1.print(b.x,6);
	Serial1.print(",");
	Serial1.print(b.y,6);
	Serial1.print(",");
	Serial1.println(b.z,6);

	for (int i = 0 ; i < 10 ; i ++){
		a = b*a;

	a.getEuler(out); 

	Serial1.print(out[2]*57.2957795131,6);
	Serial1.print(">");
	Serial1.print(a.w,6);
	Serial1.print(",");
	Serial1.print(a.x,6);
	Serial1.print(",");
	Serial1.print(a.y,6);
	Serial1.print(",");
	Serial1.println(a.z,6);

	}

	if (out[2]*57.2957795131 > 10.01 || out[2]*57.2957795131< 9.99){
		Serial1.print ("Expected heading of ");
		Serial1.print (out[2]*57.2957795131);
		Serial1.print (" == ");
  		Serial1.println(10);
  		return;
	}

	a = Quaternion<double>(0,0,360); //90 degrees on x
	b = Quaternion<double>(90,0,90); //90 degrees on x
	a = b*a;
	a.getEuler(out); 
	Serial1.print(out[2]*57.2957795131,6);
	Serial1.print(">");
	Serial1.print(a.w,6);
	Serial1.print(",");
	Serial1.print(a.x,6);
	Serial1.print(",");
	Serial1.print(a.y,6);
	Serial1.print(",");
	Serial1.println(a.z,6);

 	b = Quaternion<double>(0,0,0); //90 degrees on x

 	a=b*a;

	a.getEuler(out); 
 	Serial1.print(out[2]*57.2957795131,6);
	Serial1.print(">");
	Serial1.print(a.w,6);
	Serial1.print(",");
	Serial1.print(a.x,6);
	Serial1.print(",");
	Serial1.print(a.y,6);
	Serial1.print(",");
	Serial1.println(a.z,6);

	 b = Quaternion<double>(-90,0,-90); //90 degrees on x

	  a=b*a;

	a.getEuler(out); 
 	Serial1.print(out[2]*57.2957795131,6);
	Serial1.print(">");
	Serial1.print(a.w,6);
	Serial1.print(",");
	Serial1.print(a.x,6);
	Serial1.print(",");
	Serial1.print(a.y,6);
	Serial1.print(",");
	Serial1.println(a.z,6);


	 flash = 1000;


	Serial1.println("Done");

}

void loop() {
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(flash);               // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(flash);               // wait for a second
}
