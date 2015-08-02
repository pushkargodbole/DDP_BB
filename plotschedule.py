import matplotlib.pyplot as plt
from math import sqrt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.patches import FancyArrowPatch
from mpl_toolkits.mplot3d import proj3d
from numpy import arange, ones
import sys

class Arrow3D(FancyArrowPatch):
    def __init__(self, xs, ys, zs, *args, **kwargs):
        FancyArrowPatch.__init__(self, (0,0), (0,0), *args, **kwargs)
        self._verts3d = xs, ys, zs

    def draw(self, renderer):
        xs3d, ys3d, zs3d = self._verts3d
        xs, ys, zs = proj3d.proj_transform(xs3d, ys3d, zs3d, renderer.M)
        self.set_positions((xs[0],ys[0]),(xs[1],ys[1]))
        FancyArrowPatch.draw(self, renderer)

def plotschedule(schedulefile, mapfile, mapplotfile):
    mp = open(mapplotfile, 'r')
    m = open(mapfile, 'r')
    s = open(schedulefile, 'r')
    fig = plt.figure()
    ax = fig.gca(projection='3d')
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
        ax.text(float(elems[0]), float(elems[1])-0.4, 0., str(count))
        count+=1
        
    ax.set_xlim(min(mapx)-1, max(mapx)+1)
    ax.set_ylim(min(mapy)-1, max(mapy)+1)
    for i in range(len(map)):
        for j in range(len(map[i])):
            ax.plot([mapx[i], mapx[map[i][j]['node']]], [mapy[i], mapy[map[i][j]['node']]], 'k')
            ax.plot([mapx[i], mapx[map[i][j]['node']]], [mapy[i], mapy[map[i][j]['node']]], 'rx')
            #ax.text((mapx[i] + mapx[map[i][j]['node']])/2, (mapy[i] + mapy[map[i][j]['node']])/2-0.4, 0, '('+str(int(map[i][j]['length']))+')')
    cost = s.readline()
    mint = 1000.0
    maxt = 0
    minnode = 1000
    maxnode = 0
    ax.set_title("Flight scheduling : time evolution")
    ax.set_zlabel('time')
    ax.grid(b=True, which='major')
    colours = [ 'b', 'g', 'r', 'c', 'm', 'y', 'DarkOrange', 'ForestGreen', 'Indigo ', 'MediumAquaMarine', 'MidnightBlue', 'Teal', 'SteelBlue', 'LightPink']
    count = 0
    for line in s:
        elems = line.split(':')
        try:
            flight = int(elems[0].strip())
            elems = elems[1].split('-')
            nodes = []
            times = []
            x = []
            y = []
            for elem in elems:
                elem = elem.strip()
                elem = elem.split()
                node = int(elem[0].strip())
                time = float(elem[1].strip())
                if time > maxt: maxt = time
                if time < mint: mint = time
                nodes.append(node)
                times.append(time)
                x.append(mapx[node-1])
                y.append(mapy[node-1])
            for i in range(len(nodes)-1):
                a = Arrow3D([x[i], x[i+1]],[y[i], y[i+1]],[times[i], times[i+1]], mutation_scale=20, lw=1, arrowstyle="-|>", color=colours[count%len(colours)])
                ax.add_artist(a)
            ax.plot(x, y, times, 'o', color=colours[count%len(colours)], label='flight '+str(count+1))
            count+=1
        except ValueError:
            pass
    ax.set_zticks(arange(mint, maxt+1))
    ax.set_zlim(mint, maxt)
    ax.legend(loc='center left', bbox_to_anchor=(1, 0.5))
    plt.show()

if __name__ == "__main__":                
    if len(sys.argv) == 4:    
        plotschedule(sys.argv[1], sys.argv[2], sys.argv[3]) # Sol Map Mapplot
    else:
        print "3 arguments expected,", len(sys.argv)-1, "given."
