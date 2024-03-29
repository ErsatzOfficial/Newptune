/*Developer: Ersatz_Art Company
 * Code for audio suit
 * Developed for Teensy 3.6
 * Software open source and free of use
 */


#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            Input;          //xy=103,309.3333435058594
AudioEffectChorus        chorus1;        //xy=379,351.3333435058594
AudioMixer4              Ch_Sel;         //xy=555,322.3333435058594
AudioEffectFreeverb      freeverb2;      //xy=665,455.3333435058594
AudioEffectFlange        flange1;        //xy=669,405.3333435058594
AudioMixer4              Fl_Sel;         //xy=785,335.3333435058594
AudioEffectDelay         delay1;         //xy=905,607.3333435058594
AudioMixer4              Delay_Mixer;    //xy=909,483.3333435058594
AudioMixer4              Delay_Sel;      //xy=985,333.3333435058594
AudioEffectFreeverb      freeverb1;      //xy=1122,409.3333435058594
AudioEffectFlange        flange2;        //xy=1134,468.3333435058594
AudioMixer4              Rev_Sel;        //xy=1205,327.3333435058594
AudioAmplifier           R_Chanel_Amp;   //xy=1386,312.3333435058594
AudioAmplifier           L_Chanel_Amp;   //xy=1386,364.3333435058594
AudioOutputI2S           Output;         //xy=1553,347.3333435058594
AudioConnection          patchCord1(Input, 0, Ch_Sel, 0);
AudioConnection          patchCord2(Input, 0, chorus1, 0);
AudioConnection          patchCord3(chorus1, 0, Ch_Sel, 1);
AudioConnection          patchCord4(Ch_Sel, 0, Fl_Sel, 0);
AudioConnection          patchCord5(Ch_Sel, flange1);
AudioConnection          patchCord6(Ch_Sel, freeverb2);
AudioConnection          patchCord7(freeverb2, 0, Fl_Sel, 2);
AudioConnection          patchCord8(flange1, 0, Fl_Sel, 1);
AudioConnection          patchCord9(Fl_Sel, 0, Delay_Sel, 0);
AudioConnection          patchCord10(Fl_Sel, 0, Delay_Mixer, 0);
AudioConnection          patchCord11(delay1, 0, Delay_Mixer, 1);
AudioConnection          patchCord12(delay1, 0, Delay_Sel, 1);
AudioConnection          patchCord13(Delay_Mixer, delay1);
AudioConnection          patchCord14(Delay_Sel, 0, Rev_Sel, 0);
AudioConnection          patchCord15(Delay_Sel, freeverb1);
AudioConnection          patchCord16(Delay_Sel, flange2);
AudioConnection          patchCord17(freeverb1, 0, Rev_Sel, 1);
AudioConnection          patchCord18(flange2, 0, Rev_Sel, 2);
AudioConnection          patchCord19(Rev_Sel, R_Chanel_Amp);
AudioConnection          patchCord20(Rev_Sel, L_Chanel_Amp);
AudioConnection          patchCord21(R_Chanel_Amp, 0, Output, 0);
AudioConnection          patchCord22(L_Chanel_Amp, 0, Output, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=109,242.33334350585938
// GUItool: end automatically generated code


#define MIXER_ON 0.9
#define MIXER_OFF 0.0

//Set Effect Values for Patch -> NOAM!
#define CHORUS 10 //plus la valeur est élevée plus la voix donne aigue ET ça bouffe plus de volume... (j'ai test entre 10 et 50, à 50 on entend presque plus)
#define FLANGE 10.0  //
#define REVERB_ROOMSIZE 1.0
#define REVERB_DAMPING 1.0

/*Chorus & Flange Setup*/
#define CHORUS_DELAY_LENGTH (256*AUDIO_BLOCK_SAMPLES) // test multiples de 16 (16, 32, 64, 128), si plus élévé, ça donne le côté plus robotique (64 et 128 donnent bien)
short delayline[CHORUS_DELAY_LENGTH];

#define FLANGE_DELAY_LENGTH (64*AUDIO_BLOCK_SAMPLES) //plus c'est élevé, plus les oscillations en font sont aigues (entre 24 et 36 ça donne)
short delayline1[FLANGE_DELAY_LENGTH];
int s_idx = FLANGE_DELAY_LENGTH/4;
int s_depth = FLANGE_DELAY_LENGTH/4;

  /*Switches*/
int Patch_Pin = 26; //SW5
int Regie_Connect_Pin = 27; //SW4
int Reverb_Pin = 28; //SW3
int Flange_Pin = 29;//SW2
int Chorus_Pin = 30; //SW1
int Delay_Pin = 31;//Pot11

int Patch = 0;
int Regie_Connect = 0; 
int Reverb = 0;
int Flange = 0;
int Chorus = 0; 
int Delay = 0;

/*Pots*/
int ChorusPot = 38;//Pot6
int ReverbPot = 37;//Pot5
int FlangePot = 36; //Pot4
int DelayPot = 35; //Pot3
int DelayPot1 = 34;//Pot2
int VolumePot = A14; //Pot1

/*Effects Lvl*/
float ChorusValue = 0.0;
float ChorusValue1 = 0.0;

float ReverbValue = 0.0;
float ReverbValue1 = 0.0;

float FlangeValue = 0.0;
float FlangeValue1 = 0.0;

float DelayValue = 0.0;
float DelayValue1 = 0.0;
float DelayValue2 = 0.0;
float DelayValue3 = 0.0;
    
float VolumeValue = 0.0;
float VolumeValue1 = 0.0;

   

void setup() {
  Serial.begin(9600);
  
  AudioMemory(420);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.7);  
  sgtl5000_1.inputSelect(AUDIO_INPUT_MIC);
  sgtl5000_1.micGain(20);
  sgtl5000_1.adcHighPassFilterDisable();

  pinMode(Reverb_Pin, INPUT_PULLUP);
  pinMode(Flange_Pin, INPUT_PULLUP);
  pinMode(Chorus_Pin, INPUT_PULLUP);
  pinMode(Delay_Pin, INPUT_PULLUP);
  pinMode(Patch_Pin, INPUT_PULLUP);
  pinMode(Regie_Connect_Pin, INPUT_PULLUP);

  //Biquad Setup
 // biquad1.setLowpass(3,18*10^3,0.707);
  //biquad1.setHighpass(3,80,0.707);
 // biquad1.setBandpass(,,);
 // biquad1.setNotch(,,);
 // biquad1.setLowShelf(,,);
 // biquad1.setHighShelf(,,);
 //biquad1.setCoefficients(,);

  //Mixer Init
  Ch_Sel.gain(0, MIXER_OFF);//no chorus
  Ch_Sel.gain(1, MIXER_OFF);//chorus

  Fl_Sel.gain(0, MIXER_OFF);//no flange no reverb
  Fl_Sel.gain(1, MIXER_OFF);//flange1
  Fl_Sel.gain(2, MIXER_OFF);//reverb2

  Delay_Sel.gain(0, MIXER_OFF);//no delay
  Delay_Sel.gain(1, MIXER_OFF);//delay

  Rev_Sel.gain(0, MIXER_OFF);//No reverb no flange
  Rev_Sel.gain(1, MIXER_OFF);//reverb1
  Rev_Sel.gain(2, MIXER_OFF);//flange2

  R_Chanel_Amp.gain(MIXER_OFF);
  L_Chanel_Amp.gain(MIXER_OFF);

  //Effects Init
  //Chorus
  chorus1.begin(delayline,CHORUS_DELAY_LENGTH, CHORUS);
   
  //Reverbs
  freeverb1.roomsize(REVERB_ROOMSIZE);
  freeverb1.damping(REVERB_DAMPING);
  freeverb2.roomsize(REVERB_ROOMSIZE);
  freeverb2.damping(REVERB_DAMPING);

  //Flanges
  flange1.begin(delayline1,FLANGE_DELAY_LENGTH,s_idx,s_depth,FLANGE);
  flange2.begin(delayline1,FLANGE_DELAY_LENGTH,s_idx,s_depth,FLANGE);

  //Delay
  Delay_Mixer.gain(0, MIXER_OFF);
  Delay_Mixer.gain(1, MIXER_OFF);
  delay1.delay(0,0.0);
  
}

