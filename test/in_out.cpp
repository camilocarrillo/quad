#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <cmath>

#define take_off_T 1625  // take-off Throttle
#define min_spin_T 1581  // start-rotating-propellers 
#define max_allowed_T 1660 // max-allowed Throttle
#define min_allowed_T 1580 // min-allowed Throttle
#define tolerance 0.0   // stability tolerance in degrees
#define OUTPUT_READABLE_YAWPITCHROLL
#define S 0
#define W 4
#define E 5
#define N 7 
#define printlog true
#define PI 3.1415926

using namespace std;

double N_t;
double W_t;
double E_t;
double S_t;
bool ramping=false;

double yaw0=0,pitch0=0,roll0=0;  
double yaw=0,pitch=0,roll=0;  

ofstream logfile;
ofstream myfile;

double Kp=0.25;
double Ki=0.02;
double Kd=0.25;

double pitch_error=0, pitch_error_old=0, pitch_derivative=0, pitch_output=0, pitch_integral=0;
double roll_error=0,  roll_error_old=0,  roll_derivative=0,  roll_output=0,  roll_integral=0;
double yaw_error=0,   yaw_error_old=0,   yaw_derivative=0,   yaw_output=0,   yaw_integral=0;


uint GetMeanThrottle(){
  return int((N_t+E_t+S_t+W_t)*0.25);
};

bool Valid_Throttle(uint throttle){
  if(ramping==false && throttle<min_allowed_T){ std::cout<<"ERROR: min throttle excceed min_spin_T = "<<min_spin_T<<" asked_T="<<throttle<<std::endl; return false;}
  if(ramping==false && throttle>max_allowed_T){ std::cout<<"ERROR: max throttle excceed max_allowed_T = "<<max_allowed_T<<" asked_T="<<throttle<<std::endl; return false;}
  return true;
}

void  Throttle(uint motorID,uint throttle){
  if(Valid_Throttle(throttle)==false) return;
  if(motorID==N)N_t=throttle;
  else if(motorID==S)S_t=throttle;
  else if(motorID==W)W_t=throttle;
  else if(motorID==E)E_t=throttle;
  if(printlog){
    logfile<<"to_plot: "<<roll<<" "<<pitch<<" "<<yaw<<" "<<N_t<<" "<<S_t<<" "<<E_t<<" "<<W_t<<std::endl; 
  }
}

void  ThrottleAllplusplus(){
  Throttle(N,N_t+1); 
  Throttle(E,E_t+1); 
  Throttle(S,S_t+1); 
  Throttle(W,W_t+1); 
}

void  ThrottleAll(uint throttle){
  Throttle(N,throttle); 
  Throttle(E,throttle); 
  Throttle(S,throttle); 
  Throttle(W,throttle); 
}

void  InitMotors(){
  cout<<"INFO: Init Motors"<<endl;
  ramping=true;
  ThrottleAll(0);
  ThrottleAll(1500);
  ramping=false;
  usleep(1000000);
}

void  StopMotors(){
  cout<<"INFO: Stop Motors"<<endl;
  ramping=true;
  ThrottleAll(0);
  ramping=false;
  usleep(200000);
}

void Ramping(uint motorID,uint final_Throttle){
  if(Valid_Throttle(final_Throttle)==false) return;
  ramping=true;
  uint initial_Throttle;
  if(motorID==N){initial_Throttle=N_t; cout<<"INFO: Ramping N from "<<initial_Throttle<<" to "<<final_Throttle<<endl;}
  else if(motorID==S){initial_Throttle=S_t; cout<<"INFO: Ramping S from "<<initial_Throttle<<" to "<<final_Throttle<<endl;}
  else if(motorID==W){initial_Throttle=W_t; cout<<"INFO: Ramping W from "<<initial_Throttle<<" to "<<final_Throttle<<endl;}
  else if(motorID==E){initial_Throttle=E_t; cout<<"INFO: Ramping E from "<<initial_Throttle<<" to "<<final_Throttle<<endl;}
  else cout<<"WARNING  WRONG motorID : "<<motorID<<endl;
  
  if(initial_Throttle==final_Throttle) return;
  
  if(initial_Throttle<final_Throttle){
    for(uint throttle=initial_Throttle;throttle<=final_Throttle;throttle++){
      Throttle(motorID,throttle);
      if(throttle<=final_Throttle-4) usleep(50000); 
    }
  }else{
    for(uint throttle=initial_Throttle;throttle>=final_Throttle;throttle--){
      Throttle(motorID,throttle);
      if(throttle>=final_Throttle+4) usleep(50000); 
    }
  }
  ramping=false;
  //  usleep(500000);
}

void RampingAll(uint final_Throttle){
  if(Valid_Throttle(final_Throttle)==false) return;
  uint initial_Throttle_N=N_t;
  uint initial_Throttle_S=S_t;
  uint initial_Throttle_W=W_t;
  uint initial_Throttle_E=E_t;
  
  double steps=10;
  
  for(double t=1;t<=steps;t++){
    Ramping(N,initial_Throttle_N+uint((final_Throttle-initial_Throttle_N)*t/steps));
    Ramping(E,initial_Throttle_E+uint((final_Throttle-initial_Throttle_E)*t/steps));
    Ramping(S,initial_Throttle_S+uint((final_Throttle-initial_Throttle_S)*t/steps));
    Ramping(W,initial_Throttle_W+uint((final_Throttle-initial_Throttle_W)*t/steps));
  }
}

void  Stabilise(float timeS,double Kp,double Ki,double  Kd,uint value_T){   // PID
  double dt=timeS;
  pitch_error      = fabs(pitch0 - pitch) > tolerance? pitch0 - pitch :0;
  pitch_integral   = pitch_integral + pitch_error*dt;
  pitch_derivative = (pitch_error - pitch_error_old)/dt;
  pitch_output     = int(Kp*pitch_error + Ki*pitch_integral + Kd*pitch_derivative);	
  pitch_error_old  = pitch_error;
  
  roll_error      = fabs(roll0 - roll) > tolerance? roll0 - roll :0;
  roll_integral   = roll_integral + roll_error*dt;
  roll_derivative = (roll_error - roll_error_old)/dt;
  roll_output     = int(Kp*roll_error + Ki*roll_integral + Kd*roll_derivative);	
  roll_error_old  = roll_error;
  
  yaw_error      = fabs(yaw0 - yaw) > tolerance? yaw0 - yaw :0;
  yaw_integral   = yaw_integral + yaw_error*dt;
  yaw_derivative = (yaw_error - yaw_error_old)/dt;
  yaw_output     = int(Kp*yaw_error + Ki*yaw_integral + Kd*yaw_derivative);	
  yaw_error_old  = yaw_error;
  
  Throttle(N,value_T+pitch_output); Throttle(S,value_T-pitch_output);
  Throttle(E,value_T+roll_output);  Throttle(W,value_T-roll_output);
}

int main () {
  logfile.open("log.txt");
  logfile<<"Conf: "<<Kp<<" "<<Ki<<" "<<Kd<<endl;
  InitMotors();                       // start motors pulse 1500us  
  ThrottleAll(min_spin_T);
  RampingAll(take_off_T); 
  for(double t=0;t<=300;t++){
    double w=2*PI/50;
    pitch=20*sin(w*t);
    roll=30*cos(2*w*t);
    Stabilise(1,Kp,Ki,Kd,take_off_T);
  }
  RampingAll(take_off_T);
  RampingAll(min_spin_T);
  StopMotors();                 
  return 0;
}
