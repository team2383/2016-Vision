/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "USBCamera.h"
#include "ErrorBase.h"
#include "nivision.h"
#include "NIIMAQdx.h"

#include "HAL/cpp/priority_mutex.h"
#include <thread>
#include <memory>
#include <condition_variable>
#include <tuple>
#include <vector>

class CameraServer {
 private:


 protected:
  CameraServer();

 public:
  static CameraServer* GetInstance();
  void Serve();
};
