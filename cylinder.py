#!/usr/bin/env python
"""keysight_convert.py v0.1 (written in python 2.7)

convert csv files to keysight/agilent 81150A binary waveforms
x values have to be floats between 0 and 1 (first point should start with x=0)
y values have to be floats between -1 and +1

if --normalize flag is on, input waveform will be automatically fit into the allowed value range

csv format:

<xval1>,<yval1>
<xval2>,<yval2>
<xval3>,<yval3>
...

example:

./keysight_convert.py example.csv

2016 by Michael Wiebusch
http://mwiebusch.de
"""
import numpy as np
from matplotlib import pyplot as plt
import argparse
import re
from mpl_toolkits.mplot3d import Axes3D


#def normalize(vec):
  #vec=vec-vec[0]
  #vec=vec/max(abs(vec))
  #return vec

def main():

  ## Parse command-line arguments
  #parser = argparse.ArgumentParser(usage=__doc__)
  ##parser.add_argument("-i","--input", help="input file (csv)")
  #parser.add_argument("filename", help="input file (csv)")
  #parser.add_argument("-o","--output", help="output binary file, otherwise input filename + .wfm extension will be used")
  #parser.add_argument("-n","--normalize", help="normalize input waveform to limits of binary format", action='store_true')
  #args = parser.parse_args()

  
  #outfile=infile_base+".wfm"
  
  #if args.output:
    #outfile=args.output

  #a = np.loadtxt(open(args.filename,"rb"),delimiter=",",skiprows=1)

  #print a
  
  point_counter=0
  
  phi_start=0
  phi_range=np.pi*2
 
 # quarter cell
  #y_min=-250
  #y_max=3250
  #z_min=-250
  #z_max=2750
  
  ##full cell
  y_min=-3250
  y_max=3250
  z_min=-2750
  z_max=2750
  
  ## half cell
  z_min=-250
  
  # some extra points in z
  #z_min=2750
  #z_max=3150
  
  # some extra points in y
  y_min=-1500
  y_max=-250
  
  
  r0 = 50
  
  dr1 = 100
  r1 = 650
  
  dr2 = 500
  r2 = 2450
  
  dr3 = 200
  r3 = 2750
  
  dr4 = 100
  r4 = 4500
  
  # some extra points in z
  dr4 = 150
  
  #radiae=np.concatenate( ( np.linspace(50,600,3) , np.linspace(,600,3)   ) , axis=0 ) 
  radiae = np.arange(r0,r1,dr1)
  radiae = np.hstack( (radiae, np.arange(r1,r2,dr2)) )
  radiae = np.hstack( (radiae, np.arange(r2,r3,dr3)) )
  radiae = np.hstack( (radiae, np.arange(r3,r4,dr4)) )
  radiae = np.hstack( (radiae, [2550]) )
  
  #for the xo scan pattern
  #radiae = [1150,2640]
  
  
  #radiae= np.linspace(0.5,10,10 ) 
  
  #upper_radial_distance=1
  radial_distance=0.5
  
  #y=np.zeros(points)
  #z=np.zeros(points)
  
  #z0 = 3100
  #y0 = 4025
  #x0 = 4100
  
  x0 = 3600
  y0 = 3743
  z0 = 4000
  
  new_x = -1399 # yz_vol_scan_87
  #~ new_x = -2000 # yz_vol_scan_88
  #~ new_x = -2700
  
  angle = 40 # deg
  angle_rad = angle/360.0*np.pi*2
  
  
  y=[]
  z=[]
  
  lab_x=[]
  lab_y=[]
  lab_z=[]
  
  for r in radiae:
    #points=np.floor(phi_range*r/radial_distance)
    points=11
    #if r < 5:
      #points=11
    if r < r4:
      points=32*4#+1
    if r < r3:
      points=24*4#+1
    if r < r2:
      points=12*4#+1
    if r < 1000:
      points=6*4#+1
    if r < r1:
      points=6*2#+1
    if r < 260:
      points=6#+1
   
    if r == 2550:
      points=32*4
      
    # some extra points in z
    #points=32*2
      
    # for the xo scan_pattern
    #if r == 2640:
      #points=12
    #if r == 1150:
      #points=12
      
    for i in range(0,points):
      #print i
      phi=phi_start + phi_range * i/(points)
      #new_x = 1305
      new_y = r*np.cos(phi)
      new_z = r*np.sin(phi)
      if (r==2550 and not( (abs(np.pi/2 -phi) < np.pi/6) or (abs(3*np.pi/2 -phi) < np.pi/6)   )):
        continue
      
      if new_y >= y_min and new_y <= y_max and new_z >= z_min and new_z <= z_max:
        y.append(new_y)
        z.append(new_z)
        point_counter+=1
        rot_x = x0 + new_x*np.cos(angle_rad) + new_y*np.cos(angle_rad+np.pi/2)
        rot_y = y0 + new_x*np.sin(angle_rad) + new_y*np.sin(angle_rad+np.pi/2)
        rot_z = z0 + new_z
        lab_x.append(rot_x)
        lab_y.append(rot_y)
        lab_z.append(rot_z)
        print str(int(rot_x))+" "+str(int(rot_y))+" "+str(int(rot_z))
  
  
  
  #### for the xo pattern 
  #### first diagonal
  #for i in np.linspace(-3000,3000,20):
    #new_x=0
    #new_y=i
    #new_z=i/3.0*2.5
    #if new_y >= y_min and new_y < y_max and new_z >= z_min and new_z < z_max:
      #y.append(new_y)
      #z.append(new_z)
      #point_counter+=1
      #rot_x = x0 + new_x*np.cos(angle_rad) + new_y*np.cos(angle_rad+np.pi/2)
      #rot_y = y0 + new_x*np.sin(angle_rad) + new_y*np.sin(angle_rad+np.pi/2)
      #rot_z = z0 + new_z
      #lab_x.append(rot_x)
      #lab_y.append(rot_y)
      #lab_z.append(rot_z)
      #print str(int(rot_x))+" "+str(int(rot_y))+" "+str(int(rot_z))
      
  #### other diagonal    
  #for i in np.linspace(-3000,3000,20):
    #new_x=0
    #new_y=i
    #new_z=-i/3.0*2.5
    #if new_y >= y_min and new_y < y_max and new_z >= z_min and new_z < z_max:
      #y.append(new_y)
      #z.append(new_z)
      #point_counter+=1
      #rot_x = x0 + new_x*np.cos(angle_rad) + new_y*np.cos(angle_rad+np.pi/2)
      #rot_y = y0 + new_x*np.sin(angle_rad) + new_y*np.sin(angle_rad+np.pi/2)
      #rot_z = z0 + new_z
      #lab_x.append(rot_x)
      #lab_y.append(rot_y)
      #lab_z.append(rot_z)
      #print str(int(rot_x))+" "+str(int(rot_y))+" "+str(int(rot_z))
    
  
  

  #y=np.linspace(0,9,10)
  #z=np.linspace(10,19,10)
  
  print "number of points: "+str(point_counter)

  fig = plt.figure()
  plt.plot(y,z,"o")
  plt.show()
  
  fig = plt.figure()
  ax = Axes3D(fig)
  
  ax.scatter(lab_x,lab_y,lab_z)
  ax.set_xlabel('x')
  ax.set_ylabel('y')
  ax.set_zlabel('z')
  plt.show()

  #no_points = x.size
    
   
if __name__ == "__main__":
  main()
