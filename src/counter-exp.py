import os
import random
import subprocess
import itertools
import pprint



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
paras = {
    'nthreads'       :[1,2,4,8,16],
    'cnt_per_thread' :[10**i for i in range(6,9)],
    'mode'           :[0, 1]
    }


paralist = ParameterCominations(paras)
paralist = paralist * 5
random.shuffle(paralist) # for the principle of exp design

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



