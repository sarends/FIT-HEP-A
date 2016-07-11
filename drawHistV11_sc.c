#include <string>
#include <iostream>
#include <sstream>

void drawHistV11_sc(TString filename) {
    
    // --------------------------------------------------------------------
    // ---------------------- Makes tree ----------------------------------
    
    // opens stream from data file
    fstream input(filename,ios::in);
    
    // Creates root file - parameters are name and options, "recreate" parameter overwrites file if already exists
    TFile f("data.root","recreate");
    TTree  *t1 = new TTree("t1","t1");
    
    // Creates branches in "t1" tree - parameters are branchname, address, leaflist (here F is type float)
    Float_t pcurrent, ptime;
    t1->Branch("pcurrent",&pcurrent,"pcurrent/F");
    t1->Branch("ptime",&ptime,"ptime/F");
    
    // Fills current branch from file, time from counter
    input>>pcurrent;
    ptime=0;
    t1->Fill();
    int numCurrentEntries = 1;
    
    while ( input>>pcurrent ) {
        ptime+=1;
        t1->Fill();
        numCurrentEntries++;
    }
    
    t1->Write();
    
    // ----------------------------------------------------------------
    // ----------------- Makes "current" histogram  -------------------
    
    gStyle->SetOptStat(1111111111);
    gStyle->SetOptFit(1111);
    
    c1 = new TCanvas("c1","plots",0,0,800,700);
    
    // Definitionf of temmplate histos
    TH1F *hpcurrent = new  TH1F("hpcurrent","",200,-10,10);
    TH1F *temp = new  TH1F("temp","",200,-10,10);
    TH2F *currentVtime = new TH2F("currentVtime","",20,0,95737,60,-15,15);
    
    // label formatting
    hpcurrent->SetTitle("Current Distribution");
    hpcurrent->SetXTitle("Current Value [nA]");
    hpcurrent->GetXaxis()->CenterTitle();
    hpcurrent->SetYTitle("Frequency");
    hpcurrent->GetYaxis()->CenterTitle();
    hpcurrent->GetYaxis()->SetTitleOffset(1.5); //// ADDED
    // style formatting
    hpcurrent->UseCurrentStyle();
    hpcurrent->Draw();
    t1->Draw("pcurrent>>hpcurrent","","");
    hpcurrent->Fit("gaus");
    
    c1->SaveAs("Histcurrent.png"); // NEED TO MAKE GRAPH NAME VARIABLE
    
    
    
    // -------------------------------------------------------------------
    // ----------------- Makes "current v time" histogram ------------
    
    //t1->Draw("pcurrent>>temp");
    //Float_t meanY = h->GetMean();
    //Float_t RMS = temp->GetRMS();
    
    // creates a current v time graph
    t1->Draw("pcurrent:ptime>>currentVtime","","hcolz"); // VAR CUT
    currentVtime->SetMarkerStyle(20);
    currentVtime->SetMarkerSize(1);
    
    
    // label formatting
    currentVtime->SetXTitle("Time [s]");
    currentVtime->GetXaxis()->CenterTitle();
    currentVtime->GetXaxis()->SetTimeDisplay(1);
    currentVtime->GetXaxis()->SetTimeFormat("%m %d %H:%M");
    currentVtime->SetYTitle("Current [nA]");
    currentVtime->GetYaxis()->CenterTitle();
    currentVtime->SetZTitle("Frequency");
    currentVtime->GetZaxis()->CenterTitle();
    currentVtime->SetTitle("Current V Time");
    
    currentVtime->Draw("samehcolz");
    
    c1->SaveAs("HistcurrentVtime.png");
    
    /*
     
     stringstream cutt;
     cutt << "pcurrent>" << meanY+RMS << " || pcurrent<" << meanY-RMS;
     
     char charCut = static_cast char cutt;
     
     cout << "cut: " <<  << endl;
     
     t1.Draw("pcurrent:ptime>>sparkCut",charCut,"");
     
     Float_t upper = meanY + RMS;
     Float_t lower = meanY - RMS;
     
     TCut cut1 = "pcurrent>1";
     TCut cut2 = "pcurrent<0";
     t1.Draw("pcurrent:ptime>>sparkCut",cut1&&cut2,"");
     
     c1->SaveAs("Histsparks.png");
     
     Float_t sparks = sparkCut->GetEntries();
     cout << sparks << " sparks " << endl;
     */
    
}
