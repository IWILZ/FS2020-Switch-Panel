# FS2020 - Switch Panel with Arduino
A simple external switches and LEDs panel to manage some FS2020 (for PC) airplanes commands.

![fig0](https://user-images.githubusercontent.com/94467184/142744166-63711825-c21c-4483-9d37-9ca438069cd0.jpg)


If you are a Flight Simulator enthusiast and would try to build your first external panel to send some commands and read some status from FS, probably you will be interested in this project because **it's simple, flexible and cheap**.

A short video here: https://www.youtube.com/watch?v=05aUgwHtf3Q

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
2. a program (FS2020TA.exe) that manages the bidirectional communications with FS (you will find a link to this program later in this document)
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

Optionally you can also use a little **5Vcc passive buzzer** (see the next picture) just as a "sound confirmation" of any button-press whenever you send a command to FS2020.

![fig4](https://user-images.githubusercontent.com/94467184/143272337-8f0b1c5f-56fa-4433-81ad-36c0374db240.jpg)

For the components you will find some links at the end of this document.

## **Input/Output connections**
Every Arduino pin can be configured as an Input or an Output by the program so all it's very flexible. Obviously every LED connection has to be an Output (any output pin produces a 5Vcc when in high level) and every button/switch an Input but in this last case the program have to configure it like an "INPUT_PULLUP" pin to avoid problems with random status reading.

In the following picture you can see how to connect a generic LED and a generic button/switch.

![fig3](https://user-images.githubusercontent.com/94467184/142747592-85055ccf-bf22-468f-97f8-a19cade9ffb0.jpg)

**IMPORTANT:** to avoid a damage of the micro controller itself **DON'T CONNECT A LED DIRECTLY to the Arduino**, but **use a resistor to limit the current** flowing to the LED. **The resistor value depends on the LED brand and colour** (normally red ones need a lower value resistor than green ones) but you could start with a value of **1KOhm** and then change it to find the right value/light for your LED. If you have a tester you can also measure the current flowing into the LED considering that the maximum current on a output PIN of the Arduino cannot **never exceed 20mA**. If you cannot measure the current, just look at your LED's light and don't exceed with its brightness. 

Each button should be "normally opened" so it will "close the circuit" to the ground only when pressed.

## **The FS2020TA.exe**
Yes this is the software "bridge" to make a bi-directional communication between Arduino and FS2020. 
The program is free, was developed by **Matthias Schaaf** and can be dwonloaded from: https://github.com/Seahawk240/Microsoft-FS2020-To-Arduino

FS2020TA.exe (wich uses SimConnect.dll) is very simple and strong and the communication is made using standard **Serial** functions. To get informations from FS you have to make a sort of list within FS2020TA and then you will use **Serial.readStringUntil()** into the Arduino sketch. In the same way to send commands you simply have to use **Serial.print()**.

You can also find a video description here: https://www.youtube.com/watch?v=EVqY8KhdZI8 and for more informations, please read the official documentation from Matthias Schaaf.

About FS2020 variables, you can also find some useful info here: https://docs.flightsimulator.com/html/index.htm#t=Programming_Tools%2FSimVars%2FSimulation_Variables.htm

## **The communication protocol with FS2020TA**
The communications is made simply sending and receiving strings over the USB connection of the PC. 

**The sintax of a parameter received** is **"@ID/index=value$"** where '@', '/' and '$' are markers to identify the 3 values "**ID**", "**index**" and "**value**". 
1. **ID** = idientifies each parameter
2. **index** = idientifies different objects having the same parameter (for example index=1 for the "active frequency" of NAV1 but index=2 for NAV2)
3. **value** = is the value itself and can be an integer, float, string, degree, boolean, etc depending of the kind of parameter

Of course depending on the "ID" value, the program has to manage "value" converting it from a string to a number if necessary.

## **Define of I/O pins**
Just to give you a bit more informations, i will show you wich pin i use in my project, but if you want change your Arduino board or for some other reasons you want to change your pins you will just change their "defines" accordingly.

```
/*************************************************************
            Buttons and switches input pins
 *************************************************************/
#define IN_ENGINE_START       2
#define IN_ENGINE_STOP        3
#define IN_FLAPS_DEC          4
#define IN_FLAPS_INC          5
#define IN_PITOT_HEAT         6   // Switch 
#define IN_LIGHTS_NAV         7
#define IN_LIGHTS_LAND        8
#define IN_RUDDER_TRIM_RIGHT  10
#define IN_RUDDER_TRIM_LEFT   11

/*************************************************************
                     LEDs output pins
 *************************************************************/
#define OUT_LIGHTS_LAND       A0
#define OUT_LIGHTS_NAV        A1
#define OUT_FLAPS_1           A5 
#define OUT_FLAPS_2           A4
#define OUT_FLAPS_3           A3
#define OUT_FLAPS_4           A2

// ************** Optional buzzer on PIN 9 ****************
// To activate it uncomment the following line
//#define BUZZER_CONNECTED
#define BUZZER_PIN            9 // Pin 9 for the buzzer
```

## **Where to buy components**
Sorry, coming soon.


