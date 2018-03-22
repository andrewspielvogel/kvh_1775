/**
 * @file
 * @date July 2016.
 * @author Andrew Spielvogel (andrewspielvogel@gmail.com).
 * @brief Class for attitude adaptive identification on SO(3). 
 */


#ifndef ATT_EST_H
#define ATT_EST_H

#include <Eigen/Core>
#include <truenorth/so3_att.h>
#include <truenorth/fog_bias.h>


/**
 * @brief Class for attitude adaptive identificaiton on SO(3).
 */
class AttEst
{
public:

  /**
   * @brief Constructor.
   *
   * @param k Estimation gains and rolling mean window size (k(0): kg, k(1): kw, k(2): kf).
   * @param R0 Initial NED 2 Instrument Alignment estimation.
   * @param lat Latitude.
   */
  AttEst(Eigen::VectorXd k,Eigen::Matrix3d R0, float lat);

  
  virtual ~AttEst(void); /**< Destructor. */
    
  /**
   * @brief Cycle estimation once.
   *
   * @param ang Angular velocity measurement.
   * @param acc Linear acceleration measurement.
   * @param mag Magnetometer measurement.
   * @param dt Time between last two measurements.
   */
  void step(Eigen::Vector3d ang,Eigen::Vector3d acc, float dt);

  
  SO3Att att;
  FOGBias bias;
  

};

#endif
