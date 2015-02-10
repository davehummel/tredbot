#include <dh_averaged_measure.h>


AveragedMeasure<int32_t> am = AveragedMeasure<int32_t>(100);

void setup(){ 
	pinMode(13, OUTPUT);
	Serial1.begin(115200);
	Serial1.print("Test Start\n");
	int16_t temp;
	 
	temp = am.getValue();

	if (temp!=0){
		Serial1.print("Init Value:Expected 0 got ");
		Serial1.println(temp);
		return;
	}

	for (int i = 0 ; i < 20; i++){
		am.addMeasure(i,i*10+1);
		temp = am.getValue(); 

		if (am.getReady()){
				Serial1.print(" Expected Not Ready ");
				return; 
		} 

	} 
 

	am.addMeasure(20,202);

	am.printDebugStatus();
	if (!am.getReady()){
		Serial1.print(" Expected Ready ");
		return; 
	}

	temp = am.getValue();

	if (temp!=5){
		Serial1.print(" Fill:Expected 5 got ");
		Serial1.println(temp);

		return;
	}

	for ( int i = 0 ;i < 10; i++){
		am.addMeasure(25,213+i*10);
		temp = am.getValue();
		Serial1.print("Val = ");
		Serial1.println(temp);
	}


	temp = am.getValue();

	if (temp!=15){
		Serial1.print(" Fill:Expected 15 got ");
		Serial1.println(temp);

		return;
	}

	am.printDebugStatus();

	am.addMeasure(0,600);
	temp = am.getValue();
	am.printDebugStatus();
	if (temp!=25){
		Serial1.print(" Fill:Expected 25 got ");
		Serial1.println(temp);

		return;
	}
	 

	am.addMeasure(0,900);
	temp = am.getValue();
	am.printDebugStatus();

	if (temp!=0){
		Serial1.print(" Fill:Expected 0 got ");
		Serial1.println(temp);

		return;
	}



	for (int i = 0 ; i < 10; i++){
		am.addMeasure(100,901+i*10);
		temp = am.getValue(); 

		if (temp!=0){
			Serial1.print(" Fill:Expected 0 got ");
			Serial1.println(temp);

			return;
		}
 
	} 

	am.addMeasure(100,1000);
	am.printDebugStatus();
	am.shiftQueues();
	am.printDebugStatus();

	temp = am.getValue(); 

		if (temp!=100){
			Serial1.print(" Fill:Expected 100 got ");
			Serial1.println(temp);

			return;
		}



	Serial1.println("Test Complete");
	



}

void loop() {
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);               // wait for a second
}
