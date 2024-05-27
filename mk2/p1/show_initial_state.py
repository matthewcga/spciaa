import matplotlib.pyplot as plt
import numpy as np

def check_point(x, y):
    circle1 = (x - 0.3)**2 + (y - 0.6)**2 <= 0.2**2
    cirlce2 = (x - 0.7)**2 + (y - 0.6)**2 <= 0.2**2
    line1 = y > 0.6233
    line_left = y >= -1.25*x + 0.75
    line_right = y >= 1.25*x - 0.5
    # compare all values and return or operation
    in_heart = ((circle1 | cirlce2) & line1) | (line_left & line_right & ~line1)
    return in_heart

# show the initial state on the screen using matplotlib
def show_initial_state():
    x = np.linspace(0, 1, 1000)
    y = np.linspace(0, 1, 1000)
    X, Y = np.meshgrid(x, y)
    Z = check_point(X, Y)
    plt.contourf(X, Y, Z, levels=[0, 0.5, 1], colors=['white', 'pink'])
    plt.show()

show_initial_state()
