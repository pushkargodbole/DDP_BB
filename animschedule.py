import matplotlib.pyplot as plt
from numpy import arange
from time import sleep
from plotschedule import plotschedule
from math import ceil
import sys

def animschedule(mapfile, mapplotfile, schedulefile, flightfile):
    mp = open(mapplotfile, 'r')
    m = open(mapfile, 'r')
    s = open(schedulefile, 'r')
    f = open(flightfile, 'r')
    plt.ion()
    fig = plt.figure(figsize=(6*3.13,3*3.13))
    ax = fig.add_subplot(111)
    #plot, = ax.plot(x, rho)
    map = []
    for line in m:
        line = line.strip()
        elems = line.split()
        mapnode = []
        for i in range(0, len(elems), 3):
            mapnode.append({'node':int(elems[i]), 'length':float(elems[i+1])})
        map.append(mapnode)
    mapx = []
    mapy = []
    count = 1
    for line in mp:
        line = line.strip()
        elems = line.split()
        mapx.append(float(elems[0]))
        mapy.append(float(elems[1]))
        ax.annotate(count, (float(elems[0]), float(elems[1])-0.3))
        count+=1
        
    plt.xlim(min(mapx)-1, max(mapx)+1)
    plt.ylim(min(mapy)-1, max(mapy)+1)
    for i in range(len(map)):
        for j in range(len(map[i])):
            ax.plot([mapx[i], mapx[map[i][j]['node']]], [mapy[i], mapy[map[i][j]['node']]], 'b')
            ax.annotate('('+str(int(map[i][j]['length']))+')', ((mapx[i] + mapx[map[i][j]['node']])/2, (mapy[i] + mapy[map[i][j]['node']])/2-0.3))
            ax.plot([mapx[i], mapx[map[i][j]['node']]], [mapy[i], mapy[map[i][j]['node']]], 'rx')
    
    schedule = []
    tmax = 0;     
    cost = s.readline()
    speed = []
    for line in f:
        elems = line.split()
        speed.append(float(elems[3]))
        
    for line in s:
        elems = line.split(':')
        try:
            flight = int(elems[0].strip())
            elems = elems[1].split('-')
            nodes = []
            times = []
            for elem in elems:
                elem = elem.strip()
                elem = elem.split()
                node = int(elem[0].strip())-1
                time = float(elem[1].strip())
                if time > tmax : tmax = time
                nodes.append(node)
                times.append(time)
            schedule.append({'nodes':nodes, 'times':times})
        except ValueError:
            pass    
    flightsx = [mapx[schedule[i]['nodes'][0]] for i in range(len(schedule))]
    flightsy = [mapy[schedule[i]['nodes'][0]] for i in range(len(schedule))]
    flights = ['F'+str(i+1) for i in range(len(schedule))]
    plot, = ax.plot(flightsx, flightsy, 'g^', ms=10)
    fig.canvas.draw()
    activenode = [0 for i in range(len(schedule))]
    
    dt = 0.05
    for ti in arange(0, tmax, dt):
        for i in range(len(schedule)):
            flight = schedule[i]
            #print 'Bang!'
            #print i, ':', ti, activenode[i]
            if activenode[i] < len(flight['nodes'])-2:
                if ti >= flight['times'][activenode[i]+1]:
                    activenode[i] += 1
                    
            if ti < flight['times'][activenode[i]+1]:
                for connect in map[flight['nodes'][activenode[i]]]:
                    #print 'dong'
                    if connect['node'] == flight['nodes'][activenode[i]+1]:
                        length = connect['length']
                #print i, ':', ti, flight['times'][activenode[i]]        
                if ti - flight['times'][activenode[i]] < ceil(length/speed[i]) and ti - flight['times'][activenode[i]] >= 0:
                    movex = (mapx[flight['nodes'][activenode[i]+1]] - mapx[flight['nodes'][activenode[i]]])*dt/ceil(length/speed[i])
                    movey = (mapy[flight['nodes'][activenode[i]+1]] - mapy[flight['nodes'][activenode[i]]])*dt/ceil(length/speed[i])
                    flightsx[i] += movex
                    flightsy[i] += movey
                #print i, flightsx[i], flightsy[i]
        
        plot.set_xdata(flightsx)
        plot.set_ydata(flightsy)
        ann = {}
        for i in range(len(flights)):
            ann[i] = ax.annotate(flights[i], (flightsx[i]+0.8,flightsy[i]+0.8))
        ax.set_title('t = '+str(round(ti+dt, 2)), horizontalalignment='left')
        fig.canvas.draw()
        for i in range(len(ann)):
            ax.texts.remove(ann[i])
    ann = {}
    for i in range(len(flights)):
        ann[i] = ax.annotate(flights[i], (flightsx[i]+0.8,flightsy[i]+0.8))
    plt.ioff()
    plt.show()
    
if __name__ == "__main__":
    if len(sys.argv) == 5:    
        animschedule(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4]) # Map Mapplot Sol Flights
        plotschedule(sys.argv[3], sys.argv[1], sys.argv[2]) # Sol
    else:
        print "4 arguments expected,", len(sys.argv)-1, "given."
