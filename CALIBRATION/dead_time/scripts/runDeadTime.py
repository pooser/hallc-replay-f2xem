#!/usr/bin/python

import sys, os, re, pickle
import numpy as np
import ROOT as R

spec           = sys.argv[1]
runNums        = sys.argv[2]
edtmTdcCutLow  = float (sys.argv[3])
edtmTdcCutHigh = float (sys.argv[4])
trigTdcCutLow  = float (sys.argv[5])
trigTdcCutHigh = float (sys.argv[6])

def hyphen_range(s):
    """ Takes a range in form of "a-b" and generate a list of numbers between a and b inclusive.
    Also accepts comma separated ranges like "a-b,c-d,f" will build a list which will include
    Numbers from a to b, a to d and f"""
    s = "".join(s.split())
    r = set()
    for x in s.split(','):
        t = x.split('-')
        if len(t) not in [1,2]: raise SyntaxError("hash_range is given its arguement as "+s+" which seems not correctly formated.")
        r.add(int(t[0])) if len(t) == 1 else r.update(set(range(int(t[0]),int(t[1])+1)))
    l = list(r)
    l.sort()
    return l

dd = { }
dd['runList']          = hyphen_range(runNums)
dd['bcm4aCurrentMean'] = []
dd['bcm4aCurrentCut']  = []
dd['trigRateMean']     = []

for index, run in enumerate(dd['runList']) :
    rif = R.TFile('outRootFiles/%s_diagnosticHistosRun_%d.root' % (spec, run), 'read')
    
    bcm4aCurrentHisto  = rif.FindObjectAny('bcm4aCurrent')
    bcm4aCurrentCounts = np.array(bcm4aCurrentHisto)[:-2]
    bcm4aCurrentValues = np.linspace(bcm4aCurrentHisto.GetXaxis().GetXmin(), bcm4aCurrentHisto.GetXaxis().GetXmax() - bcm4aCurrentHisto.GetXaxis().GetBinWidth(1), num = bcm4aCurrentHisto.GetNbinsX())
    bcm4aCurrentMean   = bcm4aCurrentValues[np.argmax(bcm4aCurrentCounts)]
    bcm4aCurrentCut    = bcm4aCurrentMean - 2.0
   
    if (spec == 'hms')  : trigRateHisto  = rif.FindObjectAny('htrig1Rate')
    if (spec == 'shms') : trigRateHisto  = rif.FindObjectAny('ptrig1Rate')
    trigRateCounts = np.array(trigRateHisto)[:-2]
    trigRateValues = np.linspace(trigRateHisto.GetXaxis().GetXmin(), trigRateHisto.GetXaxis().GetXmax() - trigRateHisto.GetXaxis().GetBinWidth(1), num = trigRateHisto.GetNbinsX())
    trigRateMean   = trigRateValues[np.argmax(trigRateCounts)]

    dd['bcm4aCurrentMean'].append(bcm4aCurrentMean)
    dd['bcm4aCurrentCut'].append(bcm4aCurrentCut)
    dd['trigRateMean'].append(trigRateMean)

for index, run in enumerate(dd['runList']) :
    runMacro = 'scripts/deadTime.C("%s", 1, %d, %f,  %f,  %f,  %f,  %f)' % (spec, run, dd['bcm4aCurrentCut'][index], edtmTdcCutLow, edtmTdcCutHigh, trigTdcCutLow, trigTdcCutHigh)
    runRoot  = 'root -l -b -q \'%s\'' % (runMacro)
    os.system(runRoot)

dd['tlt']          = []
dd['tltErr']       = []
dd['clt']          = []
dd['cltErr']       = []
dd['elt']          = []
dd['eltErr']       = []
dd['cltEt']        = []
dd['cltEtErr']     = []
dd['eltEt']        = []
dd['eltEtErr']     = []
dd['cltNoEdtm']    = []
dd['cltNoEdtmErr'] = []
dd['eltNoEdtm']    = []
dd['eltNoEdtmErr'] = []

outFile = spec+'Output.txt'
with open(outFile) as search :
    for line in search :
        line = line.rstrip()
        if ('Total Live Time') in line : 
            data = line.split('+/-')
            dd['tlt'].append(float (re.findall('\d+\.\d+', data[0])[0]))
            dd['tltErr'].append(float (re.findall('\d+\.\d+', data[1])[0]))
        if ('Computer Live Time') in line and 'ET' not in line and 'No EDTM' not in line : 
            data = line.split('+/-')
            dd['clt'].append(float (re.findall('\d+\.\d+', data[0])[0]))
            dd['cltErr'].append(float (re.findall('\d+\.\d+', data[1])[0]))
        if ('Electronic Live Time') in line and 'ET' not in line and 'No EDTM' not in line : 
            data = line.split('+/-')
            dd['elt'].append(float (re.findall('\d+\.\d+', data[0])[0]))
            dd['eltErr'].append(float (re.findall('\d+\.\d+', data[1])[0]))
        if ('Computer Live Time ET') in line : 
            data = line.split('+/-')
            dd['cltEt'].append(float (re.findall('\d+\.\d+', data[0])[0]))
            dd['cltEtErr'].append(float (re.findall('\d+\.\d+', data[1])[0]))
        if ('Electronic Live Time ET') in line : 
            data = line.split('+/-')
            dd['eltEt'].append(float (re.findall('\d+\.\d+', data[0])[0]))
            dd['eltEtErr'].append(float (re.findall('\d+\.\d+', data[1])[0]))
        if ('Computer Live Time No EDTM') in line : 
            data = line.split('+/-')
            dd['cltNoEdtm'].append(float (re.findall('\d+\.\d+', data[0])[0]))
            dd['cltNoEdtmErr'].append(float (re.findall('\d+\.\d+', data[1])[0]))
        if ('Electronic Live Time No EDTM') in line : 
            data = line.split('+/-')
            dd['eltNoEdtm'].append(float (re.findall('\d+\.\d+', data[0])[0]))
            dd['eltNoEdtmErr'].append(float (re.findall('\d+\.\d+', data[1])[0]))

pickle.dump(dd, open('%sDataDict.pkl' % spec, 'wb'))
