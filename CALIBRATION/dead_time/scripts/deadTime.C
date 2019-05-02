#include <time.h>
#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TMath.h>

using namespace std;

TFile *inFile;
TTree *dataTree, *scalerTree;
TString specl, specu;
TBranchElement *evBranch;
 
Double_t edtmTdcCutLow, edtmTdcCutHigh;
Double_t trigTdcCutLow, trigTdcCutHigh;

UInt_t nDataEntries, nScalerEntries, ievent, jevent;

Int_t  eventType;
UInt_t eventTypeCntr,  eventTypeCntrErr;

UInt_t edtmTdcHitCntr_beamOn,       edtmTdcHitCntr_beamOff;
UInt_t trigTdcHitCntr_beamOn,       trigTdcHitCntr_beamOff;
UInt_t trigTdcHitCntrNoEdtm_beamOn, trigTdcHitCntrNoEdtm_beamOff;
UInt_t edtmScalerCntr_beamOn,       edtmScalerCntr_beamOff;
UInt_t trigScalerCntr_beamOn,       trigScalerCntr_beamOff;
UInt_t trigScalerCntrNoEdtm_beamOn, trigScalerCntrNoEdtm_beamOff;
UInt_t eventTypeCntr_beamOn,        eventTypeCntr_beamOff;

Double_t edtmTdcHitCntrErr_beamOn,       edtmTdcHitCntrErr_beamOff;
Double_t trigTdcHitCntrErr_beamOn,       trigTdcHitCntrErr_beamOff;
Double_t trigTdcHitCntrNoEdtmErr_beamOn, trigTdcHitCntrNoEdtmErr_beamOff;

Double_t edtmScalerCntrErr_beamOn,       edtmScalerCntrErr_beamOff;
Double_t trigScalerCntrErr_beamOn,       trigScalerCntrErr_beamOff;
Double_t trigScalerCntrNoEdtmErr_beamOn, trigScalerCntrNoEdtmErr_beamOff;
Double_t eventTypeCntrErr_beamOn,        eventTypeCntrErr_beamOff;

Double_t bcm4aAvgCurrent,    bcm4bAvgCurrent;
Double_t bcm4aScalerCurrent, bcm4bScalerCurrent;

Double_t edtmTdcTime, trigTdcTime;
Double_t edtmScaler, trigScaler;

Double_t compLiveTime, compLiveTimeEt, compLiveTimeNoEdtm;
Double_t elecLiveTime, elecLiveTimeEt, elecLiveTimeNoEdtm;
 
Double_t compLiveTimeErr, compLiveTimeEtErr, compLiveTimeNoEdtmErr;
Double_t elecLiveTimeErr, elecLiveTimeEtErr, elecLiveTimeNoEdtmErr;

Double_t totalLiveTime, totalLiveTimeErr;

Double_t calcGaussErr(UInt_t numCounts) {
  return 1./TMath::Sqrt(numCounts);
}

Double_t calcGaussRatioErr(UInt_t numerCnts, UInt_t denomCnts) {
  return ((Double_t) numerCnts / (Double_t) denomCnts) * TMath::Sqrt((1./((Double_t) numerCnts)) + (1./((Double_t) denomCnts)));
}

Double_t calcBinomErr(UInt_t numCounts) {
  return 0.5*TMath::Sqrt(1./numCounts);
}

Double_t calcBinomRatioErr(UInt_t numerCnts, UInt_t denomCnts) {
  return (0.5*(Double_t) numerCnts / (Double_t) denomCnts) * TMath::Sqrt((1./((Double_t) numerCnts)) + (1./((Double_t) denomCnts)));
}

Double_t calcRatioErr(Double_t numer, Double_t numerErr, Double_t denom, Double_t denomErr) {
  return (numer/denom) * TMath::Sqrt(TMath::Power(numerErr/numer, 2.) + TMath::Power(denomErr/denom, 2.));
}

