import os
import random
import subprocess
import itertools
import pprint
import sys


def ParameterCominations(parameter_dict):
    """
    Get all the cominbation of the values from each key
    http://tinyurl.com/nnglcs9

    Input: parameter_dict={
                    p0:[x, y, z, ..],
                    p1:[a, b, c, ..],
                    ...}
    Output: [
             {p0:x, p1:a, ..},
             {..},
             ...
            ]
    """
    d = parameter_dict
    return [dict(zip(d, v)) for v in itertools.product(*d.values())]

# nthreads cnt_per_thread mode
a = 1000
b = 5000
length = 100
paras = {
    'nthreads'       :[1,2,4,8],
    'cnt_per_thread' :range(a, b, (b-a)/100),
    'mode'           :[0, 1]
    }


paralist = ParameterCominations(paras)
#print len(paralist)
paralist = paralist * 8 
#print len(paralist)
random.shuffle(paralist) # for the principle of exp design
#print len(paralist)
#exit(0)

headerprinted = False
for conf in paralist:
    exe = './counter'
    cmd = [exe, conf['nthreads'], conf['cnt_per_thread'], conf['mode']]
    cmd = [str(x) for x in cmd]
    #print cmd
    #exit(0)
    #subprocess.call(cmd, env=runenv)
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    proc.wait()
    for line in proc.stdout:
        if 'DATAROWMARKER' in line:
            print line,
        if 'MYHEADERROWMARKER' in line and headerprinted == False:
            print line,
            headerprinted = True
        sys.stdout.flush()



