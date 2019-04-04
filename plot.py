import numpy as np
import matplotlib.pyplot as plt

data1_x=[]
data1_y=[]
with open("log/1958/logCacheEnabled.txt", "r") as file:
    tmp_data = file.read()
    tmp_data = [int(x) for x in tmp_data.split('\n') if x != ""]
    for i in range(len(tmp_data)):
        data1_x.append(tmp_data[i] - tmp_data[0])
        data1_y.append(i)

data2_x=[]
data2_y=[]
with open("log/1958/logCacheDisabled.txt", "r") as file:
    tmp_data = file.read()
    tmp_data = [int(x) for x in tmp_data.split('\n') if x != ""]
    for i in range(len(tmp_data)):
        data2_x.append(tmp_data[i] - tmp_data[0])
        data2_y.append(i)

print(data1_x)
print(data1_y)

print(data2_x)
print(data2_y)

plt.plot(data1_x, data1_y, 'r--', data2_x, data2_y, 'k')
plt.show()
