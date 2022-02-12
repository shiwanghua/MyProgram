from math import radians, cos, sin, asin, sqrt

import matplotlib.pyplot as plt
import matplotlib
matplotlib.rcParams['font.family'] = 'SimHei'  # 将全局的字体设置为黑体

def Distance(lng1,lat1,lng2,lat2): #根据经纬度计算两点距离, 返回值单位是公里
    # lng1,lat1,lng2,lat2 = (120.12802999999997,30.28708,115.86572000000001,28.7427)
    lng1, lat1, lng2, lat2 = map(radians, [float(lng1), float(lat1), float(lng2), float(lat2)])  # 经纬度转换成弧度
    dlon = lng2 - lng1
    dlat = lat2 - lat1
    a = sin(dlat / 2) ** 2 + cos(lat1) * cos(lat2) * sin(dlon / 2) ** 2
    distance = 2 * asin(sqrt(a)) * 6371 * 1000  # 地球平均半径，6371km
    distance = round(distance / 1000, 3)
    return distance

# 10进制整数转换为RGB格式 #+6位十六进制数的字符串
def Hex_to_RGB(colorNumber):
    r = hex(int(colorNumber/65536) % 256 )[2:]   # [2:] 是为了去掉0x 两个字符
    g = hex(int(colorNumber/256)%256)[2:]
    b = hex(colorNumber%256)[2:]
    if(len(r)<2):   #  填充，保证2位数
        r='0'+r
    if (len(g) < 2):
        g='0'+g
    if (len(b) < 2):
        b='0'+b
    rgb = "#"+r+ g + b
    return rgb

# 画闪电路径, 带箭头围成圈
def route(x0, y0, x, y, plt, N_begin, N_end,color):
    plt_arrow(x0,y0,x[0],y[0],color,0.0001,0.0003,0.00015)
    for i in range(N_begin, N_end):
        x_begin, y_begin = x[i], y[i]
        x_end, y_end = x[i+1], y[i+1]
        plt_arrow(x_begin, y_begin, x_end, y_end, color,0.0001,0.0003,0.00015)

def plt_arrow(x_begin,y_begin,x_end,y_end,color,w,hw,hl):
    plt.arrow(x_begin, y_begin, x_end - x_begin, y_end - y_begin,
             length_includes_head=True,     # 增加的长度包含箭头部分
             width=w,head_width = hw, head_length =hl, fc=color, ec=color) # fc 是箭头颜色，ec是线的颜色


