// code for get request to api

const request = require("request-promise");

const url = "http://1c56a381.ngrok.io";

const uri_dict = {temperature:"command", 
 				  humidity:"humid", 
   				  acclerometer:"accel",
 				  gyroscope:"gyro"};


function getEntry(dateArg, dataType){
	console.log(url+"/"+uri_dict[dataType]+"?"+uri_dict[dataType]+"="+dateArg);
	return request.get(url+"/"+uri_dict[dataType]+"?"+uri_dict[dataType]+"="+dateArg);
}


function your_code(){
	// you will need a date argument to pass in
	let date = "2019-03-16" // for example
	let dtype = "gyroscope" // for example

	// since an http request is an asynchronous request, and javascript is weird,
	// code doesn't run sequentially in the order that it is written.
	// for this we need "promises" (the arrow function below)
	// .then, promise that when this function returns, do this. 
	getEntry(date, dtype).then(
		(response) => {
			console.log(response)
			console.log("put the rest of the logic here")
			// response will be the requested value for the requested date.
		},
		(error) => {
			console.log(error)
		});
}

your_code();