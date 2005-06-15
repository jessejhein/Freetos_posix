ReadMe.txt of mwlike8(microwindows in 8-bit mcu)

Contents: (arithmetical order)
* Modules
* Requirement
* Technical Info.
* Version Number
* ToDo

========================================================================================================
Version Number (0.01.XX is testing version, 0.02.XX will be stable version)
        0.01.03 : 
        		* working version before May 2005

========================================================================================================
Description 
* this mwlike8 aims to make microwindows work in 8-bit mcu
* this system provides any ui interface

========================================================================================================
Requirements
* fixable to control number of LED(s) in ON, OFF, +ve pulse and -ve pulse

========================================================================================================
Technical Info.
* block diagram
		|
		v
	  nano-X
	  	|
   +----+----+----
   |	|	 | ... any ui hw devices
   v	v	 v
  LEDs LCD Buzzer
* Data flow
		<...> -- external
	
       <init>
      -----------------> [init]
      						matching hw connections
      						close all LEDs and its variable

       <on/off/+ve pulse/-ve pulse led>
      -----------------------------------> [ON/OFF/+ve pulse/-ve pulse LED(which led)]
													set corresponding LED status

       <insert event>
      -----------------------------------> [insert a event(what event)]
													insert event to event queue

	  <main Loop from OS>
	  	main processes (should be put in srvmain.c for linlike8 switch mode)
	  			|
	  			+---> get next events
	  			|		|
	  			|		+----> return time out event, call time out event customer method
	  			|
	  			+---> ... 
	  			|
* Methods
	insert a event(what event)
		insert event to event queue
	init
		close all hw LEDs
			by reading the off LED function array[LED_NUM]
			call all LEDs off function
		init. all LED's variables
	get next events
		variables
			LED status array[LED_NUM]
		chk event
			timeout event
				return timeout event
				timeout event for internal
					looping for all LEDs
						do what LED status array[LED_NUM] ask to do
			... event
	ON/OFF/+ve pulse/-ve pulse LED(which led)
		variables
			which LED
		get the LED status array[which LED]
		set the corresponding cmd to this LED status array[which LED]
	real time ON/OFF/+ve pulse/-ve pulse LED(which led)
		variables
			which LED
		call normal ON/OFF/+ve pulse/-ve pulse LED(which led)
		do the hw ctrl as well
	time out event customer method
		<a place for customer code>
		
========================================================================================================
Modules
* LED control
	* state diagram
														------------- this signal cannot detect, 
														|
														v
	API, customer ctrl			^			   v       ^ v						v
	
	real time API cust. ctrl								  ^    v	^
	
	timeout event			|		|		|		|		|		|		|		|
	
						 ON			|---------------|		  |----|	|-----------|
	real LED,			OFF --------|				|---------|	   |----|			|---
	
	where 
		^ -- ON signal
		v -- OFF signal

========================================================================================================
ToDo
* LED
	* old LED events is overwritten by new LED event
