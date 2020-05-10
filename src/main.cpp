/***************************************************************/
/*                  INCLUDE FILES                              */
/***************************************************************/
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

/***************************************************************/
/*                  PINS DEFINE                                */
/***************************************************************/
#define PHY_PIN_OUT_DOUBLE_GARAGE_DOOR                D6
#define PHY_PIN_OUT_SINGLE_GARAGE_DOOR                D7
#define PHY_PIN_OUT_EXTRAONE_RELAY                    D8
#define PHY_PIN_OUT_EXTRATWO_RELAY                    D5

#define PHY_PIN_IN_DOUBLE_GARAGE_DOOR                 D2
#define PHY_PIN_IN_SINGLE_GARAGE_DOOR                 D1

#define GARAGE_OPEN_CLOSE_RELAY_DELAY                 1500   /* 1.5 Sec */
#define DOOROPEN_NOTIFICATION                         2000

/***************************************************************/
/*                  Task Timing                                */
/***************************************************************/
#define TASK_TIMING_10MS                              10
#define TASK_TIMING_100MS                             100
#define TASK_TIMING_500MS                             500    /* .05  Sec */
#define TASK_TIMING_1000MS                            1000          

/***************************************************************/
/*                  Virtual Pin                               */
/***************************************************************/
#define VIRTUAL_PIN_DOUBLE_GARAGE_DOOR_STATUS_OUT          V1
#define VIRTUAL_PIN_SINGLE_GARAGE_DOOR_STATUS_OUT          V2
#define VIRTUAL_PIN_GARAGE_DOOR_NOTIFICATION_OUT           V3
#define VIRTUAL_PIN_LCD_DISPLAY_OUT                        V5                                       

#define VIRTUAL_PIN_DOUBLE_GARAGE_DOOR_IN    V11
#define VIRTUAL_PIN_SINGLE_GARAGE_DOOR_IN    V12
#define VIRTUAL_PIN_EXTRAONE_IN              V13
#define VIRTUAL_PIN_EXTRATWO_IN              V14 
#define VIRTUAL_PIN_RESET_FLAG_CLEAR_IN      V15                                                                                                                                                                               
/***************************************************************/
/*                  MACRO DEFINE                               */
/***************************************************************/
#define __deBugCode_ENABLE__                                (0)

/***************************************************************/
/*                  BLYNK & WIFI Define                        */
/***************************************************************/

/***************************************************************/
/*                  static                         */
/***************************************************************/
int NotificationStatus;
int ResetStatus;

/***************************************************************/
/*                 Setup Wifi and Blynk Token                  */
/***************************************************************/
//char auth_token[] = "tdrz506FZu7IrDCjac3dJSeyVwWKKKvs";   /* Garage Board */
char auth_token[] = "VZKyzz_FtD6Wldcfn97ZsuA0kqDFN_7S";   /*  Testing board */
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "molly";
char ssid_password[] = "BhAktApur";

WidgetLED DoubleGarageDoorStatus(VIRTUAL_PIN_DOUBLE_GARAGE_DOOR_STATUS_OUT);
WidgetLED SingleGarageDoorStatus(VIRTUAL_PIN_SINGLE_GARAGE_DOOR_STATUS_OUT);
WidgetLCD LcdDisplayInformation(VIRTUAL_PIN_LCD_DISPLAY_OUT);
BlynkTimer IntervalTimer;

/* Send Garage Door status to Blynk */
void DoubleGarageDoorStatusSend()
{
    int DoubleDoorStatus = digitalRead(PHY_PIN_IN_DOUBLE_GARAGE_DOOR);
    if (DoubleDoorStatus) 
    {
        DoubleGarageDoorStatus.off();
    } 
    else 
    {
        DoubleGarageDoorStatus.on();
    }
}

void SingleGarageDoorStatusSend()
{
    int SingleDoorStatus = digitalRead(PHY_PIN_IN_SINGLE_GARAGE_DOOR);
    if (SingleDoorStatus) 
    {
        SingleGarageDoorStatus.off();
    } 
    else 
    {
        SingleGarageDoorStatus.on();
    }
}

BLYNK_WRITE(VIRTUAL_PIN_GARAGE_DOOR_NOTIFICATION_OUT) {
    NotificationStatus = param.asInt();
}

