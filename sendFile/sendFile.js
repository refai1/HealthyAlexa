const request = require("request-promise");
const fs = require("fs");


// change these variables as needed
var uri = "http://127.0.0.1:5000/getImage"
var filename = "TissueOxygenation.txt";

var myfile = fs.readFile(filename,function(err, file){
	if (err){
		throw err;
	}
	console.log("hello");
	var r = request({
		method: 'PUT',
		uri: uri
	});
	r.body = file;
	r.then(function(response){
		console.log("success", response);		
	}).catch(function (error){
		console.log("error", error)
	});
});
