from drs import drs

from random import uniform
from random import random
from random import randint

import os


def mkdir(path):
    folder = os.path.exists(path)

    if not folder:
        os.makedirs(path)

def generateProcessor(n, Sm):
    upper_bound = [1 for x in range (0, n - 1)]
    lower_bound = [0 for x in range (0, n - 1)]
    processorSet = drs(n - 1, Sm - 1, upper_bounds=upper_bound, lower_bounds=lower_bound)
    processorSet.append(1)
    return processorSet



def generateTaskSet(Sm, UB, p_hu, u_max, P_min, p_hc, R_max):

    U_O = 0
    U_N = 0
    taskset = []
    bound1 = Sm * UB
    bound2 = Sm * UB - 0.1 * UB
    LO_count = 0
    while True:
        D_i = randint(10, 1000)
        T_i = D_i
        p_ui = random()
        if p_ui <= p_hu:
            C_iO = uniform(1.02, u_max) * T_i
        else:
            C_iO = uniform(0.02, 1) * T_i
        P_i = uniform(P_min, 20)
        L_iO = T_i / P_i

        p_ic = random()
        if p_ic <= p_hc:
            Z_i = 'HI'
            R_i = uniform(1, R_max)
        else:
            Z_i = 'LOW'
            R_i = 1

        if C_iO < L_iO:
            continue

        C_iN = C_iO / R_i
        L_iN = L_iO / R_i

        if Z_i == 'HI':
            U_O = U_O + (C_iO / T_i)
            U_N = U_N + (C_iN / T_i)
        else:
            U_N = U_N + (C_iN / T_i)
        if (U_O > bound1 or U_N > bound1):
            if Z_i == 'HI':
                U_O = U_O - (C_iO / T_i)
                U_N = U_N - (C_iN / T_i)
            else:
                U_N = U_N - (C_iN / T_i)
            continue
        else:
            tup = (Z_i, D_i, C_iN, C_iO, L_iN, L_iO)
            if Z_i == "LOW":
                LO_count = LO_count + 1
            taskset.append(tup)
            if (U_O > bound2 or U_N > bound2):
                break
    print(U_O / Sm, U_N / Sm)
    return taskset, LO_count


def storeResults(processorSet, taskSet, elastic_degree, filePath):
    with open(filePath, 'w') as f:
        f.write(str(len(processorSet)) + '\t' + str(len(taskSet)))
        f.write('\n')
        
        for i in range(0, len(processorSet)):
            f.write('processor{}'.format(i) + '\t' + str(processorSet[i]) + '\t')
            
        
        f.write('\n')

        
        count = 0
        for i in range(0, len(taskSet)):
            if (taskSet[i][0] == "LOW"):
                f.write('task{}'.format(i) + '\t' + str(taskSet[i][0]) + '\t' + str(taskSet[i][1]) + '\t' + str(taskSet[i][2]) + '\t' + str(taskSet[i][3]) + '\t' + str(taskSet[i][4]) + '\t' + str(taskSet[i][5]) + '\t' + str(elastic_degree[count]))
                count = count + 1
            else:
                f.write('task{}'.format(i) + '\t' + str(taskSet[i][0]) + '\t' + str(taskSet[i][1]) + '\t' + str(taskSet[i][2]) + '\t' + str(taskSet[i][3]) + '\t' + str(taskSet[i][4]) + '\t' + str(taskSet[i][5]) + '\t' + str(1))
            f.write('\n')

if __name__ == "__main__":
    # parameter setting
    n = 8
    Sm = 0.5 * n
    UB = 0.4  # max(U^N / S_m, U^O / S_m) 
    p_hu = 0.9 # probability of a task to be a high-utilization task
    u_max = 2.0 # upper bound on the overload utilization of a high-utilization task
    P_min =  5# the maximum ratio of overload critical-path length and the period
    p_hc = 0.5 # probability of a task to be a high-criticality task 
    R_max = 2.0 # the maximum ratio of overload and nominal utilizations
    average_omega = 10 # the average omega elastic degree
    
    folder = "./test/{}_{}_{}_{}_{}_{}_{}_{}_{}".format(n, Sm, UB, p_hu, u_max, P_min, p_hc, R_max, average_omega)
    mkdir(folder)
    for i in range(0, 1000):
        ProcessorSet = generateProcessor(n, Sm)

        taskSet, LO_number = generateTaskSet(Sm, UB, p_hu, u_max, P_min, p_hc, R_max)
        filename = 'test{}.test'.format(i)
        upper_bound = [average_omega + 2 for x in range (0, LO_number)]
        lower_bound = [2 for x in range (0, LO_number)]
        elastic_degree = drs(LO_number, average_omega * LO_number, upper_bounds=upper_bound, lower_bounds=lower_bound)
        path = folder + "/" + filename
        storeResults(ProcessorSet, taskSet, elastic_degree, path)
    
    '''
    ProcessorSet = generateProcessor(n, Sm)

    taskSet, LO_number = generateTaskSet(Sm, UB, p_hu, u_max, P_min, p_hc, R_max)
    filename = './{}_{}.test'.format(Sm, UB)
    upper_bound = [average_omega + 2 for x in range (0, LO_number)]
    lower_bound = [2 for x in range (0, LO_number)]
    elastic_degree = drs(LO_number, average_omega * LO_number, upper_bounds=upper_bound, lower_bounds=lower_bound)
    storeResults(ProcessorSet, taskSet, elastic_degree, filename)
    '''