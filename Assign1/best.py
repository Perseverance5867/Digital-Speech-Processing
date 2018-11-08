# platform: python3 MacOS
from numpy import loadtxt
import numpy as np
import matplotlib.pyplot as plt

bestset = loadtxt("cmp.txt")
print (bestset)
x = bestset[0:50, 1:2] * 100
y = bestset[0:50, 0:1]

num = 0
lar = -1
for N in range(50):
	if bestset[N][0] >= lar:
		num = (N + 1) * 100
		lar = bestset[N][0]

print ("The iteration with the best accuracy is:", num)
print ("The accuracy of the iteration above is:", lar)
plt.title('Iteration V.S. Accuracy')
plt.xlabel('Iteration')
plt.ylabel('Accuracy')
plt.plot(x, y)
plt.show()
