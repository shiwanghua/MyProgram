from pandas import read_csv
from pandas.core.frame import DataFrame
from datetime import datetime
from datetime import timedelta
from DataSetClass import DataSet   # 导入自定义类
from Functions import Distance,Hex_to_RGB,route,plt_arrow
from AlgorithmParameter import *

import timeit  # 用于计算运行时间
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
plt.rcParams.update({'figure.max_open_warning': 0})
import matplotlib
matplotlib.rcParams['font.family'] = 'SimHei'  # 将全局的字体设置为黑体

names=['DDATETIME','LON','LAT','PLUSTYPE','PEAKURRENT','ICHEIGHT','SENSORS','MULTIPLTCITY']
filename1 = "2018.09.csv"
# f=open(filename1,encoding='UTF-8')
# data=read_csv(f,names=names)
# path = "../OriginalDataset/1.csv"
# data = read_csv(path,low_memory=False)[names]
DataSets = DataSet()

# BeforeTime = 3600      # 获取过去 BeforeTime 秒内的合理数据
# TimeDifference = 420   # 相邻两个合理数据点之间的最大时间差
# ValidDistance = 13    #不同时间点之间的最大有效距离
# SValidDistance = 10    # 同一时间的数据点之间的最大有效距离
# MaxSpeed = 0.03        # 最大风速，单位是 km/s
# MinIntensity = 2000    # 最低强度阈值
# MinNum = 1000          # 单场雷暴合理数据点个数最少值
# RowInterval = 2000     # 提取多场雷暴样本时单场雷暴原始数据点个数最少值（满足BeforeTime时间段内）
# ECRatio = 10           # Error-Correct Ratio 不合理数据点个数/合理数据点个数 的值的最大值，即最大筛除比
# MaxAnswerTimeNum = 15  # 答案集的时间点个数最大值
# PredictDegree = 5      # 多项式拟合的次数
# StopNum = 1000         # 连续StopNum个不合理数据点就停止此轮迭代

RealAnswerTimeNum = 0  # 实际提取的答案集的时间点个数
ValidDataSet = 0       # 是否get到合理的一个雷暴样本（单场雷暴数据点大于MinNum个）
ValidDataNum = 0       # 单场雷暴中的有效数据点个数
InvalidDataNum = 0     # 单场雷暴中的无效数据点个数
Error1Num = 0
Error2Num = 0
Error3Num = 0
ith_sample = 0

