from flask import Flask, render_template

app = Flask(__name__)


@app.route('/')
def index():
    return 'You should go on ADRESS/cab or ADRESS/monitor'


@app.route('/cab')
def cab_page():
    return '{"text":"You are a cab"}'

@app.route('/monitor')
def monitor_page():
    return '{"text":"You are a monitor"}'

@app.route('/webclient')
def webclient():
    return render_template('index.html')


if __name__ == '__main__':
    app.run("0.0.0.0")
