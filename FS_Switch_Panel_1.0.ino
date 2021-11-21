/*
  21-11-2021
  This program manages a panel with some buttons/switches & LEDs
  to send commands and receive the status of some FS2020 parameters.

  UPDATE LOG
  1.0 --> First english version

  The panel manages and shows the state of:
  1) flaps incrementing/decrementing them to max 4 positions
  2) navigation lights (grouped in STROBE, NAV, BEACON, WING, LOGO, CABIN and PANEL)
  3) landing lights (grouped in LANDING and TAXI)
  4) pitot heat
  5) rudder trim
  6) engine(s) startup and shutdown with AUTO_START and AUTO_SHUTDOWN functions of simulator
  
  There is also a RESET button: useful when starting the flight to be sure about 
  the state of all the lights of the plane.
  Optionally each button press can be audio-confirmed making a short sound by a little 5Vcc buzzer

  Sul pannello sono presenti anche alcuni LED che indicano lo stato di:
  1) Pitot heat
  2) Flaps --> 4 LED
  3) Nav (indica lo stato di tutto il gruppo di luci di navigazione)
  4) Land (indica lo stato di luci di atterraggio + taxi)

  Due to the FS2020 logic, the "Pitot" LED is turned on in HW mode directly 
  on the panel and (unlike the others) its status is NOT read by FS2020..
*/

#define ACTIVE    LOW         // Input commands (buttons) are active at LOW state (GND)
#define NOACTIVE  HIGH

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

// Optional buzzer on PIN 9. 
// To activate it uncomment the following line
// #define BUZZER_CONNECTED
#define BUZZER_PIN            9 // For the buzzer

/*************************************************************
              Parameter IDs from FS2020
 *************************************************************/
#define FLAPS_HANDLE_INDEX      247       // Flaps position (1...4)
#define LIGHT_NAV               468       // LIGHT NAV
#define LIGHT_LANDING           464       // LIGHT LANDING
#define LIGHT_STATES            474       // Light status mask

// About LIGHT_STATES see 
// "https://docs.microsoft.com/en-us/previous-versions/microsoft-esp/cc526981(v=msdn.10)?redirectedfrom=MSDN#AircraftLightsData"
// For my purposes, in this program i grouped more than 1 light together 
// so each button will change the state of a whole group. 

// The meaning of each bit is the following:
// --00 0000 0000 -> *********** ALL OFF *********** 
// --11 1111 1111 -> *********** ALL ON  *********** 
// --00 0000 0001 -> light NAV         -> group NAV
// --00 0000 0010 -> light BEACON      -> group NAV
// --00 0000 0100 -> light LANDING     -> group LAND
// --00 0000 1000 -> light TAXI        -> group LAND
// --00 0001 0000 -> light STROBE      -> group NAV 
// --00 0010 0000 -> light PANEL       -> group NAV 
// --00 0100 0000 -> light RECOGNITION -> group LAND (probably unuseful)
// --00 1000 0000 -> light WING        -> group NAV
// --01 0000 0000 -> light LOGO        -> group NAV
// --10 0000 0000 -> light CABIN       -> group NAV

// ********** Hexadecimal mask for each light *************
#define MASK_NAV          0x0001
#define MASK_BEACON       0x0002
#define MASK_LANDING      0x0004
#define MASK_TAXI         0x0008
#define MASK_STROBE       0x0010
#define MASK_PANEL        0x0020
#define MASK_RECOGNITION  0x0040
#define MASK_WING         0x0080
#define MASK_LOGO         0x0100
#define MASK_CABIN        0x0200

/*************************************************************
              Definition of commands to FS2020
 *************************************************************/
#define FLAPS_INCR            "@289/$"  // 
#define FLAPS_DECR            "@287/$"  // 
#define ENGINE_AUTO_SHUTDOWN  "@272/$"  //
#define ENGINE_AUTO_START     "@273/$"  //
#define PITOT_HEAT_ON         "@620/$"  //
#define PITOT_HEAT_OFF        "@619/$"  //
#define RUDDER_TRIM_LEFT      "@722/$"  //
#define RUDDER_TRIM_RIGHT     "@723/$"  //