# 输入查询时间，提取实时的单场雷暴数据
def GetDataSetByInputTime(data:DataFrame)->(DataFrame,DataFrame):

    y,m,d,h,mi = map(int, input("请输入查询时间（年 月 日 时 分）：").split())
    nowTime = datetime(y,m,d,h,mi)

    #这样才能在子函数里修改全局变量
    global ValidDataNum,InvalidDataNum,ValidDataSet,MaxAnswerTimeNum,RealAnswerTimeNum
    ValidDataNum=0
    InvalidDataNum=0
    ValidDataSet = 0
    RowNum = len(data)
    l=0
    r=RowNum-1
    # 二分法查找当前时间所在的行，如果有多行，取最大值
    while l<r:
        mid=int(l+(r-l)/2)
        t = datetime.strptime(data[names[0]][mid], "%Y/%m/%d %H:%M")
        if(t>nowTime):
            r=mid-1
        elif(t<nowTime):
            l=mid+1
        else:
            l=mid
            break
    t = datetime.strptime(data[names[0]][l], "%Y/%m/%d %H:%M")
    if(t>nowTime):
        while(l>0 and t>nowTime):
            l -= 1
            t=datetime.strptime(data[names[0]][l], "%Y/%m/%d %H:%M")
    else:
        while(t<=nowTime):
            l += 1
            t = datetime.strptime(data[names[0]][l], "%Y/%m/%d %H:%M")
        l-=1


    copy_l = l
    # 此时 l 为最接近当前时间之前的合理数据点的行号
    # 获取下一时刻的数据点，即答案
    answer_i = l + 1  # 此时的待预测地点所在的行, l+1与l的时间一定不相同
    NowTime = datetime.strptime(data[names[0]][l], "%Y/%m/%d %H:%M") # 有可能数据集中没有输入的那个查询时间点
    AnswerBeginTime = datetime.strptime(data[names[0]][answer_i], "%Y/%m/%d %H:%M")
    AnswerEndTime = AnswerBeginTime + timedelta(seconds = (MaxAnswerTimeNum-1)*60)

    # 防止答案数据有断层，对答案时间段内的数据做检测，检测到一个与第 l 行的数据点匹配即可通过检测
    while(answer_i<RowNum):
        aTime=datetime.strptime(data[names[0]][answer_i], "%Y/%m/%d %H:%M")
        timeDif = (aTime - NowTime).seconds
        if(aTime>AnswerEndTime or timeDif>TimeDifference):
            break
        dis = Distance(data['LON'][answer_i], data['LAT'][answer_i], data['LON'][l], data['LAT'][l])
        if (dis <= ValidDistance and abs(data['PEAKURRENT'][answer_i]) >= MinIntensity
            and dis / timeDif <= MaxSpeed):  # 按道理这里的timeDif不可能为0
            ValidDataSet = 1 # 找到合理的一行
            break
        answer_i+=1

    if (ValidDataSet == 0):
        print("错误2: 答案数据集不合理（雷暴消失）。")
        DataSets.Error2Num+=1
        return (None, None)

    # 检测完毕，开始获取答案集数据
    RealAnswerTimeNum = 1
    list = [answer_i]  # 记录行号
    lasti = answer_i
    answer_i = answer_i + 1
    lastDifferentTime = datetime.strptime(data[names[0]][lasti], "%Y/%m/%d %H:%M")
    aTime = datetime.strptime(data[names[0]][answer_i], "%Y/%m/%d %H:%M")

    while (answer_i < RowNum and AnswerEndTime >= aTime):
        while(answer_i < RowNum and AnswerEndTime >= aTime):

            dis=Distance(data['LON'][lasti], data['LAT'][lasti], data['LON'][answer_i],data['LAT'][answer_i])
            timeDif=(aTime-lastDifferentTime).seconds # 还是有可能为0的

            if (timeDif == 0):
                vDis = SValidDistance # 相同时间的数据点最大合理距离
            else:
                vDis = ValidDistance

            if ( dis<= vDis and abs(data['PEAKURRENT'][answer_i]) >= MinIntensity
                and (timeDif == 0 or dis / timeDif <= MaxSpeed)):
                list.append(answer_i)
                lasti = answer_i
                if(aTime>lastDifferentTime):
                    lastDifferentTime = aTime
                    RealAnswerTimeNum += 1  # 得到一个不同的时间点就加一
                answer_i+=1
                aTime = datetime.strptime(data[names[0]][answer_i], "%Y/%m/%d %H:%M")
                break

            answer_i += 1
            aTime = datetime.strptime(data[names[0]][answer_i], "%Y/%m/%d %H:%M")

    AnswerData = data.loc[list]
    AnswerData.reset_index(drop=True, inplace=True)


    # 获取历史合理数据集
    list=[l]
    t=datetime.strptime(data[names[0]][l], "%Y/%m/%d %H:%M")
    EarliestTime = nowTime-timedelta(seconds = BeforeTime)  # 最早的合理时间，即收集  EarliestTime 到 nowTime 这段时间内的数据
    while(l>0 and EarliestTime<=t):
        l2 = l
        t2 = datetime.strptime(data[names[0]][l2], "%Y/%m/%d %H:%M")  # 后一个合理的数据点的时间
        # 寻找前一个合理的数据点, 这中间可能有多个不合理的数据点，直到找到一个合理的才跳出循环
        while(l>0):
            l -= 1
            t = datetime.strptime(data[names[0]][l], "%Y/%m/%d %H:%M")
            if(EarliestTime>t): # 超出了最早时间，停止搜索
                break

            dis = Distance(data['LON'][l2], data['LAT'][l2], data['LON'][l], data['LAT'][l])
            timeDif = (t2-t).seconds
            if (timeDif == 0):
                vDis = SValidDistance # 相同时间的数据点最大合理距离
            else:
                vDis = ValidDistance

            if(dis<=vDis and timeDif < TimeDifference and abs(data['PEAKURRENT'][l])>=MinIntensity and (timeDif==0 or  dis/timeDif<=MaxSpeed )):
                    list.insert(0, l)  # 插入头部，按时间顺序排列
                    break

    ValidDataNum = len(list)
    InvalidDataNum = copy_l-l-ValidDataNum
    if(ValidDataNum<MinNum):
        ValidDataSet = 0
        print("错误1：合理数据点个数不足（"+str(ValidDataNum)+"个)。")
        DataSets.Error1Num+=1
        return (None,None)
    elif(InvalidDataNum/ValidDataNum > ECRatio):
        ValidDataSet = 0
        print("错误3: 数据筛除比过大, 为"+str(round(InvalidDataNum/ValidDataNum,2))+"。")
        DataSets.Error3Num+=1
        return (None, None)
    else:
        ValidDataSet = 1
        ASampleData=data.loc[list]   # 获取前一段时间内合理的行数据
        ASampleData.reset_index(drop=True, inplace=True)

    return (ASampleData,AnswerData)

