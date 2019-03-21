This folder contains the backend that will run on the device connected to the hexiwear. Due to unforseen issues with the Hexiwear's bluetooth, and with the raspberry pi's serial interface, we had to use our own laptops. to host the backend. However it is entirely possible to host the database, backend, and API on a Raspberry Pi.

The backend consists of the following:
	- Flask App (app.py, rrdtest.2, PyRRD.py)
	- rrdtool databases (*.rrd)
	- The dataReceived directory and populateFromFile.py

Due to the device malfunctions, we used http put requests to transfer sensor data files from Hanna's device to Ahmed's device (where the backend is hosted).
The code used for these transfers can be found in the sendFile directory.

app.py receives those files through the route /getImage and saves them into the dataReceived directory.

To run this flask app you will need to install Flask in a python3 (preferably) virtual env. You will need to run ngrok and edit the url in the index.js file.
	- From within the env, "flask run"
	- "ngrok http 5000"
	- Populater the rrds. Feel free to change any paramters, such as daysBack (for how many days to store), how often rrdtool should archive your data, add datasources, add archives, or change cosnsolidation functions.

