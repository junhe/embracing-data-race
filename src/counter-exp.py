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
b = 10000
length = 10
paras = {
    'nthreads'       :[1,2,4,8],
    'cnt_per_thread' :range(a, b, (b-a)/length),
    'mode'           :[0, 1],
    'delay_factor'   :[0] + [10**i for i in range(0,6)],
    'intra_delay'    :[0]
    }

#paras = {
    #'nthreads'       :[1, 2, 4, 8],
    #'cnt_per_thread' :range(1000, 10000, 1000),
    #'mode'           :[0, 1],
    #'delay_factor'   :[0],
    #'intra_delay'    :[0]
    #}

paralist = ParameterCominations(paras)
#print len(paralist)
paralist = paralist * 16
#print len(paralist)
random.shuffle(paralist) # for the principle of exp design
#print len(paralist)
#exit(0)

headerprinted = False
for conf in paralist:
    exe = './counter'
    cmd = [exe, conf['nthreads'], 
                conf['cnt_per_thread'], 
                conf['mode'],
                conf['delay_factor'],
                conf['intra_delay']
                ]
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



