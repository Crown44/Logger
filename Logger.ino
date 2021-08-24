 //Time
const int     period= 10000; //period (ms) to check if the status has changed and deciding if the data need to be send
unsigned long period_start_time; // timer
unsigned long timer;
const int     transfer_dur=10; //transfert estimated_duration
const byte    nb_pulsmax=100;

//Signals
  //digital Inputs
const byte    Dig_Inputs_Pin[]         = {2,3,4,5}; //Pin Number

const byte    nb_Dig_Inputs = sizeof(Dig_Inputs_Pin);
const boolean Dig_Inputs_Initial[nb_Dig_Inputs]     = {0,0,0,0}; //Pin Number
const int    Dig_Inputs_Period_Min[nb_Dig_Inputs]  = {100,100,100,100}; //Pin Number
const int    Dig_Inputs_Pulse_Min[nb_Dig_Inputs]   = {50,50,50,50}; //Pin Number
const int    Dig_Inputs_Period_Max[nb_Dig_Inputs]  = {6000,6000,6000,6000}; //Pin Number
const int    Dig_Inputs_Pulse_Max[nb_Dig_Inputs]   = {6000,6000,6000,6000}; //Pin Number

      //Required log
const byte    Log_Dig_Inputs_Status[]      ={0,1};
const byte    Log_Dig_Inputs_Period_Count[]={};
const byte    Log_Dig_Inputs_Period[]      ={};
const byte    Log_Dig_Inputs_Period_Range[]={2};
const byte    Log_Dig_Inputs_Period_Full[] ={3};
const byte    Log_Dig_Inputs_Pulse[]       ={};
const byte    Log_Dig_Inputs_Pulse_Range[] ={2};
const byte    Log_Dig_Inputs_Pulse_Full[]  ={3};

    //DataTables
boolean Dig_Inputs_Status[nb_Dig_Inputs][2];
byte Dig_Inputs_Change[nb_Dig_Inputs];
byte Dig_Inputs_Period_Count[nb_Dig_Inputs];
byte Dig_Inputs_Pulse_Count[nb_Dig_Inputs];
unsigned long Dig_Inputs_Period_Timers[nb_Dig_Inputs];
unsigned long Dig_Inputs_Pulse_Timers[nb_Dig_Inputs];

const byte Dig_Inputs_Status_length = sizeof(Log_Dig_Inputs_Status);
const byte Dig_Inputs_Period_Count_length = sizeof(Log_Dig_Inputs_Period_Count);

const byte Dig_Inputs_Period_length = sizeof(Log_Dig_Inputs_Period);
int  Dig_Inputs_Period[Dig_Inputs_Period_length];

const byte Dig_Inputs_Period_Range_length = sizeof(Log_Dig_Inputs_Period_Range);
int  Dig_Inputs_Period_Range[Dig_Inputs_Period_Range_length][3];

const byte Dig_Inputs_Period_Full_length = sizeof(Log_Dig_Inputs_Period_Full);
int  Dig_Inputs_Period_Full[Dig_Inputs_Period_Full_length][nb_pulsmax];

const byte Dig_Inputs_Pulse_length = sizeof(Log_Dig_Inputs_Pulse);
int  Dig_Inputs_Pulse[Dig_Inputs_Pulse_length];

const byte Dig_Inputs_Pulse_Range_length = sizeof(Log_Dig_Inputs_Pulse_Range);
int  Dig_Inputs_Pulse_Range[Dig_Inputs_Pulse_Range_length][3];

const byte Dig_Inputs_Pulse_Full_length = sizeof(Log_Dig_Inputs_Pulse_Full);
int  Dig_Inputs_Pulse_Full[Dig_Inputs_Pulse_Full_length][nb_pulsmax];


  //Analog Inputs
const byte    Ana_Inputs_Pin[]         = {A0,A1}; //Pin Number
const byte    nb_Ana_Inputs = sizeof(Ana_Inputs_Pin);
    //Required log
const byte    Log_Ana_Inputs_Avg[]      ={0,1};
//const byte    Log_Ana_Inputs_Values[]   ={};
    //DataTables
//unsigned long Ana_Inputs_Sum[nb_Ana_Inputs];

const byte Ana_Inputs_Avg_length = sizeof(Log_Ana_Inputs_Avg);
unsigned long  Ana_Inputs_Sum[Ana_Inputs_Avg_length];