// --------------- Lights ------------------
#define LANDING_LIGHTS_ON     "@464/$"  //
#define LANDING_LIGHTS_OFF    "@463/$"  //
#define STROBES_ON            "@787/$"  //
#define STROBES_OFF           "@786/$"  //
#define PANEL_LIGHTS_ON       "@611/$"  //
#define PANEL_LIGHTS_OFF      "@610/$"  //

#define TOGGLE_BEACON_LIGHTS  "@854/$"  // 
#define TOGGLE_LOGO_LIGHTS    "@884/$"  // 
#define TOGGLE_NAV_LIGHTS     "@889/$"  // 
#define TOGGLE_WING_LIGHTS    "@918/$"  // 
#define TOGGLE_TAXI_LIGHTS    "@910/$"  // 
#define TOGGLE_CABIN_LIGHTS   "@855/$"  // 
#define TOGGLE_AIRCRAFT_LABELS "@844/$" // What is this??

/*******************************************************************************************/
// Global vars
/*******************************************************************************************/
byte FlapsHandleIndex     = 1;
byte PitotStatus          = NOACTIVE;
bool NavLightsOn, LandingLightsOn;

//Global variables to store the incoming data from FS2020
int FromFS_Id, FromFS_Index;
String FromFS_Value;

// ***********************************************************************
// setup()
// ***********************************************************************
void setup() {
  Serial.begin(115200);
  Serial.setTimeout(5);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  // Input pins must be "pullup"
  pinMode(IN_ENGINE_START, INPUT_PULLUP);
  pinMode(IN_ENGINE_STOP, INPUT_PULLUP);
  pinMode(IN_FLAPS_DEC, INPUT_PULLUP);
  pinMode(IN_FLAPS_INC, INPUT_PULLUP);
  pinMode(IN_PITOT_HEAT, INPUT_PULLUP);
  pinMode(IN_LIGHTS_NAV, INPUT_PULLUP);
  pinMode(IN_LIGHTS_LAND, INPUT_PULLUP);
  pinMode(IN_RUDDER_TRIM_LEFT, INPUT_PULLUP);
  pinMode(IN_RUDDER_TRIM_RIGHT, INPUT_PULLUP);

  // Setting LED pins as output
  pinMode(OUT_LIGHTS_LAND, OUTPUT);
  pinMode(OUT_LIGHTS_NAV, OUTPUT);
  pinMode(OUT_FLAPS_1, OUTPUT);
  pinMode(OUT_FLAPS_2, OUTPUT);
  pinMode(OUT_FLAPS_3, OUTPUT);
  pinMode(OUT_FLAPS_4, OUTPUT);
  
  // To be sure that i have a well known state at the start, 
  // the program switches on every light
  SetupStartingLights();  
  
} // setup

