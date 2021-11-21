# FS2020 - Switch Panel
A simple external switches and LEDs panel to manage some FS2020 (for PC) airplanes commands.

![fig0](https://user-images.githubusercontent.com/94467184/142744166-63711825-c21c-4483-9d37-9ca438069cd0.jpg)


If you are a Flight Simulator enthusiast and would try to build your first external panel to send some commands and read some status from FS, probably you will be interested in this project because **it's simple, flexible and cheap**.

## **Why this project**
I'm quite a beginner in the Arduino world but i'm also really interested in flight both simulated and real (Ultralight Planes). Few years ago i bought 5 **Arduino Nano** to build a Star Tracker: a device capable to rotate at the same rotation speed of the earth. That project was a solution to take photos (with long exposures) of the stars during the night but for that project i used only 1 of my 5 Arduino boards.

Some months ago i bought the **Microsoft Flight Simulator 2020** and i use it with a Saitek X52 (so i have a lot of buttons for the main commands) but after a while i realized that i was having some problems for:
1. reading the status of some parameters and values (for example on the Garmin display)
2. managing other FS commands using the mouse during the flight 

So in decided to start this first project using one of the Arduino Nano that i had in my drawer, trying to reach a good software architecture before building a more complex solution to manage and display also numeric values (like frequencies, altitude, air speed, etc) in a second project.


## **What it does**
The panel manages and shows the state of:
1. flaps incrementing/decrementing them to max 4 positions
2. navigation lights (grouped in STROBE, NAV, BEACON, WING, LOGO, CABIN and PANEL)
3. landing lights (grouped in LANDING and TAXI)
4. pitot heat
5. rudder trim
6. engine(s) startup and shutdown with AUTO_START and AUTO_SHUTDOWN functions of simulator
7. in addition i have also a RESET button that is useful when starting the flight to be sure about the state of all the lights of the plane

Of course you can choose to use a different number of buttons and LEDs adapting hardware and software to manage/show different simulator commands and parameters.

## **The architecture**

In the following picture you can see **3 components**: 
1. the switching panel (with Arduino and some buttons and LEDs)
2. a program (FS2020TA.exe) that manages the bidirectional communications with FS (you will find a link to that program at the end of this document)
3. the Flight Simulator itself

The panel reads values and send commands to FS using the **FS2020TA.exe** (made by Seahawk240) as a sort of communication "repeater". The Arduino board communicates with the PC and FS2020TA.exe (that uses a SimConnect.dll) using a standard USB port. The communication protocol is very simple and will be explaned later.

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
For the front panel i used a 15x8cm rectangle of a carbon fiber plate but you can use also a wood plate or anything else from about 1.5 to 3mm thick.
