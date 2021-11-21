# FS2020 - Switch Panel
A simple external switches and LEDs panel to manage some FS2020 airplanes commands.

![fig0](https://user-images.githubusercontent.com/94467184/142744166-63711825-c21c-4483-9d37-9ca438069cd0.jpg)


I'm a technician but i'm also interested in flight both simulated and real (Ultra Light Planes). Few years ago i bought 5 **Arduino Nano** to build a Star Tracker: a device capable to rotate at the same rotation speed of the earth. That project was a solution to take photos (with a long exposure) of the stars during the night but for that project i used only 1 of 5 Arduino.

Some months ago i bought the new **Microsoft Flight Simulator 2020** and after a while i realized that i was having a hard time reading the status of some parameters and commands on the simulator. Consider that i wear reading glasses and compared to the old version (FS2004) some writings on the panels of the aircraft are smaller.

Furthermore, i had long wanted to try to build my own additional panel for the simulator using one of the Arduino Nano that i had in my drawer: and so i started this project that is the first and the simple one of my (two) DIY panels for the Flight Simulator.

## **What it does**
The panel manages and shows the state of:
1. flaps, incrementing/decrementing to max 4 positions
2. navigation lights (grouped in STROBE, NAV, BEACON, WING, LOGO, CABIN and PANEL)
3. landing lights (grouped in LANDING and TAXI)
4. pitot heat
5. rudder trim
6. startup and shutdown the engine(s) with AUTO_START and AUTO_SHUTDOWN functions
7. in addition i have also a RESET button that is useful when starting the flight to be sure about the state of all the lights of the plane

Of course everyone can choose to use a different number of buttons and LEDs to manage different simulator commands and parameters.

## **The architecture**

In the following picture you can see **3 components**: 
1. the switching panel (with Arduino and some buttons and LEDs)
2. a sort of "bridge" program managing the bidirectional communications with FS
3. the Flight Simulator itself

The panel reads parameters and send commands to FS using the **FS2020TA.exe** (you will find a link to for FS2020 at the end) as a sort of communication "repeater" and Arduino communicates with the PC and FS2020TA.exe (that uses a SimConnect.dll) using a standard USB port.

<img src="https://user-images.githubusercontent.com/94467184/142736651-6c75b9a9-ad6f-494a-b993-45f7de33e24a.jpg" width="70%" height="70%">

## **What you need**
What you need is:
1. an ordinary Arduino board (not necessarily a Nano)
2. some switches and/or buttons
3. some LEDs and resistors 
4. one or more small breadboards 
5. a soldering iron
6. some small section wires to connect switches and LEDs to the Arduino board

![fig2](https://user-images.githubusercontent.com/94467184/142744891-d1c6cf09-d8b0-4657-97cb-fe291e369c9b.jpg)

Due to the small power consumption, **the panel is simply powered by the 5Vcc of the USB** connection and basically the only limit of the number of status LEDs and command buttons/switches is the number of I/O pin of the Arduino board you are using.

In my case i used **9 buttons/switches + 6 status LEDs**.
