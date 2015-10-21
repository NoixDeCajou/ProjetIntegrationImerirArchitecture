# coding=utf-8
import json
import signal, sys, ssl
from optparse import OptionParser
from threading import Thread
from flask import Flask, render_template
import time
import CityParser
import Websockets
from SimpleWebSocketServer import SimpleWebSocketServer, SimpleSSLWebSocketServer
from Websockets import SimpleChat, broadcast
from pprint import pprint


"""

Objet Flask servant a l'envoi du client web et des pages json servant à l'initialisation de l'application

"""
app = Flask(__name__)


availibilityOfAreas = []

cabFound = False

server = ""

"""

Message annonçant qu'il n'y a rien sur cette route ...

"""
@app.route('/')
def index():
    return 'This is not the page you are looking for (go see /webclient)'

"""

On attribut l'id 0 au cab, et on lui transmet le numero de port sur lequel ouvrir la websocket

"""
@app.route('/cab')
def cab_page():
    # enregistrer comme quoi il est co, et lui envoyer un ID 0
    global cabFound

    if cabFound == False:
        print("cab found")
        cabFound = True
        return '{"error":false, "portWebSocket":8000, "id":0}'
    else:
        print("cab rejected")
        return '{"error":true, "explication":"A cab has already been selected"}'

"""

On attribut un id au cab, on lui transmet le numero de port sur lequel ouvrir la websocket, et on lui transmet
la map


"""
@app.route('/monitor')
def monitor_page():
    # tell le monitor its ID, the map, and error (true or false)
    # TODO

    global availibilityOfAreas

    i = 0
    while (i < len(availibilityOfAreas)):

        if availibilityOfAreas[i] == True:
            availibilityOfAreas[i] = False
            print("monitor found")
            startWebSocketServerIfReady()
            return '{"error":false, "portWebSocket":8000, "id":' + str(i + 1) + ', "map":' + json.dumps(Websockets.rootObject['rootObject']) + ' }'

        i += 1
    print("monitor rejected")
    return '{"error":true, "explication":"No area is available"}'
    # return '{"text":"You are a monitor"}'

"""

Le client devra acceder à cette route grace a son navigateur pour acceder au webclient, on lui renvoi la page appelée index.html

"""
@app.route('/webclient')
def webclient():
    return render_template('index.html')

"""

On appelle cette fonction lorsque l'attribu une area à un client
Si toutes les areas ont été attribuées, elle lance le thread broadcast runner qui va envoyer régulièrement aux monitors et cab
des mises à jours concernant la position du taxi et les requetes en cours

"""
def startWebSocketServerIfReady():

    print("in startWebSocketServerIfReady")

    allAreasAssigned = True
    for b in availibilityOfAreas:
        if(b == True):
            allAreasAssigned = False

    print("allAreasAssigned: " + str(allAreasAssigned) )

    if allAreasAssigned:
        broadcaster = BroadcastRunner()
        broadcaster.start()


"""

Cette classe représente le thread de broadcast de mises à jours
Il va régulièrement envoyer aux monitors et cab la position du taxi, et les requetes en attente

"""
class BroadcastRunner(Thread):
    def __init__(self):
        Thread.__init__(self)

    """
    Fonction executée dans un thread à part
    """
    def run(self):

        print("starting to send maps")

        print("rootObject: ")
        pprint(Websockets.rootObject)

        while(True):

            #pprint(SimpleExampleServer.rootObject)

            msg = '{"cabInfo":' + json.dumps(Websockets.rootObject['cabInfo']) + ', '
            msg += '"nbCabRequest":' + str( len(Websockets.rootObject['cabRequest']) ) + ', '
            msg += '"cabRequests":' + json.dumps(Websockets.rootObject['cabRequest'])
            msg += '}'

            broadcast(msg)
            server.serve()
            #print(msg)
            #print("str: " + str(msg))

            time.sleep(0.5)
        pass

    pass


"""

Cette classe représente le thread de Flask

"""
class ServiceRunner(Thread):
    def __init__(self):
        Thread.__init__(self)

    def run(self):
        app.run("0.0.0.0")

    pass

"""

Point d'entrée du programme

"""
if __name__ == '__main__':

    # initialiser l'objet rootObject à partir du fichier rootObject.json
    # rootObject représente le "modèle" de l'application
    with open('rootObject.json') as data_file:
        Websockets.rootObject = json.load(data_file)
        pprint(Websockets.rootObject)

    # on initialise availibilityOfAreas, une variable qui sera utile plus tard pour vérifier si toutes les areas ont été attribuées
    for area in Websockets.rootObject['rootObject']['areas']:
        availibilityOfAreas.append(True)


    print("in main:")
    print(Websockets.rootObject)

    # SimpleExampleServer.graphMap = CityParser.getGraphe(SimpleExampleServer.rootObject['rootObject'])



    print("after getgraphe")

    print("before run")

    # app.run("0.0.0.0")
    runner = ServiceRunner()
    runner.start()

    print("after")



    parser = OptionParser(usage="usage: %prog [options]", version="%prog 1.0")
    parser.add_option("--host", default='', type='string', action="store", dest="host", help="hostname (0.0.0.0)")

    (options, args) = parser.parse_args()

    cls = SimpleChat

    print( "host" )
    print(options.host)


    # on instancie le serveur websocket
    server = SimpleWebSocketServer(options.host, 8000, cls)


    def close_sig_handler(signal, frame):
        server.close()
        sys.exit()


    signal.signal(signal.SIGINT, close_sig_handler)

    print('about to launch server')


    # on allume le serveur websocket
    server.serveforever()
