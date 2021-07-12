//edit version
#include <Servo.h>
#include <ESP8266WiFi.h>

#define servo_pin D1
#define openDoorSwitch_pin D2
#define greenLED_pin D0
#define yellowLED_pin D4
#define closeDoorTime 250

// Enter your wifi network name and Wifi Password
const char* ssid = "LLC-iPAD";
const char* password = "12qwaszx";

// Set web server port number to 80
WiFiServer server(80);
Servo myservo;
// Variable to store the HTTP request
String header;

// These variables store current output state of LED
String doorOpenState = "off";

// Assign output variables to GPIO pins
const int redLED = 2;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void initialize() {
	openDoor();

	for (int i = 0; i < 5; i++) {
		digitalWrite(yellowLED_pin, HIGH);
		delay(50);
		digitalWrite(yellowLED_pin, LOW);
		delay(50);
	}
	delay(1000);
	closeDoor();
	digitalWrite(yellowLED_pin, HIGH);
}

void openDoor() {
	while (digitalRead(openDoorSwitch_pin) == HIGH) {
    myservo.write(45);//OPEN DOOR
    Serial.println("SERVO MOVING");
    digitalWrite(greenLED_pin, LOW);
}
myservo.write(90);
}

void closeDoor() {
	myservo.write(150);
	delay(closeDoorTime);
	digitalWrite(greenLED_pin, HIGH);
	myservo.write(90);
}
void pinSetup() {
	myservo.attach(servo_pin);
	myservo.writeMicroseconds(1500);
	pinMode(openDoorSwitch_pin, INPUT_PULLUP);
	pinMode(greenLED_pin, OUTPUT);
	pinMode(yellowLED_pin, OUTPUT);
	digitalWrite(greenLED_pin, LOW);
	digitalWrite(yellowLED_pin, LOW);
	myservo.write(90);
}

void webserverHandle(){

WiFiClient client = server.available(); // Listen for incoming clients

if (client) { // If a new client connects,
Serial.println("New Client."); // print a message out in the serial port
String currentLine = ""; // make a String to hold incoming data from the client
currentTime = millis();
previousTime = currentTime;
while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
	currentTime = millis(); 
if (client.available()) { // if there's bytes to read from the client,
char c = client.read(); // read a byte, then
Serial.write(c); // print it out the serial monitor
header += c;
if (c == '\n') { // if the byte is a newline character
// if the current line is blank, you got two newline characters in a row.
// that's the end of the client HTTP request, so send a response:
	if (currentLine.length() == 0) {
// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
// and a content-type so the client knows what's coming, then a blank line:
		client.println("HTTP/1.1 200 OK");
		client.println("Content-type:text/html");
		client.println("Connection: close");
		client.println();

// turns the GPIOs on and off
		if (header.indexOf("GET /2/on") >= 0 && doorOpenState=="off") {
			Serial.println("RED LED is on");
			doorOpenState = "on";
			openDoor();
		} else if (header.indexOf("GET /2/off") >= 0 && doorOpenState=="on") {
			Serial.println("RED LED is off");
			doorOpenState = "off";
			closeDoor();
		} 

// Display the HTML web page
		client.println("<!DOCTYPE html><html>");
		client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
		client.println("<link rel=\"icon\" href=\"data:,\">");
// CSS to style the on/off buttons 
		client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
		client.println(".buttonRed { background-color: #ff0000; border: none; color: white; padding: 16px 40px; border-radius: 60%;");
		client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
		client.println(".buttonOff { background-color: #77878A; border: none; color: white; padding: 16px 40px; border-radius: 70%;");
		client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}</style></head>");

// Web Page Heading
		client.println("<body><h1>My LED Control Server</h1>");

// Display current state, and ON/OFF buttons for GPIO 2 Red LED 
		client.println("<p>Red LED is " + doorOpenState + "</p>");
// If the doorOpenState is off, it displays the OFF button 
		if (doorOpenState=="off") {
			client.println("<p><a href=\"/2/on\"><button class=\"button buttonOff\">OFF</button></a></p>");
		} else {
			client.println("<p><a href=\"/2/off\"><button class=\"button buttonRed\">ON</button></a></p>");
		} 
// The HTTP response ends with another blank line
		client.println();
// Break out of the while loop
		break;
} else { // if you got a newline, then clear currentLine
	currentLine = "";
}
} else if (c != '\r') { // if you got anything else but a carriage return character,
currentLine += c; // add it to the end of the currentLine
}
}
}
// Clear the header variable
header = "";
// Close the connection
client.stop();
Serial.println("Client disconnected.");
Serial.println("");
}
}
void WiFiSetup(){
	// Connect to Wi-Fi network with SSID and password
	Serial.print("Connecting to ");
	Serial.println(ssid);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
// Print local IP address and start web server
	Serial.println("");
	Serial.println("WiFi connected.");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	server.begin();
}
void setup() {
	Serial.begin(115200);
// Initialize the output variables as outputs
	pinSetup();
  //Serial.println("pinSetup Finished.");
	initialize();
//WiFi Setup
	WiFiSetup();
}

void loop(){
	webserverHandle();

}