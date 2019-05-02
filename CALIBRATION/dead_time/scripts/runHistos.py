#!/usr/bin/python

import sys, os

spec     = sys.argv[1]
runNums  = sys.argv[2]
edtmRate = float (sys.argv[3])

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
    return lX

runList = hyphen_range(runNums)

for index, run in enumerate(runList) :
    runMacro = 'scripts/makeHistos.C("%s", 1, %d, %.1f)' % (spec, run, edtmRate)
    runRoot  = 'root -l -b -q \'%s\'' % (runMacro)
    os.system(runRoot)
