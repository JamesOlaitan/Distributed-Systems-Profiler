# Distributed Systems Profiler
 A Python and C++ project to simulate a distributed system using FastAPI services, Prometheus for metrics aggregation and Grafana for real-time dashboards 

## Build and Launch ROS2 Bridge

```bash
colcon build --packages-select prometheus_bridge
. install/setup.bash
ros2 launch prometheus_bridge prometheus_bridge.launch.py
```
