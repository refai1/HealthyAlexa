
- index.js

This is the index file for the portion of our application's backend that is hosted on the AWS Lambda servers. 

This file has 2 important dependencies. 1) alexa-sdk, and 2) request-promise

Before uploading this to your AWS Lambda instance, you must first have ngrok installed. Ngrok provides a secure tunnel to your localhost so the Lambda function can make remote API calls to the Flask App running on your device.
	- Once ngrok is installed, run the command "flask run"
	- then run "ngrok http 5000" where 5000 specifies the port (flask default 5000)
	- copy and paste the ngrok.io url into the variable "url" at the top of index.js.
	- Unfortunately, the free version of ngrok does not allow you to use your own url. Every time you run the flask app and tunnel to it, ngrok will assign a new url, which you must paste into index.js so that the lambda is able to make remote API calls.

To run this on your own AWS lambda instance:
	- make sure you have nodejs installed.
		- run the command "npm init"
		- run the command "npm install alexa-sdk --save"
		- run the command "npm install request --save"
		- run the command "npm install request-promise --save"
	- Compress the node_modules folder and the index.js file in a single archive
	- Upload the archive to the aws lambda instance and click "save"


- healthyAlexa.JSON

This is the speech model for the alexa skill.
It consists of a JSON object which highlights the skill's intents and sample utterances.
- Simply copy and paste it into the Amazon Developer Console in the JSON Editor corresponding to your skill.