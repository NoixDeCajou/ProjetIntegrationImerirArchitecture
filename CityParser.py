from pprint import pprint
import math

__author__ = 'Nathan'

import json

verticeList=list()
edgeList=list()

with open('city.json') as data_file:
    themap = json.load(data_file)
    pprint(themap)
    pprint("---------------------")
    #pprint(themap['areas'])
    #for area in themap['areas']:
    #    pprint("---------------------")
    #    pprint(area)
    for map in themap['areas']:
        for point in (map['map']['vertices']):
            verticeList.append([map['name']+'.'+point['name'],point['x'],point['y']])
        for arc in (map['map']['streets']):
            for point in verticeList:
                if point[0]==(map['name']+'.'+arc['path'][0]):
                    x1=point[1]
                    y1=point[2]
                if point[0]==(map['name']+'.'+arc['path'][1]):
                    x2=point[1]
                    y2=point[2]


            dist=math.sqrt(math.pow(x2-x1,2)+math.pow(y2-y1,2))

            edgeList.append([map['name']+'.'+arc['path'][0],map['name']+'.'+arc['path'][1],dist])
            print([map['name']+'.'+arc['path'][0],map['name']+'.'+arc['path'][1],dist])