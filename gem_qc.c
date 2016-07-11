//
//  gem_qc.c
//  GEM QUALITY CONTROL (Current Leakage Measurement)
//
//  Created by Sarah Arends on 07/07/16.
//  Copyright © 2016 Sarah Arends. All rights reserved.
//

#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>

void gem_qc(TString filename) {
    
    // ------------------------------------ Writes tree from measurement data ------------------------------------
    
    // Variables for holding data
    Float_t ptime(0), pcurrent, pvoltageset, pvoltagemon, delay;
    string header;
    Int_t sparks = 0;
    Int_t sparkThreshold = 50; // Threshold on current value [nA]
    bool voltageHigh;
    std::string pfoil;
    
    // Opens stream from data file
    fstream input(filename,ios::in);
    
    // Creates root file and tree
    TFile f("data.root","recreate");
    TTree  *t1 = new TTree("t1","t1");
    
    // Creates branches in "t1" tree
    // Parameters are branchname, address, leaflist (here F is type float)
    t1->Branch("ptime",&ptime,"ptime/F");
    t1->Branch("pcurrent",&pcurrent,"pcurrent/F");
    t1->Branch("pvoltageset",&pvoltageset,"pvoltageset/F");
    t1->Branch("pvoltagemon",&pvoltagemon,"pvoltagemon/F");
    t1->Branch("pfoil",&pfoil,"pfoil/C");
    
    // Fills tree from data stream
    getline(input, header);
    
    std::istringstream oss(header);
    while(oss >> pfoil) {
    }
    
    while ( input>>delay>>pcurrent>>pvoltageset>>pvoltagemon ) {
        
        // Counts distinct sparks (current spikes)
        if (abs(pcurrent) > sparkThreshold)
            // Is true if voltage spikes
            voltageHigh=1;
        else if (voltageHigh) {
            // If voltage is low, but was just high, increments spark
            sparks++;
            voltageHigh = 0;
        }
        
        ptime+=delay;
        //cout << pcurrent << endl;
        t1->Fill();
    }
    
    t1->Write();
    
    cout << "sparks: " << sparks << endl;
    
    // ---------------------------------------------------------------------------------------------------------
    
    // Variables for data ranges
    Int_t timeMin = 0;
    Int_t timeMax = t1->GetMaximum("ptime"); // Automatically scales time axis
    Int_t currentMin = -50;
    Int_t currentMax = 50;
    Int_t voltageMin = 0;
    Int_t voltageMax = 700;
    Int_t nbinx = 500;
    Int_t nbincurr = 60;
    Int_t nbinvolt = 350;
    
    // Variables for graph formatting
    Float_t label_font = 0.025;
    Float_t axis_offset=1.5;
    Float_t label_size = 0.03;
    Float_t markersize = 0.2;
    Float_t titlesize = 0.033;
    
    // ------------------------- Makes "current/voltage:time" histogram  ----------------------------------------
    
    // Plot definitions
    TCanvas *c1 = new TCanvas("c1","plots",0,0,800,700);
    TPad *pad1 = new TPad("pad1","",0,0,1,1);
    TPad *pad2 = new TPad("pad2","",0,0,1,1);
    
    // Plot formatting and statistics
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);
    pad2->SetFillStyle(4000); // sets transparency
    
    
    // Formats current vs time histo
    TH2F *currentVtime = new TH2F("currentVtime","",nbinx,timeMin,timeMax,nbincurr,currentMin,currentMax);
    currentVtime->SetTitle("GEM foil test");
    currentVtime->SetXTitle("Time [s]");
    currentVtime->GetXaxis()->CenterTitle();
    currentVtime->SetYTitle("Current [nA]");
    currentVtime->GetYaxis()->CenterTitle();
    currentVtime->GetYaxis()->SetTitleOffset(1.4);
    currentVtime->SetMarkerStyle(20);
    currentVtime->SetMarkerSize(markersize);
    currentVtime->SetLabelSize(label_size);
    currentVtime->SetTitleSize(titlesize);
    gPad->SetGrid();
    
    // Formats voltage vs time histo
    TH2F *voltageVtime = new TH2F("voltageVtime","",nbinx,timeMin,timeMax,nbinvolt,voltageMin,voltageMax);
    voltageVtime->SetMarkerColor(2);
    voltageVtime->SetMarkerStyle(20);
    voltageVtime->SetMarkerSize(markersize);
    voltageVtime->SetLabelSize(label_size);
    voltageVtime->SetTitleSize(titlesize);
    
    // Draws current vs time histo
    t1->Draw("pcurrent:ptime>>currentVtime","pvoltageset>0");
    TProfile *currentprofile = new TProfile("currentprofile","currentprofile",nbincurr,currentMin,currentMax);
    currentprofile = currentVtime->ProfileX();
    pad1->Draw();
    pad1->cd();
    currentVtime->SetFillColorAlpha(kRed, 0);
    currentVtime->SetMarkerColorAlpha(kRed, 0);
    gPad->SetGrid();
    currentVtime->Draw("same");
    gPad->SetGrid();
    currentprofile->Draw("same");
    pad1->Update();
    pad1->Modified();
    c1->cd();
    
    Double_t dy = (voltageMax-voltageMin)/0.8; // 10% margins top and bottom
    Double_t dx = (timeMax-timeMin)/0.8; // 10% margins left and right
    pad2->Range(timeMin-0.1*dx,voltageMin-0.1*dy,timeMax+0.1*dx,voltageMax+0.1*dy);
    pad2->Draw();
    pad2->cd();
    
    // Draws another axis on right side of the pad
    TGaxis *axis = new TGaxis(timeMax,voltageMin,timeMax,voltageMax,voltageMin,voltageMax,50510,"+L");
    axis->SetLabelColor(2);
    axis->SetLineColor(2);
    axis->SetLabelSize(label_size);
    axis->SetTitle("Voltage [V]");
    axis->SetTitleColor(2);
    axis->SetTitleOffset(axis_offset);
    axis->CenterTitle(kTRUE);
    axis->SetTitleSize(titlesize);
    axis->Draw("");
    
    // Draws voltage vs time histo
    t1->Draw("pvoltageset:ptime>>voltageVtime","","same");
    voltageVtime->Draw("same");
    pad2->Update();
    
    // Adds text box with header contents
    TPaveText *pt2 = new TPaveText(0.11,0.85,0.5,0.9,"NDC");
    pt2->AddText(header.c_str());
    // pt2->AddLine(.0,.5,1.,.5);
    
    stringstream sparkData;
    sparkData << sparks << " spark(s) with magnitude >" << sparkThreshold << "nA";
    //string sparkDataString = sparkData.str();
    pt2->AddText(sparkData.str().c_str());
    pt2->Draw();
    
    c1->SaveAs("HistcurrentVtime.png");
    
    // ----------------------------------- 1D Histo at 500V -------------------------------------------------------
    
    // Displays statistics
    gStyle->SetOptStat(1001111111);
    gStyle->SetOptFit(1111);
    
    // Creates new canvas and histogram
    TCanvas *c2 = new TCanvas("c2","plots",0,0,800,700);
    TH1F *hpcurrent = new  TH1F("hpcurrent","",200,-50,50);
    
    // Creates graph labels
    hpcurrent->SetTitle("Current Distribution at 500V");
    hpcurrent->SetXTitle("Current Value [nA]");
    hpcurrent->GetXaxis()->CenterTitle();
    hpcurrent->SetYTitle("Frequency");
    hpcurrent->GetYaxis()->CenterTitle();
    
    // Formatting
    hpcurrent->UseCurrentStyle();
    hpcurrent->Draw();
    t1->Draw("abs(pcurrent)>>hpcurrent","ptime>=60 && ptime<=660","");
    hpcurrent->Fit("gaus","","",0,50);
    pt2->Draw();
    
    c2->SaveAs("Histcurrent.png");
    
}
