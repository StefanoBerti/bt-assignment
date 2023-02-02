/*
 *   Copyright (c) 2022 Michele Colledanchise
 *   All rights reserved.
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:

 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.

 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 */

/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/


#include <behaviortree_cpp_v3/condition_node.h>
#include "person_is_focused.h"

#include <chrono>
#include <thread>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <ActionRecognition.h>


PersonIsFocused::PersonIsFocused(string name, const NodeConfiguration& config) :
    ConditionNode(name, config)
{
    is_ok_ = init(name);


}

bool PersonIsFocused::init(std::string name)
{
  yarp::os::Network yarp;

  yarp::os::Port client_port;

  std::string server_name = "/Components/FakeActionRecognition/Server"s;
  std::string client_name = "/BT/" + name + "/FakeActionRecognition/Client"s;

  client_port.open("client_name");

  // connect to server
  if (!yarp.connect("client_name",server_name))
  {
     std::cout << "Error! Could not connect to server " << server_name << '\n';
     return false;
  }

  action_recognition_client_.yarp().attachAsClient(client_port);
  return true;
}

NodeStatus PersonIsFocused::tick()
{
    return action_recognition_client_.is_focused() ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}