This is the project of road bump detection and identification (robudeid)

<b>Implemented with:</b><br>
Blas<br>
libLinear 1.9.4<br>
OpenCV 2.4.3<br>
C/C++ using Microsoft Visual Studio 2012 IDE.<br>

<b>System flowchart:</b><br>
<img src='http://i.imgur.com/O8vUFbm.png'>
<br><br><br>

<br>
Abstract<br>
<br>
Advanced vehicle safety is a recently emerging issue, appealed from the rapidly explosive population of cars. Increasing driver assistance systems have been designed for warning drivers of what should be noticed by analyzing surrounding environments with sensors and/or cameras. As one of the hazard road conditions, road bumps not only damage vehicles but also cause serious danger, especially at night or under poor lighting conditions.<br>
In this thesis we propose a vision-based road bump detection system using a front-mounted car camcorder, which tends to be widespread deployed. First, we estimate the vehicle motion through input video and infer the existence of road bumps. Once a bump is detected, objects at the vicinities of the detected bumps are then recognized. Consequently, the information of this bump is then extracted. The location obtained from GPS and information is reported to a central server, so that other vehicles can receive warnings when approaching the detected bumpy regions. Encouraging experimental results show that the proposed system can detect road bumps efficiently and effectively. It can be expected that traffic security will be significantly promoted through the mutually beneficial mechanism that a driver who is willing to report the bumps he/she encounters can receive warnings issued from others as well.