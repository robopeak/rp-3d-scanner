The RP 3D-Scanner firmware requires the library code and the building system of Arduino-Lite. 
In order to build it, please follow these steps:

> Download Arduino-Lite from Google Code if you have not done so before.
  
  Arduino-Lite project home page: 
      http://code.google.com/p/arduino-lite/

  You may download the zip-package from the downloads tag or simply svn check-out it.

> Setup the Arduino-Lite if you have not done so before.
  
  The setup progress is very simple, just extract the zip file and click to run a patch file.

  You may refer to the following tutorial for details:
      http://www.robopeak.net/blog/?p=151

> Copy the 3d-scanner/ folder into the <Arduino-Lite>/sketch/ folder

> The firmware also requires avr SPI/Servo driver libraries. These libraries currently haven't been released to the public by RoboPeak. You need to provide similar implementations. 

> Entering <Arduino-Lite>/sketch/rp_usb_connector/ folder, simply click build.cmd to build the firmware
  Alternatively, you may type the make command manually. To do so, just click launchsh.cmd and you will see a bash shell.


--------
Arduino-Lite is an lightweight and efficient runtime system for AVR chips based on the work of Arduino Project.

For more details, refer to the following articles
  Introduction to Arduino-Lite:
     http://www.robopeak.net/blog/?p=131

  How to install and use Arduino-Lite:
     http://www.robopeak.net/blog/?p=151

  Arduino-Lite Interface & Development Mannual
     http://www.robopeak.net/blog/?p=107 
