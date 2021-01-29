// flick_music.ino - Arduino Musical Instrument Program
// author: LanmiLab, twitter.com/lanmiLab

#include <Wire.h>
#include "flick.h"
#include <synth.h>
#include "notes.h"

#define LED_PIN_GREEN  8
#define LED_PIN_ORANGE  9

#define FIRST_OCTAVE  3
#define LAST_OCTAVE  4
#define DEFAULT_OCTAVE  3

const uint8_t octaves[] = {NOTE_B0, NOTE_C1, NOTE_C2, NOTE_C3, NOTE_C4, NOTE_C5, NOTE_C6, NOTE_C7};
uint8_t currentOctave = DEFAULT_OCTAVE;

Flick *flick;
synth music;
uint16_t prevX, prevY, prevZ;
uint8_t ledStateGreen = 1;
uint8_t ledStateOrange = 0;

void UpdateLeds() {
  if (currentOctave > DEFAULT_OCTAVE) {
    ledStateGreen = 1;
    ledStateOrange = 1;
  } else if (currentOctave < DEFAULT_OCTAVE) {
    ledStateGreen = 0;
    ledStateOrange = 1;
  } else {
    ledStateGreen = 1;
    ledStateOrange = 0;
  }

  digitalWrite(LED_PIN_GREEN, ledStateGreen);
  digitalWrite(LED_PIN_ORANGE, ledStateOrange);
}

void setup() {

  Serial.begin(115200);
  while(!Serial){};

  pinMode(5,  OUTPUT); // audio out GND
  digitalWrite(5, LOW);

  music.begin(DIFF);

  music.setupVoice(0,SINE,60,ENVELOPE0,80,64);
  music.setupVoice(1,SINE,65,ENVELOPE1,90,64);
  music.setupVoice(2,TRIANGLE,60,ENVELOPE3,100,64);
  music.setupVoice(3,TRIANGLE,60,ENVELOPE3,80,64);

  pinMode(10,  OUTPUT); // Power to flick board
  digitalWrite(10, HIGH);
  pinMode(LED_PIN_GREEN,  OUTPUT);
  pinMode(LED_PIN_ORANGE,  OUTPUT);

  flick = new Flick(12, 13);
  flick->touchCallback = onTouchCb;
  //flick->airWheelCallback = airwheelCb;
  flick->xyzCallback = onXYZCb;
  flick->gestureCallback = onGestureCb;
  delay(300); 
  Serial.println("flick setup.. ");
  int ret = flick->SetRuntimeParameter(0xa1, 0x1f, 0xFFFFFFFF); // lock data output   <-----
  Serial.println(ret,DEC);
  ret = flick->SetRuntimeParameter(0x80, 0x1b, 0x1F);// disable calibration

  digitalWrite(LED_PIN_GREEN, ledStateGreen);
  digitalWrite(LED_PIN_ORANGE, ledStateOrange);
}

void loop() {
  flick->Poll();
  delay(150); 
}

void onXYZCb(uint16_t x, uint16_t y, uint16_t z){
  int32_t dx, dy, dz;
  uint8_t x8 = x >> 13;
  uint8_t y4 = y >> 14;
  if ( z == 65535) return;
  
  uint8_t length = (z>>10)+60;
  length = length > 110 ? 110 : length;
  music.setLength(y4,length);

  music.mTrigger(y4, octaves[currentOctave] + 32 + x8*2);
  if ((x != prevX) || (y != prevY)) {
    // make leds blink
    digitalWrite(LED_PIN_GREEN, !ledStateGreen);
    digitalWrite(LED_PIN_ORANGE, !ledStateOrange);
    delay(10);
  }
  digitalWrite(LED_PIN_GREEN, ledStateGreen);
  digitalWrite(LED_PIN_ORANGE, ledStateOrange);

  /*Serial.print("x4: ");
   Serial.print(x8,DEC);
   Serial.print(", y4: ");
   Serial.print(y4,DEC);
   Serial.print(", octave: ");
   Serial.print(currentOctave,DEC);
   Serial.print(", : ");
   Serial.println(octaves[currentOctave],DEC);*/
   
  prevX = x;
  prevY = y;
  prevZ = z;
}

