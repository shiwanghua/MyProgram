# 为了让其他文件共享参数
BeforeTime = 1800      # 获取过去 BeforeTime 秒内的合理数据
TimeDifference = 300   # 相邻两个合理数据点之间的最大时间差
ValidDistance = 9    #不同时间点之间的最大有效距离
SValidDistance = 9    # 同一时间的数据点之间的最大有效距离
MaxSpeed = 0.03        # 最大风速，单位是 km/s
MinIntensity = 1500    # 最低强度阈值
MinNum = 200          # 单场雷暴合理数据点个数最少值
RowInterval = 1000     # 提取多场雷暴样本时单场雷暴原始数据点个数最少值（满足BeforeTime时间段内）
ECRatio = 10           # Error-Correct Ratio 不合理数据点个数/合理数据点个数 的值的最大值，即最大筛除比
MaxAnswerTimeNum = 10  # 答案集的时间点个数最大值
PredictDegree = 3      # 多项式拟合的次数
StopNum = 500         # 连续StopNum个不合理数据点就停止此轮迭代

# BeforeTime = 3600      # 获取过去 BeforeTime 秒内的合理数据
# TimeDifference = 300   # 相邻两个合理数据点之间的最大时间差
# ValidDistance = 10    #不同时间点之间的最大有效距离
# SValidDistance = 10    # 同一时间的数据点之间的最大有效距离
# MaxSpeed = 0.03        # 最大风速，单位是 km/s
# MinIntensity = 1500    # 最低强度阈值
# MinNum = 500          # 单场雷暴合理数据点个数最少值
# RowInterval = 1000     # 提取多场雷暴样本时单场雷暴原始数据点个数最少值（满足BeforeTime时间段内）
# ECRatio = 10           # Error-Correct Ratio 不合理数据点个数/合理数据点个数 的值的最大值，即最大筛除比
# MaxAnswerTimeNum = 10  # 答案集的时间点个数最大值
# PredictDegree = 5      # 多项式拟合的次数
# StopNum = 500         # 连续StopNum个不合理数据点就停止此轮迭代