#include <sstream>
#include <fstream>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <helper_funcs/helper_funcs.h>
#include <truenorth/bias_est.h>
#include <truenorth/gyro_data.h>
#include <string>
#include <math.h>

int main(int argc, char* argv[])
{

  int hz = 1000;
  float lat = 39.32*M_PI/180;


  Eigen::VectorXd k(4);
  k<<0.1,.001,0,0;

  std::string out_file_name = "/home/spiels/log/processedbias2.csv";
  std::string in_file_name = "/home/spiels/log/ICRA2018/2017_8_8_9_38.KVH";

  Eigen::Vector3d rpy(0.0,0,0.0);

  printf("RUNNING BIAS ESTIMATION ON CSV FILE: %s\n",in_file_name.c_str());
  printf("WRITING TO FILE: %s\n",out_file_name.c_str());

  std::ifstream infile(in_file_name.c_str());
  FILE *outfile;
  outfile = fopen(out_file_name.c_str(),"w");
  
  BiasEst bias(k,lat);

  GyroData gyro_data(hz);
  Eigen::Matrix3d Rni_phins;
  char msg_type[32];

  std::string line;
  int samp_processed = 0;
  
  Eigen::Matrix3d Ralign = rpy2rot(rpy);
  Eigen::Vector3d w_err(.1,.1,1);
  w_err = w_err*0*M_PI/180;
  Eigen::Matrix3d R_err = skew(w_err).exp();

  while (std::getline(infile, line))
  {
    sscanf(line.c_str(),"%[^,],%lf,%lf,%lf,%lf,%lf,%lf, %lf,%lf,%lf, %f, %d, %lf,%lf, %*d, %*d, %*d, %*d, %*d, %*d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf \n",msg_type,&gyro_data.ang(0),&gyro_data.ang(1),&gyro_data.ang(2),&gyro_data.acc(0),&gyro_data.acc(1),&gyro_data.acc(2),&gyro_data.mag(0),&gyro_data.mag(1),&gyro_data.mag(2),&gyro_data.temp,&gyro_data.seq_num,&gyro_data.timestamp,&gyro_data.comp_timestamp,&Rni_phins(0,0),&Rni_phins(0,1),&Rni_phins(0,2),&Rni_phins(1,0),&Rni_phins(1,1),&Rni_phins(1,2),&Rni_phins(2,0),&Rni_phins(2,1),&Rni_phins(2,2));

    //sscanf(line.c_str(),"%lf,%lf,%lf,%lf,%lf,%lf, %lf,%lf,%lf, %f, %d, %lf,%lf, %*d, %*d, %*d, %*d, %*d, %*d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf \n",&gyro_data.ang(0),&gyro_data.ang(1),&gyro_data.ang(2),&gyro_data.acc(0),&gyro_data.acc(1),&gyro_data.acc(2),&gyro_data.mag(0),&gyro_data.mag(1),&gyro_data.mag(2),&gyro_data.temp,&gyro_data.seq_num,&gyro_data.timestamp,&gyro_data.comp_timestamp,&Rni_phins(0,0),&Rni_phins(0,1),&Rni_phins(0,2),&Rni_phins(1,0),&Rni_phins(1,1),&Rni_phins(1,2),&Rni_phins(2,0),&Rni_phins(2,1),&Rni_phins(2,2));

    Eigen::Vector3d phins_rpy = rot2rph(Rni_phins*Ralign);
    
    bias.step(Rni_phins*Ralign*R_err,gyro_data.ang,gyro_data.acc,gyro_data.mag,1.0/((float)hz));

    fprintf(outfile,"BIAS_PRO,%f,%.10f,%.10f,%.10f,%.10f,%.10f,%.10f,%.10f,%.10f,%.10f,%.10f,%.10f,%.10f\n",gyro_data.timestamp,bias.w_b(0),bias.w_b(1),bias.w_b(2),phins_rpy(0),phins_rpy(1),phins_rpy(2),bias.a_b(0),bias.a_b(1),bias.a_b(2),bias.m_b(0),bias.m_b(1),bias.m_b(2));

    samp_processed++;
    

    if ((samp_processed) % (hz*60) == 0) {
      
      int seconds = samp_processed/hz;
      int hours   = seconds/3600;
      int minutes = (seconds - hours*3600)/60;
      seconds = seconds - hours*3600 - minutes*60;
      printf("%02d:%02d:%02d OF DATA PROCESSED\n",hours,minutes,seconds); 

    
    }
  }




}