void onGestureCb(FlickGesture_t gesture, FlickGestureClass_t gestClass, bool isEdgeFlick, bool inProgress){
  const char *g, *cl;
  switch (gesture){ 
  case GESTURE_GARBAGE_MODEL:
    g = "GARBAGE_MODEL";
    break;
  case FLICK_WEST_TO_EAST:
    g = "FLICK_WEST_TO_EAST";
    //currentOctave += (currentOctave<LAST_OCTAVE) ? 1: 0;
    break;
  case FLICK_EAST_TO_WEST:
    g = "FLICK_EAST_TO_WEST";
    //currentOctave -= (currentOctave>FIRST_OCTAVE) ? 1 : 0;
    break;
  case FLICK_SOUTH_TO_NORTH:
    g = "FLICK_SOUTH_TO_NORTH";
    break;
  case FLICK_NORTH_TO_SOUTH:
    g = "FLICK_NORTH_TO_SOUTH";
    break;
  case CIRCLE_CLOCKWISE:
    g = "CIRCLE_CLOCKWISE";
    break;
  case CIRCLE_COUNTER_CLOCKWISE:
    g = "CIRCLE_COUNTER_CLOCKWISE";
    break;
  default:
    g = "NO_GESTURE";
  }
  
  switch (gestClass){ 
  case GESTURE_CLASS_GARBAGE_MODEL:
    cl = "GARBAGE_MODEL";
    break;
  case FLICK_GESTURE:
    cl = "FLICK_GESTURE";
    break;
  case CIRCULAR_GESTURE:
    cl = "CIRCULAR_GESTURE";
    break;
  default:
    cl = "UNKNOWN";
  }
  
  //printf("Gesture: %s, class: %s, edge flick: %s, in progress: %s \n", g, cl, isEdgeFlick?"yes":"no", inProgress?"yes":"no" );
  /*Serial.print(g);
  Serial.print('\n');*/
}

void onTouchCb( FlickTouch_t touch, uint16_t period ){
  int gotTap = 0;
  switch (touch){ 
  case TOUCH_SOUTH_ELECTRODE:
    //Serial.print("TOUCH_SOUTH_ELECTRODE:");
    break;
  case TOUCH_WEST_ELECTRODE:
    //Serial.print("TOUCH_WEST_ELECTRODE:");
    break;
  case TOUCH_NORTH_ELECTRODE:
    //Serial.print("TOUCH_NORTH_ELECTRODE:");
    break;
  case TOUCH_EAST_ELECTRODE:
    //Serial.print("TOUCH_EAST_ELECTRODE:");
    break;
  case TOUCH_CENTER_ELECTRODE:
    //Serial.print("TOUCH_CENTER_ELECTRODE:");
    break;
  case TAP_SOUTH_ELECTRODE:
    //Serial.print("TAP_SOUTH_ELECTRODE:");
    gotTap = TAP_SOUTH_ELECTRODE;
    break;
  case TAP_WEST_ELECTRODE:
    //Serial.print("TAP_WEST_ELECTRODE:");
    gotTap = TAP_WEST_ELECTRODE;
    break;
  case TAP_NORTH_ELECTRODE:
    //Serial.print("TAP_NORTH_ELECTRODE:");
    gotTap = TAP_NORTH_ELECTRODE;
    break;
  case TAP_EAST_ELECTRODE:
    //Serial.print("TAP_EAST_ELECTRODE:");
    gotTap = TAP_EAST_ELECTRODE;
    break;
  case TAP_CENTER_ELECTRODE:
    //Serial.print("TAP_CENTER_ELECTRODE:");
    gotTap = TAP_CENTER_ELECTRODE;
    break;
  case DOUBLE_TAP_SOUTH_ELECTRODE:
    //Serial.print("DOUBLE_TAP_SOUTH_ELECTRODE:");
    break;
  case DOUBLE_TAP_WEST_ELECTRODE:
    //Serial.print("DOUBLE_TAP_WEST_ELECTRODE:");
    gotTap = DOUBLE_TAP_WEST_ELECTRODE;
    break;
  case DOUBLE_TAP_NORTH_ELECTRODE:
    //Serial.print("DOUBLE_TAP_NORTH_ELECTRODE:");
    break;
  case DOUBLE_TAP_EAST_ELECTRODE:
    //Serial.print("DOUBLE_TAP_EAST_ELECTRODE:");
    gotTap = DOUBLE_TAP_EAST_ELECTRODE;
    break;
  case DOUBLE_TAP_CENTER_ELECTRODE:
    //Serial.print("DOUBLE_TAP_CENTER_ELECTRODE:");
    break;
  default:
    //Serial.print("Not defined touch:");
    break;
  }

   if (touch==TAP_EAST_ELECTRODE) {
    currentOctave += (currentOctave<LAST_OCTAVE) ? 1: 0;
    UpdateLeds();
    //Serial.println("octave increase");
  } else if (touch==TAP_WEST_ELECTRODE) {
    currentOctave -= (currentOctave>FIRST_OCTAVE) ? 1 : 0;
    UpdateLeds();
  }
}

void airwheelCb(int32_t a) {
  /*Serial.println("Got airwheel ");
  Serial.print(a);
  Serial.print('\n');*/
}
