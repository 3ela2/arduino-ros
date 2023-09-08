
#include <ros.h>
#include <std_msgs/String.h>
#include <Servo.h>

// Define ROS node handle and subscribers
ros::NodeHandle nh;
std_msgs::String message;

// Flags for controlling door, curtains, and window to avoid opening an open door  and so on
bool door_open = false;
bool curtains_open = false;
bool window_open = false;
bool day=true;

// Define Servo variables
Servo curtain_servo;
Servo window_servo;

// Define Motor, LED and buzzer pins
#define led1_pin 11
#define  buzzer_pin 10
#define motorpin1 5
#define motorpin2 2
#define motor_enb 3

// Motor/servo control constants
#define  motorspeed   150 // Adjust the motor speed as needed
#define motorDuration   1000 // Motor movement duration in milliseconds

void setup() {
  // Initialize ROS node
  nh.initNode();
  
  // Subscribe to ROS topic
  ros::Subscriber<std_msgs::String> sub("smart_4", &callback_function); // Replace "callback_function" with your actual callback function
nh.subscribe(sub);
  Serial.begin(9600);

  
  curtain_servo.attach(9);   // Attach curtain servo to pin 10
  window_servo.attach(8);    // Attach window servo to pin 11


  pinMode(led1_pin, OUTPUT);
  pinMode(motorpin1, OUTPUT);
  pinMode(motorpin2, OUTPUT);
  pinMode(motor_enb, OUTPUT);
  pinMode(buzzer_pin, OUTPUT);
  digitalWrite(led1_pin, LOW);
}
void callback_function(){
if (strcmp(message.data, "HI") == 0 && strcmp(message.data, "BY") == 0) { //Checking the header & ooter to make sure message is sent correctly
    char command = message.data[2];
    doWork(command);
  }
}
void loop() {
  nh.spinOnce(); // Handle ROS messages

  // if(Serial.available()){
  //   char c= Serial.read();
  //   doWork(c);
  // }
}


void doWork(char c ){

    switch (c) {
      case '1':
        if (!door_open) {
          open_door();
          Serial.println("DOOR");
        }
        break;
        
      case '2':
        if (door_open) {
          close_door();
        }
        break;
        
      case '3':
        if (!curtains_open) {
          open_curtains();
          day=false;
        }
        break;
        
      case '4':
        if (curtains_open) {
          close_curtains();
          day=true;
        }
        break;
        
      case '5':
        emergency();
        break;
        
      case '6':
        emergency_ends();
   
        break;
        
      default:
        // Invalid command
        break;
    }
}

void open_door() {
  digitalWrite(motorpin1,HIGH);
  digitalWrite(motorpin2,LOW);
  analogWrite(motor_enb,motorspeed);
  delay(motorDuration);
   analogWrite(motor_enb,0);
  door_open = true;
}

void close_door() {
  digitalWrite(motorpin2,HIGH);
  digitalWrite(motorpin1,LOW);
  analogWrite(motor_enb,motorspeed);
  delay(motorDuration);
  analogWrite(motor_enb,0);
  door_open = false;
}

void open_curtains() {
  curtain_servo.write(90); // Open the curtains
  digitalWrite(led1_pin, HIGH); // Turn on LED1
  curtains_open = true;
}

void close_curtains() {
  curtain_servo.write(0); // Close the curtains
  digitalWrite(led1_pin, LOW); // Turn off LED1
  curtains_open = false;
}

void emergency() {
  window_servo.write(90); // Open the window
  if(!curtains_open) open_curtains(); // Open the curtains simultaneously
  if(!door_open) open_door(); // Open the door simultaneously
  digitalWrite(buzzer_pin, HIGH); // Activate the buzzer
  tone(buzzer_pin,200);
  window_open = true;
}

void emergency_ends() {
  window_servo.write(0); // Close the window
  if(day) close_curtains(); // Close the curtains simultaneously
  close_door(); // Close the door simultaneously
  digitalWrite(buzzer_pin, LOW); // Deactivate the buzzer
  noTone(buzzer_pin);
  window_open = false;
}