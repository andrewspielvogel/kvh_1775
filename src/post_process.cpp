#include <Eigen/Core>
#include <helper_funcs/helper_funcs.h>
#include <truenorth/att_est.h>
#include <helper_funcs/gyro_data.h>
#include <string>
#include <iostream>
#include <fstream>




int main(int argc, char* argv[])
{



  /**************************************************
   *         CHECK FOR CORRECT ARGUMENTS
   **************************************************/
  if (argc != 2)
  {

    printf("USAGE: post_process CONFIG_FILE.m\n");
    return 1;
    
  }  
  
  /***************************************************
   *
   * LOAD CONFIG FILE
   *
   ***************************************************/

  const config_params params = load_params(argv[1]);
  print_loaded_params(params);


  /***************************************************
   *
   * INITIALIZE ESTIMATOR
   *   
   ***************************************************/
  
  AttEst att(params);
  GyroData gyro_data(params.hz);

  printf("***********************************\n");
  printf("    RUNNING ATTITUDE ESTIMATION\n");
  printf("***********************************\n");

  std::ifstream infile(params.i_file.c_str());
  FILE *outfile;
  outfile = fopen(params.o_file.c_str(),"w");

  std::string line;
  double time_start = 0.0;
  bool start = false;
  Eigen::Vector3d att_euler_ang;
  int hours = 0;
  int minutes = 0;
  int seconds = 0;

  int cnt = 1;
  
  Eigen::Vector3d rpy_align = rot2rph(params.R_align);
  Eigen::Vector3d rpy_Ro    = rot2rph(params.R0);

  

  
  fprintf(outfile,"PARAMS,%s,%d,%.10f,%s,%s,%f,%f,%f,%f,%f,%f,%.10f,%.10f,%.10f,%.10f,%.10f,%.10f,%.10f,%.10f,%.10f,%.10f,%.10f,%.10f,%f,%f,%f,%f,%f,%f\n",params.last_mod.c_str(),params.hz,params.lat,params.o_file.c_str(),params.i_file.c_str(),rpy_align(0),rpy_align(1),rpy_align(2),rpy_Ro(0),rpy_Ro(1),rpy_Ro(2),params.K_acc(0,0),params.K_acc(1,1),params.K_acc(2,2),params.K_ang_bias(0,0),params.K_ang_bias(1,1),params.K_ang_bias(2,2),params.K_acc_bias(0,0),params.K_acc_bias(1,1),params.K_acc_bias(2,2),params.K_E_n(0,0),params.K_E_n(1,1),params.K_E_n(2,2),params.K_g(0,0),params.K_g(1,1),params.K_g(2,2),params.K_north(0,0),params.K_north(1,1),params.K_north(2,2));


  int samp_cnt = 0;
  
  while (std::getline(infile, line))
  {

    samp_cnt += 1;
    char msg_type[32];
    int year;
    int month;
    int day;
    int hour;
    int minute;
    float second;

    double rov_time;
    double ros_time;
    Eigen::Vector3d a_c;
    Eigen::Vector3d ang;
    Eigen::Vector3d acc;

    Eigen::VectorXi status(6);
    sscanf(line.c_str(),"%s %d/%d/%d %d:%d:%f %lf %lf %lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%f,%d,%lf,%lf,%d,%d,%d,%d,%d,%d,%lf,%lf,%lf\n",msg_type,&year,&month,&day,&hour,&minute,&second,&rov_time,&ros_time,&gyro_data.ang(0),&gyro_data.ang(1),&gyro_data.ang(2),&gyro_data.acc(0),&gyro_data.acc(1),&gyro_data.acc(2),&gyro_data.mag(0),&gyro_data.mag(1),&gyro_data.mag(2),&gyro_data.temp,&gyro_data.seq_num,&gyro_data.timestamp,&gyro_data.comp_timestamp,&status(0),&status(1),&status(2),&status(3),&status(4),&status(5),&a_c(0),&a_c(1),&a_c(2));

    
    if (!start)
      {

	start = true;
	time_start = rov_time;
      
      }
    float time = rov_time - time_start;
    acc = gyro_data.acc*9.81;
    ang = gyro_data.ang;

    //att.step(ang,acc-skew(ang)*a_c,1.0/(float)params.hz);
    att.step(ang,acc,1.0/(float)params.hz);

    att_euler_ang = rot2rph(att.att.R_ni*params.R_align.transpose());


    //if ((samp_cnt % 50) == 0) {
      fprintf(outfile,"ATT_PRO,%d,%02d,%02d,%02d,%02d,%02f,%f,%f,%f,%f,%.10f,%.10f,%.10f,%.10f,%.10f,%.10f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d,%.10f,%.10f,%.10f\n",year,month,day,hour,minute,second,rov_time,
	    att_euler_ang(0),att_euler_ang(1),att_euler_ang(2),                // CSV col 8, 9, 10
	    att.bias.w_b(0),att.bias.w_b(1),att.bias.w_b(2),                   // CSV col 11, 12, 13
	    att.bias.east(0),att.bias.east(1),att.bias.east(2), // CSV col 14, 15, 16
	    att.bias.a_b(0),att.bias.a_b(1),att.bias.a_b(2),                   // CSV col 17, 18, 19 
	    att.bias.acc_hat(0),att.bias.acc_hat(1),att.bias.acc_hat(2),       // CSV col 20, 21, 22
	    acc(0),acc(1),acc(2),                                              // CSV col 23, 24, 25
	    ang(0),ang(1),ang(2),                                              // CSV col 26, 27, 28
	    gyro_data.mag(0),gyro_data.mag(1),gyro_data.mag(2),                // CSV col 29, 30, 31
	    gyro_data.temp,                                                    // CSV col 31, 31, 33
	    gyro_data.seq_num,                                                 
	    status(0),status(1),status(2),status(3),status(4),status(5),
	    a_c(0),a_c(1),a_c(2));

      //}
    

    if ((((int)time) % (1800) < 10))
      {
      
      hours   = ((int) time)/3600;
      int minutes_ = ((int) time - hours*3600)/60;
      if (minutes_ != minutes)
	{
	minutes = minutes_;
	char buffer [256];
	// int n = sprintf(buffer,"%02d:%02d:00 OF DATA PROCESSED",hours,minutes);
	// std::cout<<"\r"<<buffer<<"\n";
	}
      }
  }
  printf("\n");
  infile.close();
  fclose(outfile);

  return 0;
}
