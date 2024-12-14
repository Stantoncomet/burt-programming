import numpy as np
import numpy as np
from scipy.interpolate import make_interp_spline
import matplotlib.pyplot as plt 
 
x = np.array([])
y = np.array([])

intx = input("Press RET To Begin")
while intx != 'DONE':
    intx = input("X value?: ")

    if intx != 'DONE':
        x = np.append(x, int(intx))
        print(x)
 
inty = input("Press RET To Continue")
while inty != 'DONE':
    inty = input("Y value?: ")

    if inty != 'DONE':
        y = np.append(y, int(inty))
        print(y)

spline = make_interp_spline(x, y, bc_type='natural')
 
smooth_x = np.linspace(x.min(), x.max(), 500)
smooth_y = spline(smooth_x)
 
plt.plot(smooth_x, smooth_y)
plt.title("MATE Float Depth and Time")
plt.xlabel("Time (Seconds)")
plt.ylabel("Depth (Meters)")
plt.savefig("plot.png")