// ***********************************************************************
// loop()
// ***********************************************************************
void loop() {
  int i, j;

/* **debug**
  // Switches on-off the builtin LED (13) just to show we are executing the main loop
  digitalWrite(LED_BUILTIN, HIGH);
  delay (10);
  digitalWrite(LED_BUILTIN, LOW);
  delay (10);
*/

  // Read status parameters from FS2020 (see "FS2020TA.exe" documentation)
  GetParamFromFS2020();

  ShowLEDFlaps();
  ManagePitot();
  ShowLightsState();

  // For each button/switch get it's status and, 
  // if active, send the right command to FS
  if (ButtonActive(IN_ENGINE_START)){
    Serial.print(ENGINE_AUTO_START);  
    CommandSent();    
  }
  if (ButtonActive(IN_ENGINE_STOP)){
    Serial.print(ENGINE_AUTO_SHUTDOWN);  
    CommandSent();    
  }

  // Flaps
  if (ButtonActive(IN_FLAPS_DEC)){
    Serial.print(FLAPS_DECR);  
    CommandSent();    
  }
  if (ButtonActive(IN_FLAPS_INC)){
    Serial.print(FLAPS_INCR);  
    CommandSent();    
  }
  
  // Rudder trim
  if (ButtonActive(IN_RUDDER_TRIM_LEFT)){
    Serial.print(RUDDER_TRIM_LEFT);  
    CommandSent();    
  }  if (ButtonActive(IN_RUDDER_TRIM_RIGHT)){
    Serial.print(RUDDER_TRIM_RIGHT);  
    CommandSent();    
  }

  // NAV lights group
  if (ButtonActive(IN_LIGHTS_NAV)){
    if (NavLightsOn){
      Serial.print(STROBES_OFF);
      Serial.print(PANEL_LIGHTS_OFF);
    } else {
      Serial.println(STROBES_ON);
      Serial.print(PANEL_LIGHTS_ON);
    }
    Serial.print(TOGGLE_NAV_LIGHTS);
    Serial.print(TOGGLE_BEACON_LIGHTS);
    Serial.print(TOGGLE_WING_LIGHTS);
    Serial.print(TOGGLE_LOGO_LIGHTS);
    Serial.print(TOGGLE_CABIN_LIGHTS);
    NavLightsOn=!NavLightsOn;     
    CommandSent();    
  }
  
  // LANDING lights group
  if (ButtonActive(IN_LIGHTS_LAND)){
    if (LandingLightsOn){
      Serial.print(LANDING_LIGHTS_OFF);
    } else {
      Serial.print(LANDING_LIGHTS_ON);
    }
    Serial.print(TOGGLE_TAXI_LIGHTS);
    LandingLightsOn=!LandingLightsOn;     
    CommandSent();    
  } 
} // Loop

/***********************************************************
  GetParamFromFS2020()
  The sintax of the string received from FS2020TA.exe is:
  <@ID/index=value$> (see "FS2020TA.exe" documentation)
 ***********************************************************/
void GetParamFromFS2020() {
String dummy, tmp_str;
int at, slash, dollar, equal;   // Markers for special character position inside the string

  if (Serial.available() > 0) {
    // Read the string until the forst '\n'
    dummy = Serial.readStringUntil('\n');

    at = dummy.indexOf('@');
    slash = dummy.indexOf('/');
    equal = dummy.indexOf('=');
    dollar = dummy.indexOf('$');

    // The '@' must be the first char otherwise the string is invalid
    if (at == 0) {
      tmp_str = dummy.substring(at + 1, slash);         // Get "id" from the string
      FromFS_Id = tmp_str.toInt();                      // 
      tmp_str = dummy.substring(slash + 1, equal);      // Get "index" from the string
      FromFS_Index = tmp_str.toInt();                   // 
      FromFS_Value = dummy.substring(equal+1, dollar);  // Get "value" from the string
    }
  }
} // GetParamFromFS2020()

// ***********************************************************************
// SetupStartingLights()
// Unfortunatly some lights don't have a On/Off command but only a "togle" one.
// This means that at the start i have to test the LIGHT_STATES mask and then
// "togle" each "off" light just to be sure to be in well known state. 
// ***********************************************************************
void SetupStartingLights() {
bool go_on=true;

  Serial.print(LANDING_LIGHTS_ON);  
  Serial.print(STROBES_ON);  
  Serial.print(PANEL_LIGHTS_ON);

  // The following lights don't have a "on/off" command
  do {
    GetParamFromFS2020();
    // Abbiamo letto una maschera della luci da FS?
    if (FromFS_Id == LIGHT_STATES) { 
      if ((FromFS_Value.toInt() & MASK_BEACON)==0)   // Light off?
        Serial.print(TOGGLE_BEACON_LIGHTS);   // Let me switch on
      if ((FromFS_Value.toInt() & MASK_LOGO)==0)     // Light off?
        Serial.print(TOGGLE_LOGO_LIGHTS);     // Let me switch on
      if ((FromFS_Value.toInt() & MASK_NAV)==0)      // Light off?
        Serial.print(TOGGLE_NAV_LIGHTS);      // Let me switch on
      if ((FromFS_Value.toInt() & MASK_WING)==0)     // Light off?
        Serial.print(TOGGLE_WING_LIGHTS);     // Let me switch on
      if ((FromFS_Value.toInt() & MASK_TAXI)==0)     // Light off?
        Serial.print(TOGGLE_TAXI_LIGHTS);     // Let me switch on
      if ((FromFS_Value.toInt() & MASK_CABIN)==0)    // Light off?
        Serial.print(TOGGLE_CABIN_LIGHTS);    // Let me switch on
      
      go_on=false; // we can return
    }
  } while (go_on);
  
  CommandSent();
  NavLightsOn=true;
  LandingLightsOn=true;
} // SetupStartingLights()

