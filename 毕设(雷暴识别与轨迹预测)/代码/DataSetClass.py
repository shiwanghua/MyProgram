from pandas.core.frame import DataFrame
from datetime import datetime
from datetime import timedelta
from Functions import Distance
from AlgorithmParameter import MinIntensity,MaxAnswerTimeNum,SValidDistance
import pandas as pd
import numpy as np
import matplotlib
import copy # 用于深度拷贝

matplotlib.rcParams['font.family'] = 'SimHei'  # 将全局的字体设置为黑体
pd.set_option('display.unicode.ambiguous_as_wide',True) # 调整DataFrame的输出，不过还是不完美
pd.set_option('display.unicode.east_asian_width',True)

names=['DDATETIME','LON','LAT','PLUSTYPE','PEAKURRENT','ICHEIGHT','SENSORS','MULTIPLTCITY']
ed={'DDATETIME':[],'LON':[],'LAT':[],'PLUSTYPE':[],'PEAKURRENT':[],'ICHEIGHT':[],'SENSORS':[],'MULTIPLTCITY':[]}

dataSetParaNames=['合理数据点数','不合理数据点数','ECRatio','答案集数据点数','答案集时间点数'] # 数据集参数名
dpDF = {'合理数据点数':[], '不合理数据点数':[], 'ECRatio':[], '答案集数据点数':[], '答案集时间点数':[]}

resultNames=['预测时间','距离差','是否误报','是否漏报'] # 结果集的列名
resultDF = {'预测时间':[], '距离差':[],'是否误报':[],'是否漏报':[]}

