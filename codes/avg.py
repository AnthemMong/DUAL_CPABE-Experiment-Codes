import os
import time

def sumlist(list1, list2):
    for i in range(len(list1)):
        list1[i] = list1[i] + list2[i]
        list1[i] = round(list1[i], 2)
    return list1

shellorder = input("bash:")
times = 100
filename = "data.txt"

def average(matrixlist):
    listsum = [0.0] * len(matrixlist[0])
    for i in range(len(matrixlist)):
        sumlist(listsum, matrixlist[i])
    
    for i in range(len(listsum)):
        listsum[i] = round(listsum[i] / len(matrixlist), 2)

    return listsum

if __name__ == "__main__":
    if os.path.exists(filename):
        os.remove(filename)
    os.mknod(filename)

    for i in range(times):
        os.system(shellorder + " >> " + filename)
        time.sleep(0.1)
    items = 0
    matrixlist = []
    delindex = []
    # with open(filename, 'r') as file:
    #     line = file.readline().strip()
    #     itemsum = [0.0]*len(line.split(", "))
    #     while line:
    #         items += 1
    #         linestr = line.split(", ")
    #         linenum = list(map(float, linestr))
    #         sumlist(itemsum, linenum)
    #         # print(itemsum)
    #         line = file.readline().strip()
    # print(average(itemsum, items))

    with open(filename, 'r') as file:
        line = file.readline().strip()
        itemsum = [0.0]*len(line.split(", "))
        while line:
            
            linestr = line.split(", ")
            linenum = list(map(float, linestr))
            matrixlist.append(linenum)
            items += 1
            line = file.readline().strip()
    avlist = average(matrixlist)
    # print("AVG", avlist)
    while True:
        for i in range(len(matrixlist)):
            for j in range(len(avlist)):
                minone = min(avlist[j], matrixlist[i][j])
                maxone = max(avlist[j], matrixlist[i][j])
                if minone == 0:
                    continue
                if avlist[j] < 1 and (maxone-minone)/minone >= 1/2:
                    if i not in delindex:
                        delindex.append(i)
                        print(matrixlist[i])
                elif avlist[j] > 1 and avlist[j] < 10 and (maxone-minone)/minone >= 1/3:
                    if i not in delindex:
                        delindex.append(i)
                        print(matrixlist[i])
                elif avlist[j] > 10 and avlist[j] < 50 and (maxone-minone)/minone >= 1/4:
                    if i not in delindex:
                            delindex.append(i)
                            print(matrixlist[i])
                elif avlist[j] > 50 and avlist[j] < 100 and (maxone-minone)/minone >= 1/5:
                    if i not in delindex:
                            delindex.append(i)
                            print(matrixlist[i]) 
                elif avlist[j] > 100 and (maxone-minone)/minone >= 1/5:
                    if i not in delindex:
                            delindex.append(i)
                            print(matrixlist[i]) 
        if len(delindex) > 0:
            counter = 0
            for k in delindex:
                matrixlist.pop(k-counter)
                counter += 1
            avlist = average(matrixlist)
            delindex.clear()
        else:
            break;
    print("AVG", avlist)
        

