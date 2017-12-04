#!/usr/bin/python

import numpy as np
import math
import matplotlib.pyplot as plt
import matplotlib.ticker as mtick
from numpy import genfromtxt
from pandas import read_csv
from matplotlib.backends.backend_pdf import PdfPages
import getopt,sys


def main(argv):

    o_file = ''
    i_file = ''
    exp = 'KVH'

    
    try:
        opts,args = getopt.getopt(argv,"he:i:o:",["ifile=","help","ofile=","exp="])
    except getopt.GetoptError:
        print "USAGE:"
        print 'plot_att.py -i <estimatoroutputfile> -o <plotoutput>'
        sys.exit(2)
    for opt,arg in opts:
        if opt in ("-h","--help"):
            print "USAGE:"
            print 'plot_att.py -i <estimatoroutputfile> -o <plotoutput>'
            print "-i , --ifile : .KVH file input to attitude estimator."
            print "-o , --ofile : .csv file output of attitude estimator."
            sys.exit()
        elif opt in ("-i","--ifile"):
            i_file = arg
        elif opt in ("-o","--ofile"):
            o_file = arg
        elif opt in ("-e","--exp"):
            exp = arg

    print "LOADING FILE: " + i_file
    
    params = read_csv(i_file,nrows=1,header=None)
    data = read_csv(i_file,skiprows=1,header=None)

    print "LOADED FILE: " + i_file

    print "GENERATING PLOTS"

    print "SAVING TO: " + o_file
    pp = PdfPages(o_file)

    plt.figure(0)
    plt.axis('off')
    plt.text(0.5,0.7,"CONFIG PARAMS USED:\n",ha='center',va='center')
    plt.text(0.05,0.2,"  Date Modified: " + str(params.as_matrix()[0][1]) +
             "\n                    Hz: " + str(params.as_matrix()[0][2]) +
             "\n                  LAT: " + str(params.as_matrix()[0][3]) +
             "\n Processed File: " + str(params.as_matrix()[0][4]) +
             "\n           KVH File: " + str(params.as_matrix()[0][5]) +
             "\nAlignment(rpy): " + str(params.as_matrix()[0][6:9]) +
             "\n            Ro(rpy): " + str(params.as_matrix()[0][9:12]) +
             "\n                      k: " + str(params.as_matrix()[0][12:18]) + "\n")
    pp.savefig(plt.figure(0))
    plt.close("all")

    plt.figure(1)
    plt.suptitle('Estimated Attitude',y=0.99)
    plt.subplot(311)
    plt.plot(data.as_matrix([1])-data.as_matrix([1])[0],data.as_matrix([2])*180.0/math.pi,label=exp)
    plt.plot(data.as_matrix([1])-data.as_matrix([1])[0],data.as_matrix([5])*180.0/math.pi,label="PHINS")
    plt.ylabel('Roll (degrees)')
    plt.xlabel('Seconds (s)')
    plt.axis([0,data.as_matrix([1])[-1]-data.as_matrix([1])[0], -200, 200])
    plt.grid(True)
    plt.legend(bbox_to_anchor=(0., 1., 1., 1.), loc=3,ncol=2, mode="expand", borderaxespad=0.25, fontsize=12)
    plt.subplot(312)
    plt.plot(data.as_matrix([1])-data.as_matrix([1])[0],data.as_matrix([3])*180.0/math.pi,data.as_matrix([1])-data.as_matrix([1])[0],data.as_matrix([6])*180.0/math.pi)
    plt.ylabel('Pitch (degrees)')
    plt.xlabel('Seconds (s)')
    plt.axis([0,data.as_matrix([1])[-1]-data.as_matrix([1])[0], -200, 200])
    plt.grid(True)
    plt.subplot(313)
    plt.plot(data.as_matrix([1])-data.as_matrix([1])[0],data.as_matrix([4])*180.0/math.pi,data.as_matrix([1])-data.as_matrix([1])[0],data.as_matrix([7])*180.0/math.pi)
    plt.ylabel('Heading (degrees)')
    plt.xlabel('Seconds (s)')
    plt.axis([0,data.as_matrix([1])[-1]-data.as_matrix([1])[0], -200, 200])
    plt.grid(True)
    pp.savefig(plt.figure(1))
    plt.close("all")
    
    plt.figure(2)
    plt.suptitle('Estimated Attitude Error')
    plt.subplot(311)
    plt.plot(data.as_matrix([1])-data.as_matrix([1])[0],data.as_matrix([2])*180.0/math.pi-data.as_matrix([5])*180.0/math.pi)
    plt.ylabel('Roll (degrees)')
    plt.xlabel('Seconds (s)')
    plt.axis([0,data.as_matrix([1])[-1]-data.as_matrix([1])[0], -10, 10])
    plt.grid(True)
    plt.subplot(312)
    plt.plot(data.as_matrix([1])-data.as_matrix([1])[0],data.as_matrix([3])*180.0/math.pi-data.as_matrix([6])*180.0/math.pi)
    plt.ylabel('Pitch (degrees)')
    plt.xlabel('Seconds (s)')
    plt.axis([0,data.as_matrix([1])[-1]-data.as_matrix([1])[0], -10, 10])
    plt.grid(True)
    plt.subplot(313)
    plt.plot(data.as_matrix([1])-data.as_matrix([1])[0],data.as_matrix([4])*180.0/math.pi-data.as_matrix([7])*180.0/math.pi)
    plt.ylabel('Heading (degrees)')
    plt.xlabel('Seconds (s)')
    plt.axis([0,data.as_matrix([1])[-1]-data.as_matrix([1])[0], -30, 30])
    plt.grid(True)
    pp.savefig(plt.figure(2))
    plt.close("all")

    plt.figure(3)
    plt.suptitle('Angular Velocity Sensor Bias')
    plt.subplot(311)
    plt.plot(data.as_matrix([1])-data.as_matrix([1])[0],data.as_matrix([8]))
    plt.ylabel('x (radians/s)')
    plt.xlabel('Seconds (s)')
    plt.axis([0,data.as_matrix([1])[-1]-data.as_matrix([1])[0], -3./100000., 3./100000.])
    plt.ticklabel_format(axis='y', style='sci', scilimits=(-2, 2))
    plt.grid(True)
    plt.subplot(312)
    plt.plot(data.as_matrix([1])-data.as_matrix([1])[0],data.as_matrix([9]))
    plt.ylabel('y (radians/s)')
    plt.xlabel('Seconds (s)')
    plt.axis([0,data.as_matrix([1])[-1]-data.as_matrix([1])[0], -3./100000., 3./100000.])
    plt.ticklabel_format(axis='y', style='sci', scilimits=(-2, 2))
    plt.grid(True)
    plt.subplot(313)
    plt.plot(data.as_matrix([1])-data.as_matrix([1])[0],data.as_matrix([10]))
    plt.ylabel('z (radians/s)')
    plt.xlabel('Seconds (s)')
    plt.axis([0,data.as_matrix([1])[-1]-data.as_matrix([1])[0], -3./100000., 3./100000.])
    plt.ticklabel_format(axis='y', style='sci', scilimits=(-2, 2))
    plt.grid(True)
    pp.savefig(plt.figure(3))
    plt.close("all")

    plt.figure(4)
    plt.suptitle('Linear Accelerometer Sensor Bias')
    plt.subplot(311)
    plt.plot(data.as_matrix([1])-data.as_matrix([1])[0],data.as_matrix([14]))
    plt.ylabel('x (g)')
    plt.xlabel('Seconds (s)')
    plt.axis([0,data.as_matrix([1])[-1]-data.as_matrix([1])[0], -1./100., 1./100.])
    plt.ticklabel_format(axis='y', style='sci', scilimits=(-2, 2))
    plt.grid(True)
    plt.subplot(312)
    plt.plot(data.as_matrix([1])-data.as_matrix([1])[0],data.as_matrix([15]))
    plt.ylabel('y (g)')
    plt.xlabel('Seconds (s)')
    plt.axis([0,data.as_matrix([1])[-1]-data.as_matrix([1])[0], -1./100., 1./100.])
    plt.ticklabel_format(axis='y', style='sci', scilimits=(-2, 2))
    plt.grid(True)
    plt.subplot(313)
    plt.plot(data.as_matrix([1])-data.as_matrix([1])[0],data.as_matrix([16]))
    plt.ylabel('z (g)')
    plt.xlabel('Seconds (s)')
    plt.axis([0,data.as_matrix([1])[-1]-data.as_matrix([1])[0], -1./100., 1./100.])
    plt.ticklabel_format(axis='y', style='sci', scilimits=(-2, 2))
    plt.grid(True)
    pp.savefig(plt.figure(4))
    plt.close("all")

    plt.figure(5)
    plt.suptitle('q_tilde')
    plt.subplot(311)
    plt.plot(data.as_matrix([1])-data.as_matrix([1])[0],data.as_matrix([17])*180.0/math.pi)
    plt.ylabel('x (degrees)')
    plt.xlabel('Seconds (s)')
    plt.axis([0,data.as_matrix([1])[-1]-data.as_matrix([1])[0], -200., 200.])
    plt.grid(True)
    plt.subplot(312)
    plt.plot(data.as_matrix([1])-data.as_matrix([1])[0],data.as_matrix([18])*180.0/math.pi)
    plt.ylabel('y (degrees)')
    plt.xlabel('Seconds (s)')
    plt.axis([0,data.as_matrix([1])[-1]-data.as_matrix([1])[0], -200., 200.])
    plt.grid(True)
    plt.subplot(313)
    plt.plot(data.as_matrix([1])-data.as_matrix([1])[0],data.as_matrix([19])*180.0/math.pi)
    plt.ylabel('z (degrees)')
    plt.xlabel('Seconds (s)')
    plt.axis([0,data.as_matrix([1])[-1]-data.as_matrix([1])[0], -200., 200.])
    plt.grid(True)
    pp.savefig(plt.figure(5))
    plt.close("all")

    plt.figure(6)
    plt.suptitle('Gravity Estimate')
    plt.subplot(311)
    plt.plot(data.as_matrix([1])-data.as_matrix([1])[0],data.as_matrix([20]))
    plt.ylabel('x (g)')
    plt.xlabel('Seconds (s)')
    plt.axis([0,data.as_matrix([1])[-1]-data.as_matrix([1])[0], -1.2, 1.2])
    plt.grid(True)
    plt.subplot(312)
    plt.plot(data.as_matrix([1])-data.as_matrix([1])[0],data.as_matrix([21]))
    plt.ylabel('y (g)')
    plt.xlabel('Seconds (s)')
    plt.axis([0,data.as_matrix([1])[-1]-data.as_matrix([1])[0], -1.2, 1.2])
    plt.grid(True)
    plt.subplot(313)
    plt.plot(data.as_matrix([1])-data.as_matrix([1])[0],data.as_matrix([22]))
    plt.ylabel('z (g)')
    plt.xlabel('Seconds (s)')
    plt.axis([0,data.as_matrix([1])[-1]-data.as_matrix([1])[0], -1.2, 1.2])
    plt.grid(True)
    pp.savefig(plt.figure(6))
    plt.close("all")


    pp.close()

        
    
if __name__ == "__main__":
    main(sys.argv[1:])
