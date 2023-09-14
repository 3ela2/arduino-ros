// Importing important libraries
#include <ros.h>
#include <std_msgs/String.h>

// Define ROS node handle and publishers
ros::NodeHandle nh;
std_msgs::String message;
ros::Publisher pub("from_sensors_to_actuators", &message);
int input;
int pass;
int count =0;
#define ldr A3
#define gas A2
long duration, distance;
#define trigPin 12
#define echoPin 11
bool emergency=false,intrpt=false;

void setup() {
   //If arduino notices that gas sensor reading is rising it will stop everything and activate the interrupt function
   attachInterrupt(gas, interruptt, RISING); 
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

void loop() {
 ultrasonic_sensor();  // reads the ultrasonic readings and open the door when someone is in front of the door
smoke_sensor();   // reads the gas sensor readings and open doors and windows in case of gas leakings
ldr_sensor(); // reads the LDR readings and open curtains at night and close them during day
  // Delay before next reading
  delay(500);
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
  // Serial.println(distance);
  delay(5);
  if (distance < 30) {
    message.data = "1"; // Message for opening the door
    //Serial.println("OPEN THE DOOR");
  }
  else{
    message.data = "2"; // message for closing the door
    // Serial.println("CLOOOOOOOSE THE DOOR");
  }
    pub.publish(&message); //sending the message to the actuators node
    
}

void interruptt(){
  intrpt= analogRead(gas) > 200; // activating the bool
  smoke_sensor(); // calling smoke function to send the msg to actuators
}
void smoke_sensor() {
  if (intrpt & !emergency)  // if interrupt was called at non-emergency state
  { 
    message.data = "5"; //emergency message 
    pub.publish(&message);
    emergency=true;
  }
  else{
    if(emergency && analogRead(gas) <200) // if emergency mode was activated but gas is now below limit
    {
      message.data = "6"; // end of emergency message
    pub.publish(&message);
    // Serial.println("emergencyERGENCY ENDSSS");
    emergency=0;
    intrpt=0;
    }
  }
}

void ldr_sensor() {
  int ldr_read = analogRead(ldr);
  //Serial.println(ldr_read);
  
  if (ldr_read <200) {
    // Serial.println("OPEN THE CURTAINS");
    message.data = "3"; //opening curtains at night message 
  } else {
    message.data = "4"; // closing curtains at day message
    // Serial.println("CLOOSE THE CURTAINS");
  }
  // Serial.println(ldr_read);
  pub.publish(&message);
}
