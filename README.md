# SmartTree Electrical System Design 
This repo contains the PCB design and Arduino code for the SmartTree electrical system. As a part of Engineers for a Sustainable World (ESW) at Northwestern, SmartTree is a student-run design project aimed to bring solar energy to Northwestern and get the community involved in sustainability. SmartTree provides a ‘trunk’ of solar-powered AC and USB outlets centered around a seating area, so anyone can bring his or her laptop or phone outside without having to worry about running low on battery. Having SmartTree on campus will not only allow students to best utilize technology outdoors, but also raise awareness for the potential solutions clean energy can bring to everyday life. Learn more about ESW @ NU here. 

The repo's directory is briefly described as below:

### Construction Documentation
contains the overall construction and design documentation, including hardware and software overview of the system. 

### BMS Software Documentation
contains the documentation for the battery management system software. It includes the design rationale of the software and some messaging protocal we use to communicate between the BMS board and the screen board. 

### PCB 
contains the PCB design of the battery management system (BMS) and the cell monitor unit (CMU), refer to the documentation for more details. 

### SmartTree Client
contains the client code of the software. The client is the microcontroller in charge of the screen update for the UI. It receives sensor data from the server and displays it on the screen. 

### SmartTree Server
contains the server code which controls and monitors the cell monitor unit and send out the information to the client for UI. 

### Testing
contains some the testing data logged on an SD card. 
