#define SlimROOT_cxx
#include <iostream>
#include "SlimROOT.hh"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "TRandom3.h"
#include <map>
#include <vector>

const bool scan = false;
const int scan_Evt = 492090;
const int modu = 1e4;
const double clight = 299.792;  // mm/ns
using namespace std;

float CalcRadius( float x1, float y1,
                     float x2, float y2,
                     float x3, float y3);

void SlimROOT::Loop()
{

    TString outfileName("dp_ana.root");
    TFile* fout;   
    if(!scan) fout = TFile::Open(outfileName, "RECREATE");
    TTree* tout = new TTree("treeANA","Dark Photon");
    
    // Register vaiables
    Int_t       _EventID    ;
    float    _Rndm[4]    ;
    float    _Pi[5]      ;
    float    _Pf[4]      ;
    float    _ECalE      ;
    //float    _ECalE_0    ;
    float    _ECalE_0p1MeV;
    float    _ECalE_0p5MeV;
    float    _ECalE_1MeV ;
    float    _ECalE_2MeV ;
    float    _ECalE_3MeV ;
    float    _ECalE_4MeV ;
    float    _ECalE_5MeV ;
    Int_t       _Nb_ECal    ;
    float    _ECalPosV[3]; // the variance along x,y,z respectively
    float    _ECalPosX[1500];
    float    _ECalPosY[1500];
    float    _HCalE      ;
    float    _HCalPosV[3]; // the variance along x,y,z respectively
    
    tout->Branch("EventID"      , &_EventID     , "EventID/I")      ;
    tout->Branch("Pi"           , &_Pi          , "Pi[5]/F")  ;
    tout->Branch("Pf"           , &_Pf          , "Pf[4]/F")  ;
    tout->Branch("Rndm"         , &_Rndm        , "Rndm[4]/F") ;
    tout->Branch("ECalE"        , &_ECalE       , "ECalE/F")        ;
    //tout->Branch("ECalE0"        , &_ECalE_0       , "ECalE0/F")        ;
    tout->Branch("ECalE0p1"     , &_ECalE_0p1MeV, "ECalE_0p1MeV/F")        ;
    tout->Branch("ECalE0p5"     , &_ECalE_0p5MeV, "ECalE_0p5MeV/F")        ;
    tout->Branch("ECalE1"       , &_ECalE_1MeV  , "ECalE_1MeV/F")        ;
    tout->Branch("ECalE2"       , &_ECalE_2MeV  , "ECalE_2MeV/F")        ;
    tout->Branch("ECalE3"       , &_ECalE_3MeV  , "ECalE_3MeV/F")        ;
    tout->Branch("ECalE4"       , &_ECalE_4MeV  , "ECalE_4MeV/F")        ;
    tout->Branch("ECalE5"       , &_ECalE_5MeV  , "ECalE_5MeV/F")        ;
    tout->Branch("Nb_ECal"      , &_Nb_ECal     , "Nb_ECal/I")        ;
    tout->Branch("ECalPosV"     , &_ECalPosV    , "ECalPosV[3]/F")  ;
    tout->Branch("ECalPosX"     , &_ECalPosX    , "ECalPosX[Nb_ECal]/F")  ;
    tout->Branch("ECalPosY"     , &_ECalPosY    , "ECalPosY[Nb_ECal]/F")  ;
    tout->Branch("HCalE"        , &_HCalE       , "HCalE/F")        ;
    tout->Branch("HCalPosV"     , &_HCalPosV    , "HCalPosV[3]/F")  ;

    
    if (fChain == 0) return;
    //Long64_t nentries = fChain->GetEntriesFast();
    Long64_t nentries = 1e6;
    Long64_t nbytes = 0, nb = 0;

    for (Long64_t jentry=0; jentry<nentries;jentry++) {
    //for (Long64_t jentry=0; jentry<5;jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   nbytes += nb;
        // if (Cut(ientry) < 0) continue;
        if( jentry % modu == 0) cout<<"evt = "<<jentry<<endl;
         
        _EventID = EventID;

        for(int i = 0; i< 4; i++) {
            _Rndm[i] = Rndm[i];
        }

        TRandom3 rnd(1234);

        // Analyzed Tag Tracker
        Int_t tTrkIdx[7];
        Int_t ntTrk = 0;
        Int_t prev_id = 0;

        for(int i=0; i<Nb_tagTrk; i++) {
            
            if( tTrkID[i] == 1 && ntTrk<7 && tChamberNb[i] > prev_id ) {
                tTrkIdx[ntTrk] = i;
                ntTrk++;
                prev_id = tChamberNb[i];
            }
        }

        for(int i=1; i<ntTrk-1; i++) {
            if( ntTrk<2 ) break;
            float r  = CalcRadius( tTrkPosY[ tTrkIdx[i-1] ]*rnd.Gaus(1,0.025), tTrkPosZ[ tTrkIdx[i-1] ]*rnd.Gaus(1,0.025),
                                      tTrkPosY[ tTrkIdx[i  ] ]*rnd.Gaus(1,0.025), tTrkPosZ[ tTrkIdx[i  ] ]*rnd.Gaus(1,0.025),
                                      tTrkPosY[ tTrkIdx[i+1] ]*rnd.Gaus(1,0.025), tTrkPosZ[ tTrkIdx[i+1] ]*rnd.Gaus(1,0.025));
                    
            float pt = 0.3*3.0*r/1000;
            //cout<<"     pt: "<< pt <<", r:"<<r<<", "<<tTrkIdx[i]<<endl;
            _Pi[i-1] = pt;
        }

        // Analyzed Rec Tracker
        Int_t rTrkIdx[6];
        Int_t nrTrk = 0;
        prev_id = 0;

        for(int i=0; i<Nb_recTrk; i++) {
            if( rTrkID[i] == 1 && nrTrk<7 && rChamberNb[i] > prev_id ) {
                rTrkIdx[nrTrk] = i;
                nrTrk++;
                prev_id = rChamberNb[i];
            }
        }

        for(int i=1; i<nrTrk-1; i++) {
            if( nrTrk<2 ) break;
            float r  = CalcRadius( rTrkPosY[ rTrkIdx[i-1] ]*rnd.Gaus(1,0.025), rTrkPosZ[ rTrkIdx[i-1] ]*rnd.Gaus(1,0.025),
                                      rTrkPosY[ rTrkIdx[i  ] ]*rnd.Gaus(1,0.025), rTrkPosZ[ rTrkIdx[i  ] ]*rnd.Gaus(1,0.025),
                                      rTrkPosY[ rTrkIdx[i+1] ]*rnd.Gaus(1,0.025), rTrkPosZ[ rTrkIdx[i+1] ]*rnd.Gaus(1,0.025));
                    
            float pt = 0.3*1.5*r/1000;
            //cout<<"     pt: "<< pt <<", r:"<<r<<", "<<rTrkIdx[i]<<endl;
            _Pf[i-1] = pt;
        }

        // Analyzed ECal
        const double ECal_coeff = 4/3.9;

        TH1D *hE = new TH1D("hE","hE",80000,0.,8000.);
        TH1D *hpx = new TH1D("hpx","hpx",100,-500.,500.);
        TH1D *hpy = new TH1D("hpy","hpy",100,-500.,500.);
        TH1D *hpz = new TH1D("hpz","hpz",100,0., 440.);
        
        std::vector <std::string> Cuts = { "noCut", "At0.1MeV", "At0.5MeV", "At1MeV", "At2MeV", "At3MeV", "At4MeV", "At5MeV" };
        std::map <std::string, TH1D> Histograms;
        Histograms.clear();
        for (auto cut : Cuts) Histograms[cut] = TH1D(cut.c_str(), cut.c_str(), 80000, 0., 8000.);

        _Nb_ECal = Nb_ECal;
        for(int i=0; i<Nb_ECal; i++) {
            double Ernd = rnd.Gaus(1,0.05);
            double Er = (Ernd >= 0) ? ECalE[i] * Ernd : 0 ;
            hE->Fill( Er );
            hpx->Fill(ECalPosX[i]);
            hpy->Fill(ECalPosY[i]);
            hpz->Fill(ECalPosZ[i]);

            _ECalPosX[i] = ECalPosX[i];
            _ECalPosY[i] = ECalPosY[i];

            if (Er>=5) Histograms["At5MeV"].Fill(Er);
            if (Er>=4) Histograms["At4MeV"].Fill(Er);
            if (Er>=3) Histograms["At3MeV"].Fill(Er);
            if (Er>=2) Histograms["At2MeV"].Fill(Er);
            if (Er>=1) Histograms["At1MeV"].Fill(Er);
            if (Er>=0.5) Histograms["At0.5MeV"].Fill(Er);
            if (Er>=0.1) Histograms["At0.1MeV"].Fill(Er);
            if (Er>=0) Histograms["noCut"].Fill(Er);

        }

        float TotalEnergy = hE->GetMean() * hE->GetEntries() * ECal_coeff;
        float ECalXRMS = hpx->GetRMS();
        float ECalYRMS = hpy->GetRMS();
        float ECalZRMS = hpz->GetRMS();

        delete hE;
        delete hpx;
        delete hpy;
        delete hpz;
        
        //_ECalE_0 = TotalEnergy;
        _ECalE = Histograms["noCut"].GetMean() * Histograms["noCut"].GetEntries() * ECal_coeff;
        _ECalE_0p1MeV = Histograms["At0.1MeV"].GetMean() * Histograms["At0.1MeV"].GetEntries() * ECal_coeff;
        _ECalE_0p5MeV = Histograms["At0.5MeV"].GetMean() * Histograms["At0.5MeV"].GetEntries() * ECal_coeff;
        _ECalE_1MeV = Histograms["At1MeV"].GetMean() * Histograms["At1MeV"].GetEntries() * ECal_coeff;
        _ECalE_2MeV = Histograms["At2MeV"].GetMean() * Histograms["At2MeV"].GetEntries() * ECal_coeff;
        _ECalE_3MeV = Histograms["At3MeV"].GetMean() * Histograms["At3MeV"].GetEntries() * ECal_coeff;
        _ECalE_4MeV = Histograms["At4MeV"].GetMean() * Histograms["At4MeV"].GetEntries() * ECal_coeff;
        _ECalE_5MeV = Histograms["At5MeV"].GetMean() * Histograms["At5MeV"].GetEntries() * ECal_coeff;
        //_ECalE = TotalEnergy;
        _ECalPosV[0] = ECalXRMS;
        _ECalPosV[1] = ECalYRMS;
        _ECalPosV[2] = ECalZRMS;
        
        // Analyze HCal
        const double HCal_coeff = 16.*8000./8363.;

        hE = new TH1D("hE","hE",80000,0.,8000.);
        hpx = new TH1D("hpx","hpx",1000,-1500.,1500.);
        hpy = new TH1D("hpy","hpy",1000,-1500.,1500.);
        hpz = new TH1D("hpz","hpz",1000,600., 3650.);
        for(int i=0; i<Nb_HCal; i++) {
            hE->Fill(HCalE[i]);
            hpx->Fill(HCalPosX[i]);
            hpy->Fill(HCalPosY[i]);
            hpz->Fill(HCalPosZ[i]);
        }

        TotalEnergy = hE->GetMean() * hE->GetEntries() * HCal_coeff;
        float HCalXRMS = hpx->GetRMS();
        float HCalYRMS = hpy->GetRMS();
        float HCalZRMS = hpz->GetRMS();

        delete hE;
        delete hpx;
        delete hpy;
        delete hpz;
        
        _HCalE = TotalEnergy;
        _HCalPosV[0] = HCalXRMS;
        _HCalPosV[1] = HCalYRMS;
        _HCalPosV[2] = HCalZRMS;
        
        
        // End Reading
        tout->Fill();

    }


    if(!scan) {
        fout->WriteTObject( tout );
        fout->Close();
        if (fout) delete fout;
    }

}

// oooOooo...oooOooo...oooOooo...oooOooo...oooOooo...oooOooo...oooOooo...oooOooo...oooOooo...oooOooo...

float CalcRadius( float x1, float y1,
                     float x2, float y2,
                     float x3, float y3)
{
    float r = 0;
    float A = x1*(y2-y3) - y1*(x2-x3) + x2*y3-x3*y2;
    float B = (x1*x1+y1*y1)*(y3-y2) + (x2*x2+y2*y2)*(y1-y3) + (x3*x3+y3*y3)*(y2-y1);
    float C = (x1*x1+y1*y1)*(x2-x3) + (x2*x2+y2*y2)*(x3-x1) + (x3*x3+y3*y3)*(x1-x2);
    float D = (x1*x1+y1*y1)*(x3*y2-x2*y3) + (x2*x2+y2*y2)*(x1*y3-x3*y1) + (x3*x3+y3*y3)*(x2*y1-x1*y2);

    if ( A != 0 ) r = sqrt( (B*B+C*C-4*A*D)/(4*A*A) );
    return r;
}