class DataSet:

    SampleNum : int = 0    # 样本个数
    Error1Num : int = 0    # 数据点个数不够的结果出现的次数
    Error2Num : int = 0    # 答案数据集不合理的结果出现的次数
    Error3Num : int = 0    # 筛选比过大的结果出现的次数
    PredictDone : bool = 0 # 是否做了预测
    PredictScore = 0       # 预测分数
    IsSimplified = 0       # 数据集是否简化，简化后InvalidDataNum没变，ValidDataNum变小，二者的比不再是ECRatio
    SampleData = []        # 数组的每一个元素为一个样本数据集
    AnswerData = []        # 数组的每一个元素为对应的答案数据集
    PredictData = []       # 预测数据集
    ResultData = []        # 评估结果数据
    # ValidDataNum = []      # 筛选样本时得到的合理数据点个数，即样本大小
    # InvalidDataNum = []    # 筛选样本时筛除的不合理数据点个数
    # AnswerDataNum = []     # 答案数据集中的数据点个数
    # ECRatio = []           # 由于化简后不能用InvalidDataNum/ValidDataNum得到，故先保存起来
    # AnswerTimeNum = []     # 答案集中数据时间点的个数
    ParameterData : pd.DataFrame(dpDF)

    # 定义构造方法
    def __init__(self):
        self.SampleNum = 0
        self.Error1Num = 0
        self.Error2Num = 0
        self.Error3Num = 0
        self.IsSimplified = 0
        self.ParameterData = pd.DataFrame(dpDF,dtype='int')
        self.ParameterData[dataSetParaNames[2]] = self.ParameterData[dataSetParaNames[2]].astype('float')  # 左值也要取到列，debug了好久
        self.ParameterData.index.name='样本号'
        return

    def InsertData(self,ASampleDataSet:DataFrame, AnswerDataSet:DataFrame,validNum:int,inValidNum: int, realAnsTimeNum):
        if(ASampleDataSet is None or AnswerDataSet is None):  #  数据集提取失败调用此函数时可能为空
            return
        self.SampleNum += 1
        self.SampleData.append(ASampleDataSet)
        self.AnswerData.append(AnswerDataSet)
        # self.ValidDataNum.append(validNum)
        # self.InvalidDataNum.append(inValidNum)
        # self.ECRatio.append(round(inValidNum / validNum, 2))  # 此时validNum一定大于0
        # self.AnswerDataNum.append(len(AnswerDataSet))
        # self.AnswerTimeNum.append(realAnsTimeNum)
        insertRow = pd.DataFrame([[int(validNum), int(inValidNum),
                                   round(inValidNum / validNum, 2), int(len(AnswerDataSet)),
                                   int(realAnsTimeNum)]], columns=dataSetParaNames)
        self.ParameterData = self.ParameterData.append(insertRow, ignore_index=True)

    def UpdateErrorParameter(self,e1,e2,e3):
        self.Error1Num, self.Error2Num, self.Error3Num = e1, e2, e3

    def get(self,i:int)-> (DataFrame,DataFrame,int,int):
        if(i>=self.SampleNum or i < 0 ):
            return (None,None,-1,-1)
        return (self.SampleData[i],self.AnswerData[i],self.ParameterData[dataSetParaNames[0]][i],self.ParameterData[dataSetParaNames[1]][i])

    def print(self):
        print("\n样本个数："+str(self.SampleNum))
        print("错误1次数："+str(self.Error1Num))
        print("错误2次数：" + str(self.Error2Num))
        print("错误3次数：" + str(self.Error3Num)+'\n')

        # print("\n   样 本 号   ", end='    ')
        # for i in range(self.SampleNum):
        #     print(str(i + 1), end='     ')
        # print('\n\n合理数据点个数', end='   ')
        # for i in range(self.SampleNum):
        #     print(str(self.ValidDataNum[i]), end='  ')
        # print('\n\n不合理数据点个数', end=' ')
        # for i in range(self.SampleNum):
        #     print(str(self.InvalidDataNum[i]), end='  ')
        # print("\n\n   ECRatio 值  ", end='   ')
        # for i in range(self.SampleNum):
        #     print(str(self.ECRatio[i]), end='    ')
        # print("\n\n   答案集时间点个数  ", end='  ')
        # for i in range(self.SampleNum):
        #     print(str(self.AnswerTimeNum[i]), end='     ')
        # print("\n\n")

        print(self.ParameterData)
        print("\n")

    # 简化数据集
    def SimplifyDataSet(self):
        for i in range(self.SampleNum):
            # 将每场雷暴中同一时间发生的闪电数据取平均值
            sample=self.SampleData[i]
            new_sample=pd.DataFrame(ed)
            len = self.ParameterData[dataSetParaNames[0]][i]
            j = 0
            while(j<len):
                ATime = datetime.strptime(sample[names[0]][j], "%Y/%m/%d %H:%M")
                j_copy=j
                sum_PEAKURRENT = abs(sample[names[4]][j])
                sum_lon = sample[names[1]][j]#*sum_PEAKURRENT
                sum_lat = sample[names[2]][j]#*sum_PEAKURRENT
                sum_MULTIPLTCITY = sample[names[7]][j]
                j+=1
                while(j<len and ATime==datetime.strptime(sample[names[0]][j], "%Y/%m/%d %H:%M")):
                    absIntensity=abs(sample[names[4]][j])
                    sum_lon+=sample[names[1]][j]#*absIntensity
                    sum_lat+=sample[names[2]][j]#*absIntensity
                    sum_PEAKURRENT+=absIntensity
                    sum_MULTIPLTCITY+=sample[names[7]][j]
                    j+=1
                count=j-j_copy
                insertRow = pd.DataFrame([[sample[names[0]][j_copy], sum_lon/count, sum_lat/count,0, sum_PEAKURRENT/count, 0, 0, sum_MULTIPLTCITY/count]], columns=names)
                new_sample = new_sample.append(insertRow, ignore_index=True)
            self.SampleData[i]=new_sample
            self.ParameterData.loc[i,dataSetParaNames[0]] = new_sample.shape[0]

            answer = self.AnswerData[i]
            new_answer = pd.DataFrame(ed)
            len=answer.shape[0]
            j=0
            while(j<len):
                ATime = datetime.strptime(answer[names[0]][j], "%Y/%m/%d %H:%M")
                j_copy = j
                sum_PEAKURRENT = abs(answer[names[4]][j])
                sum_lon = answer[names[1]][j]#*sum_PEAKURRENT
                sum_lat = answer[names[2]][j]#*sum_PEAKURRENT
                sum_MULTIPLTCITY = answer[names[7]][j]
                j += 1
                while (j < len and ATime == datetime.strptime(answer[names[0]][j], "%Y/%m/%d %H:%M")):
                    absIntensity = abs(answer[names[4]][j])
                    sum_lon += answer[names[1]][j]#*absIntensity
                    sum_lat += answer[names[2]][j]#*absIntensity
                    sum_PEAKURRENT += absIntensity
                    sum_MULTIPLTCITY += answer[names[7]][j]
                    j+=1
                count=j-j_copy
                insertRow = pd.DataFrame([[answer[names[0]][j_copy], sum_lon/count, sum_lat/count,0, sum_PEAKURRENT/count, 0, 0, sum_MULTIPLTCITY/count]], columns=names)
                new_answer=new_answer.append(insertRow, ignore_index=True)
            self.AnswerData[i] = new_answer
            self.ParameterData.loc[i,dataSetParaNames[3]] = new_answer.shape[0]

        self.IsSimplified=1
        print("数据集化简完毕。\n")

    # 计算中心位置时考虑强度
    def SimplifyDataSetnew(self):
        for i in range(self.SampleNum):
            # 将每场雷暴中同一时间发生的闪电数据取平均值
            sample=self.SampleData[i]
            new_sample=pd.DataFrame(ed)
            len = self.ParameterData[dataSetParaNames[0]][i]
            j = 0
            while(j<len):
                ATime = datetime.strptime(sample[names[0]][j], "%Y/%m/%d %H:%M")
                j_copy=j
                sum_PEAKURRENT = abs(sample[names[4]][j])
                sum_lon = sample[names[1]][j]*sum_PEAKURRENT
                sum_lat = sample[names[2]][j]*sum_PEAKURRENT
                sum_MULTIPLTCITY = sample[names[7]][j]
                j+=1
                while(j<len and ATime==datetime.strptime(sample[names[0]][j], "%Y/%m/%d %H:%M")):
                    absIntensity=abs(sample[names[4]][j])
                    sum_lon+=sample[names[1]][j]*absIntensity
                    sum_lat+=sample[names[2]][j]*absIntensity
                    sum_PEAKURRENT+=absIntensity
                    sum_MULTIPLTCITY+=sample[names[7]][j]
                    j+=1
                count=j-j_copy
                insertRow = pd.DataFrame([[sample[names[0]][j_copy], sum_lon/sum_PEAKURRENT, sum_lat/sum_PEAKURRENT,0, sum_PEAKURRENT/count, 0, 0, sum_MULTIPLTCITY/count]], columns=names)
                new_sample = new_sample.append(insertRow, ignore_index=True)
            self.SampleData[i]=new_sample
            self.ParameterData.loc[i,dataSetParaNames[0]] = new_sample.shape[0]

            answer = self.AnswerData[i]
            new_answer = pd.DataFrame(ed)
            len=answer.shape[0]
            j=0
            while(j<len):
                ATime = datetime.strptime(answer[names[0]][j], "%Y/%m/%d %H:%M")
                j_copy = j
                sum_PEAKURRENT = abs(answer[names[4]][j])
                sum_lon = answer[names[1]][j]*sum_PEAKURRENT
                sum_lat = answer[names[2]][j]*sum_PEAKURRENT
                sum_MULTIPLTCITY = answer[names[7]][j]
                j += 1
                while (j < len and ATime == datetime.strptime(answer[names[0]][j], "%Y/%m/%d %H:%M")):
                    absIntensity = abs(answer[names[4]][j])
                    sum_lon += answer[names[1]][j]*absIntensity
                    sum_lat += answer[names[2]][j]*absIntensity
                    sum_PEAKURRENT += absIntensity
                    sum_MULTIPLTCITY += answer[names[7]][j]
                    j+=1
                count=j-j_copy
                insertRow = pd.DataFrame([[answer[names[0]][j_copy], sum_lon/sum_PEAKURRENT, sum_lat/sum_PEAKURRENT,0, sum_PEAKURRENT/count, 0, 0, sum_MULTIPLTCITY/count]], columns=names)
                new_answer=new_answer.append(insertRow, ignore_index=True)
            self.AnswerData[i] = new_answer
            self.ParameterData.loc[i,dataSetParaNames[3]] = new_answer.shape[0]

        self.IsSimplified=1
        print("数据集化简完毕。\n")

    def Predict(self, degree:int): # 多项式拟合的次数 预测未来predictNum个时间的位置
        if (self.IsSimplified!=1) :
            print("请先化简数据集。\n")  # 进而得到答案集中的时间点个数
            return

        for i in range(self.SampleNum):
            # predictNum = int(self.ParameterData[dataSetParaNames[4]][i])   # 之前是答案集中有这么多个预测点就预测这么多个数据
            dataNum    = int(self.ParameterData[dataSetParaNames[0]][i])

            x = np.arange(1, dataNum + 1)
            # sampleTime = self.SampleData[i][names[0]]
            # beginTime = datetime.strptime(sampleTime[0], "%Y/%m/%d %H:%M")
            # x=[1]
            # for j in range(1,dataNum):
            #     jTime = datetime.strptime(sampleTime[j], "%Y/%m/%d %H:%M")
            #     x.append((jTime-beginTime).seconds/60+1)

            xp = np.arange(1, dataNum +1 + MaxAnswerTimeNum )   #  用于预测的 xp，在x结尾增加predictNum个位置
            # xp=copy.deepcopy(x)
            # for j in range(0,MaxAnswerTimeNum):
            #     xp.append(xp[-1]+1)

            lon = self.SampleData[i][names[1]]
            fun_lon  = np.polyfit(x, lon, degree)    #  拟合出多项式各次项的系数
            poly_lon = np.poly1d(fun_lon)              #  生成预测多项式
            lon_vals = poly_lon(xp)  # 拟合经度值值
            lon_vals = lon_vals[dataNum:]      # 前面dataNum个是拟合出的值，这里没用到

            lat = self.SampleData[i][names[2]]
            fun_lat = np.polyfit(x, lat, degree)
            poly_lat = np.poly1d(fun_lat)
            lat_vals = poly_lat(xp)
            lat_vals = lat_vals[dataNum:]

            pea = self.SampleData[i][names[4]]  # 强度
            fun_pea = np.polyfit(x, pea, degree)  # 拟合出多项式各次项的系数
            poly_pea = np.poly1d(fun_pea)  # 生成预测多项式
            pea_vals = poly_pea(xp)  # 拟合经度值值
            pea_vals = pea_vals[dataNum:]

            predictData = pd.DataFrame(ed)
            predictTime = datetime.strptime(self.SampleData[i][names[0]][dataNum-1], "%Y/%m/%d %H:%M")
            for j in range(0,MaxAnswerTimeNum):
                predictTime=predictTime+timedelta(seconds=60)
                insertRow = pd.DataFrame([[predictTime.strftime('%Y/%m/%d %H:%M'), lon_vals[j], lat_vals[j], 0, pea_vals[j], 0, 0, 0]], columns=names)
                predictData=predictData.append(insertRow, ignore_index=True)
            self.PredictData.append(predictData)
        print("预测完毕。\n\n")
        self.PredictDone=1

    def Evaluate(self):

        if (self.SampleNum == 0):
            print("无样本数据，评估结束。")
            return

        for i in range(self.SampleNum):
            miss = 0  # 漏报个数
            wrong = 0  # 误报个数
            resultData = pd.DataFrame(resultDF,dtype='int')
            resultData[resultNames[1]] = resultData[resultNames[1]].astype('float')
            answerData=self.AnswerData[i]
            answert_i=0 # 用于遍历i
            predictData=self.PredictData[i]
            nowTime=datetime.strptime(self.SampleData[i][names[0]][self.ParameterData[dataSetParaNames[0]][i]-1], "%Y/%m/%d %H:%M")
            futureTime = nowTime
            for j in range(0,MaxAnswerTimeNum):
                futureTime+=timedelta(seconds = 60)
                isMiss = 0  # 是否漏报
                isWrong = 0  # 是否误报
                if(answert_i<self.ParameterData[dataSetParaNames[3]][i] and
                   futureTime==datetime.strptime(answerData[names[0]][answert_i], "%Y/%m/%d %H:%M")
                   and abs(answerData[names[4]][answert_i])>=MinIntensity):  #  此时存在闪电
                    if(abs(predictData[names[4]][j])<MinIntensity): # 预测不存在，即漏报
                        isMiss = 1
                        miss+=1
                        disDiff=-1 # 无效距离
                    else:
                        # 计算距离差和强度差
                        disDiff = Distance(answerData[names[1]][answert_i], answerData[names[2]][answert_i],
                                           predictData[names[1]][j], predictData[names[2]][j])
                        if (disDiff <= SValidDistance):
                            self.PredictScore += 1
                    answert_i+=1
                else: # 此时不存在闪电
                    if(abs(predictData[names[4]][j])>=MinIntensity):  # 预测存在，即误报
                        isWrong = 1
                        wrong+=1
                        disDiff = -1  # 无效距离
                    else: # 准确预测
                        disDiff = 0
                        self.PredictScore += 1

                insertRow = pd.DataFrame([[(futureTime-nowTime).seconds,disDiff,isWrong,isMiss]], columns=resultNames)
                resultData = resultData.append(insertRow, ignore_index=True)

            self.ResultData.append(resultData)
            print("样本 "+str(i+1)+"：")
            print("漏报："+str(miss)+"  误报："+str(wrong))
            print(resultData)
            print('\n',end='')

        miss = 0   # 前score分钟的漏报个数
        wrong = 0  # 前score分钟的误报个数
        right = 0  # 前score分钟的准确预报个数（还有一种是实际有雷暴、预测有雷暴但是距离不合格）
        score = int(self.PredictScore/self.SampleNum)
        if(score==0):
            score+=1
        for i in range(self.SampleNum):
            for j in range(score):
                if(self.ResultData[i][resultNames[2]][j]==1):
                    wrong+=1
                elif(self.ResultData[i][resultNames[3]][j]==1):
                    miss+=1
                elif(self.ResultData[i][resultNames[1]][j]<=SValidDistance): # 这时不可能为-1了
                    right+=1

        print("预测总分："+str(self.PredictScore))
        print("样本个数："+str(self.SampleNum))
        print("平均分："+str(round(self.PredictScore/self.SampleNum,2)))
        pointNum = self.SampleNum*score
        print("准确率：" + str(round((right) / pointNum, 2)))
        print("误报率：" + str(round((wrong) / pointNum, 2)))
        print("漏报率：" + str(round((miss) / pointNum, 2)))





