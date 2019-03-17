from flask import Flask, request
from rrdtest2 import *

app = Flask(__name__)

@app.route('/')
def heartrate():
	#string = exampleTemp()
	#print(string)
	#return string
	print("they called!")
	return "Hello!!!"


@app.route('/command', methods=['GET'])
def handle_command():
    command = request.args.get('command','')
    fetchForDate(command)
    return command+" was your command!"