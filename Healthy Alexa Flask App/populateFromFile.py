from rrdtest2 import *
from random import randint
from progressbar import ProgressBar

import math


def main():
	
	# define daysBack to be the amount of days in the past we will input
	# data.
	daysBack = 10

	# Create RRD for each of our guages

	# Empty data source arrays
	humDS = []
	tempDS = []
	accDS = []
	gyrDS = []
	ppgDS = []

	# Empty archive arrays
	humRRA = []
	tempRRA =[]
	accRRA = []
	gyrRRA = []
	ppgRRA = []

	# Create data sources. set to go unknown after 10 minutes with no reading 
	humDS.append(createDataSource("humidity", "GAUGE", 600))
	tempDS.append(createDataSource("temperature", "GAUGE", 600))
	accDS.append(createDataSource("accelerometer", "GAUGE", 600))
	gyrDS.append(createDataSource("gyroscope", "GAUGE", 600))
	ppgDS.append(createDataSource("gyroscope", "GAUGE", 600))

	# Create Archives. To average the readings from every hour. Store for 10 days
	humRRA.append(createRRA("AVERAGE", 3600, daysBack*24))
	tempRRA.append(createRRA("AVERAGE", 3600, daysBack*24))
	accRRA.append(createRRA("AVERAGE", 3600, daysBack*24))
	gyrRRA.append(createRRA("AVERAGE", 3600, daysBack*24))
	ppgRRA.append(createRRA("MAX", 3600, daysBack*24))

	#Get current time down to the second.
	ts = int(time.time())
	print(ts - daysBack*3600*24 - 1)
	# Create empty RRDs. Those will store data every second
	createRRD("humidity", ts-(daysBack*3600*24)-1, humDS, humRRA, step=1)
	createRRD("temperature", ts-(daysBack*3600*24)-1, tempDS, tempRRA, step=1)
	createRRD("accelerometer", ts-(daysBack*3600*24)-1, accDS, accRRA, step=1)
	createRRD("gyroscope", ts-(daysBack*3600*24)-1, gyrDS, gyrRRA, step=1)
	createRRD("ppg", ts-(daysBack*3600*24)-1, ppgDS, ppgRRA, step=1)

	# process file data to obtain examples of different actions.

	# parse walking data
	f = open("dataReceived/walking.txt", 'r')

	# Arrays for data to be input
	walkHumVals = []
	walkTempVals = []
	walkAccVals = []
	walkGyrVals = []
	
	for line in f:
		if (line[0] == "H"):
			number = int(line.split()[1])
			walkHumVals.append(number)
		elif(line[0] == "T"):
			number = int(line.split()[1])
			walkTempVals.append(number)
		elif(line[0] == "a"):
			xyz = line.split()
			x = float(xyz[1][:-1])
			y = float(xyz[3][:-1])
			z = float(xyz[5][:-1])
			number = round(math.sqrt(x**2+y**2+z**2),3)
			walkAccVals.append(number)
		elif(line[0] == "g"):
			xyz = line.split()
			x = float(xyz[1][:-1])
			y = float(xyz[3][:-1])
			z = float(xyz[5][:-1])
			number = round(math.sqrt(x**2+y**2+z**2),3)
			walkGyrVals.append(number)
	f.close()

	# Parse running data
	f = open("dataReceived/running.txt", 'r')
	runHumVals = []
	runTempVals = []
	runAccVals = []
	runGyrVals = []

	for line in f:
		if (line[0] == "H"):
			number = int(line.split()[1])
			runHumVals.append(number)
		elif(line[0] == "T"):
			number = int(line.split()[1])
			runTempVals.append(number)
		elif(line[0] == "a"):
			xyz = line.split()
			x = float(xyz[1][:-1])
			y = float(xyz[3][:-1])
			z = float(xyz[5][:-1])
			number = round(math.sqrt(x**2+y**2+z**2),3)
			runAccVals.append(number)
		elif(line[0] == "g"):
			xyz = line.split()
			x = float(xyz[1][:-1])
			y = float(xyz[3][:-1])
			z = float(xyz[5][:-1])
			number = round(math.sqrt(x**2+y**2+z**2),3)
			runGyrVals.append(number)
	f.close()

	# Parse standing data
	f = open("dataReceived/standing.txt", 'r')
	standHumVals = []
	standTempVals = []
	standAccVals = []
	standGyrVals = []

	for line in f:
		if (line[0] == "H"):
			number = int(line.split()[1])
			standHumVals.append(number)
		elif(line[0] == "T"):
			number = int(line.split()[1])
			standTempVals.append(number)
		elif(line[0] == "a"):
			xyz = line.split()
			x = float(xyz[1][:-1])
			y = float(xyz[3][:-1])
			z = float(xyz[5][:-1])
			number = round(math.sqrt(x**2+y**2+z**2),3)
			standAccVals.append(number)
		elif(line[0] == "g"):
			xyz = line.split()
			x = float(xyz[1][:-1])
			y = float(xyz[3][:-1])
			z = float(xyz[5][:-1])
			number = round(math.sqrt(x**2+y**2+z**2),3)
			standGyrVals.append(number)
	f.close()

	f = open("dataReceived/ppgPD2Red1.txt")
	f2 = open("dataReceived/ppgPD2IR1.txt")
	ppgVals = []

	for line in f:
		number = float(line.strip())
		ppgVals.append(number)
	
	i=0
	for line in f2:
		number = float(line.strip())
		ppgVals[i] = ppgVals[i] - number
		i+=1

	f.close()
	f2.close()

	# Two things to consider:
	## 1. Decide whether to input walking, running, or stationary
	### 	each 2.5 hour period, randomly decide which data to pull from.
	###		iterate over that while inputing the data
	###		Make stationary 3x as likely as walking. walking 2x as likely
	###		as running.
	## 2. Assume sampling rate is 1 second

	# 0 = running, 1 = walking, 2 = stationary
	possibleActivity = [0, 1, 1, 2, 2, 2, 2, 2, 2]
	reading_time = ts - daysBack*3600*24
	mode = 0

	print(reading_time, "-", ts)
	humArgList = []
	tempArgList = []
	accArgList = []
	gyrArgList = []
	ppgArgList = []
	pbar = ProgressBar()
	for i in pbar(range(daysBack*3600*24)):
		#print(">",end='')
		if (i % (3600*2.5) == 0):
			mode = possibleActivity[randint(0,8)]
		#if (i % 1000 == 0):
		#	print(">", end='')
		#Find value to add based on selected mode
		#print(i, end='')
		if (mode == 0):
			hum2add = runHumVals[i%len(runHumVals)]
			temp2add = runTempVals[i%len(runTempVals)]
			acc2add = runAccVals[i%len(runAccVals)]
			gyr2add = runGyrVals[i%len(runGyrVals)]
		elif (mode == 1):
			hum2add = walkHumVals[i%len(walkHumVals)]
			temp2add = walkTempVals[i%len(walkTempVals)]
			acc2add = walkAccVals[i%len(walkAccVals)]
			gyr2add = walkGyrVals[i%len(walkGyrVals)]
		else:
			hum2add = standHumVals[i%len(standHumVals)]
			temp2add = standTempVals[i%len(standTempVals)]
			acc2add = standAccVals[i%len(standAccVals)]
			gyr2add = standGyrVals[i%len(standGyrVals)]

		ppg2add = ppgVals[i%len(ppgVals)]

		humArgList.append((reading_time,hum2add))
		tempArgList.append((reading_time,temp2add))
		accArgList.append((reading_time,acc2add))
		gyrArgList.append((reading_time,gyr2add))
		ppgArgList.append((reading_time,ppg2add))
		## Update statements
		if (len(humArgList) == 1000):
			batchUpdateRRD("humidity", humArgList)
			humArgList = []
			batchUpdateRRD("temperature", tempArgList)
			tempArgList = []
			batchUpdateRRD("accelerometer", accArgList)
			accArgList = []
			batchUpdateRRD("gyroscope", gyrArgList)
			gyrArgList = []
			batchUpdateRRD("ppg", ppgArgList)
			ppgArgList = []
			
		# updateRRD("humidity", (reading_time, hum2add))
		# updateRRD("temperature", (reading_time, temp2add))
		# updateRRD("accelerometer", (reading_time, acc2add))
		# updateRRD("gyroscope", (reading_time, gyr2add))
		reading_time += 1

main()