// ---------------------------------------------------------------------
// Include
// ---------------------------------------------------------------------
#include <Arduino.h>
#include <arm.h> //ATIM library for LoRaWAN connection
#include <LowPower.h>
#include <IsTime.h>

// ---------------------------------------------------------------------
// Define
// ---------------------------------------------------------------------
// The 3 colours LED on the Airboard is driven by 3 different digital pins:
// PIN 5 is Green colour, PIN 9 is Red colour, PIN 6 is Blue color
#define LED_OK 5 
#define LED_ERROR 9
#define LED_TX 6

#define TILT_PIN 3  // Tilt sensor output pin (D3)
const static int interval = 30000; // 30 seconds interval

// ---------------------------------------------------------------------
// Global variables
// ---------------------------------------------------------------------
volatile long count = 0; // count number of sent messages
long unsigned previous_millis;
String buf = "TX N="; // LoRaWAN message that will be sent to Objenious
byte msgg[3];           // Store the data to be uploaded to Objenious Network


//Instance of  the class Arm
Arm Objenious; // Needed to make work the LoRaWAN module

// ---------------------------------------------------------------------
// This function makes blink the LED
// ---------------------------------------------------------------------

void Blink(byte PIN, int DELAY_MS) 
{ 
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}

// ---------------------------------------------------------------------
// This function is triggered when a vibration is detected
// ---------------------------------------------------------------------
void tiltStateChange(void) {
  if (IsTime(previous_millis, interval)) { // ignore redundant vibrations
    previous_millis = millis();

    // Power up !
    //LowPower.powerSave(SLEEP_500MS, ADC_OFF, BOD_OFF, TIMER2_OFF);

    // Send the notification through LoRaWan network
    Blink(LED_TX,100);      // After sending the data we blink the BLUE led for 100 milliseconds
    delay(1000);
    count++; //increment counter
    msgg[1] = ((byte) (count>>8));                     // https://www.arduino.cc/en/Reference/Bitshift
    msgg[2] = (byte) count;
    Objenious.Send(msgg, sizeof(msgg));               // Send the temp to Objenious network 
    
    // Power down state with ADC and BOD module disabled
    //LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

  }
}

// ---------------------------------------------------------------------
// Implemented functions
// ---------------------------------------------------------------------
void setup()
{ 
    // Interrupt: Each time the tilt sensor status changes, the mcu executes the "tiltStateChange" function
    attachInterrupt(TILT_PIN - 2, tiltStateChange, CHANGE);
  
    // We have to configure the digital pins as output in order to use the LED
    pinMode(LED_ERROR, OUTPUT); // RED color
    pinMode(LED_TX, OUTPUT); // BLUE color   
    pinMode(LED_OK, OUTPUT); // GREEN color 
    // Then we ensure the LEDs are OFF
    digitalWrite(LED_ERROR, LOW);
    digitalWrite(LED_TX, LOW);
    digitalWrite(LED_OK, LOW);


// ---------------------------------------------------------------------
// LoRaWAN module Init and configuration
// ---------------------------------------------------------------------
    
    //Init of the LoRaWAN module - Red light if error, Green light if Ok 
    if(Objenious.Init(&Serial) != ARM_ERR_NONE)
    {
        digitalWrite(LED_ERROR, HIGH);
    }
    else
    {
        digitalWrite(LED_OK, HIGH);
    }

    // Configuration of the LoRaWAN module
    Objenious.SetMode(ARM_MODE_LORAWAN);

    Objenious.LwEnableRxWindows(true);
    Objenious.LwEnableTxAdaptiveSpeed(true);
    Objenious.LwEnableDutyCycle(true);
    Objenious.LwEnableTxAdaptiveChannel(true);
    Objenious.LwEnableRx2Adaptive(true);
    
    Objenious.LwEnableOtaa(true);
    
    //Apply the configuration to the module.
    Objenious.UpdateConfig();
    
    delay(8000); // delay needed for the module to connect to Objenious

    // If the module connected succesfully then Green LED will be ON
    // If the module couldn't connect then Red LED will be ON
    // We then turn them both OFF after the 8 seconds
    digitalWrite(LED_OK, LOW);
    digitalWrite(LED_ERROR, LOW);
    
// ---------------------------------------------------------------------
// Here starts your code :D
// ---------------------------------------------------------------------

    msgg[0]=2; // This byte will indicate to Objenious platform what kind
               // of sketch we are using and hence how to decode the data:
               //   - 1 = Temperature data
               //   - 2 = Push button data
               //   - 3 = Window/Door open data

}


// ---------------------------------------------------------------------
// How the code works:
// Each time you press the button an interrupt will call the push() function
// This function will turn TRUE the "state" boolean variable.
// Each time the "state" variable is TRUE, the micro will:
//      * Send a LoRaWAN message to objenious
//      * Blink the BLUE LED for 100milisecond
//      * Print on the SoftwareSerial the same information sent to objenious
//      * Increment the sent messages counter.
// The buf string is defined at the beginning of this code, you can find more information 
// on how to concatenate strings at https://www.arduino.cc/en/Tutorial/StringAdditionOperator
// ---------------------------------------------------------------------

void loop()
{
    
}