//Various
const int myBaud = 9600;
unsigned long count = 0;
byte master_read = 0;
byte master_input[2] = {0,0};

boolean init_performed = false;

void setup()
{
  Serial.begin(myBaud);
  myinit();
}

void loop()
{ 
  if (Serial.available() > 0){read_serial();}
  dig_inputs_scrut();
  ana_inputs_scrut();
  count ++;
  timer= millis() - period_start_time;
  delay(2);
}

void read_serial(){
    master_read=Serial.read();
    if (master_read==48){myinit();}
    if (master_read==49){
      dig_inputs_write_scrut();
      init_performed=false;
    }
    if (master_read==50){
      ana_inputs_write_scrut();
      init_performed=false;
    }
}

void myinit(){
   if (init_performed){
      dig_inputs_write_init();
   }else{
   dig_inputs_init();
   ana_inputs_init();
   period_start_time = millis();
   init_performed=true;
   count = 0;
  }
}

void dig_inputs_init(){
  for (byte i = 0; i < nb_Dig_Inputs; i++){
    pinMode(Dig_Inputs_Pin[i],INPUT);
    Dig_Inputs_Status[i][0]=Dig_Inputs_Status[i][1]=digitalRead(Dig_Inputs_Pin[i]);
    
    Dig_Inputs_Change[i]=0;
    Dig_Inputs_Period_Count[i]=0;
    Dig_Inputs_Pulse_Count[i]=0;
    Dig_Inputs_Period_Timers[i]=0;
    Dig_Inputs_Pulse_Timers[i]=0;
  } 
  dig_inputs_period_init();
  dig_inputs_period_range_init();
  dig_inputs_period_full_init();
  dig_inputs_pulse_init();
  dig_inputs_pulse_range_init();
  dig_inputs_pulse_full_init();
}

void ana_inputs_init(){
  ana_inputs_avg_init();
}

void ana_inputs_avg_init(){
  for (byte i = 0; i < Ana_Inputs_Avg_length; i++){
  Ana_Inputs_Sum[i]=0; 
  } 
}
void ana_inputs_scrut(){
  ana_inputs_avg_scrut(); 
}

void ana_inputs_avg_scrut(){
  for (byte i = 0; i < Ana_Inputs_Avg_length; i++){
  Ana_Inputs_Sum[i]+=analogRead(Ana_Inputs_Pin[Log_Ana_Inputs_Avg[i]]); 
  } 
}

void ana_inputs_write_scrut(){
  Serial.print("2/");
  Serial.print(count);
  Serial.print("/");
  Serial.print(timer);
  Serial.print("/");
  for (byte i = 0; i < Ana_Inputs_Avg_length; i++){if (i>0){Serial.print(";");} Serial.print(int(Ana_Inputs_Sum[Log_Ana_Inputs_Avg[i]]/count));}
  Serial.println("/2");
}

void dig_inputs_scrut(){
  for (byte i = 0; i < nb_Dig_Inputs; i++){
    Dig_Inputs_Status[i][1] = digitalRead(Dig_Inputs_Pin[i]);
    if (Dig_Inputs_Status[i][0] != Dig_Inputs_Status[i][1]){
      if (Dig_Inputs_Status[i][1] == Dig_Inputs_Initial[i]){
        Dig_Inputs_Change[i]=1;
        Dig_Inputs_Period_Count[i]++;
      }else{Dig_Inputs_Change[i]=2;}
      Dig_Inputs_Status[i][0]=Dig_Inputs_Status[i][1];
    }else{Dig_Inputs_Change[i]=0;};
  }
  dig_inputs_period_scrut();
  dig_inputs_period_range_scrut();
  dig_inputs_period_full_scrut();
  dig_inputs_pulse_scrut();
  dig_inputs_pulse_range_scrut();
  dig_inputs_pulse_full_scrut();
}

