#include "AiEsp32RotaryEncoder.h"
#include "Arduino.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ROTARY_ENCODER_A_PIN 34
#define ROTARY_ENCODER_B_PIN 39
#define ROTARY_ENCODER_BUTTON_PIN 36
#define ROTARY_ENCODER_STEPS 4
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, -1, ROTARY_ENCODER_STEPS);

uint16_t settings[23];
#define AktBeklemeSure 0
#define AktCalismaSure 1
#define AktCalismaMinSure 2
#define AktSensorTip 3 // NC-NO
#define AsitAlmaSure 4
#define AlkaliAlmaSure 5
#define DeterjanBeklemeSure 6
#define DeterjanTakvimi 7 // Bit hesabı 0 bit 1 gün olarak 7 bit 0 Asit 1 Alkali
#define OnYikamaSure 8
#define AnaYikamaSure 9
#define SonYikamaSure 10
#define DrenajCalismaSure 11
#define DrenajTip 12 // NC-NO
#define SuHataSure 13
#define OnYikamaSicaklik 14 // Sicak - Ilık - Soğuk
#define JetAcikSure 15
#define JetKapaliSure 16
#define JetAktif 17 // Kapalı - Açık
#define TankValfAcikSure 18
#define TankValfKapaliSure 19
#define TankValfAktif 20 // Kapalı - Açık
#define KurulamaAktif 21
#define KurulamaSure 22
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool blinkState = true;
unsigned long preMs =  0;
bool isSettingScreen = false;
//paramaters for button
unsigned long shortPressAfterMiliseconds = 50;   //how long short press shoud be. Do not set too low to avoid bouncing (false press events).
unsigned long longPressAfterMiliseconds = 1000;  //how long čong press shoud be.


String options[] = {"Select settings", "How many (1...100)"};

String StrSettingList[] = {"P0 ", "P1 ", "P2 ", "P3 ", "P4 ", "P5 ", "P6 ", "P7 ", "P8 ", "P9 ", "P10", "P11", "P12", "P13", "P14", "P15","P16", "P17", "P18", "P19", "P20", "P21", "P22",};

int selectedSettings = 0;

int currentOption = 0;


int setSettingsBoundaries(int selectedSettings) {
  switch (selectedSettings) {
      case AktSensorTip:
          return 1;
          break;
      case DeterjanTakvimi:
          return 6;
          break;
      case DrenajTip:
          return 1;
          break;
      case OnYikamaSicaklik:
          return 2;
          break;
      case JetAktif:
          return 1;
          break;
      case TankValfAktif:
          return 1;
          break;
      default:
          rotaryEncoder.setAcceleration(250);
          return 65535;  
          break;
    }
}


void setForOption(int newOption)
{
    currentOption = newOption;

    switch (newOption)
    {
    case 0:
        rotaryEncoder.setBoundaries(0, 0, false);
        rotaryEncoder.setEncoderValue(selectedSettings);
        break;
    case 1:
        rotaryEncoder.setBoundaries(0, 22, true);
        rotaryEncoder.setEncoderValue(selectedSettings);
        break;
    case 2:
        rotaryEncoder.setBoundaries(0, setSettingsBoundaries(selectedSettings), false); //how many pieces 1 to 100; do not go from 10 to 1
        rotaryEncoder.setEncoderValue(settings[selectedSettings]);
        break;
    
    default:
        break;
    }

    Serial.println(options[currentOption]);
}

void rotary_onButtonClick()
{
    static unsigned long lastTimePressed = 0;
    if (millis() - lastTimePressed < 200) 
        return;
    
    lastTimePressed = millis();

    int selecedValue = rotaryEncoder.readEncoder();

    switch (currentOption)
    {
    case 1: //"Select fast food"
        selectedSettings = selecedValue;
        setForOption(2);
        rotaryEncoder.setAcceleration(250);
        break;
    case 2: //"Select quantity"
        settings[selectedSettings] = selecedValue;
        setForOption(1);
        rotaryEncoder.setAcceleration(0);
        break;
    default:
        break;
    }
}

String intQuanttiyToStr(int selecedValue, int selectedSettings) {
    String ncNo[]={"   NO","   NC"};
    String asitAlkali[]={"    0","    1","   00","   01","   10","   11","  000"};
    String sicakIlikSoguk[]={"  SCK","  ILK","  SGK"};
    String acikKapali[]={"  KPL","  ACK"};
    String str = "";
    
    switch (selectedSettings) {
        case AktSensorTip:
            return ncNo[selecedValue];
            break;
        case DeterjanTakvimi:
            return asitAlkali[selecedValue];
            break;
        case DrenajTip:
            return ncNo[selecedValue];
            break;
        case OnYikamaSicaklik:
            return sicakIlikSoguk[selecedValue];
            break;
        case JetAktif:
            return acikKapali[selecedValue];
            break;
        case TankValfAktif:
            return acikKapali[selecedValue];
            break;
        default:
            str.concat(selecedValue);
            for(int i=5-str.length(); i>0; --i)
            {str = " " + str;}
            return str;  
            break;
    }
}


