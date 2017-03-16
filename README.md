# STM3246NG-DISCO Ethernet / LDT 

**NOTE** This is a experimental PoC and not currently being developed. 

---

### Overview of code structure

- The microcontrollers are essentially hosting a server and constantly in a waiting state.
	- The transmitter microcontroller is always waiting for input from a connected user and will 
	  execute a given valid command.
	
	- The receiver microcontroller is constantly running a digitalreadin until the photodiode circuit
	  outputs a sufficient voltage to break the loop. Once the readin is complete, the receiver will clear
	  its ethernet buffer (sometimes, it may need another packet to clear it).
	  
PCs -> Java application using sockets on the specified port to connect to the microcontroller "servers".

NOTE: DO NOT SEND MORE THAN 100 BYTES AT A TIME! THE BUFFER ON THE MICROCONTROLLER WILL OVERFLOW AND
CRASH THE PROGRAM

If you have any questions feel free to email us via the contact button on the Members page
-> http://www.eecs.ucf.edu/seniordesign/fa2015sp2016/g32/
