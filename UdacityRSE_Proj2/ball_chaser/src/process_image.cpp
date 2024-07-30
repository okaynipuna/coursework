#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>
#include <std_msgs/Float64.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    //ROS_INFO_STREAM("Driving Robot: linear x = "<< lin_x << ", angular z = " << ang_z);
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z; 
    
    //TODO: Change from help AI
    if (client.call(srv)) {
        ROS_INFO("Robot moving towards the ball.");
    } else {
        ROS_ERROR("Failed to call service command_robot");
    }
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{
    float drive_ang_z = 0.0;
    float drive_lin_x = 0.0;
    int white_pixel = 255;
    bool is_white_pixel = false;
    int white_pixel_loc[2] = {0, 0};

    // Loop through each pixel in the image and check if there's a bright white one
    for (int i = 0; i < img.height ; i++) {
        for (int j = 0; j < img.step; j++){
            int index = i * img.step + j;
            /* When first white pixel is found, break pixel search and record location of white pixel.
               Really, I would want to collect all white pixels and find the centroid, but that's probably for later. 
               That approach is probably moot since the first white pixel seen in processing the image stream will
               be the horizantal midpoint, which for this application is center. 
            */
            if (img.data[index] == white_pixel) {
                is_white_pixel = true;
                white_pixel_loc[0] = i; // Not really useful for this prompt, but leaving it. 
                white_pixel_loc[1] = j;
                //ROS_INFO_STREAM("White Pixel Seen at: x: " << j);
                break;
            }
        }
    }

    // Then, identify if this pixel falls in the left, mid, or right side of the image
    if (white_pixel_loc[1] < (img.step / 3)) {
        // Drive angular_z clockwise such that the ball is centered and drive forward
        //ROS_INFO_STREAM("White Pixel is in the left");
    //  drive_lin_x = 0.5;  // Unsure if the requirement is to drive and turn or drive only when centered
        drive_ang_z = 0.5;
    } else if (white_pixel_loc[1] > (img.step * 2/3)){
        // Drive angular_z CCW such that the ball is centered and drive forward
        //ROS_INFO_STREAM("White Pixel is in the right");
    //  drive_lin_x = 0.5;  // Unsure if the requirement is to drive and turn or drive only when centered
        drive_ang_z = -0.5;
    } else {
        // Eh. Close enough. Drive forward only
        //ROS_INFO_STREAM("White Pixel is in the center");
        drive_lin_x = 0.5;
        drive_ang_z = 0.0;
    }

    // Request a stop when there's no white ball seen by the camera
    if (is_white_pixel == false) {
        ROS_INFO_STREAM("Stopping. No white ball seen by camera.");
        drive_lin_x = 0.0;
        drive_ang_z = 0.0;
    }

    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    drive_robot(drive_lin_x, drive_ang_z);
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}