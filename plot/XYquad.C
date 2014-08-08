#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#define take_off_T 1625  // take-off Throttle
#define min_spin_T 1580  // start-rotating-propellers 
#define max_allowed_T 1660 // max-allowed Throttle


void XYquad(){
  const int n = 362;	
  float t[n],roll[n], pitch[n], yaw[n], N[n],S[n],E[n],W[n];
  string text;
  ifstream data;

  data.open("log.txt");
  
  int k = 0;

  while(!data.eof() && k<n){
    t[k]=k;
    data>>text>>roll[k]>>pitch[k]>>yaw[k]>>N[k]>>S[k]>>E[k]>>W[k];
    k++;
  }
  
    TGraphErrors * grp_roll = new TGraph(n,t,roll);
    TGraphErrors * grp_pitch = new TGraph(n,t,pitch);
    TGraphErrors * grp_yaw = new TGraph(n,t,yaw);
    TGraphErrors * grp_N = new TGraph(n,t,N);
    TGraphErrors * grp_S = new TGraph(n,t,S);
    TGraphErrors * grp_E = new TGraph(n,t,E);
    TGraphErrors * grp_W = new TGraph(n,t,W);
    
    TCanvas * Ca0 = new TCanvas("Ca0","Canvas",1200,800);    

    Ca0->Divide(2,3);
    Ca0_1->cd();
    grp_roll->SetTitle("roll");  grp_roll->SetLineColor(kRed);
    grp_roll->SetMarkerStyle(20);  grp_roll->SetMarkerSize(0.7);
    grp_roll->SetMinimum(-90);  grp_roll->SetMaximum(90);
    grp_roll->GetXaxis()->SetTitle("t (A.U.)"); grp_roll->GetYaxis()->SetTitle("roll(degrees)");
    grp_roll->Draw("AP");

    Ca0_2->cd();
    grp_pitch->SetTitle("pitch");  grp_pitch->SetLineColor(kRed);
    grp_pitch->SetMarkerStyle(20);  grp_pitch->SetMarkerSize(0.7);
    grp_pitch->SetMinimum(-90);  grp_pitch->SetMaximum(90);
    grp_pitch->GetXaxis()->SetTitle("t (A.U.)"); grp_pitch->GetYaxis()->SetTitle("pitch(degrees)");
    grp_pitch->Draw("AP");
    
    Ca0_3->cd();
    grp_N->SetTitle("N");  grp_N->SetLineColor(kRed);
    grp_N->SetMarkerStyle(20);  grp_N->SetMarkerSize(0.7);
    grp_N->SetMinimum(min_spin_T);  grp_N->SetMaximum(max_allowed_T);
    grp_N->GetXaxis()->SetTitle("t (A.U.)"); grp_N->GetYaxis()->SetTitle("N(pulse)");
    grp_N->Draw("AP");

    Ca0_4->cd();
    grp_E->SetTitle("E");  grp_E->SetLineColor(kRed);
    grp_E->SetMarkerStyle(20);  grp_E->SetMarkerSize(0.7);
    grp_E->SetMinimum(min_spin_T);  grp_E->SetMaximum(max_allowed_T);
    grp_E->GetXaxis()->SetTitle("t (A.U.)"); grp_E->GetYaxis()->SetTitle("E(pulse)");
    grp_E->Draw("AP");

    Ca0_5->cd();
    grp_S->SetTitle("S");  grp_S->SetLineColor(kRed);
    grp_S->SetMarkerStyle(20);  grp_S->SetMarkerSize(0.7);
    grp_S->SetMinimum(min_spin_T);  grp_S->SetMaximum(max_allowed_T);
    grp_S->GetXaxis()->SetTitle("t (A.U.)"); grp_S->GetYaxis()->SetTitle("S(pulse)");
    grp_S->Draw("AP");

    Ca0_6->cd();
    grp_W->SetTitle("W");  grp_W->SetLineColor(kRed);
    grp_W->SetMarkerStyle(20);  grp_W->SetMarkerSize(0.7);
    grp_W->SetMinimum(min_spin_T);  grp_W->SetMaximum(max_allowed_T);
    grp_W->GetXaxis()->SetTitle("t (A.U.)"); grp_W->GetYaxis()->SetTitle("W(pulse)");
    grp_W->Draw("AP");

    Ca0->SaveAs("XYquad.png");
    exit (0);
            
}