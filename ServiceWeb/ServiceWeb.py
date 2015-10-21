import json
import signal, sys, ssl
from optparse import OptionParser
from threading import Thread
from flask import Flask, render_template
import time
import CityParser
import SimpleExampleServer
from SimpleWebSocketServer import SimpleWebSocketServer, SimpleSSLWebSocketServer
from SimpleExampleServer import SimpleChat, broadcast
from pprint import pprint

app = Flask(__name__)


availibilityOfAreas = []

cabFound = False

server = ""


@app.route('/')
def index():
    return 'This is not the page you are looking for (go see /webclient)'


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
            return '{"error":false, "portWebSocket":8000, "id":' + str(i + 1) + ', "map":' + json.dumps(SimpleExampleServer.rootObject['rootObject']) + ' }'

        i += 1
    print("monitor rejected")
    return '{"error":true, "explication":"No area is available"}'
    # return '{"text":"You are a monitor"}'


@app.route('/webclient')
def webclient():
    return render_template('index.html')


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



class BroadcastRunner(Thread):
    def __init__(self):
        Thread.__init__(self)

    def run(self):

        print("starting to send maps")

        print("rootObject: ")
        pprint(SimpleExampleServer.rootObject)

        while(True):

            #pprint(SimpleExampleServer.rootObject)

            msg = '{"cabInfo":' + json.dumps(SimpleExampleServer.rootObject['cabInfo']) + ', '
            msg += '"nbCabRequest":' + str( len(SimpleExampleServer.rootObject['cabRequest']) ) + ', '
            msg += '"cabRequests":' + json.dumps(SimpleExampleServer.rootObject['cabRequest'])
            msg += '}'

            broadcast(msg)
            server.serve()
            #print(msg)
            #print("str: " + str(msg))

            time.sleep(1)
        pass

    pass



class ServiceRunner(Thread):
    def __init__(self):
        Thread.__init__(self)

    def run(self):
        app.run("0.0.0.0")

    pass


if __name__ == '__main__':

    # global SimpleExampleServer.rootObject

    with open('rootObject.json') as data_file:
        SimpleExampleServer.rootObject = json.load(data_file)
        pprint(SimpleExampleServer.rootObject)

    for area in SimpleExampleServer.rootObject['rootObject']['areas']:
        availibilityOfAreas.append(True)


    print("in main:")
    print(SimpleExampleServer.rootObject)

    SimpleExampleServer.graphMap = CityParser.getGraphe(SimpleExampleServer.rootObject['rootObject'])



    print("after getgraphe")

    print("before run")

    # app.run("0.0.0.0")
    runner = ServiceRunner()
    runner.start()

    print("after")

    parser = OptionParser(usage="usage: %prog [options]", version="%prog 1.0")
    parser.add_option("--host", default='', type='string', action="store", dest="host", help="hostname (0.0.0.0)")
    parser.add_option("--port", default=8000, type='int', action="store", dest="port", help="port (8000)")
    parser.add_option("--example", default='echo', type='string', action="store", dest="example", help="echo, chat")
    parser.add_option("--ssl", default=0, type='int', action="store", dest="ssl", help="ssl (1: on, 0: off (default))")
    parser.add_option("--cert", default='./cert.pem', type='string', action="store", dest="cert",
                      help="cert (./cert.pem)")
    parser.add_option("--ver", default=ssl.PROTOCOL_TLSv1, type=int, action="store", dest="ver", help="ssl version")

    (options, args) = parser.parse_args()

    cls = SimpleChat

    if options.ssl == 1:
        server = SimpleSSLWebSocketServer(options.host, options.port, cls, options.cert, options.cert,
                                          version=options.ver)
    else:
        server = SimpleWebSocketServer(options.host, options.port, cls)


    def close_sig_handler(signal, frame):
        server.close()
        sys.exit()


    signal.signal(signal.SIGINT, close_sig_handler)

    print('about to launch server')

    server.serveforever()