# 输入文件名，提取多场雷暴样本数据集
def GetDataSetsByFile(fileName):

    global DataSets,ValidDataNum,InvalidDataNum,ValidDataSet,ith_sample,Error1Num,Error2Num,Error3Num
    data = read_csv(fileName,low_memory=False)[names]
    RowNum = len(data)
    i = 0
    Endi = RowNum-RowInterval
    ASampleData= pd.DataFrame(names)
    AnswerData =  pd.DataFrame(names)
    while(i<Endi):
        i_copy = i+1 #  从1开始, 用于打印起始行号
        StartTime = datetime.strptime(data[names[0]][i], "%Y/%m/%d %H:%M")
        EndTime = StartTime+timedelta(seconds = BeforeTime)
        if(EndTime<datetime.strptime(data[names[0]][i+RowInterval], "%Y/%m/%d %H:%M")):
        #  原始数据点少于RowInterval
            i+=1
            continue

        # 尝试提取一个样本数据集
        copy_i = i  # 第一个合理数据点的行号，用于计算不合理数据点个数
        ith_sample+=1
        list = [i]
        InvalidDataNum = 0
        ValidDataSet = 0
        NextTime = StartTime # 初始化
        EndTime-=timedelta(seconds = 60*(MaxAnswerTimeNum-1)) # 留出一段时间的数据供构造答案集使用
        Stop=0 # 是否停止识别
        while(i<RowNum and Stop==0):
            lastTime = datetime.strptime(data[names[0]][i], "%Y/%m/%d %H:%M")
            l = i
            while(True):  # 找一个合理的数据点
                l = l + 1
                if (l == RowNum):
                    Stop=1
                    break
                NextTime = datetime.strptime(data[names[0]][l], "%Y/%m/%d %H:%M")
                # if(l-i>=StopNum):
                #     # 去掉当前时间点的数据，否则后面会出错
                #     while (l < RowNum and datetime.strptime(data[names[0]][l], "%Y/%m/%d %H:%M") == NextTime):
                #         l += 1
                #     Stop = 1
                #     break
                if(NextTime>EndTime):
                    Stop = 1
                    break

                dis = Distance(data['LON'][l], data['LAT'][l], data['LON'][i], data['LAT'][i])
                timeDif = (NextTime - lastTime).seconds
                if (timeDif == 0):
                    vDis = SValidDistance  # 相同时间的数据点最大合理距离
                else:
                    vDis = ValidDistance
                if (dis <= vDis and timeDif <= TimeDifference and abs(data['PEAKURRENT'][l]) >= MinIntensity
                    and (timeDif == 0 or dis / timeDif <= MaxSpeed)):
                        list.append(l)  # 找到一个合理数据点
                        i = l
                        break
                # if (timeDif > TimeDifference):
                #     Stop = 1
                #     break
        i = l
        ValidDataNum = len(list)
        InvalidDataNum = i-copy_i-ValidDataNum
        if (ValidDataNum < MinNum):
            ValidDataSet = 0
            print("第"+str(ith_sample)+"次样本识别（从文件"+fileName[19:]+"第"+str(i_copy)+"行到第"+ str(l) + "行）识别失败--错误1: 合理数据点个数不足（"+str(ValidDataNum)+"个)。")
            Error1Num+=1
            continue
        elif(InvalidDataNum/ValidDataNum > ECRatio):
            ValidDataSet = 0
            print("第"+str(ith_sample)+"次样本识别（从文件"+fileName[19:]+"第"+str(i_copy)+"行到第"+ str(l) + "行）识别失败--错误3: 筛除比过大，为"+str(round(InvalidDataNum/ValidDataNum,2))+"。")
            Error3Num += 1
            continue
        else:
            ASampleData = data.loc[list]  # 获取前一段时间内合理的行数据
            ASampleData.reset_index(drop=True, inplace=True)

        if(i==RowNum):
            break

        # 提取对应的答案集
        lastValidi=list[len(list)-1]      # 上一个合理的数据点
        lastTime = datetime.strptime(data[names[0]][lastValidi], "%Y/%m/%d %H:%M")
        AnswerBeginTime = datetime.strptime(data[names[0]][i], "%Y/%m/%d %H:%M")
        AnswerEndTime = AnswerBeginTime + timedelta(seconds=(MaxAnswerTimeNum - 1) * 60)
        while(i<RowNum):
            aTime = datetime.strptime(data[names[0]][i], "%Y/%m/%d %H:%M")
            timeDif=(aTime-lastTime).seconds # 这里不是与 i-1 比较，而是与lastValidi
            if (aTime > AnswerEndTime or timeDif > TimeDifference):
                break
            dis = Distance(data['LON'][i], data['LAT'][i], data['LON'][lastValidi], data['LAT'][lastValidi])
            if (dis <= ValidDistance and abs(data['PEAKURRENT'][i]) >= MinIntensity
                    and  dis / timeDif <= MaxSpeed):  # 按道理这里的timeDif不可能为0
                ValidDataSet = 1  # 找到合理的一行
                break
            i += 1
        if(ValidDataSet==0):
            print("第"+str(ith_sample)+"次样本识别（从文件"+fileName[19:]+"第"+str(i_copy)+"行到第"+ str(i) + "行）识别失败--错误2: 答案数据集不合理。")
            Error2Num += 1
            continue
        # 检测完毕，开始获取样本的答案集数据
        RealAnswerTimeNum = 1
        list = [i]  # 记录行号i
        lasti = i
        i = i + 1
        lastTime = datetime.strptime(data[names[0]][lasti], "%Y/%m/%d %H:%M")
        aTime = datetime.strptime(data[names[0]][i], "%Y/%m/%d %H:%M")
        while (i < RowNum and AnswerEndTime >= aTime):
            while(i< RowNum and AnswerEndTime >= aTime):

                dis = Distance(data['LON'][lasti], data['LAT'][lasti], data['LON'][i], data['LAT'][i])
                timeDif = (aTime - lastTime).seconds
                if (timeDif == 0):
                    vDis = SValidDistance  # 相同时间的数据点最大合理距离
                else:
                    vDis = ValidDistance
                if (dis <= vDis and abs(data['PEAKURRENT'][i]) >= MinIntensity
                          and (timeDif == 0 or dis / timeDif <= MaxSpeed)):
                    list.append(i)
                    lasti = i
                    if(lastTime<aTime):
                        lastTime = aTime
                        RealAnswerTimeNum+=1
                    i += 1
                    aTime = datetime.strptime(data[names[0]][i], "%Y/%m/%d %H:%M")
                    break
                i += 1
                aTime = datetime.strptime(data[names[0]][i], "%Y/%m/%d %H:%M")
        AnswerData = data.loc[list]
        AnswerData.reset_index(drop=True, inplace=True)

        DataSets.InsertData(ASampleData,AnswerData,ValidDataNum,InvalidDataNum,RealAnswerTimeNum)
        print("第"+str(ith_sample)+"次样本识别（从文件"+fileName[19:]+"第"+str(i_copy)+"行到第"+str(i)+"行）识别成功--第"+str(DataSets.SampleNum)+"个样本。")
        # DataSets.SimplifyDataSet()
        # DataSets.Predict(5)
        # DrawPointMap(DataSets.SampleNum-1, 3)

    DataSets.UpdateErrorParameter(Error1Num,Error2Num,Error3Num)
    DataSets.print()
    if(DataSets.SampleNum>0):
        ValidDataSet = 1 # 有样本数据集，确保可以画图
        del data,ASampleData,AnswerData

