from PyRRD import *
import random

def exampleTemp():
	dataSources = []
	RRArchives = []
	
	delay = 1
	# create the data sources
	dataSources.append(createDataSource("temperature", "GAUGE", 600))
	RRArchives.append(createRRA("LAST", 1, 24))
	RRArchives.append(createRRA("AVERAGE",10,10))

	ts = int(time.time())
	
	while(ts % delay != 0):
		time.sleep(1)
		ts = int(time.time())

	createRRD("temperature", ts, dataSources, RRArchives, delay)

	entries = 20
	for i in range(entries):
		time.sleep(delay)
		ts = int(time.time())
		print("Reading temperature at time", ts2date(ts), "...")

		hr = 80+random.randint(0,10)
		updateRRD("temperature", (ts, hr))
		print("Temperature:", hr, "added to rrd")
		

	result = fetchRRD("temperature", "AVERAGE", ts-entries*delay, ts)
	finalres = printFetch(result)
	return finalres

#This function populates the rrd for the last 3 days with temperature readings every 5 minutes.
# 5 minutes = 300 seconds
# 1 hour is 12*5 mins = 300*12 = 3600
# one day is 86400 seconds
def example2():

	dataSources = []
	RRArchives = []

	# create the data sources
	dataSources.append(createDataSource("temperature", "GAUGE", 600))
	# averages data every hour for 24 hours. (24 entires of 12-five-minute readings) 
	RRArchives.append(createRRA("AVERAGE", 12, 24))
	# averages data every day for 3 days
	RRArchives.append(createRRA("AVERAGE",12*24,3))

	
	ts = int(time.time())

	createRRD("temperature", ts-259200, dataSources,RRArchives)

	reading_time = ts-259200
	# 3 days worth of 5 minutes
	for i in range(864):
		tmp = 90+random.randint(0,10)
		updateRRD("temperature", (reading_time, tmp))
		reading_time = reading_time+300

	result = fetchRRD("temperature", "AVERAGE", ts-259200, ts)
	printFetch(result)

def fetchForDate(date):
	ts = int(time.time())

	result = fetchRRD("temperature","AVERAGE", ts-259200, ts).split("\n")
	print(result)
	return result

#def main():
#	example2()


#main()