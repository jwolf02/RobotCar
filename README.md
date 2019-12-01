# RobotCar
Remote controlled robot car
  
## Project
This project consists of two programs, one is rchost which runs on  
the robot car (but can also be run on a PC platform).  
The other is rcmonitor-ui which will be run on a PC and displays  
the streamed video and various state information.  
Users can type controls in the monitor program that are sended to  
the host.  
Optionally an object detector can be employed to detect objects in the  
image stream which are then marked before they are streamed over the  
network.  
Configuration parameters are loaded from a config file.  

## Control    
Control the car by WSAD (maybe you need to adjust the controls if your wiring differs)  
Press q to stop car and x to end program  

