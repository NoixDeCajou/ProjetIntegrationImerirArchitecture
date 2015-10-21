from pprint import pprint
import math
from Dijkstra import Graph

__author__ = 'Nathan'

#import json

verticeList = list()
edgeList = list()


def getGraphe(jsonmap):

    print("in getGraphe")

    themap = jsonmap
    #pprint(themap)
    pprint("---------------------")
    # pprint(themap['areas'])
    # for area in themap['areas']:
    #    pprint("---------------------")
    #    pprint(area)
    for map in themap['areas']:
        for point in (map['map']['vertices']):
            verticeList.append([map['name'] + '.' + point['name'], point['x'], point['y']])
        for arc in (map['map']['streets']):
            for point in verticeList:
                if point[0] == (map['name'] + '.' + arc['path'][0]):
                    x1 = point[1]
                    y1 = point[2]
                if point[0] == (map['name'] + '.' + arc['path'][1]):
                    x2 = point[1]
                    y2 = point[2]
            edgeList.append([unicode(map['name'] + '.' + arc['path'][0]),
                             unicode(map['name'] + '.' + arc['path'][1]),
                             math.sqrt(math.pow(x2 - x1, 2) + math.pow(y2 - y1, 2))])
            #print([map['name'] + '.' + arc['path'][0], map['name'] + '.' + arc['path'][1],
            #       math.sqrt(math.pow(x2 - x1, 2) + math.pow(y2 - y1, 2))])

        for arc in (map['map']['bridges']):
            for point in verticeList:
                if point[0] == (map['name'] + '.' + arc['from']):
                    x1 = point[1]
                    y1 = point[2]
                if point[0] == (arc['to']['area'] + '.' + arc['to']['vertex']):
                    x2 = point[1]
                    y2 = point[2]
            edgeList.append(
                [unicode(map['name'] + '.' + arc['from']),
                 unicode(arc['to']['area'] + '.' + arc['to']['vertex']),
                 arc['weight']])
            #print([map['name'] + '.' + arc['from'], arc['to']['area'] + '.' + arc['to']['vertex'], arc['weight']])


    g = Graph()

    print("vertices")
    for vertex in verticeList:

        print (vertex)

        g.add_vertex(vertex[0])

        pass

    print("edges")
    for edge in edgeList:

        print (edge)

        g.add_edge(edge[0], edge[1], edge[2])

        pass

    return g

def getPoids(pointA, pointB):

    for edge in edgeList:

        if edge[0] == unicode(pointA) and edge[1] == unicode(pointB):
            print(edge[2])
            return edge[2]
        elif edge[0] == unicode(pointB) and edge[1] == unicode(pointA):
            print(edge[2])
            return edge[2]

    print "problem in getPoids"

    return 0
