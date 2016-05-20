/*
 * gyro_publisher.cpp
 * node for publishing sensor data
 *
 * created July 2015
 * Andrew Spielvogel
 * andrewspielvogel@gmail.com
 */


#include "ros/ros.h"
#include "std_msgs/String.h"
#include <kvh_1775/serial_io.h>
#include "kvh_1775/gyro_sensor_data.h"
#include <eigen/Eigen/Core>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <sstream>
#include <string>
#include <stdlib.h>



int main(int argc, char **argv)
{

    // initialize
    ros::init(argc, argv, "kvh_1775");

    // must initialize if "~" for param passing
    ros::NodeHandle n("~");

    // initialize publisher
    ros::Publisher chatter = n.advertise<kvh_1775::gyro_sensor_data>("gyro_data",1000);

    // rate in Hz
    int rate = 1000;
    n.getParam("rate",rate);
    ros::Rate loop_rate(rate);

    // port name
    std::string name = "/dev/ttyUSB0";
    n.getParam("port",name);

    // baud rate
    int baud = 921600;
    n.getParam("baud",baud);
    
    // instrument alignment matrix
    std::string instr_align = "1,0,0,0,-1,0,0,0,-1"; 
    n.getParam("instr_align",instr_align);
    
    boost::char_separator<char> sep(",");
    boost::tokenizer<boost::char_separator<char> > tokens(instr_align,sep);    

    Eigen::MatrixXd R_align(9,1);
    int i = 0;
    BOOST_FOREACH (const std::string& t, tokens)
    {

      R_align(i) = strtod(t.c_str(),NULL);  
      i++;

    }
    R_align.resize(3,3);

    // initialize serial port
    SerialPort serial(1.0,0.005,0.005,0.005,.3, R_align);

    // connect to serial port
    bool connected =  serial.start(name.c_str(),baud);

    if (connected==false)
    {
	ROS_ERROR("port not opened");
	return 0;
    }
    else 
    {
	ROS_INFO("connected to port: %s",name.c_str());
    }

    int cur_time_since_data = 0;

    while (ros::ok())
    {

      // initialize data_msg
      kvh_1775::gyro_sensor_data data_msg;

      // fill data_msg with sensor packet
      for (int i=0;i<3;i++)
      {
	data_msg.ang.at(i) = serial.data.ang(i);
	data_msg.acc.at(i) = serial.data.acc(i);
	data_msg.mag.at(i) = serial.data.mag(i);
	data_msg.acc_est.at(i) = serial.data.acc_est(i);
	data_msg.bias_acc.at(i) = serial.data.bias_acc(i);
	data_msg.bias_ang.at(i) = serial.data.bias_ang(i);
	data_msg.bias_z.at(i) = serial.data.bias_z(i);
	data_msg.att.at(i) = serial.data.att(i);
      }
	
      for (int i=0;i<6;i++)
      {
	data_msg.status.at(i) = serial.data.status.at(i);
      }
	
      data_msg.temp = serial.data.temp;
      data_msg.stamp = ros::Time::now();
      data_msg.seq_num = serial.data.seq_num;

      // publish packet
      chatter.publish(data_msg);

      int time_from_last_msg = (int)abs(ros::Time::now().toSec()-serial.data.prev_time);

      if (time_from_last_msg>=1)
      {

	if(cur_time_since_data != time_from_last_msg)
	{
	  ROS_ERROR("Lost Connection. No data for %d seconds",time_from_last_msg);
	  cur_time_since_data = time_from_last_msg;
	}
	if (time_from_last_msg>9)
	{
	
	  ROS_ERROR("No data for over %d seconds. Closing node...",time_from_last_msg);
	  serial.stop();
	  return 1;

	}

      }
      
      ros::spinOnce();
      
      loop_rate.sleep();

    }

    serial.stop();
    return 0;
}
