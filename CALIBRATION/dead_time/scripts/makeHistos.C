#include <time.h>
#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TMath.h>
#include <TH1.h>

using namespace std;

TFile *inFile, *outFile;
TTree *dataTree, *scalerTree;
TString specl, specu;

TH1F *trigTdcHisto, *edtmTdcHisto, *trigRateHisto;
TH1F *bcm4aCurrentHisto, *bcm4bCurrentHisto, *eventTypeHisto;
 
UInt_t nDataEntries, nScalerEntries, ievent, jevent;

Double_t bcm4aAvgCurrent, bcm4bAvgCurrent;

Double_t edtmTdcTime, trigTdcTime, trigScalerRate;

TBranchElement *evBranch;

Int_t eventType;

void makeHistos(TString spec, UInt_t trigNum, UInt_t runNum, Double_t edtmRate) {
  
  // Initialize the analysis clock
  clock_t t;
  t = clock();

  if (spec == "shms") {specl = "p"; specu = "P";}
  if (spec == "hms")  {specl = "h"; specu = "H";}

  inFile  = new TFile(Form("ROOTfiles/"+spec+"_replay_production_all_%d_-1.root", runNum));
  // inFile  = new TFile(Form("ROOTfiles/dead-time-win-cuts/"+spec+"_replay_production_all_%d_-1.root", runNum));
  // inFile  = new TFile(Form("ROOTfiles/dead-time-no-win-cuts/"+spec+"_replay_production_all_%d_-1.root", runNum));
  outFile = new TFile(Form("outRootFiles/%s_diagnosticHistosRun_%d.root", string(spec).c_str(), runNum), "RECREATE");

  edtmTdcHisto      = new TH1F(specl+"edtmTdc", "EDTM TDC Time; TDC Time (ns); Number of Entries / 100 ps", 10000, 0, 1000);
  trigTdcHisto      = new TH1F(Form(specl+"trig%dTdc", trigNum), Form("TRIG %d TDC Time; TDC Time (ns); Number of Entries / 100 ps", trigNum), 10000, 0, 1000);
  trigRateHisto     = new TH1F(Form(specl+"trig%dRate", trigNum), Form("TRIG %d Rate; Rate (Hz); Number of Entries / 10 Hz", trigNum), 50000, 0, 500000);
  bcm4aCurrentHisto = new TH1F("bcm4aCurrent", "BCM4A Current; Current (#mu A); Number of Entries / 100 nA", 1000, 0, 100);
  bcm4bCurrentHisto = new TH1F("bcm4bCurrent", "BCM4B Current; Current (#mu A); Number of Entries / 100 nA", 1000, 0, 100);
  eventTypeHisto    = new TH1F("eventType", "CODA Event Type; Event Type; Number of Entries", 4, -0.5, 3.5);

  // =:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=

  dataTree = dynamic_cast <TTree*> (inFile->FindObjectAny("T"));
  dataTree->SetBranchAddress("T."+spec+"."+specl+"EDTM_tdcTime", &edtmTdcTime);
  dataTree->SetBranchAddress(Form("T."+spec+"."+specl+"TRIG%d_tdcTime", trigNum), &trigTdcTime);
  dataTree->SetBranchAddress(specu+".bcm.bcm4a.AvgCurrent", &bcm4aAvgCurrent);
  dataTree->SetBranchAddress(specu+".bcm.bcm4b.AvgCurrent", &bcm4bAvgCurrent);

  evBranch = dynamic_cast <TBranchElement*> (dataTree->GetBranch("fEvtHdr.fEvtType"));
  dataTree->SetBranchAddress("fEvtHdr.fEvtType", &eventType);

  nDataEntries = dataTree->GetEntries();
  // nDataEntries = 10000;

  cout << "\n****************************************************" << endl;
  cout << nDataEntries << " Events in TTree T Will Be Processed"   << endl;
  cout << "****************************************************\n" << endl;

  for(ievent = 0; ievent < nDataEntries; ievent++) { 
    dataTree->GetEntry(ievent);
    if (edtmTdcTime > 0.0) edtmTdcHisto->Fill(edtmTdcTime);
    if (trigTdcTime > 0.0) trigTdcHisto->Fill(trigTdcTime);
    if (bcm4aAvgCurrent > 1.0) bcm4aCurrentHisto->Fill(bcm4aAvgCurrent); 
    if (bcm4bAvgCurrent > 1.5) bcm4bCurrentHisto->Fill(bcm4bAvgCurrent); 

    evBranch->GetEntry(ievent);
    eventType = evBranch->GetValue(0, 0);
    eventTypeHisto->Fill(eventType);

    if (ievent % 10000 == 0 && ievent != 0)
      cout << ievent << " Events Have Been Processed..." << endl;
  } // data tree entries loop

  // =:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=

  scalerTree = dynamic_cast <TTree*> (inFile->FindObjectAny("TS"+specu));

  scalerTree->SetBranchAddress(Form(specu+"."+specl+"TRIG%d.scalerRate", trigNum), &trigScalerRate);

  nScalerEntries = scalerTree->GetEntries();
 
  cout << "\n****************************************************" << endl;
  cout << nScalerEntries << " Events in TTree TS" << specu << " Will Be Processed" << endl;
  cout << "****************************************************\n" << endl;

  for(jevent = 0; jevent < nScalerEntries; jevent++) { 
    scalerTree->GetEntry(jevent);   
    if (trigScalerRate > (edtmRate+edtmRate*0.10)) trigRateHisto->Fill(trigScalerRate);
    if (jevent % 1000 == 0 && jevent != 0)
      cout << jevent << " Events Have Been Processed..." << endl;
  } // scaler tree entries loop

  outFile->Write();

  // Calculate the analysis rate
  t = clock() - t;
  printf ("The Analysis Took %.1f seconds \n", ((float) t) / CLOCKS_PER_SEC);
  printf ("The Analysis Event Rate Was %.3f kHz \n", (ievent + jevent + 2) / (((float) t) / CLOCKS_PER_SEC*1000.));

}
