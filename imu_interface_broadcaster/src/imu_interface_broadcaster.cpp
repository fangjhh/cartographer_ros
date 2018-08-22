#include <ros/ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Float64MultiArray.h>
#include <sensor_msgs/Imu.h>
#include <tf/transform_broadcaster.h>
#include <sstream>
#include "packet.h"
#include "imu_data_decode.h"
#include "serial.h"

#define M_G 10.0
#define pi 3.1416
/**
 * This tutorial demonstrates simple sending of messages over the ROS system.
 */
 struct STime
{
  unsigned char ucYear;
  unsigned char ucMonth;
  unsigned char ucDay;
  unsigned char ucHour;
  unsigned char ucMinute;
  unsigned char ucSecond;
  unsigned short usMiliSecond;
};


struct SAcc
{
  short a[3];
  short T;
};

struct SGyro
{
  short w[3];
  short T;
};

struct SAngle
{
  short Angle[3];
  short T;
};

struct SMag
{
  short h[3];
  short T;
};

struct SQuat
{
  short Q[4];
};
struct STime    stcTime;
struct SAcc     stcAcc;
struct SGyro    stcGyro;
struct SAngle   stcAngle;
struct SMag     stcMag;
struct SQuat    stcQuat;
int main(int argc, char **argv)
{
  ros::init(argc, argv, "imu_interface_broadcaster");
  ros::NodeHandle n;
  
  float Roll = 0.0;
  float Pitch = 0.0;
  float Yaw = 0.0;

  float Gyro_x = 0.0;
  float Gyro_y = 0.0;
  float Gyro_z = 0.0;

  double Acc_x = 0.0;
  double Acc_y = 0.0;
  double Acc_z = 0.0;

  double Q_x = 0.0;
  double Q_y = 0.0;
  double Q_z = 0.0;
  double Q_w = 0.0;
  
  int16_t Acc[3] = {0};
  int16_t Gyo[3] = {0};
  int16_t Mag[3] = {0};
  float Eular[3] = {0};
  float Quat[4] = {0};
  

  sensor_msgs::Imu imu_data;


  double timeScanCur = 0.0;

  ros::Publisher broad_caster_pub = n.advertise<sensor_msgs::Imu>("/imu/data", 50);
  
  ros::Rate loop_rate(1000);
  int count = 0;
  /**
   * Parameters to get IMU datas
   */
  open_serial_port();
  
  while (ros::ok())
  {
    read_once(Acc, Gyo, Mag, Eular, Quat);
    /**
     * This is a message object. You stuff it with data, and then publish it.
     */
   Roll = (float)Eular[0]*pi/180;
   Pitch = (float)Eular[1]*pi/180;
   Yaw = (float)Eular[2]*pi/180;

   Gyro_x = (float)Gyo[0]*pi*0.1/180;
   Gyro_y = (float)Gyo[1]*pi*0.1/180;
   Gyro_z = (float)Gyo[2]*pi*0.1/180;

   Q_x = (double)((float)Quat[1]);
   Q_y = (double)((float)Quat[2]);
   Q_z = (double)((float)Quat[3]);
   Q_w = (double)((float)Quat[0]);

     Acc_x = (double)Acc[0]*M_G/1000;
     Acc_y = (double)Acc[1]*M_G/1000;
     Acc_z = (double)Acc[2]*M_G/1000;
     

     imu_data.header.seq=0;
     imu_data.header.stamp = ros::Time::now();
     imu_data.header.frame_id = "imu_link";

     imu_data.orientation.x = Q_x;
     imu_data.orientation.y = Q_y;
     imu_data.orientation.z = Q_z;
     imu_data.orientation.w = Q_w;

     imu_data.linear_acceleration.x = Acc_x;
     imu_data.linear_acceleration.y = Acc_y;
     imu_data.linear_acceleration.z = Acc_z;
     
     imu_data.angular_velocity.x = Gyro_x;
     imu_data.angular_velocity.y = Gyro_y;
     imu_data.angular_velocity.z = Gyro_z;
     
     imu_data.orientation_covariance[0]=-(double)1.0;
  
      count++;
      if(count >= 10)
       {
          count = 0;
          broad_caster_pub.publish(imu_data);
          //ROS_INFO("Roll:%.3f Pitch:%.3f Yaw:%.3f, Q_x:%.3f Q_y:%.3f Q_z:%.3f ", Roll, Pitch, Yaw, Quat[1], Quat[2], Quat[3]);
       }

    /**
     * The publish() function is how you send messages.
     */
    ros::spinOnce();
    loop_rate.sleep();
  }
  close_serial_port();
  return 0;
}
