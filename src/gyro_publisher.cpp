/*
 * gyro_publisher.cpp
 * node for publishing sensor data
 *
 * created July 2015
 * Andrew Spielvogel
 * andrewspielvogel@gmail.com
 */


#include <ros/ros.h>
#include <kvh_1775/serial_io.h>
#include <kvh_1775/gyro_sensor_data.h>
#include <kvh_1775/helper_funcs.h>
#include <Eigen/Core>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

#define NODE_RESTART_TIME 1 // time to wait while no data before restarting (in seconds)


int main(int argc, char **argv)
{

    // initialize node
    ros::init(argc, argv, "kvh_1775");

    // must initialize if "~" for param passing
    ros::NodeHandle n("~");

    // initialize publisher
    ros::Publisher chatter = n.advertise<kvh_1775::gyro_sensor_data>("gyro_data",1000);


    /*
     * Load in params
     */

    // rate in Hz
    int rate = 1000;
    n.getParam("rate",rate);
    ros::Rate loop_rate(rate);

    // port name
    std::string port = "/dev/ttyUSB0";
    n.getParam("port",port);

    // log file location
    std::string log_location = "/var/log/KVH/";
    n.getParam("log_loc",log_location);

    // baud rate
    int baud = 921600;
    n.getParam("baud",baud);
    
    // instrument alignment matrix
    std::string instr_align = "1,0,0,0,1,0,0,0,1"; 
    n.getParam("instr_align",instr_align);
    Eigen::MatrixXd R_align = parse_string(instr_align);
    R_align.resize(3,3);

    // estimation gains
    std::string gains = "1.0,0.005,0.005,0.005,0.3"; 
    n.getParam("gains",gains);
    Eigen::VectorXd k = parse_string(gains);



    /*
     * INITIALIZE SERIAL PORT
     */

    SerialPort serial(k(0),k(1),k(2),k(3),k(4), R_align,log_location.c_str());

    // connect to serial port
    bool connected =  serial.start(port.c_str(),baud);

    if (connected==false)
    {
	ROS_ERROR("port not opened");
	return 0;
    }
    else 
    {
	ROS_INFO("connected to port: %s",port.c_str());
    }



    /*      
     * MAIN LOOP
     */

    // initialize time since data var
    int cur_time_since_data = 0;

    while (ros::ok())
    {

      // initialize data_msg
      kvh_1775::gyro_sensor_data data_msg;

      // fill data_msg with data packet
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

      // check if still getting data
      int time_from_last_msg = (int)abs(ros::Time::now().toSec()-serial.data.prev_time);

      if (time_from_last_msg>=1)
      {

	if(cur_time_since_data != time_from_last_msg)
	{
	  ROS_ERROR("Lost Connection. No data for %d seconds",time_from_last_msg);
	  cur_time_since_data = time_from_last_msg;
	}
	if (time_from_last_msg>NODE_RESTART_TIME)
	{
	
	  ROS_ERROR("No data for over %d seconds. Restarting node...",time_from_last_msg);
	  serial.stop();
          // connect to serial port
	  serial.start(port.c_str(),baud);

	}

      }
      
      ros::spinOnce();
      
      loop_rate.sleep();

    }

    serial.stop();
    return 0;
}
