import signal, sys, ssl
from optparse import OptionParser
from threading import Thread
from flask import Flask, render_template
from SimpleWebSocketServer import SimpleWebSocketServer, SimpleSSLWebSocketServer
from SimpleExampleServer import SimpleChat


app = Flask(__name__)


@app.route('/')
def index():
    return 'You should go on ADRESS/cab or ADRESS/monitor'


@app.route('/cab')
def cab_page():
    # enregistrer comme quoi il est co, et lui envoyer un ID 0
    return '{"text":"You are a cab"}'

@app.route('/monitor')
def monitor_page():
    # tell le monitor its ID, the map, and error (true or false)
    return '{"text":"You are a monitor"}'

@app.route('/webclient')
def webclient():
    return render_template('index.html')

class ServiceRunner(Thread):
    def __init__(self):
        Thread.__init__(self)

    def run(self):
        app.run("0.0.0.0")

    pass

if __name__ == '__main__':
    print("before run")

    #app.run("0.0.0.0")
    runner = ServiceRunner()
    runner.start()

    print("after")

    parser = OptionParser(usage="usage: %prog [options]", version="%prog 1.0")
    parser.add_option("--host", default='', type='string', action="store", dest="host", help="hostname (0.0.0.0)")
    parser.add_option("--port", default=8000, type='int', action="store", dest="port", help="port (8000)")
    parser.add_option("--example", default='echo', type='string', action="store", dest="example", help="echo, chat")
    parser.add_option("--ssl", default=0, type='int', action="store", dest="ssl", help="ssl (1: on, 0: off (default))")
    parser.add_option("--cert", default='./cert.pem', type='string', action="store", dest="cert", help="cert (./cert.pem)")
    parser.add_option("--ver", default=ssl.PROTOCOL_TLSv1, type=int, action="store", dest="ver", help="ssl version")

    (options, args) = parser.parse_args()

    cls = SimpleChat


    if options.ssl == 1:
        server = SimpleSSLWebSocketServer(options.host, options.port, cls, options.cert, options.cert, version=options.ver)
    else:
        server = SimpleWebSocketServer(options.host, options.port, cls)

    def close_sig_handler(signal, frame):
        server.close()
        sys.exit()

    signal.signal(signal.SIGINT, close_sig_handler)

    print('about to launch server')

    server.serveforever()