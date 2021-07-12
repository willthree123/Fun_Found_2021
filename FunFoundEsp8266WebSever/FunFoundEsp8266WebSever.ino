#include <Servo.h>
#include <ESP8266WiFi.h>

#define servo_pin D1
#define openDoorSwitch_pin D2
#define greenLED_pin 12
#define yellowLED_pin 2
#define closeDoorTime 250

const char* ssid = "LLC-iPAD";    //  Your Wi-Fi Name
const char* password = "12qwaszx";   // Wi-Fi Password
int value = LOW;

WiFiServer server(80);
Servo myservo;


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
  while (digitalRead(openDoorSwitch_pin) == LOW) {
    myservo.write(45);//OPEN DOOR
    Serial.println("SERVO MOVING");
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

void WiFiSetup() {

  Serial.begin(115200); //Default Baudrate
  Serial.print("Connecting to the Newtork");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  server.begin();  // Starts the Server
  Serial.println("Server started");
  Serial.print("IP Address of network: "); // will IP address on Serial Monitor
  Serial.println(WiFi.localIP());
  Serial.print("Copy and paste the following URL: https://"); // Will print IP address in URL format
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void webSeverRequestCheck() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  while (!client.available()) {
    delay(1);
  }
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  if (request.indexOf("/DOOR1=OPEN") != -1 && value==LOW) {
    digitalWrite(yellowLED_pin, LOW); // Turn LED ON
    openDoor();
    Serial.println(F("Door Opened."));
    value = HIGH;
  }

  if (request.indexOf("/DOOR1=CLOSE") != -1 && value==HIGH) {
    digitalWrite(yellowLED_pin, HIGH); // Turn LED OFF
    closeDoor();
    Serial.println(F("Door Closed."));
    value = LOW;
  }
  //*------------------HTML Page Code---------------------*//



  client.println("HTTP/1.1 200 OK"); //
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<html>");
  client.println("<link rel=\"preconnect\" href=\"https://fonts.googleapis.com\"> <link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin> <link href=\"https://fonts.googleapis.com/css2?family=Open+Sans&display=swap\" rel=\"stylesheet\">");
  client.print(" DOOR STATUS: ");
  client.print(F("<style>body {background-color: #1e1e1e ;font-family: 'Open Sans', sans-serif; color:white; display:flex; flex-direction:column; align-items:center; justify-items:center;}</style>"));

  if (value == HIGH) {
    client.print("OPEN");
  }

  else {
    client.print("CLOSE");
  }

  client.println("<br><br>");
  client.println("<h1><a href=\"/DOOR1=OPEN\"\"><button>OPEN</button></a></h1>");
  client.println("<h1><a href=\"/DOOR1=CLOSE\"\"><button>CLOSE</button></a></h1><br />");
  client.println("</html>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}
void setup() {

  WiFiSetup();
  Serial.println("WiFiSetup Finished.");
  //delay(1000);
  //Serial.println("Setup started.");
  pinSetup();
  //Serial.println("pinSetup Finished.");
  initialize();
  //Serial.println("initialize Finished.");
}
void loop() {
  webSeverRequestCheck();
}
