'''
The MIT License (MIT)
Copyright (c) 2013 Dave P.
'''
import json
from pprint import pprint

import signal, sys, ssl
import string
# from ServiceWeb import handleMsg
from threading import Thread
from time import sleep
import CityParser
import Dijkstra
from SimpleWebSocketServer import WebSocket, SimpleWebSocketServer, SimpleSSLWebSocketServer
from optparse import OptionParser

clients = []

idRequestMax = 2

rootObject = ""

graphMap = ""

theShortestPath = []


class SimpleChat(WebSocket):
    def handleMessage(self):
        # for client in list(clients):
        # if client != self:
        #    client.sendMessage(self.address[0] + ' - ' + self.data)

        print("in handleMessage")

        broadcast(self.address[0] + ' - ' + self.data)
        # broadcast("test broadcast on handleMessage")

        messageReceived(self.data)

    def handleConnected(self):
        print self.address, 'connected'
        # for client in list(clients):
        #    client.sendMessage(self.address[0] + u' - connected')
        clients.append(self)

    def handleClose(self):
        clients.remove(self)
        print self.address, 'closed'
        # for client in list(clients):
        #    client.sendMessage(self.address[0] + u' - disconnected')


def broadcast(message):
    for client in clients:
        print("in the for of broadcast")
        # send___(client, unicode(message))
        # client.sendMessage(unicode(message))
        send___(client, unicode(message))


def send___(client, msg):
    print("in send")
    client.sendMessage(msg)


def messageReceived(msg):
    global idRequestMax
    global rootObject

    global theShortestPath

    print("in messageReceived")

    print(msg)
    try:
        jsonReceived = json.loads(msg)

        if jsonReceived['id'] == 0:
            # msg from taxi
            print("from taxi")

            idRequest = jsonReceived['idCabRequest']

            accepted = jsonReceived['accepted']

            if accepted == True:
                # request idRequest accepted
                # faire recherche de plus court chemin

                # {
                # "idCabRequest":0,
                # "area": "Quartier Nord",
                # "location": {
                # "area": "Quartier Nord",
                # "locationType": "vertex",
                # "location": "b"
                # }
                # }


                for req in rootObject['cabRequest']:
                    if req['idCabRequest'] == idRequest:
                        theShortestPath = Dijkstra.doDijkstra(graphMap,
                                                              unicode(str(
                                                                  rootObject['cabInfo']['loc_now']['area']) + "." + str(
                                                                  rootObject['cabInfo']['loc_now']['location'])),
                                                              unicode(str(req['location']['area']) + "." + str(
                                                                  req['location']['location'])))

                # et lancer le deplacement

                mover = MoverRunner()
                mover.start()

                pass
            else:
                # request idRequest rejected
                pass

            # supprimer la requete avec l'id idRequest de la liste de requetes
            for req in rootObject["cabRequest"]:

                if req["idCabRequest"] == jsonReceived['id']:
                    rootObject["cabRequest"].remove(req)
                    pass
                pass

            pass
        else:
            # msg from client
            print("from client")

            print(jsonReceived['cabRequest'])

            jsonReceived['cabRequest']['idCabRequest'] = idRequestMax + 1

            print(jsonReceived['cabRequest'])
            idRequestMax += 1

            print("----------------------------------")

            print("rootObject:")
            print(rootObject)

            rootObject['cabRequest'].append(jsonReceived['cabRequest'])

            print(rootObject)

            pass
    except Exception as n:
        print("Exception Received:")
        print(n)
        return

    pass


class MoverRunner(Thread):
    def __init__(self):
        Thread.__init__(self)

    def run(self):
        global theShortestPath

        print("in moverrunner run")

        # on bouge le taxi toutes les x secondes, x etant le poid de l'arrete

        # le 1er terme est a ignorer car c'est le point actuel

        pointActuel = theShortestPath[0]
        del theShortestPath[0]

        print("theshortestpath: ")
        pprint(theShortestPath)

        coeffSleep = 2.5


        for point in theShortestPath:

            print("before sleep")
            print(pointActuel)
            print(point)

            poid = CityParser.getPoids(pointActuel, point)

            sleep(coeffSleep*poid)

            # go to point

            rootObject['cabInfo']['loc_now']['area'] = unicode( ".".join( (point.split('.'))[-1] ) )

            rootObject['cabInfo']['loc_now']['location'] = unicode( (point.split('.'))[-1] )


            rootObject['cabInfo']['odometer'] = rootObject['cabInfo']['odometer'] + poid


            pointActuel = point

            print("le point " + point + " a ete atteind")
        pass

    pass




    # if __name__ == "__main__":
    #
    #    parser = OptionParser(usage="usage: %prog [options]", version="%prog 1.0")
    #    parser.add_option("--host", default='', type='string', action="store", dest="host", help="hostname (localhost)")
    #    parser.add_option("--port", default=8000, type='int', action="store", dest="port", help="port (8000)")
    #    parser.add_option("--example", default='echo', type='string', action="store", dest="example", help="echo, chat")
    #    parser.add_option("--ssl", default=0, type='int', action="store", dest="ssl", help="ssl (1: on, 0: off (default))")
    #    parser.add_option("--cert", default='./cert.pem', type='string', action="store", dest="cert",
    #                      help="cert (./cert.pem)")
    #    parser.add_option("--ver", default=ssl.PROTOCOL_TLSv1, type=int, action="store", dest="ver", help="ssl version")
    #
    #    (options, args) = parser.parse_args()
    #
    #    cls = SimpleChat
    #
    #    if options.ssl == 1:
    #        server = SimpleSSLWebSocketServer(options.host, options.port, cls, options.cert, options.cert,
    #                                          version=options.ver)
    #    else:
    #        server = SimpleWebSocketServer(options.host, options.port, cls)
    #
    #
    #    def close_sig_handler(signal, frame):
    #        server.close()
    #        sys.exit()
    #
    #
    #    signal.signal(signal.SIGINT, close_sig_handler)
    #
    #    print('about to launch server')
    #
    #    server.serveforever()
    #