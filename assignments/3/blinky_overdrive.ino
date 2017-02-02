// the setup function runs once when you press reset or power the board
void setup() {
	// initialize digital pin LED_BUILTIN as an output.
	pinMode(2, OUTPUT);
	pinMode(3, OUTPUT);
}

// infinite loop
void loop() {

	// alternate the LEDs
	digitalWrite(2, HIGH);
	digitalWrite(3, LOW);

	// wait for half a second
	delay(500);

	// alternate the LEDs
	digitalWrite(2, LOW);
	digitalWrite(3, HIGH);

	// wait for half a second
	delay(500);
}