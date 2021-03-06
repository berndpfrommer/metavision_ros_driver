// -*-c++-*---------------------------------------------------------------------------------------
// Copyright 2021 Bernd Pfrommer <bernd.pfrommer@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef METAVISION_ROS_DRIVER__DRIVER_ROS2_H_
#define METAVISION_ROS_DRIVER__DRIVER_ROS2_H_

#include <camera_info_manager/camera_info_manager.hpp>
#include <dvs_msgs/msg/event_array.hpp>
#include <event_array_msgs/msg/event_array.hpp>
#include <map>
#include <memory>
#include <prophesee_event_msgs/msg/event_array.hpp>
#include <rclcpp/rclcpp.hpp>
#include <std_srvs/srv/trigger.hpp>
#include <string>

#include "metavision_ros_driver/event_publisher_ros2.h"
#include "metavision_ros_driver/metavision_wrapper.h"

namespace metavision_ros_driver
{
class DriverROS2 : public rclcpp::Node
{
public:
  explicit DriverROS2(const rclcpp::NodeOptions & options);
  ~DriverROS2();

private:
  bool stop();
  rcl_interfaces::msg::SetParametersResult parameterChanged(
    const std::vector<rclcpp::Parameter> & params);
  void onParameterEvent(std::shared_ptr<const rcl_interfaces::msg::ParameterEvent> event);
  bool start();
  void addBiasParameter(
    const std::string & name, int min_val, int max_val, const std::string & desc);
  void initializeBiasParameters();
  void declareBiasParameters();
  void saveBiases(
    const std::shared_ptr<std_srvs::srv::Trigger::Request> request,
    const std::shared_ptr<std_srvs::srv::Trigger::Response> response);
  void secondaryReady(std_msgs::msg::Header::ConstSharedPtr msg);
  MetavisionWrapper::HardwarePinConfig getHardwarePinConfig() const;
  // ---------  variables
  typedef std::map<std::string, rcl_interfaces::msg::ParameterDescriptor> ParameterMap;
  std::shared_ptr<MetavisionWrapper> wrapper_;
  std::shared_ptr<camera_info_manager::CameraInfoManager> infoManager_;
  rclcpp::Node::OnSetParametersCallbackHandle::SharedPtr callbackHandle_;
  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr saveBiasesService_;
  std::shared_ptr<rclcpp::Subscription<rcl_interfaces::msg::ParameterEvent, std::allocator<void>>>
    parameterSubscription_;
  ParameterMap biasParameters_;
  sensor_msgs::msg::CameraInfo cameraInfoMsg_;
  std::string cameraInfoURL_;
  rclcpp::Subscription<std_msgs::msg::Header>::SharedPtr secondaryReadySub_;
  // different types of publishers depending on message type
  typedef EventPublisherROS2<prophesee_event_msgs::msg::EventArray> PropheseePublisher;
  typedef EventPublisherROS2<dvs_msgs::msg::EventArray> DVSPublisher;
  typedef EventPublisherROS2<event_array_msgs::msg::EventArray> EventArrayPublisher;
  std::shared_ptr<PropheseePublisher> prophPub_;
  std::shared_ptr<DVSPublisher> dvsPub_;
  std::shared_ptr<EventArrayPublisher> eventArrayPub_;

  std::string frameId_;
};
}  // namespace metavision_ros_driver
#endif  // METAVISION_ROS_DRIVER__DRIVER_ROS2_H_