void deadTime(TString spec, UInt_t trigNum, UInt_t runNum, Double_t bcmCut, Double_t edtmCutLow, Double_t edtmCutHigh, Double_t trigCutLow, Double_t trigCutHigh) {
  
  // Initialize the analysis clock
  clock_t t;
  t = clock();

  if (spec == "shms") {
    specl = "p"; specu = "P";
  }
  if (spec == "hms") {
    specl = "h"; specu = "H";
  }

  edtmTdcCutLow = edtmCutLow; edtmTdcCutHigh = edtmCutHigh;
  trigTdcCutLow = trigCutLow; trigTdcCutHigh = trigCutHigh;
 
  inFile = new TFile(Form("ROOTfiles/"+spec+"_replay_production_all_%d_-1.root", runNum));
  
  // =:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=

  dataTree = dynamic_cast <TTree*> (inFile->FindObjectAny("T"));

  dataTree->SetBranchAddress("T."+spec+"."+specl+"EDTM_tdcTime", &edtmTdcTime);
  dataTree->SetBranchAddress(Form("T."+spec+"."+specl+"TRIG%d_tdcTime", trigNum), &trigTdcTime);
  dataTree->SetBranchAddress(specu+".bcm.bcm4a.AvgCurrent", &bcm4aAvgCurrent);
  dataTree->SetBranchAddress(specu+".bcm.bcm4b.AvgCurrent", &bcm4bAvgCurrent);

  evBranch = dynamic_cast <TBranchElement*> (dataTree->GetBranch("fEvtHdr.fEvtType"));

  nDataEntries = dataTree->GetEntries();
  // nDataEntries = 1000;

  cout << "\n****************************************************" << endl;
  cout << nDataEntries << " Events in TTree T Will Be Processed"   << endl;
  cout << "****************************************************\n" << endl;

  edtmTdcHitCntr_beamOn       = 0; edtmTdcHitCntr_beamOff       = 0;
  trigTdcHitCntr_beamOn       = 0; trigTdcHitCntr_beamOff       = 0;
  trigTdcHitCntrNoEdtm_beamOn = 0; trigTdcHitCntrNoEdtm_beamOff = 0;
  eventTypeCntr_beamOn        = 0; eventTypeCntr_beamOff        = 0;

  for(ievent = 0; ievent < nDataEntries; ievent++) { 
    dataTree->GetEntry(ievent);
    if (bcm4aAvgCurrent > bcmCut) { 
      // if (edtmTdcTime > edtmTdcCutLow && edtmTdcTime < edtmTdcCutHigh) edtmTdcHitCntr_beamOn += 1;
      // Required for double peak in EDTM self timing distribution (wonky reference 4-fold reference time)
      if ((edtmTdcTime > edtmTdcCutLow && edtmTdcTime < edtmTdcCutHigh) || (edtmTdcTime > 155. && edtmTdcTime < 157.)) edtmTdcHitCntr_beamOn += 1;
      if (trigTdcTime > trigTdcCutLow && trigTdcTime < trigTdcCutHigh) trigTdcHitCntr_beamOn += 1;
    }
    if (bcm4aAvgCurrent < bcmCut) { 
      // if (edtmTdcTime > edtmTdcCutLow && edtmTdcTime < edtmTdcCutHigh) edtmTdcHitCntr_beamOff += 1;
      // Required for double peak in EDTM self timing distribution (wonky reference 4-fold reference time)
      if ((edtmTdcTime > edtmTdcCutLow && edtmTdcTime < edtmTdcCutHigh) || (edtmTdcTime > 155. && edtmTdcTime < 157.)) edtmTdcHitCntr_beamOff += 1;
      if (trigTdcTime > trigTdcCutLow && trigTdcTime < trigTdcCutHigh) trigTdcHitCntr_beamOff += 1;
    }

     evBranch->GetEntry(ievent);
     eventType = evBranch->GetValue(0, 0);
     if (spec == "shms" && eventType == 1) {
       eventTypeCntr += 1;
       if (bcm4aAvgCurrent > bcmCut) eventTypeCntr_beamOn  += 1;
       if (bcm4aAvgCurrent < bcmCut) eventTypeCntr_beamOff += 1;
     }
     if (spec == "hms"  && eventType == 2) {
       eventTypeCntr += 1;
       if (bcm4aAvgCurrent > bcmCut) eventTypeCntr_beamOn  += 1;
       if (bcm4aAvgCurrent < bcmCut) eventTypeCntr_beamOff += 1;
     }
 
    if (ievent % 10000 == 0 && ievent != 0)
      cout << ievent << " Events Have Been Processed..." << endl;
  } // data tree entries loop

  trigTdcHitCntrNoEdtm_beamOn  = trigTdcHitCntr_beamOn  - edtmTdcHitCntr_beamOn;
  trigTdcHitCntrNoEdtm_beamOff = trigTdcHitCntr_beamOff - edtmTdcHitCntr_beamOff;

  edtmTdcHitCntrErr_beamOn        = calcBinomErr(edtmTdcHitCntr_beamOn);
  edtmTdcHitCntrErr_beamOff       = calcBinomErr(edtmTdcHitCntr_beamOff);
  trigTdcHitCntrErr_beamOn        = calcBinomErr(trigTdcHitCntr_beamOn);
  trigTdcHitCntrErr_beamOff       = calcBinomErr(trigTdcHitCntr_beamOff);
  trigTdcHitCntrNoEdtmErr_beamOn  = calcBinomErr(trigTdcHitCntrNoEdtm_beamOn);
  trigTdcHitCntrNoEdtmErr_beamOff = calcBinomErr(trigTdcHitCntrNoEdtm_beamOff);
  eventTypeCntrErr                = calcBinomErr(eventTypeCntr);
  eventTypeCntrErr_beamOn         = calcBinomErr(eventTypeCntr_beamOn);
  eventTypeCntrErr_beamOff        = calcBinomErr(eventTypeCntr_beamOff);

  // =:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=

  scalerTree = dynamic_cast <TTree*> (inFile->FindObjectAny("TS"+specu));

  scalerTree->SetBranchAddress(specu+".EDTM.scaler", &edtmScaler);
  scalerTree->SetBranchAddress(Form(specu+"."+specl+"TRIG%d.scaler", trigNum), &trigScaler);
  scalerTree->SetBranchAddress(specu+".BCM4A.scalerCurrent", &bcm4aScalerCurrent);
  scalerTree->SetBranchAddress(specu+".BCM4B.scalerCurrent", &bcm4bScalerCurrent);

  nScalerEntries = scalerTree->GetEntries();
 
  cout << "\n****************************************************" << endl;
  cout << nScalerEntries << " Events in TTree TS" << specu << " Will Be Processed" << endl;
  cout << "****************************************************\n" << endl;

  edtmScalerCntr_beamOn       = 0; edtmScalerCntr_beamOff       = 0;
  trigScalerCntr_beamOn       = 0; trigScalerCntr_beamOff       = 0;
  trigScalerCntrNoEdtm_beamOn = 0; trigScalerCntrNoEdtm_beamOff = 0;

  for(jevent = 0; jevent < nScalerEntries; jevent++) { 
    scalerTree->GetEntry(jevent);
    if (bcm4aScalerCurrent > bcmCut) { 
      edtmScalerCntr_beamOn = edtmScaler - edtmScalerCntr_beamOff;
      trigScalerCntr_beamOn = trigScaler - trigScalerCntr_beamOff;
    }
    if (bcm4aScalerCurrent < bcmCut) { 
      edtmScalerCntr_beamOff = edtmScaler - edtmScalerCntr_beamOn;
      trigScalerCntr_beamOff = trigScaler - trigScalerCntr_beamOn;
    }
    if (jevent % 1000 == 0 && jevent != 0)
      cout << jevent << " Events Have Been Processed..." << endl;
  } // scaler tree entries loop

  trigScalerCntrNoEdtm_beamOn  = trigScalerCntr_beamOn  - edtmScalerCntr_beamOn;
  trigScalerCntrNoEdtm_beamOff = trigScalerCntr_beamOff - edtmScalerCntr_beamOff;

  if (edtmScalerCntr_beamOn + edtmScalerCntr_beamOff != edtmScaler)
    cout << "!!!!! ERROR: EDTM Scaler Calculations Not Reliable !!!!!" << endl;
  if (trigScalerCntr_beamOn + trigScalerCntr_beamOff != trigScaler)
    cout << "!!!!! ERROR: TRIG Scaler Calculations Not Reliable !!!!!" << endl;
  if (eventTypeCntr_beamOn  + eventTypeCntr_beamOff  != eventTypeCntr)
    cout << "!!!!! ERROR: Event Type  Calculations Not Reliable !!!!!" << endl;
 
  edtmScalerCntrErr_beamOn        = calcBinomErr(edtmScalerCntr_beamOn);
  edtmScalerCntrErr_beamOff       = calcBinomErr(edtmScalerCntr_beamOff);
  trigScalerCntrErr_beamOn        = calcBinomErr(trigScalerCntr_beamOn);
  trigScalerCntrErr_beamOff       = calcBinomErr(trigScalerCntr_beamOff);
  trigScalerCntrNoEdtmErr_beamOn  = calcBinomErr(trigScalerCntrNoEdtm_beamOn);
  trigScalerCntrNoEdtmErr_beamOff = calcBinomErr(trigScalerCntrNoEdtm_beamOff);

  totalLiveTime      = (Double_t) edtmTdcHitCntr_beamOn       / (Double_t) edtmScalerCntr_beamOn;
  compLiveTime       = (Double_t) trigTdcHitCntr_beamOn       / (Double_t) trigScalerCntr_beamOn;
  compLiveTimeEt     = (Double_t) eventTypeCntr_beamOn        / (Double_t) trigScalerCntr_beamOn;
  compLiveTimeNoEdtm = (Double_t) trigTdcHitCntrNoEdtm_beamOn / (Double_t) trigScalerCntrNoEdtm_beamOn;
  elecLiveTime       = totalLiveTime / compLiveTime;
  elecLiveTimeEt     = totalLiveTime / compLiveTimeEt;
  elecLiveTimeNoEdtm = totalLiveTime / compLiveTimeNoEdtm;

  totalLiveTimeErr      = calcBinomRatioErr(edtmTdcHitCntr_beamOn,       edtmScalerCntr_beamOn);
  compLiveTimeErr       = calcBinomRatioErr(trigTdcHitCntr_beamOn,       trigScalerCntr_beamOn);
  compLiveTimeEtErr     = calcBinomRatioErr(eventTypeCntr_beamOn,        trigScalerCntr_beamOn);
  compLiveTimeNoEdtmErr = calcBinomRatioErr(trigTdcHitCntrNoEdtm_beamOn, trigScalerCntrNoEdtm_beamOn);
  elecLiveTimeErr       = calcRatioErr(totalLiveTime, totalLiveTimeErr, compLiveTime,       compLiveTimeErr);
  elecLiveTimeEtErr     = calcRatioErr(totalLiveTime, totalLiveTimeErr, compLiveTimeEt,     compLiveTimeEtErr);
  elecLiveTimeNoEdtmErr = calcRatioErr(totalLiveTime, totalLiveTimeErr, compLiveTimeNoEdtm, compLiveTimeNoEdtmErr);

  cout << "\n=:=:=:=:=:=:=:=:=:=:=: EDTM =:=:=:=:=:=:=:=:=:=:=:\n" << endl;
  cout << "Beam On  TDC Counter    = " << edtmTdcHitCntr_beamOn    << endl;
  cout << "Beam Off TDC Counter    = " << edtmTdcHitCntr_beamOff   << endl;
  cout << "Beam On  Scaler Counter = " << edtmScalerCntr_beamOn    << endl;
  cout << "Beam Off Scaler Counter = " << edtmScalerCntr_beamOff   << endl;
  cout << "End of Run Scaler Read  = " << edtmScaler               << endl;
  cout << "\n=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:=:\n" << endl;

  cout << "\n|-|-|-|-|-|-|-|-|-|-|- TRIG |-|-|-|-|-|-|-|-|-|-|-\n"             << endl;
  cout << "Beam On  TDC Counter            = " << trigTdcHitCntr_beamOn        << endl;
  cout << "Beam Off TDC Counter            = " << trigTdcHitCntr_beamOff       << endl;
  cout << "Beam On  TDC Counter No EDTM    = " << trigTdcHitCntrNoEdtm_beamOn  << endl;
  cout << "Beam Off TDC Counter No EDTM    = " << trigTdcHitCntrNoEdtm_beamOff << endl;
  cout << "Beam On  Scaler Counter         = " << trigScalerCntr_beamOn        << endl;
  cout << "Beam Off Scaler Counter         = " << trigScalerCntr_beamOff       << endl;
  cout << "Beam On  Scaler Counter No EDTM = " << trigScalerCntrNoEdtm_beamOn  << endl;
  cout << "Beam Off Scaler Counter No EDTM = " << trigScalerCntrNoEdtm_beamOff << endl;
  cout << "End of Run Scaler Read          = " << trigScaler                   << endl;
  cout << "\n|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-\n"             << endl;

  cout << "\n|-|-|-|-|-|-|-|-|-| EVENT TYPE -|-|-|-|-|-|-|-|-|-\n"  << endl;
  cout << "Beam On Event Type Counter  = " << eventTypeCntr_beamOn  << endl;
  cout << "Beam Off Event Type Counter = " << eventTypeCntr_beamOff << endl;
  cout << "Event Type Counter          = " << eventTypeCntr         << endl;
  cout << "\n|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-\n"  << endl;


  cout << Form("Total Live Time              = %.2f +/- %.2f", totalLiveTime*100.,      totalLiveTimeErr*100.)      << " %" << endl;
  cout << Form("Computer Live Time           = %.2f +/- %.2f", compLiveTime*100.,       compLiveTimeErr*100.)       << " %" << endl;
  cout << Form("Electronic Live Time         = %.2f +/- %.2f", elecLiveTime*100.,       elecLiveTimeErr*100.)       << " %" << endl;
  cout << Form("Computer Live Time ET        = %.2f +/- %.2f", compLiveTimeEt*100.,     compLiveTimeEtErr*100.)     << " %" << endl;
  cout << Form("Electronic Live Time ET      = %.2f +/- %.2f", elecLiveTimeEt*100.,     elecLiveTimeEtErr*100.)     << " %" << endl;
  cout << Form("Computer Live Time No EDTM   = %.2f +/- %.2f", compLiveTimeNoEdtm*100., compLiveTimeNoEdtmErr*100.) << " %" << endl;
  cout << Form("Electronic Live Time No EDTM = %.2f +/- %.2f", elecLiveTimeNoEdtm*100., elecLiveTimeNoEdtmErr*100.) << " % \n" << endl;

  // Calculate the analysis rate
  t = clock() - t;
  printf ("The Analysis Took %.1f seconds \n", ((float) t) / CLOCKS_PER_SEC);
  printf ("The Analysis Event Rate Was %.3f kHz \n", (ievent + jevent + 2) / (((float) t) / CLOCKS_PER_SEC*1000.));

}