/* For Google to support Double Door */
BLYNK_WRITE(VIRTUAL_PIN_DOUBLE_GARAGE_DOOR_IN)
{
    int dooropenStatus = param.asInt();
    digitalWrite(PHY_PIN_OUT_DOUBLE_GARAGE_DOOR, dooropenStatus);
    delay(GARAGE_OPEN_CLOSE_RELAY_DELAY);
    digitalWrite(PHY_PIN_OUT_DOUBLE_GARAGE_DOOR, !dooropenStatus);
}

/* For Google to support Single  Door */
BLYNK_WRITE(VIRTUAL_PIN_SINGLE_GARAGE_DOOR_IN)
{
    int dooropenStatus = param.asInt();
    digitalWrite(PHY_PIN_OUT_SINGLE_GARAGE_DOOR, dooropenStatus);
    delay(GARAGE_OPEN_CLOSE_RELAY_DELAY);
    digitalWrite(PHY_PIN_OUT_SINGLE_GARAGE_DOOR, !dooropenStatus);
}

/* For Google to support XXXXX */
BLYNK_WRITE(VIRTUAL_PIN_EXTRAONE_IN)
{
    int Status = param.asInt();
    digitalWrite(PHY_PIN_OUT_EXTRAONE_RELAY, Status);
}

/* For Google to support XXXXX */
BLYNK_WRITE(VIRTUAL_PIN_EXTRATWO_IN)
{
    int Status = param.asInt();
    digitalWrite(PHY_PIN_OUT_EXTRATWO_RELAY, Status);
}

/***************************************************************/
/*                  Display info in LCD Display                */
/***************************************************************/
void LcdDisplayWidget()
{
    int readDoubleDoorStatus = digitalRead(PHY_PIN_IN_DOUBLE_GARAGE_DOOR);
    int readSingleDoorStatus = digitalRead(PHY_PIN_IN_SINGLE_GARAGE_DOOR);
    if(!ResetStatus)
    {
        if(readDoubleDoorStatus)
        {
            LcdDisplayInformation.print(0,0, "Double Dr Open  ");
        }
        else
        {
            LcdDisplayInformation.print(0,0, "Double Dr Closed");
        }
        
        if(readSingleDoorStatus)
        {
            LcdDisplayInformation.print(0,1, "Single Dr Open  ");
        }
        else
        {
            LcdDisplayInformation.print(0,1, "Single Dr Closed");
        }
    }  
}

/***************************************************************/
/*                  Clear Reset Flag by User                  */
/***************************************************************/
BLYNK_WRITE(VIRTUAL_PIN_RESET_FLAG_CLEAR_IN)
{
    ResetStatus = false;
}

/***************************************************************/
/*                  Setup                                      */
/***************************************************************/
void setup()
{
#if(__deBugCode_ENABLE__)
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
#endif

    // Setup physical button pin (active low)
    pinMode(PHY_PIN_IN_DOUBLE_GARAGE_DOOR,   INPUT_PULLUP);
    pinMode(PHY_PIN_IN_SINGLE_GARAGE_DOOR,   INPUT_PULLUP);
    pinMode(PHY_PIN_OUT_DOUBLE_GARAGE_DOOR,  OUTPUT);
    pinMode(PHY_PIN_OUT_SINGLE_GARAGE_DOOR,  OUTPUT);
    pinMode(PHY_PIN_OUT_EXTRAONE_RELAY,      OUTPUT);
    pinMode(PHY_PIN_OUT_EXTRATWO_RELAY,      OUTPUT);  

    digitalWrite(PHY_PIN_OUT_DOUBLE_GARAGE_DOOR, HIGH);
    digitalWrite(PHY_PIN_OUT_SINGLE_GARAGE_DOOR, HIGH);
    digitalWrite(PHY_PIN_OUT_EXTRAONE_RELAY, HIGH);
    digitalWrite(PHY_PIN_OUT_EXTRATWO_RELAY, HIGH);
  
    Blynk.begin(auth_token, ssid, ssid_password);
    LcdDisplayInformation.clear();
    LcdDisplayInformation.print(0,1, "Reset");
    ResetStatus = true;
 
    IntervalTimer.setInterval(TASK_TIMING_500MS,  DoubleGarageDoorStatusSend);
    IntervalTimer.setInterval(TASK_TIMING_500MS,  SingleGarageDoorStatusSend);
    IntervalTimer.setInterval(TASK_TIMING_1000MS, LcdDisplayWidget);
 }

/***************************************************************/
/*                  Main Function                              */
/***************************************************************/
void loop()
{
    Blynk.run();
    IntervalTimer.run();
}