void dig_inputs_period_init(){
  for (byte i = 0; i < Dig_Inputs_Period_length; i++){
   Dig_Inputs_Period[i]=0;
  }   
}
void dig_inputs_period_scrut(){
  for (byte i = 0; i < Dig_Inputs_Period_length; i++){
    byte Dig_Input =  Log_Dig_Inputs_Period[i];
    if (Dig_Inputs_Change[Dig_Input]==1){
      if (Dig_Inputs_Period_Count[Dig_Input]>1){
        int new_period = int(millis()-Dig_Inputs_Period_Timers[Dig_Input]);
        if (new_period<Dig_Inputs_Period_Min[Dig_Input]){
          Dig_Inputs_Period_Count[Dig_Input]--;
        }else{
        Dig_Inputs_Period[i] = int((Dig_Inputs_Period[i]*(Dig_Inputs_Period_Count[Dig_Input]-2)+new_period)/(Dig_Inputs_Period_Count[Dig_Input]-1));
        Dig_Inputs_Period_Timers[Dig_Input]=millis();
        }
      }else{
      Dig_Inputs_Period_Timers[Dig_Input]=millis();
      }
    }
  }
}
void dig_inputs_period_range_init(){
  for (byte i = 0; i < Dig_Inputs_Period_Range_length; i++){
   Dig_Inputs_Period_Range[i][0]=period;  Dig_Inputs_Period_Range[i][1]=0;  Dig_Inputs_Period_Range[i][2]=0;
  } 
}
void dig_inputs_period_range_scrut(){
  for (byte i = 0; i < Dig_Inputs_Period_Range_length; i++){
    byte Dig_Input =  Log_Dig_Inputs_Period_Range[i];
    if (Dig_Inputs_Change[Dig_Input]==1){
      if (Dig_Inputs_Period_Count[Dig_Input]>1){
        int new_period = int(millis()-Dig_Inputs_Period_Timers[Dig_Input]);
        if (new_period<Dig_Inputs_Period_Min[Dig_Input]){
          Dig_Inputs_Period_Count[Dig_Input]--;
        }else{
        if (new_period<Dig_Inputs_Period_Range[i][0]){Dig_Inputs_Period_Range[i][0]=new_period;}else if(new_period>Dig_Inputs_Period_Range[i][2]){Dig_Inputs_Period_Range[i][2]=new_period;} 
        Dig_Inputs_Period_Range[i][1] = int((Dig_Inputs_Period_Range[i][1]*(Dig_Inputs_Period_Count[Dig_Input]-2)+new_period)/(Dig_Inputs_Period_Count[Dig_Input]-1));
        Dig_Inputs_Period_Timers[Dig_Input]=millis();
        }
      }else{
      Dig_Inputs_Period_Timers[Dig_Input]=millis();
      }
      Dig_Inputs_Period_Timers[Dig_Input]=millis();
    }
  }  
}
void dig_inputs_period_full_init(){
  for (byte i = 0; i < Dig_Inputs_Period_Full_length; i++){
   for (byte j = 0; j < nb_pulsmax; j++){Dig_Inputs_Period_Full[i][j]=0;}
  }

}
void dig_inputs_period_full_scrut(){
  for (byte i = 0; i < Dig_Inputs_Period_Full_length; i++){
    byte Dig_Input =  Log_Dig_Inputs_Period_Full[i];
    if (Dig_Inputs_Change[Dig_Input]==1){
      if (Dig_Inputs_Period_Count[Dig_Input]>1){
        int new_period = int(millis()-Dig_Inputs_Period_Timers[Dig_Input]);
        if (new_period<Dig_Inputs_Period_Min[Dig_Input]){
          Dig_Inputs_Period_Count[Dig_Input]--;
        }else{
        Dig_Inputs_Period_Full[i][Dig_Inputs_Period_Count[Dig_Input]-2]= new_period;
        Dig_Inputs_Period_Timers[Dig_Input]=millis();
        }
      }else{
      Dig_Inputs_Period_Timers[Dig_Input]=millis();
      }
    Dig_Inputs_Period_Timers[Dig_Input]=millis();      
    }
  }  
}

