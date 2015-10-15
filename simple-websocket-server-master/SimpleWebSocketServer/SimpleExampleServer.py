'''
The MIT License (MIT)
Copyright (c) 2013 Dave P.
'''

import signal, sys, ssl
from threading import Thread
from time import sleep
from SimpleWebSocketServer import WebSocket, SimpleWebSocketServer, SimpleSSLWebSocketServer
from optparse import OptionParser
import json
from pprint import pprint

themap = ""
clients = []
stop_sending = False


class SimpleChat(WebSocket):

    def __init__(self):
        WebSocket.__init__(self)
        self.numeroArea = -1

    def handleMessage(self):
        for client in list(clients):
            # if client != self:
            client.sendMessage(self.address[0] + ' - ' + self.data)
            msg = (themap['areas'])[client.numeroArea]
            pprint(msg)
            client.sendMessage(msg)

            if msg_is_call_taxi(self.data):
                call_taxi(self.data)
            elif msg_is_answer_taxi(self.data):
                send_taxi(self.data)

    def handleConnected(self):
        global stop_sending
        print(self.address, 'connected')
        for client in list(clients):
            client.sendMessage(self.address[0] + u' - connected')

        # attribuer une area
        i = 0
        freeAreaFound = False
        while (i < len(themap['areas'])) and (freeAreaFound == False):
            j = 0
            areaDistributed = False
            while (j < len(clients)) and (areaDistributed == False):
                if( (clients[i]).numeroArea == i):
                    areaDistributed = True

            if areaDistributed == False:
                freeAreaFound = True
                self.numeroArea = i

        clients.append(self)
        if self.numeroArea == -1:
            self.close(1000, u'No Free Area')

        if allAreasAtributed() == True: # toutes les areas sont attribuées, start sending maps
            stop_sending = False
            sender = MapSender()
            sender.start()

    def handleClose(self):
        global stop_sending
        clients.remove(self)
        print(self.address, 'closed')
        stop_sending = True
        for client in list(clients):
            client.sendMessage(self.address[0] + u' - disconnected')


class MapSender(Thread):
    def __init__(self):
        Thread.__init__(self)

    def run(self):
        while stop_sending == False:
            for client in clients:
                msg = (themap['areas'])[client.numeroArea]

                pprint(msg)
                client.sendMessage(msg)

            pass
            sleep(1)
        pass
    pass


def msg_is_call_taxi(msg):
    print(msg)
    # TODO


def call_taxi(msg):
    print(msg)
    # TODO


def msg_is_answer_taxi(msg):
    print(msg)
    # TODO


def send_taxi(msg):
    print(msg)
    # TODO


def allAreasAtributed():

    i2 = 0
    for area2 in (themap['areas']):

        areaAtributed = False
        for client in clients:
            if client.numeroArea == i2:
                areaAtributed = True
            pass

        if areaAtributed == False:
            return False

        i2 += 1
        pass

    return True


if __name__ == "__main__":

    parser = OptionParser(usage="usage: %prog [options]", version="%prog 1.0")
    parser.add_option("--host", default='', type='string', action="store", dest="host", help="hostname (localhost)")
    parser.add_option("--port", default=8000, type='int', action="store", dest="port", help="port (8000)")
    parser.add_option("--ssl", default=0, type='int', action="store", dest="ssl", help="ssl (1: on, 0: off (default))")
    parser.add_option("--cert", default='./cert.pem', type='string', action="store", dest="cert",
                      help="cert (./cert.pem)")
    parser.add_option("--ver", default=ssl.PROTOCOL_TLSv1, type=int, action="store", dest="ver", help="ssl version")

    (options, args) = parser.parse_args()

    with open('city.json') as data_file:
        themap = json.load(data_file)
        pprint(themap)
        pprint("---------------------")
        pprint(themap['areas'])
        for area in themap['areas']:
            pprint("---------------------")
            pprint(area)


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

    server.serveforever()
