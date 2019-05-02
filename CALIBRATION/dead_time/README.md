
# Macros to calculate the various components of the live time

1. Begin with creating diagnostic histograms in order to define the cuts to be utilized.
   * python runHistos.py spec run-nums edtmRate
     * spec     = **hms** or **shms**
     * run-nums = run range in form of **a-b** or comma sepataed ranges **a-b,c-d,f**
     * edtmRate = rate of the EDTM pulser
   * Example (hms)  = python runHistos.py hms 1437-1446 100
   * Example (shms) = python runHistos.py shms 2014-2021 100

2. Once the EDTM and trigger TDC self timing cuts have been determined, the dead time can be calculated.
   * python runDeadTime.py spec run-nums edtmTdcLowCut edtmTdcHighCut trigTdcLowCut trigTdcHighCut > specOutput.txt
     * spec     = **hms** or **shms**
     * run-nums = run range in form of **a-b** or comma sepataed ranges **a-b,c-d,f**
     * edtmTdcLow(High)Cut = cuts on the EDTM timing spectra to iterate counters for total live time calculation
     * trigTdcLow(High)Cut = cuts on the trig timing spectra to iterate counters for computer live time calculation
     * > specOutput.txt = pipes output to **hmsOutput.txt** or **shmsOutput.txt** so that it can be 
       parsed for populating python dictionaries which are later used for plotting
   * Example (hms)  = python runDeadTime.py hms 1437-1446 240 242 323 325 > hmsOutput.txt
   * Example (shms) = python runDeadTime.py shms 2014-2021 123 125 272 275 > shmsOutput.txt

3. Make plots to compare the total live time to the computer live time (for low rates) and plot the electronic live times
   * python makePlots.py