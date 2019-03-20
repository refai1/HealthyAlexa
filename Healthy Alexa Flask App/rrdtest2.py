from PyRRD import *
import random

#This function populates the rrd for the last 3 days with temperature readings every 5 minutes.
# 5 minutes = 300 seconds
# 1 hour is 12*5 mins = 300*12 = 3600
# one day is 86400 seconds
def example2(dataType, days, baseval, rnge):
	dts = ["temperature", "humidity", "accelerometer", "gyroscope"]
	timeBack = days*86400

	if (dataType in dts):		 
		dataSources = []
		RRArchives = []

		# create the data sources
		dataSources.append(createDataSource(dataType, "GAUGE", 600))
		# averages data every 6 hours day for variable days
		RRArchives.append(createRRA("AVERAGE",12*6,days*4))

		
		ts = int(time.time())

		createRRD(dataType, ts-timeBack, dataSources,RRArchives)

		reading_time = ts-timeBack
		# 3 days worth of 5 minutes
		for i in range(288*days):
			tmp = baseval+random.randint(0,rnge)
			updateRRD(dataType, (reading_time, tmp))
			reading_time = reading_time+300

		result = fetchRRD(dataType, "AVERAGE", ts-timeBack, ts)
		printFetch(result)
	else:
		print("error: invalid datatype")
	return

def time2seconds(time):
	hrs = int(time.split(":")[0])
	mins = int(time.split(":")[1])	

	# the minus 2 adjusts time zones
	seconds = (hrs-2)*3600 + mins*60
	return seconds

def fetchForPeriod(startdate, enddate, starttime, endtime, dtype):
	#ts = int(time.time())
	start = int(date2ts(str2date(startdate)) + time2seconds(starttime))
	end = int(date2ts(str2date(enddate)) + time2seconds(endtime))

	prevHrSt = start - 3600
	prvHrEnd = end - 3600

	up = 0
	percent = 0

	if (dtype == "ppg"):
		result = fetchRRD(dtype, "MAX", start+1, end-1)
		temp = processPPGPeriodFetch(result)
		result = fetchRRD(dtype, "MAX", prevHrSt+1, prvHrEnd-1)
		temp2 = processPPGPeriodFetch(result)
		if (temp > temp2):
			up = 1
			percent = round((temp - temp2) / temp * 100, 2)
		elif(temp < temp2):
			up = -1
			percent = round((temp2 - temp) / temp2 * 100, 2)
		else:
			up = 0
		return {"temp":str(temp), "up":str(up), "percent":str(percent)}
		
	else:
		result = fetchRRD(dtype,"AVERAGE", start, end)
		temp = str(processPeriodFetch(result))
	

	return temp

def fetchForDate(date, dtype):
	ts = int(time.time())

	result = fetchRRD(dtype,"AVERAGE", ts-(24*60*60*10), ts)
	
	temp = str(processFetch(result, date))
	
	return temp

def processFetch(result, date):
	res = result.decode("utf-8").strip().split('\n')[2:]
	searchDate = str2date(date)
	searchTSmin = date2ts(searchDate)
	searchTSmax = searchTSmin + 24*60*60
	found = False
	vals = []

	for element in res:
		pair = element.split(' ')
		timestamp = pair[0][:-1]
		if ((float(timestamp) >= searchTSmin) and (float(timestamp) < searchTSmax)):
			vals.append(round(float(pair[1]),2))
			found = True

	if (found):
		temp = round(sum(vals)/len(vals),2)
	else:
		temp = "not available"

	return temp

def processPeriodFetch(result):
	res = result.decode("utf-8").strip().split('\n')[2:]
	vals = []
	for element in res:
		pair = element.split(' ')
		vals.append(round(float(pair[1]), 2))

	temp = round(sum(vals)/len(vals),2)

	return temp

def processPPGPeriodFetch(result):
	res = result.decode("utf-8").strip().split('\n')[2:]
	vals = []
	for element in res:
		pair = element.split(' ')
		vals.append(round(float(pair[1]), 2))

	temp = max(vals)

	return temp
# def main():
# 	example2()


# main()

def main():

	mydict = fetchForPeriod("2019-03-15","2019-03-15","20:00","21:00", "ppg")

	print(mydict)
main()