void dig_inputs_pulse_init(){
  for (byte i = 0; i < Dig_Inputs_Pulse_length; i++){
   Dig_Inputs_Pulse[i]=0;
  }   
}
void dig_inputs_pulse_scrut(){
  for (byte i = 0; i < Dig_Inputs_Pulse_length; i++){
    byte Dig_Input =  Log_Dig_Inputs_Pulse[i];
    if (Dig_Inputs_Change[Dig_Input]==1){
      Dig_Inputs_Pulse_Timers[Dig_Input]=millis();
    }else if (Dig_Inputs_Change[Dig_Input]==2){
      int new_pulse = int(millis()-Dig_Inputs_Pulse_Timers[Dig_Input]);
      if ((new_pulse>Dig_Inputs_Pulse_Min[Dig_Input])&&(new_pulse<Dig_Inputs_Pulse_Max[Dig_Input])){
        Dig_Inputs_Pulse_Count[Dig_Input]++;
        Dig_Inputs_Pulse[i] = int((Dig_Inputs_Pulse[i]*(Dig_Inputs_Pulse_Count[Dig_Input]-1)+new_pulse)/(Dig_Inputs_Pulse_Count[Dig_Input]));
      }
    }
  }  
}
void dig_inputs_pulse_range_init(){
  for (byte i = 0; i < Dig_Inputs_Pulse_Range_length; i++){
   Dig_Inputs_Pulse_Range[i][0]=period;  Dig_Inputs_Pulse_Range[i][1]=0;  Dig_Inputs_Pulse_Range[i][2]=0;
  } 
}
void dig_inputs_pulse_range_scrut(){
  for (byte i = 0; i < Dig_Inputs_Pulse_Range_length; i++){
    byte Dig_Input =  Log_Dig_Inputs_Pulse_Range[i];
    if (Dig_Inputs_Change[Dig_Input]==1){
      Dig_Inputs_Pulse_Timers[Dig_Input]=millis();
    }else if (Dig_Inputs_Change[Dig_Input]==2){
      int new_pulse = int(millis()-Dig_Inputs_Pulse_Timers[Dig_Input]);
      if ((new_pulse>Dig_Inputs_Pulse_Min[Dig_Input])&&(new_pulse<Dig_Inputs_Pulse_Max[Dig_Input])){
        Dig_Inputs_Pulse_Count[Dig_Input]++;
        Dig_Inputs_Pulse_Range[i][1] = int((Dig_Inputs_Pulse_Range[i][1]*(Dig_Inputs_Pulse_Count[Dig_Input]-1)+new_pulse)/(Dig_Inputs_Pulse_Count[Dig_Input]));
        if (new_pulse<Dig_Inputs_Pulse_Range[i][0]){Dig_Inputs_Pulse_Range[i][0]=new_pulse;}else if(new_pulse>Dig_Inputs_Pulse_Range[i][2]){Dig_Inputs_Pulse_Range[i][2]=new_pulse;} 
      }
    }
  }  
}
void dig_inputs_pulse_full_init(){
  for (byte i = 0; i < Dig_Inputs_Pulse_Full_length; i++){
   for (byte j = 0; j < nb_pulsmax; j++){Dig_Inputs_Pulse_Full[i][j]=0;}
  } 
}
void dig_inputs_pulse_full_scrut(){
  for (byte i = 0; i < Dig_Inputs_Pulse_Full_length; i++){
    byte Dig_Input =  Log_Dig_Inputs_Pulse_Full[i];
    if (Dig_Inputs_Change[Dig_Input]==1){
      Dig_Inputs_Pulse_Timers[Dig_Input]=millis();
    }else if (Dig_Inputs_Change[Dig_Input]==2){
      int new_pulse = int(millis()-Dig_Inputs_Pulse_Timers[Dig_Input]);
      if ((new_pulse>Dig_Inputs_Pulse_Min[Dig_Input])&&(new_pulse<Dig_Inputs_Pulse_Max[Dig_Input])){
        Dig_Inputs_Pulse_Count[Dig_Input]++;
        Dig_Inputs_Pulse_Full[i][Dig_Inputs_Pulse_Count[Dig_Input]-1]=new_pulse;
      }
    }
  }  
}


