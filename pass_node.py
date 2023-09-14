#!/usr/bin/env python3

import rospy
import os
import time

from std_msgs.msg import Int32

class PasswordCheckerNode:
    def __init__(self):
        rospy.init_node('password_checker_node')

        # Parameters
        self.password_filename = "password.txt"
        self.max_attempts = 3
        self.freeze_duration = 20

        # Load the password from the file
        self.load_password()

        # Initialize variables
        self.attempts = 0
        self.freeze_end_time = None

        # Publisher for the open_door_with_password topic
        self.pub = rospy.Publisher('open_with_password', Int32, queue_size=10)

    def load_password(self):
        try:
            with open(self.password_filename, 'r') as file:
                self.password = file.readline().strip()
        except IOError:
            rospy.logerr("Error reading password file.")
            rospy.signal_shutdown("Password file not found.")
    
    def check_password(self, input_password):
        return input_password == self.password

    def run(self):
        while not rospy.is_shutdown():
            if self.attempts < self.max_attempts:
                user_input = raw_input("Enter password: ")

                if self.check_password(user_input):
                    rospy.loginfo("Correct password")
                    self.pub.publish(1)  # Publish 1 on success
                    self.attempts = 0  # Reset attempts
                else:
                    rospy.logwarn("Not Correct! Try Again")
                    self.attempts += 1
            else:
                rospy.loginfo(f"Too many incorrect attempts. Freezing for {self.freeze_duration} seconds.")
                self.freeze_end_time = time.time() + self.freeze_duration
                self.attempts = 0

            if self.freeze_end_time is not None and time.time() >= self.freeze_end_time:
                self.freeze_end_time = None

if __name__ == '__main__':
    try:
        node = PasswordCheckerNode()
        node.run()
    except rospy.ROSInterruptException:
        pass