void loop() {

  Serial.print("Enter Loop       ");

 delay(20);
 Patch = digitalRead(Patch_Pin);

  R_Chanel_Amp.gain(4.0);
  L_Chanel_Amp.gain(4.0);
  
  while(Patch)
 { 
    Serial.print("Enter while patch       ");
    delay(20);
    
    //analogRead et calculs
    ChorusValue = analogRead(ChorusPot);
    ChorusValue1 = ChorusValue/1137.0; //range [0.0;0.9]

    FlangeValue=analogRead(FlangePot);
    FlangeValue1=FlangeValue/1137.0; //range [0.0;0.9]

    ReverbValue = analogRead(ReverbPot);
    ReverbValue1 = ReverbValue/1137.0; //range [0.0;0.9]

    DelayValue=analogRead(DelayPot);
    DelayValue1=(DelayValue*5.0/10230.0)+0.2;//range [0.3;0.7]
    
    DelayValue2=analogRead(DelayPot1);
    DelayValue3=(DelayValue2*2.5/10.23)+450;//range [250;450]
  
   //Digital Read
   Reverb = digitalRead(Reverb_Pin);
   Flange = digitalRead(Flange_Pin);
   Chorus = digitalRead(Chorus_Pin);
   Delay = digitalRead(Delay_Pin);  

   if(Chorus)
   {
    Ch_Sel.gain(0, 0.9-ChorusValue1);//no chorus
    Ch_Sel.gain(1,ChorusValue1 );//chorus
   }
   else
   {
    Ch_Sel.gain(0, MIXER_ON);//no chorus
    Ch_Sel.gain(1, MIXER_OFF);//chorus
   }

    if(Flange)
    {
    Fl_Sel.gain(0, 0.9-FlangeValue1);//no flange no reverb
    Fl_Sel.gain(1,FlangeValue1);//flange1
    Fl_Sel.gain(2, MIXER_OFF);//reverb2
    }
    else 
    {
    Fl_Sel.gain(0, MIXER_ON);//no flange no reverb
    Fl_Sel.gain(1, MIXER_OFF);//flange1
    Fl_Sel.gain(2, MIXER_OFF);//reverb2    
    }

    if(Delay)
    {
      Delay_Sel.gain(0, MIXER_ON/2);//no delay
      Delay_Sel.gain(1, MIXER_ON/2);//delay
      //Réglage effets
      Delay_Mixer.gain(0,DelayValue1);//Delay gain
      Delay_Mixer.gain(1,DelayValue1);
      delay1.delay(0,DelayValue3);//delay ms
    }
    else
    {
     Delay_Sel.gain(0, MIXER_ON);//no delay
     Delay_Sel.gain(1, MIXER_OFF);//delay
    }
    
   if(Reverb)
   {
    Rev_Sel.gain(0, 0.9-ReverbValue1);//No reverb no flange
    Rev_Sel.gain(1, ReverbValue1);//reverb1
    Rev_Sel.gain(2, MIXER_OFF);//flange2
   }
   else
   {
    Rev_Sel.gain(0, MIXER_ON);//No reverb no flange
    Rev_Sel.gain(1, MIXER_OFF);//reverb1
    Rev_Sel.gain(2, MIXER_OFF);//flange2
   }


  Regie_Connect = digitalRead(Regie_Connect_Pin);
  if(Regie_Connect)
  {
  //Disable speakers belt
  VolumeValue = analogRead(VolumePot);
  VolumeValue1 = VolumeValue*6/1023.0;
  R_Chanel_Amp.gain(0.0);
  L_Chanel_Amp.gain(VolumeValue1);
  }
  else
  {
  VolumeValue = analogRead(VolumePot);
  VolumeValue1 = VolumeValue*6/1023.0;
  R_Chanel_Amp.gain(VolumeValue1);
  L_Chanel_Amp.gain(VolumeValue1);
  }

  //Loop exit condition
  Patch = digitalRead(Patch_Pin);
 }
 
Serial.print("Enter Pass through       ");
  //If non of the above-> Pass_Through
  Ch_Sel.gain(0, 0.9);//no chorus
  Ch_Sel.gain(1, 0.0);//chorus

  Fl_Sel.gain(0, 0.9);//no flange no reverb
  Fl_Sel.gain(1, 0.0);//flange1
  Fl_Sel.gain(2, 0.0);//reverb2

  Delay_Sel.gain(0, 0.9);//no delay
  Delay_Sel.gain(1, 0.0);//delay

  Rev_Sel.gain(0, 0.9);//No reverb no flange
  Rev_Sel.gain(1, 0.0);//reverb1
  Rev_Sel.gain(2, 0.0);//flange2

  //R_Chanel_Amp.gain(4.0);
  //L_Chanel_Amp.gain(4.0);

  Regie_Connect = digitalRead(Regie_Connect_Pin);
  
  if(Regie_Connect)
  {
  //Disable speakers belt
    VolumeValue = analogRead(VolumePot);
  VolumeValue1 = VolumeValue*6/1023.0;
  R_Chanel_Amp.gain(0.0);
  L_Chanel_Amp.gain(VolumeValue1);
  }
  else
  {
      VolumeValue = analogRead(VolumePot);
  VolumeValue1 = VolumeValue*6/1023.0;
  R_Chanel_Amp.gain(VolumeValue1);
  L_Chanel_Amp.gain(VolumeValue1);
  }

}