# 可视化单场雷暴数据
# type=1: 用线连成圈
# type=2: 同一时间发生的数据点只取中心点（平均值），将中心点连成线段
# type=3: 在type2的基础上加上预测数据点连成的线
def DrawPointMap(ith_sample,type):
    global  ValidDataSet,ValidDataNum,InvalidDataNum,DataSets
    if(ith_sample>=DataSets.SampleNum):
        print("样本号越界，数据集中没有第"+str(ith_sample+1)+"个样本的数据。")
        return
    if(ValidDataSet == 0):
        return
    data = DataSets.SampleData[ith_sample]
    ValidDataNum = DataSets.ParameterData['合理数据点数'][ith_sample]
    InvalidDataNum = DataSets.ParameterData['不合理数据点数'][ith_sample]
    AnswerData = DataSets.AnswerData[ith_sample]

    fig = plt.figure()
    plt.xlabel('经  度',fontsize=15)
    plt.ylabel('纬  度',fontsize=15)
    beginTime = datetime.strptime(data['DDATETIME'][0], "%Y/%m/%d %H:%M")
    plt.title('最近雷暴活动图('+str(beginTime.year)+'年'+str(beginTime.month)+'月'+str(beginTime.day)+'日)',fontsize=20)

    rowNum = data.shape[0] # 行数
    x=data['LON'][:rowNum]
    y=data['LAT'][:rowNum]
    z=data['PEAKURRENT'][:rowNum]
    t=data['DDATETIME'][:rowNum]
    x.reset_index(drop=True, inplace=True)
    y.reset_index(drop=True, inplace=True)
    z.reset_index(drop=True, inplace=True)
    t.reset_index(drop=True, inplace=True)
    # 要标记的点的坐标、大小及颜色
    i=0
    timeNumber =1
    colorNumber=1
    centerx=x[i]  # 当type等于2时，记录上一个时间雷暴中心的位置，用于连成线段
    centery=y[i]
    while(i<len(x)):
        aTime = datetime.strptime(t[i], "%Y/%m/%d %H:%M")
        xt=[x[i]]
        yt=[y[i]]
        size=[abs(z[i])/50]  # 根据强度设置数据点大小
        i+=1;
        while(i<len(x)):
            nextTime=datetime.strptime(t[i], "%Y/%m/%d %H:%M")
            if(nextTime==aTime):
                xt.append(x[i])
                yt.append(y[i])
                size.append(abs(z[i])/10)
                i+=1
            else:
                break
        col=Hex_to_RGB(colorNumber)
        if(type==1): # 同一时刻的数据点连成圈
            plt.scatter(xt, yt, color = col, s=size,marker='.', label='Time'+str(timeNumber)+aTime.strftime(' %H:%M'))
            route(xt[len(xt)-1], yt[len(xt)-1], xt, yt, plt, 0, len(xt) - 1, col)
            # plt.plot(xt, yt, color = col, ls='-',label='Time'+str(timeNumber)+aTime.strftime(' %H:%M'))
        elif(type==2 or type == 3):  # 同一时刻的数据点取平均值作为雷暴中心点
            x_avg=0
            y_avg=0
            z_avg=0
            for item in xt:
                x_avg+= item
            for item in yt:
                y_avg+=item
            for item in size:
                z_avg+= item
            x_avg=[x_avg/len(xt)]
            y_avg=[y_avg/len(yt)]
            z_avg=[z_avg/len(size)]
            plt.scatter(x_avg, y_avg, color = col, s=z_avg,marker='.', label='时间'+str(timeNumber)+aTime.strftime(' %H:%M'))
            plt_arrow(centerx,centery,x_avg[0],y_avg[0],col,0.0003,0.003,0.002) #0.001,0.005,0.005
            centerx=x_avg[0]
            centery=y_avg[0]
        timeNumber+=1
        colorNumber+= 9876543

    centerx_copy = centerx
    centery_copy = centery

    # 画答案数据点
    rowNum = AnswerData.shape[0]
    x0 = AnswerData['LON']
    y0 = AnswerData['LAT']
    z0 = AnswerData['PEAKURRENT'][:rowNum]
    t0 = AnswerData['DDATETIME'][:rowNum]
    AnswerTime = datetime.strptime(AnswerData[names[0]][0], "%Y/%m/%d %H:%M")
    i = 0
    count=1
    while (i < AnswerData.shape[0]):
        aTime = datetime.strptime(t0[i], "%Y/%m/%d %H:%M")
        xt = [x0[i]]
        yt = [y0[i]]
        size = [abs(z0[i]) / 10]  # 根据强度设置数据点大小
        i += 1
        while (i < rowNum):
            nextTime = datetime.strptime(t0[i], "%Y/%m/%d %H:%M")
            if (nextTime == aTime):
                xt.append(x0[i])
                yt.append(y0[i])
                size.append(abs(z0[i]) / 10)
                i += 1
            else:
                break
        if (type == 1):
            plt.scatter(xt, yt,  color='red', s=size,marker='*', label='待预测位置:' + str(count) + AnswerTime.strftime(' %H:%M'))
            # plt.plot(xt, yt, color='red', ls='-')
            route(xt[len(xt) - 1], yt[len(yt) - 1], xt, yt, plt, 0, len(xt) - 1, 'red')
        elif (type==2 or type==3):
            x_avg = 0
            y_avg = 0
            z_avg = 0
            for item in xt:
                x_avg += item
            for item in yt:
                y_avg += item
            for item in size:
                z_avg += item
            x_avg = [x_avg / len(xt)]
            y_avg = [y_avg / len(yt)]
            z_avg = [z_avg / len(size)/10] # 稍微大一点
            plt.scatter(x_avg, y_avg, color='red', s=z_avg, marker='*', label='待预测位置' +str(count)+': '+ aTime.strftime(' %H:%M'))
            plt_arrow(centerx, centery, x_avg[0], y_avg[0], 'red', 0.0005, 0.003, 0.002) # 0.001, 0.003, 0.003
            centerx = x_avg[0]
            centery = y_avg[0]
            count+=1

    # 画预测位置并连成线
    if(type==3):
        if(DataSets.PredictDone==0):
            print("未开始预测任务。")
            return
        preData = DataSets.PredictData[ith_sample]
        lastx=preData[names[1]][0]
        lasty=preData[names[2]][0]
        plt.scatter(lastx, lasty, color='black', s=abs(preData[names[4]][0])/50, marker='*', label='预测位置:' +
                    datetime.strptime(preData[names[0]][0], "%Y/%m/%d %H:%M").strftime(' %H:%M'))
        plt_arrow(centerx_copy, centery_copy, lastx, lasty, 'black', 0.001, 0.003, 0.003)
        for i_pre in range(1, preData.shape[0]):
            preTime=datetime.strptime(preData[names[0]][i_pre], "%Y/%m/%d %H:%M")
            plt.scatter(preData[names[1]][i_pre], preData[names[2]][i_pre], color='black',
                        s=abs(preData[names[4]][i_pre])/50, marker='*', label='预测位置:' + preTime.strftime(' %H:%M'))
            plt_arrow(lastx, lasty,preData[names[1]][i_pre], preData[names[2]][i_pre], 'black', 0.001, 0.003, 0.003)
            lastx=preData[names[1]][i_pre]
            lasty=preData[names[2]][i_pre]

    ax=plt.axis()
    plt.text(ax[1]-(ax[1]-ax[0])/3,ax[2]+2*(ax[3]-ax[2])/15,"合理数据点个数："+str(ValidDataNum))
    plt.text(ax[1] - (ax[1] - ax[0]) / 3, ax[2] + (ax[3] - ax[2]) / 15, "不合理数据点个数：" + str(InvalidDataNum))
    plt.legend() # 图例
    plt.show()

