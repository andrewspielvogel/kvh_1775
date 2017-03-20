#ifndef BIAS_CONSUMER_H
#define BIAS_CONSUMER_H

#include "wqueue.h"
#include "bias_est.h"
#include "thread.h"
#include <ros/ros.h>
#include <Eigen/Core>


class BiasConsumerThread : public Thread
{
  wqueue<GyroData*>& m_queue;
 
 public:
 BiasConsumerThread(wqueue<GyroData*>& queue, Eigen::VectorXd k, float lat, float hz) : m_queue(queue), bias(k,lat) {}

  BiasEst bias;
 
  void* run()
  {

    Eigen::Matrix3d Rni;
    Rni << 1,0,0,0,-1,0,0,0,-1;
    // Remove 1 item at a time and process it. Blocks if no items are 
    // available to process.
    for (int i = 0;; i++)
    {
      GyroData* item = m_queue.remove();
      bias.step(Rni,item->ang,item->acc,item->diff);

      //delete item;
    }
    return NULL;
  }
};

#endif