/*******************************************************************************************/
// Get the flaps status from FS and show the actual position turning on it's led
/*******************************************************************************************/
void ShowLEDFlaps() {
  // Did i get a valid flaps status value?
  if (FromFS_Id == FLAPS_HANDLE_INDEX) {
    digitalWrite (OUT_FLAPS_1, LOW);
    digitalWrite (OUT_FLAPS_2, LOW);
    digitalWrite (OUT_FLAPS_3, LOW);
    digitalWrite (OUT_FLAPS_4, LOW);
    FlapsHandleIndex = FromFS_Value.toInt(); 

    switch (FlapsHandleIndex) {
      case 0:
        digitalWrite(OUT_FLAPS_1, HIGH);
        break;
      case 1:
        digitalWrite(OUT_FLAPS_2, HIGH);
        break;
      case 2:
        digitalWrite(OUT_FLAPS_3, HIGH);
        break;
      case 3:
        digitalWrite(OUT_FLAPS_4, HIGH);
        break;
    } // switch
  } // if
} // ShowLEDFlaps

/*******************************************************************************************/
// makes a buzzer sound (optional)
/*******************************************************************************************/
void CommandSent() {
#ifdef BUZZER_CONNECTED  
  tone(BUZZER_PIN, 1000, 50);
  delay(100);
#endif  
} // CommandSent

/*******************************************************************************************/
// Switching Pitot heating
/*******************************************************************************************/
void ManagePitot() {
  if (digitalRead(IN_PITOT_HEAT) == LOW && PitotStatus == NOACTIVE) {
    delay (30); // simple anti-bounce of the switch
    // i have to turn on PITOT HEAT
    Serial.print(PITOT_HEAT_ON);  
    PitotStatus = ACTIVE;
    CommandSent(); 
  } else if (digitalRead(IN_PITOT_HEAT) == HIGH && PitotStatus == ACTIVE) {
    delay (50); // Antirimbalzo
    // i have to turn off PITOT HEAT
    Serial.print(PITOT_HEAT_OFF);  
    PitotStatus = NOACTIVE;
    CommandSent(); 
  } // if
} // ManagePitot


/*******************************************************************************************/
// Read from FS2020 the state of the 2 light groups and turn on the corresponding LED when:
// 1) the MASK_NAV bit is 1
// 2) the MASK_LANDING bit is 1
/*******************************************************************************************/
void ShowLightsState() {

  // Did i get a valid LIGHT_STATES value?
  if (FromFS_Id == LIGHT_STATES) {
    // NAV group
    if (FromFS_Value.toInt() & MASK_NAV)
      digitalWrite(OUT_LIGHTS_NAV, HIGH);
    else
      digitalWrite(OUT_LIGHTS_NAV, LOW);

    // LANDING group
    if (FromFS_Value.toInt() & MASK_LANDING)
      digitalWrite(OUT_LIGHTS_LAND, HIGH);
    else
      digitalWrite(OUT_LIGHTS_LAND, LOW);

  } // if (FromFS_Id)
} // ShowLightsState


/***********************************************************
   ButtonActive() 
   Tests a button status implementing a simple anti-bounce solution.
 ***********************************************************/
bool ButtonActive(byte Button) {
bool active = false;

  if (digitalRead(Button) == LOW) {
    delay(30);
    if (digitalRead(Button) == LOW) {
      active = true;
    }
  }
  // waiting until the button will be released
  if (active) {
    do {
      // just to wait
    } while (digitalRead(Button) == LOW);
    // delay anti-rimbalzo
    delay(20);
    return (true);
  }
  else return (false);
} // ButtonActive()
