#!/usr/bin/env python3
import argparse
import matplotlib.pyplot as plt
from matplotlib.patches import Circle
from matplotlib.collections import PatchCollection

parser = argparse.ArgumentParser()
parser.add_argument("infile")
args = parser.parse_args()

lines=open(args.infile).readlines()

poses={}
for l in lines:
    s = l.split(',')
    x=float(s[0])
    y=-float(s[1])
    c=float(s[2])
    if c not in poses:
        poses[c] = []
    poses[c].append((x,y))

xs=[]
ys=[]
for group_poses in poses.values():
    xs.extend([i[0] for i in group_poses])
    ys.extend([i[1] for i in group_poses])

patches=[]
for c, (group, pts) in zip(['white','green','blue','red'], poses.items()):
    patches.extend([Circle(pt, .085, color=c) for pt in pts])

p = PatchCollection(patches)

fig, ax = plt.subplots()
ax.add_collection(p)
ax.scatter(xs,ys)
ax.set_aspect('equal')
plt.show()
