## Quick RRDTool python API which makes use of subprocess.
## Motivation behind this is that I ran into trouble installing the python
## library for rrdtool.

## author: Ahmed Refai
## email: refai1@g.ucla.edu


###############################################################################
#### TODO
## set up dates and times     DONE
## set up round robin archive formatting
## test
###############################################################################

import subprocess
import time
import datetime


###############################################################################

############################## HELPER FUNCTIONS ###############################

###############################################################################

def createDataSource(name, dataType, hb, vmin="U", vmax="U"):
	string = "DS:"+name+":"+dataType+":"+str(hb)+":"+str(vmin)+":"+str(vmax)
	return string

def createRRA(cf, step, rows, xff=0.5):
	string = "RRA:"+cf+":"+str(xff)+":"+str(step)+":"+str(rows)
	return string

def printFetch(result):
	retString=""
	result = str(result).strip()
	resString = result.split()
	
	for element in resString[2:]:
		value, ts = element.split("\\n")
		if(ts!="'"):
			t,v = ts2date(int(ts[:-1])),float(value)
			#print(t,v)
			retString +=  str(t)+":"+str(v)+"\n"
	print(retString)
	return retString



## DATE UTILS - more readable
def ts2date(ts):
	date = datetime.datetime.fromtimestamp(ts)
	return date

def date2ts(date):
	ts = datetime.datetime.timestamp(date)
	return ts

###############################################################################

################################### THE API ###################################

###############################################################################


def createRRD(name, start, dataSources, RRArchives, step=None):
	dataSources = " ".join(dataSources)
	RRArchives = " ".join(RRArchives)

	if (not step):
		string = "rrdtool create "+name+".rrd --start "+str(start)+" "+dataSources+" "+RRArchives
	else:
		string = "rrdtool create "+name+".rrd --start "+str(start)+" "+"--step="+str(step)+" "+dataSources+" "+RRArchives
		
	#return string
	#print(string)
	subprocess.run(string.split())

# Add entries to the RRD
# inputs: the name of the rrd to be updated. args, an arg tuple of (time, value)
# 		 to be added.
# outputs: runs the update statement.
def updateRRD(name, argTup):
	# check if there is an unknown value
	if (argTup[1]):
		value = argTup[1]
	else:
		argTup = "U"

	# check if update defines time as "now"
	if (argTup[0]):
		time = argTup[0]
	else:
		time = "N"

	string = "rrdtool update "+name+".rrd "+str(time)+":"+str(value)

	#return string
	#print(string)
	subprocess.run(string.split())

def fetchRRD(name, cf, start, end, res=None):
	if (res):
		string = "rrdtool fetch "+name+".rrd "+cf+" -r "+str(res)+" --start "+str(start)+" --end "+str(end)
	else:
		string = "rrdtool fetch "+name+".rrd "+cf+" --start "+str(start)+" --end "+str(end)

	#return string
	#print(string)
	results = subprocess.run(string.split(), stdout=subprocess.PIPE)
	return results.stdout
	
###############################################################################

#################################### MAIN #####################################

###############################################################################


# def main():
# 	dataSources = []
# 	RRArchives = []

# 	# ts = 920804400
# 	# date = ts2date(ts)
# 	# ts2 = date2ts(date)
# 	# print(ts2)

# 	#dataSources.append(createDataSource("heartrate", "GAUGE", 600))
# 	#RRArchives.append(createRRA("AVERAGE",5,10))
	
# 	# #createRRD("heartrate", "N", dataSources, RRArchives)
# 	# updateRRD("heartrate", (None,87))
# 	# ts = int(time.time())

# 	#fetchRRD("test", "AVERAGE", 920804400, 920809200)

# 	#dataSources.append(createDataSource("speed", "COUNTER", 600))
# 	#RRArchives.append(createRRA("AVERAGE", 1,24))
# 	#RRArchives.append(createRRA("AVERAGE", 6,10))
# 	# for i in range(15):
# 	# 	updateRRD("test", (920804700+i*300,12345+i*5))

# 	#createRRD("test", 920804400, dataSources, RRArchives)

# 	#subprocess.run("rrdtool create test.rrd --start 920804400 DS:speed:COUNTER:600:U:U RRA:AVERAGE:0.5:1:24 RRA:AVERAGE:0.5:6:10".split())
	
# 	# create the data sources
# 	dataSources.append(createDataSource("heartrate", "GAUGE", 600))
# 	RRArchives.append(createRRA("LAST", 1, 24))

# 	ts = int(time.time())

# 	createRRD("heartrate", ts-12000, dataSources, RRArchives)

# 	for i in range(20):
# 		updateRRD("heartrate", (ts-6000+300*i, 80+i))

# 	fetchRRD("heartrate", "LAST", ts-6600, ts+300)



# main()