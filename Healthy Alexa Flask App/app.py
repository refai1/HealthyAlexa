from flask import Flask, request, jsonify
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
def getTemp():
    date = request.args.get('date','')
    dtype = request.args.get('dtype', '')
    result = fetchForDate(date, dtype)

    return result

@app.route('/temp2', methods=['GET'])
def getTemp2():
    startdate = request.args.get('startdate','')
    enddate = request.args.get('enddate','')
    starttime = request.args.get('starttime','')
    endtime = request.args.get('endtime','')
    dtype = request.args.get('dtype','')
    if (starttime == ''):
    	starttime = "00:00"
    if (endtime == ''):
    	endtime = "00:00"

    result = fetchForPeriod(startdate, enddate, starttime, endtime, dtype)

    return result

@app.route('/ppg', methods=['GET'])
def getPPG():
    startdate = request.args.get('startdate','')
    enddate = request.args.get('enddate','')
    starttime = request.args.get('starttime','')
    endtime = request.args.get('endtime','')
    dtype = request.args.get('dtype','')

    if (starttime == ''):
        starttime = "00:00"
    if (endtime == ''):
        endtime = "00:00"
    
    result = fetchForPeriod(startdate, enddate, starttime, endtime, dtype)

    return jsonify(result)

# @app.route('/humid', methods=['GET'])
# def getHumidity():
#     searchDate = request.args.get('humid','')
    
#     result = fetchHumidForDate(searchDate)

#     return result

# @app.route('/accel', methods=['GET'])
# def getAccel():
#     searchDate = request.args.get('accel','')
    
#     result = fetchAccelForDate(searchDate)

#     return result

# @app.route('/gyro', methods=['GET'])
# def getGyro():
#     searchDate = request.args.get('gyro','')
    
#     result = fetchGyroForDate(searchDate)

#     return result

@app.route('/getImage', methods=['PUT'])
def getImage():
	data = request.data
	f = open("dataReceived/running.txt", 'w')
	f.write(data.decode("utf-8"))

	return "done!"


