#!/usr/bin/env python3

from math import sin, cos, sqrt

rs = [0.085, 0.07, 0.035, 0.0165]
Ws = [0.14, 0.1054, 0.053, 0.030]
dt = 0.1

for r, W in zip(rs, Ws):
    min_delta = sqrt(3*r**2)
    lhs_1 = -min_delta*sin(2*dt/(15*W)) - W*cos(2*dt/(15*W)) + W
    lhs_2 = -min_delta*sin(2*dt/(10*W)) - W/2*cos(2*dt/(10*W)) + W/2
    print('{:.4f} {:.4f}'.format(lhs_1, lhs_2))