void settingScreen()
{   
    int selecedValue = rotaryEncoder.readEncoder();
    switch (currentOption)
    {
    case 1: //"Select fast food"
        display.clearDisplay();
        display.setTextSize(3);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);// Draw white text
        display.setCursor(0,0);
        display.println(StrSettingList[selecedValue]); 
        display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);// Draw white text
        display.setTextSize(2);
        display.setCursor(68,8);             // Start at top-left corner
        display.println(intQuanttiyToStr(settings[selecedValue],selecedValue));
        display.display();
        break;
    case 2: 
        display.clearDisplay();
        display.setTextSize(3);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);// Draw white text
        display.setCursor(0,0);
        display.println(StrSettingList[selectedSettings]); 
        display.setTextSize(2);
        if (blinkState){display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);}// Draw white text
        else {display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);}// Draw white text
        display.setCursor(68,8);             // Start at top-left corner
        display.println(intQuanttiyToStr(selecedValue,selectedSettings));
        display.display();
        break;
    default:
        break;
        }
    
}

void mainScreen()
{  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);// Draw white text
  display.setTextSize(3);             // Normal 1:1 pixel scale
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("ANKA"));
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setCursor(92,8);             // Start at top-left corner
  display.println(F("v1.2.1"));
  display.setTextSize(1);
  display.setCursor(72,24);  
  display.println(F("SAGIM"));
  display.display();
}

void IRAM_ATTR readEncoderISR()
{
    rotaryEncoder.readEncoder_ISR();
}

void on_button_short_click() {
  Serial.print("button SHORT press ");
  Serial.print(millis());
  Serial.println(" milliseconds after restart");
  rotary_onButtonClick();
}

void on_button_long_click() {
  Serial.print("button LONG press ");
  Serial.print(millis());
  Serial.println(" milliseconds after restart");
   switch (currentOption)
    {
    case 0:
        currentOption=1;
        setForOption(1);
        rotaryEncoder.setAcceleration(0);
        attachInterrupt(digitalPinToInterrupt(ROTARY_ENCODER_A_PIN), readEncoderISR, CHANGE);
        attachInterrupt(digitalPinToInterrupt(ROTARY_ENCODER_B_PIN), readEncoderISR, CHANGE);
        break;
    default:
        currentOption=0;
        mainScreen();
        detachInterrupt(digitalPinToInterrupt(ROTARY_ENCODER_A_PIN));
        detachInterrupt(digitalPinToInterrupt(ROTARY_ENCODER_B_PIN));
        break;


    }


}

void handle_rotary_button() {
  static unsigned long lastTimeButtonDown = 0;
  static bool wasButtonDown = false;

  bool isEncoderButtonDown = rotaryEncoder.isEncoderButtonDown();
  //isEncoderButtonDown = !isEncoderButtonDown; //uncomment this line if your button is reversed

  if (isEncoderButtonDown) {
    Serial.print("+");  //REMOVE THIS LINE IF YOU DONT WANT TO SEE
    if (!wasButtonDown) {
      //start measuring
      lastTimeButtonDown = millis();
    }
    //else we wait since button is still down
    wasButtonDown = true;
    return;
  }

  //button is up
  if (wasButtonDown) {
    Serial.println("");  //REMOVE THIS LINE IF YOU DONT WANT TO SEE
    //click happened, lets see if it was short click, long click or just too short
    if (millis() - lastTimeButtonDown >= longPressAfterMiliseconds) {
      on_button_long_click();
    } else if (millis() - lastTimeButtonDown >= shortPressAfterMiliseconds) {
      on_button_short_click();
    }
  }
  wasButtonDown = false;
}


void setup()
{ 
  Serial.begin(115200);
    rotaryEncoder.begin();
    rotaryEncoder.setup(readEncoderISR);

    rotaryEncoder.correctionOffset=2; //try with zero or ROTARY_ENCODER_STEPS/2
    rotaryEncoder.isButtonPulldown = false;
	  rotaryEncoder.areEncoderPinsPulldownforEsp32 = true;

    setForOption(0);

    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);}

    mainScreen();
    detachInterrupt(digitalPinToInterrupt(ROTARY_ENCODER_A_PIN));
    detachInterrupt(digitalPinToInterrupt(ROTARY_ENCODER_B_PIN));
    
}

void loop()
{
  handle_rotary_button();

  unsigned long ms = millis();
    if ((ms-preMs) >= 1000)
    {
        preMs = ms;
        blinkState = !blinkState;
        settingScreen();
    }    
      
    if (rotaryEncoder.encoderChanged())
    {   
      settingScreen();
    }
    if (rotaryEncoder.isEncoderButtonClicked())
    {   
      settingScreen();
    }

    
      
  
  
}