void dig_inputs_write_scrut(){
  Serial.print("1/");
  Serial.print(count);
  Serial.print("/");
  Serial.print(timer);
  Serial.print("/");
  for (byte i = 0; i < Dig_Inputs_Status_length; i++){if (i>0){Serial.print(";");} Serial.print(Dig_Inputs_Status[Log_Dig_Inputs_Status[i]][1]);}
  Serial.print("#");
  for (byte i = 0; i < Dig_Inputs_Period_Count_length; i++){if (i>0){Serial.print(";");} Serial.print(Dig_Inputs_Period_Count[Log_Dig_Inputs_Period_Count[i]]);}
  Serial.print("#");
  for (byte i = 0; i < Dig_Inputs_Period_length; i++){if (i>0){Serial.print(";");} Serial.print(Dig_Inputs_Period[i]);}
  Serial.print("#");
  for (byte i = 0; i < Dig_Inputs_Period_Range_length; i++){if (i>0){Serial.print(";");}
    if (Dig_Inputs_Period_Range[i][2]>0){
      for (byte j = 0; j < 3; j++) {if (j>0){Serial.print(",");} Serial.print(Dig_Inputs_Period_Range[i][j]);}}}
  Serial.print("#");
  for (byte i = 0; i < Dig_Inputs_Period_Full_length; i++){if (i>0){Serial.print(";");} 
    for (byte j = 0; j < nb_pulsmax; j++) {if(Dig_Inputs_Period_Full[i][j]){if (j>0){Serial.print(",");} Serial.print(Dig_Inputs_Period_Full[i][j]);}else{break;}}}
  Serial.print("#");
  for (byte i = 0; i < Dig_Inputs_Pulse_length; i++){if (i>0){Serial.print(";");} Serial.print(Dig_Inputs_Pulse[i]);}
  Serial.print("#");
  for (byte i = 0; i < Dig_Inputs_Pulse_Range_length; i++){if (i>0){Serial.print(";");}
    if (Dig_Inputs_Pulse_Range[i][2]>0){ 
      for (byte j = 0; j < 3; j++) {if (j>0){Serial.print(",");} Serial.print(Dig_Inputs_Pulse_Range[i][j]);}}}
  Serial.print("#");
  for (byte i = 0; i < Dig_Inputs_Pulse_Full_length; i++){if (i>0){Serial.print(";");} 
    for (byte j = 0; j < nb_pulsmax; j++) {if(Dig_Inputs_Pulse_Full[i][j]){if (j>0){Serial.print(",");} Serial.print(Dig_Inputs_Pulse_Full[i][j]);}else{break;}}}
  Serial.println("/1");
}

void dig_inputs_write_init(){
  Serial.print("0/");
  Serial.print(period);
  Serial.print("/");
  for (byte i = 0; i < nb_Dig_Inputs; i++){if (i>0){Serial.print(";");} Serial.print(Dig_Inputs_Pin[i]);}
  Serial.print("/");
  for (byte i = 0; i < nb_Dig_Inputs; i++){if (i>0){Serial.print(";");} Serial.print(Dig_Inputs_Initial[i]);}
  Serial.print("/");
  for (byte i = 0; i < Dig_Inputs_Status_length; i++){if (i>0){Serial.print(";");} Serial.print(Dig_Inputs_Pin[Log_Dig_Inputs_Status[i]]);}
  Serial.print("#");
  for (byte i = 0; i < Dig_Inputs_Period_Count_length; i++){if (i>0){Serial.print(";");} Serial.print(Dig_Inputs_Pin[Log_Dig_Inputs_Period_Count[i]]);}
  Serial.print("#");
  for (byte i = 0; i < Dig_Inputs_Period_length; i++){if (i>0){Serial.print(";");} Serial.print(Dig_Inputs_Pin[Log_Dig_Inputs_Period[i]]);}
  Serial.print("#");
  for (byte i = 0; i < Dig_Inputs_Period_Range_length; i++){if (i>0){Serial.print(";");} Serial.print(Dig_Inputs_Pin[Log_Dig_Inputs_Period_Range[i]]);}
  Serial.print("#");
  for (byte i = 0; i < Dig_Inputs_Period_Full_length; i++){if (i>0){Serial.print(";");} Serial.print(Dig_Inputs_Pin[Log_Dig_Inputs_Period_Full[i]]);}
  Serial.print("#");
  for (byte i = 0; i < Dig_Inputs_Pulse_length; i++){if (i>0){Serial.print(";");} Serial.print(Dig_Inputs_Pin[Log_Dig_Inputs_Pulse[i]]);}
  Serial.print("#");
  for (byte i = 0; i < Dig_Inputs_Pulse_Range_length; i++){if (i>0){Serial.print(";");} Serial.print(Dig_Inputs_Pin[Log_Dig_Inputs_Pulse_Range[i]]);}
  Serial.print("#");
  for (byte i = 0; i < Dig_Inputs_Pulse_Full_length; i++){if (i>0){Serial.print(";");} Serial.print(Dig_Inputs_Pin[Log_Dig_Inputs_Pulse_Full[i]]);}
  Serial.println("/0");
}
