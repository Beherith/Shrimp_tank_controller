#define fanpin 5
#define lightpin 3
#define btnlow A0
#define btnpin A2
#define swlow 7
#define swpin 8
#define solenoid 2
#define ledpin 13
#define pothigh A5
#define potlow A3
#define pot A4
#define ANALOG_PRECISION 8
unsigned long time;
byte mode; //0:automatic, 1: force on, 2: force off
float hours;

void setup(){
	Serial.begin(9600);
	Serial.println("Welcome to the shrimp tank!");
	time =0;
	mode =0;
	hours=0.0;
	pinMode(fanpin,OUTPUT);
	pinMode(lightpin,OUTPUT);
	pinMode(solenoid,OUTPUT);
	pinMode(swlow,OUTPUT);
	digitalWrite(swlow,LOW);
	pinMode(swpin,INPUT_PULLUP);
	pinMode(btnpin,INPUT_PULLUP);
	pinMode(btnlow,OUTPUT);
	digitalWrite(btnlow,LOW);
	pinMode(ledpin,OUTPUT);
  
	pinMode(pothigh,OUTPUT);
	digitalWrite(pothigh,HIGH);
	pinMode(potlow,OUTPUT);
	digitalWrite(potlow,LOW);
  
	analogWrite(ledpin,0);
	//test();
}

void test(){ 
	Serial.println("Testing valve");
	digitalWrite(solenoid,HIGH);
	delay(1000);
	digitalWrite(solenoid,LOW);

	digitalWrite(fanpin,HIGH);
	Serial.print("Fan speed set to full");
	delay(3000);

	for (byte b=0; b<255;b++){
		analogWrite(lightpin,b);
		delay(300);
		Serial.print("light set to");
		Serial.println(b);
	}
}

void flashled(){
	digitalWrite(ledpin,HIGH);
	delay(100);
	digitalWrite(ledpin,LOW);
	delay(100);
}
void(* resetFunc) (void) = 0;//declare reset function at address 0

float analog(int pin){
  unsigned long int collect=0;
  for (byte i = 0; i < ANALOG_PRECISION ; i++){
    collect+=analogRead(pin);
  }
  //Serial.println(collect);
  return constrain(float(collect)/float(ANALOG_PRECISION*1023.0),0.0,1.0);
}

void loop(){
	time = millis();
	//hours = time /(60.0*60.0); //DEBUG
	hours = time /(1000.0*60.0*60.0);
	Serial.print("Current time is:");
	Serial.print(hours);
	
  float potvalue = analog(pot);
  Serial.print(" Analog pot value = ");
  Serial.println(potvalue);
	if (hours > 24.0){
		Serial.println("Day elapsed, resetting, hoping it is noon");
		resetFunc();
	}

	if (digitalRead(btnpin)==0){
		mode = (mode+1)%3;
		Serial.print("Button pressed, setting mode to");
		Serial.println(mode);
		digitalWrite(ledpin,HIGH);
		delay(500);
		digitalWrite(ledpin,LOW);
	}
	if (mode==0){
		if (hours <8.0)	{ //morning off
			analogWrite(lightpin,0);
			digitalWrite(fanpin,LOW);
		}
		
		else if (hours<10.0){ //8-10AM ramp up
			analogWrite(lightpin,(hours-8.0)*128.0*potvalue);
			digitalWrite(fanpin,LOW);
		}

		else if (hours<16.0){ //10-4pm full
			analogWrite(lightpin,255.0*potvalue);
			digitalWrite(fanpin,LOW);
		}

		else if (hours<22.0){ //4-10pm ramp down
			analogWrite(lightpin,(20.0-hours)*42.0*potvalue);
			digitalWrite(fanpin,LOW);
		}
		else{
			analogWrite(lightpin,0);
			digitalWrite(fanpin,LOW);
		}
	}
	if ((hours>13.0) && (hours<13.5)){
		digitalWrite(solenoid,HIGH);
	}else{
		if (digitalRead(swpin)==0){
			digitalWrite(solenoid,HIGH);
		}else{
			digitalWrite(solenoid,LOW);
		}
	}
	if (mode==1){ //force on
		digitalWrite(fanpin,LOW);
		analogWrite(lightpin,255*potvalue);
		digitalWrite(ledpin,HIGH);
	}
	if (mode==2){ //force off
		digitalWrite(fanpin,LOW);     
		analogWrite(lightpin,0);
		flashled();
	}
	delay(50);
}