# 画强度值图
def drawPea(data):
    plt.xlabel('时间点', fontsize=15)
    plt.ylabel('强度值', fontsize=15)
    plt.title('强度图', fontsize=20)

    z = data['MULTIPLTCITY']  # MULTIPLTCITY   PEAKURRENT
    rowNum = data.shape[0]  # 行数
    x = np.arange(0, rowNum)
    plt.plot(x, z, color='red', ls='-')
    # plt.legend()  # 图例
    plt.show()

if __name__=='__main__':
    start = timeit.default_timer()

    # 做单场雷暴识别，然后针对单场雷暴进行预测
    # OneSampleDataset,AnswerData = GetDataSetByInputTime(data)
    # DataSets.InsertData(OneSampleDataset, AnswerData,ValidDataNum,InvalidDataNum,RealAnswerTimeNum)
    # DataSets.print()
    # DrawPointMap(0, 1)
    # DrawPointMap(0, 2)
    # DataSets.SimplifyDataSet()
    # DataSets.print()
    # DrawPointMap(0, 2)
    # DataSets.Predict(PredictDegree)
    # DataSets.Evaluate()
    # DrawPointMap(0, 3)
    # drawPea(OneSampleDataset)

    path = "../OriginalDataset/"
    fileFormat = ".csv"
    filename1 = "2018.09.csv"

    for i in range(1,10):
        filename = path+str(i)+fileFormat
        GetDataSetsByFile(filename)
    end = timeit.default_timer()
    print('样本集构造完毕--运行时长：' + str(end - start) + '秒')

    DataSets.SimplifyDataSet()
    end = timeit.default_timer()
    print('数据集化简完毕--运行时长：' + str(end - start) + '秒')

    DataSets.Predict(PredictDegree)
    end = timeit.default_timer()
    print('预测完毕--运行时长：' + str(end - start) + '秒')

    DataSets.Evaluate()
    end = timeit.default_timer()
    print('评估完毕--运行时长：' + str(end - start) + '秒')

    for i in range(DataSets.SampleNum):
        DrawPointMap(i, 3)
    end = timeit.default_timer()
    print('运行时长：' + str(end - start) + '秒')

    # cols = [x for i, x in enumerate(data.columns) if data.iat[0, i] == 3]
    # 利用enumerate对row0进行遍历，将含有数字3的列放入cols中