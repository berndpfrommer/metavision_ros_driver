# -----------------------------------------------------------------------------
# Copyright 2021 Bernd Pfrommer <bernd.pfrommer@gmail.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#

import launch
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode
from launch.substitutions import LaunchConfiguration as LaunchConfig
from launch.actions import DeclareLaunchArgument as LaunchArg
from launch.actions import OpaqueFunction
from ament_index_python.packages import get_package_share_directory
import os


def launch_setup(context, *args, **kwargs):
    """Create composable node."""
    cam_name = LaunchConfig('camera_name')
    cam_str = cam_name.perform(context)
    pkg_name = 'metavision_ros_driver'
    share_dir = get_package_share_directory(pkg_name)
    bias_config = os.path.join(share_dir, 'config', 'silky_ev_cam.bias')
    container_1 = ComposableNodeContainer(
        name='metavision_driver_container_1',
        namespace='',
        package='rclcpp_components',
        executable='component_container',
        output='screen',
        composable_node_descriptions=[
            ComposableNode(
                package='metavision_ros_driver',
                plugin='metavision_ros_driver::DriverROS2',
                name=cam_name,
                parameters=[
                    {'use_multithreading': False,
                     'message_type': 'event_array',
                     'statistics_print_interval': 2.0,
                     'bias_file': bias_config,
                     'camerainfo_url': '',
                     'frame_id': '',
                     'event_message_time_threshold': 1.0e-3}],
                remappings=[
                    ('~/events', cam_str + '/events')],
                extra_arguments=[{'use_intra_process_comms': False}],
            )])
    container_2 = ComposableNodeContainer(
        name='metavision_driver_container_2',
        namespace='',
        package='rclcpp_components',
        executable='component_container',
        output='screen',
        composable_node_descriptions=[
            ComposableNode(
                package='rosbag2_composable_recorder',
                plugin='rosbag2_composable_recorder::ComposableRecorder',
                name="recorder",
                parameters=[{'topics': ['/event_camera/events'],
                             'bag_prefix': 'events_'}],
                remappings=[
                    ('~/events', cam_str + '/events')],
                extra_arguments=[{'use_intra_process_comms': True}],
            )
        ])
    return [container_1, container_2]


def generate_launch_description():
    """Create composable node by calling opaque function."""
    return launch.LaunchDescription([
        LaunchArg('camera_name', default_value=['event_camera'],
                  description='camera name'),
        OpaqueFunction(function=launch_setup)
        ])
