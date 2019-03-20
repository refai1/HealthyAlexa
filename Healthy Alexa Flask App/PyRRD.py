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

def str2date(datestr):
	date = datetime.datetime.strptime(datestr, "%Y-%m-%d")
	return date

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
		value = "U"

	# check if update defines time as "now"
	if (argTup[0]):
		time = argTup[0]
	else:
		time = "N"

	string = "rrdtool update "+name+".rrd "+str(time)+":"+str(value)
	#print(time)
	#return string
	#print(string)
	subprocess.run(string.split())

def batchUpdateRRD(name, argArray):
	string = "rrdtool update "+name+".rrd "
	count = 1
	for arg in argArray:
		# will trust, won't check
		## Will add 10 at a time
		if (count%500 == 0):
			subprocess.run(string.split())
			string = "rrdtool update "+name+".rrd "
		else:
			string = string + str(arg[0])+":"+str(arg[1])+" "
		count += 1

def fetchRRD(name, cf, start, end, res=None):
	if (res):
		string = "rrdtool fetch "+name+".rrd "+cf+" -r "+str(res)+" --start "+str(start)+" --end "+str(end)
	else:
		string = "rrdtool fetch "+name+".rrd "+cf+" --start "+str(start)+" --end "+str(end)

	#return string
	#print(string)
	results = subprocess.run(string.split(), stdout=subprocess.PIPE)
	return results.stdout
	