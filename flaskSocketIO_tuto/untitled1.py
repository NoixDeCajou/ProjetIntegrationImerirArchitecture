from flask import Flask, render_template
from flask.ext.socketio import SocketIO, emit

# tuto: http://blog.miguelgrinberg.com/post/easy-websockets-with-flask-and-gevent

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app)


@app.route('/')
def index():
    return render_template('index.html')


@app.route('/testroute/')
def index2():
    return render_template('index2.html')


@socketio.on('my event', namespace='/test')
def on_my_event(message):
    print("my event: " + message['data'])
    emit('my response', {'data': message['data']})


@socketio.on('my broadcast event', namespace='/test')
def on_broadcast(message):
    print("my broadcast: " + message['data'])
    emit('my response', {'data': message['data']}, broadcast=True)


@socketio.on('connect', namespace='/test')
def test_connect():
    print("on connect")
    emit('my response', {'data': 'Connected'})


@socketio.on('connect', namespace='/')
def test_connect():
    print("on connect no namespace")
    emit('my response', {'data': 'Connected'})

@socketio.on('disconnect', namespace='/test')
def test_disconnect():
    print('Client disconnected')


@socketio.on('message', namespace='/test')
def test_message_namespace(message):
    print("my broadcast namespace: " + message['data'])
    emit('my response', {'data': message['data']}, broadcast=True)


@socketio.on('message')
def test_message(message):
    print("my broadcast: " + message['data'])
    emit('my response', {'data': message['data']}, broadcast=True)


if __name__ == '__main__':
    socketio.run(app, '0.0.0.0')
