#include <i2c_t3.h>
#include <LIDARLite.h>

LIDARLite myLidarLite;

void setup() {
	Serial.begin(115200);
	myLidarLite.begin(0,true);
	myLidarLite.beginContinuous();
	pinMode(3, INPUT);
	delay(200);
	Serial.println("Start;");
}

uint32_t last = 0;
uint16_t count = 0;
uint16_t val = 0;
void loop() {
	


	uint32_t current = millis();
	if (current > last + 1000) {
		Serial.print("Count:");
		Serial.print(count);
		Serial.print("   ");
		Serial.println(val);
		count = 0;
		last = current;
	}
	if (!digitalRead(3)) {
		val = (myLidarLite.distanceContinuous());
	}
	count++;
}