#include <map>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include "AtlasStyle.hh"

int main(){
    //SetAtlasStyle();
    TChain* ch = new TChain("treeANA");
    for (int i=0;i<=99; i++) ch->Add(("job"+std::to_string(i)+"/dp_ana.root").c_str());
    printf("total entries: %d\n", ch->GetEntries());
    TCanvas* c = new TCanvas("c", "c", 800, 800);
    c->Print("dp.pdf[");
    gStyle->SetOptStat(0);
    
    std::map<std::string, TH1F> ECal;
    ECal.clear();
    std::vector <std::string> items = {"noCut", "C0p1MeV", "C0p5MeV", "C1MeV", "C2MeV", "C3MeV", "C4MeV", "C5MeV"};
    for (auto item: items) ECal[item] = TH1F(item.c_str(), item.c_str(), 250, 6500, 9000);
    ch->Draw("ECalE>>noCut");
    ch->Draw("ECalE_0p1MeV>>C0p1MeV");
    ch->Draw("ECalE_0p5MeV>>C0p5MeV");
    ch->Draw("ECalE_1MeV>>C1MeV");
    ch->Draw("ECalE_2MeV>>C2MeV");
    ch->Draw("ECalE_3MeV>>C3MeV");
    ch->Draw("ECalE_4MeV>>C4MeV");
    ch->Draw("ECalE_5MeV>>C5MeV");
    int color = 1;
    for (auto item: items){
        ECal[item].SetTitle("");
        ECal[item].SetLineColor(color);
        ECal[item].SetLineWidth(2);
        ECal[item].Scale(0.1);
        color ++;
    }

    c->cd();
    ECal["noCut"].SetMaximum(1600);
    ECal["noCut"].GetXaxis()->SetTitle("Total Energy in ECal [MeV]");
    ECal["noCut"].GetYaxis()->SetTitle("Events / 10 MeV");
    ECal["noCut"].Draw("HIST");
    ECal["C0p1MeV"].Draw("HIST SAME");
    ECal["C0p5MeV"].Draw("HIST SAME");
    ECal["C1MeV"].Draw("HIST SAME");
    ECal["C2MeV"].Draw("HIST SAME");
    ECal["C3MeV"].Draw("HIST SAME");
    ECal["C4MeV"].Draw("HIST SAME");
    ECal["C5MeV"].Draw("HIST SAME");

    TLegend* l = new TLegend(0.65, 0.6, 0.9, 0.9);
    l->SetBorderSize(0);
    l->SetFillStyle(0);
    l->AddEntry("noCut", "no cut", "l");
    l->AddEntry("C0p1MeV", "cut @ 0.1 MeV", "l");
    l->AddEntry("C0p5MeV", "cut @ 0.5 MeV", "l");
    l->AddEntry("C1MeV", "cut @ 1 MeV", "l");
    l->AddEntry("C2MeV", "cut @ 2 MeV", "l");
    l->AddEntry("C3MeV", "cut @ 3 MeV", "l");
    l->AddEntry("C4MeV", "cut @ 4 MeV", "l");
    l->AddEntry("C5MeV", "cut @ 5 MeV", "l");
    l->Draw();

    gStyle->SetLegendFont(42);
    gStyle->SetLegendTextSize(0.03);
    gPad->SetLeftMargin(0.15);
    c->Print("dp.pdf");

    c->cd();
    TH2F* h2 = new TH2F("h2", "h2", 1000, 0, 10, 1000, 0, 10);
    ch->Draw("ECalE/1000:HCalE/1000>>h2");
    h2->Draw();
    h2->SetTitle("");
    h2->GetXaxis()->SetTitle("Energy in HCal [GeV]");
    h2->GetYaxis()->SetTitle("Energy in ECal [GeV]");
    c->Print("dp.pdf");
    c->Print("dp.pdf]");
    return 0;
}
