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

# nthreads tablesize insertions mode
paras = {
    'nthreads'       :[1,2,4,8,16,32,64],
    'tablesize'      :[2**i for i in range(20)],
    'insertions'     :[10**i for i in range(1, 4)],
    'mode'           :[0, 1]
    }


paralist = ParameterCominations(paras)
paralist = paralist * 5
random.shuffle(paralist) # for the principle of exp design

headerprinted = False
for conf in paralist:
    exe = './hashtable'
    cmd = [exe, conf['nthreads'],   conf['tablesize'], 
                conf['insertions'], conf['mode']]
    cmd = [str(x) for x in cmd]
    #print cmd
    sys.stdout.flush()
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


