#include <ros.h>
#include <std_msgs/String.h>
#include <EEPROM.h>

// Define ROS node handle and publishers
ros::NodeHandle nh;
std_msgs::String message;
ros::Publisher pub("smart_4", &message);
int input;
int pass;
int count =0;
#define ldr A3
#define gas A2
long duration, distance;
#define trigPin 12
#define echoPin 11
bool em=false;
void setup() {
 
  // Initialize ROS node
  nh.initNode();
  EEPROM.put(0, 123);
  // Advertise ROS topic
nh.advertise(pub);

  
  // Set pin modes
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
}

int readIntFromSerial() {
   int intValue = 0;
   while (Serial.available() > 0) {
      char c = Serial.read();
      if (isdigit(c)) {
         intValue = intValue * 10 + (c - '0');
      }
   }
   return intValue;
}

void password() {
  input = readIntFromSerial();
  if (input!=NULL){ 
  count++;
  }
  EEPROM.get(0, pass);// Read the stored password from EEPROM
  if (count==1){
  if (input == pass) {
    Serial.println("Correct password!");
     //digitalWrite(test4,HIGH); 
  }
  else {
    Serial.println("wronge");
   // digitalWrite(test4,LOW);}
  }
    count--;
}
}


void loop() {
   password();
 //ultrasonic_sensor();
//smoke_sensor();
//ldr_sensor();
  // Delay before next reading
  delay(1000);
}

void ultrasonic_sensor()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) * 0.0343;
  Serial.println(distance);
  delay(5);
  if (distance < 30) {
    message.data = "HI1BY";
    Serial.println("OPEN THE DOOR");
  }
  else{
    message.data = "HI2BY";
    Serial.println("CLOOOOOOOSE THE DOOR");
  }
    pub.publish(&message);
    
}

void smoke_sensor() {
  int gas_read = analogRead(gas);
  //Serial.print("Sensor Value: ");
  //Serial.println(gas_read);
  delay(500);
  Serial.println(gas_read);
  if (gas_read > 200) {
    message.data = "HI5BY";
    Serial.println("EMERGENCYYYYYYYYY");
    pub.publish(&message);
    em=true;
  }
  else{
    if(em){
      message.data = "HI6BY";
    pub.publish(&message);
    Serial.println("EMERGENCY ENDSSS");
    em=0;
    }
  }
}

void ldr_sensor() {
  int ldr_read = analogRead(ldr);
  //Serial.println(ldr_read);
  
  if (ldr_read <200) {
    Serial.println("OPEN THE CURTAINS");
    message.data = "HI3BY";
  } else {
    message.data = "HI4BY";
    Serial.println("CLOOSE THE CURTAINS");
  }
  Serial.println(ldr_read);
  pub.publish(&message);
}
