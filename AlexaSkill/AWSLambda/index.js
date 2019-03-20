"use strict";

const request = require("request-promise")
const Alexa = require("alexa-sdk"); // import the library

//=========================================================================================================================================
//TODO: The items below this comment need your attention
//=========================================================================================================================================

//Replace with your app ID (OPTIONAL).  You can find this value at the top of your skill's page on http://developer.amazon.com.
//Make sure to enclose your value in quotes, like this:  const APP_ID = "amzn1.ask.skill.bb4045e6-b3e8-4133-b650-72923c5980f1";
const APP_ID = undefined;

// =====================================================================================================
// --------------------------------- Section 1. Data and Text strings  ---------------------------------
// =====================================================================================================
//TODO: Replace this data with your own.
//======================================================================================================

const url = "http://897f6bd4.ngrok.io"


//console.log(data[0]["date"]);
//======================================================================================================
//TODO: Replace these text strings to edit the welcome and help messages
//======================================================================================================

const skillName = "Healthy";

//This is the welcome message for when a user starts the skill without a specific intent.
const WELCOME_MESSAGE = "Welcome to " + skillName + "! Learn about your health and activity habits. For example " + getGenericHelpMessage();

//This is the message a user will hear when they ask Alexa for help in your skill.
const HELP_MESSAGE = "I can tell you various statistics about your health, such as, your average temperature during your last workout.";

//This is the message a user will hear when they begin a new search
const NEW_SEARCH_MESSAGE = getGenericHelpMessage();

//This is the message a user will hear when they ask Alexa for help while in the SEARCH state
const SEARCH_STATE_HELP_MESSAGE = getGenericHelpMessage();

const DESCRIPTION_STATE_HELP_MESSAGE = "Here are some things you can say: Tell me more, or search another date";

// This is the message use when the decides to end the search
const SHUTDOWN_MESSAGE = "Ok. ";

//This is the message a user will hear when they try to cancel or stop the skill.
const EXIT_SKILL_MESSAGE = "Ok. ";

// =====================================================================================================
// ------------------------------ Section 2. Skill Code - Intent Handlers  -----------------------------
// =====================================================================================================
// CAUTION: Editing anything below this line might break your skill.
//======================================================================================================

const states = {
	SEARCHMODE: "_SEARCHMODE",
	DESCRIPTION: "_DESCRIPTION",
	MULTIPLE_RESULTS: "_MULTIPLE_RESULTS"
};

const newSessionHandlers = {
	"LaunchRequest": function() {
		this.handler.state = states.SEARCHMODE;
		this.response.speak(WELCOME_MESSAGE).listen(getGenericHelpMessage());
		this.emit(':responseReady');
	},
	"GetForDateIntent": function() {
		this.handler.state = states.SEARCHMODE;
		this.emitWithState("GetForDateIntent");
	},
	"GetBetweenIntent": function() {
		this.handler.state = states.SEARCHMODE;
		this.emitWithState("GetBetweenIntent");
	},
	"AMAZON.YesIntent": function() {
		this.response.speak(getGenericHelpMessage()).listen(getGenericHelpMessage());
		this.emit(':responseReady');
	},
	"AMAZON.NoIntent": function() {
		this.response.speak(SHUTDOWN_MESSAGE);
		this.emit(':responseReady');
	},
	"AMAZON.RepeatIntent": function() {
		this.response.speak(HELP_MESSAGE).listen(getGenericHelpMessage());
		this.emit(':responseReady');
	},
	"AMAZON.StopIntent": function() {
		this.response.speak(EXIT_SKILL_MESSAGE);
		this.emit(':responseReady');
	},
	"AMAZON.CancelIntent": function() {
		this.response.speak(EXIT_SKILL_MESSAGE);
		this.emit(':responseReady');
	},
	"AMAZON.StartOverIntent": function() {
		this.handler.state = states.SEARCHMODE;
		var output = "Ok, starting over. " + getGenericHelpMessage();
		this.response.speak(output).listen(output);
		this.emit(':responseReady');
	},
	"AMAZON.HelpIntent": function() {
		this.response.speak(HELP_MESSAGE + getGenericHelpMessage()).listen(getGenericHelpMessage());
		this.emit(':responseReady');
	},
	"SessionEndedRequest": function() {
		this.emit("AMAZON.StopIntent");
	},
	"Unhandled": function() {
		this.handler.state = states.SEARCHMODE;
		this.emitWithState("SearchByNameIntent");
	}
};
let startSearchHandlers = Alexa.CreateStateHandler(states.SEARCHMODE, {
	"AMAZON.YesIntent": function() {
		this.response.speak(NEW_SEARCH_MESSAGE).listen(NEW_SEARCH_MESSAGE);
		this.emit(':responseReady');
	},
	"AMAZON.NoIntent": function() {
		this.response.speak(SHUTDOWN_MESSAGE);
		this.emit(':responseReady');
	},
	"AMAZON.RepeatIntent": function() {
		let output;
		if(this.attributes.lastSearch){
			output = this.attributes.lastSearch.lastSpeech;
			console.log("repeating last speech");
		}
		else{
			output = getGenericHelpMessage();
			console.log("no last speech availble. outputting standard help message.");
		}
		this.emit(":ask",output, output);
	},
	"GetForDateIntent": function() {
		GetForDateIntentHandler.call(this);
	},
	"GetBetweenIntent": function() {
		GetBetweenIntentHandler.call(this);
	},
	"AMAZON.HelpIntent": function() {
		this.response.speak(getGenericHelpMessage()).listen(getGenericHelpMessage());
		this.emit(':responseReady');
	},
	"AMAZON.StopIntent": function() {
		this.response.speak(EXIT_SKILL_MESSAGE);
		this.emit(':responseReady');
	},
	"AMAZON.CancelIntent": function() {
		this.response.speak(EXIT_SKILL_MESSAGE);
		this.emit(':responseReady');
	},
	"AMAZON.StartOverIntent": function() {
		this.handler.state = states.SEARCHMODE;
		var output = "Ok, starting over. " + getGenericHelpMessage();
		this.response.speak(output).listen(output);
		this.emit(':responseReady');
	},
	"SessionEndedRequest": function() {
		this.emit("AMAZON.StopIntent");
	},
	"Unhandled": function() {
		console.log("Unhandled intent in startSearchHandlers");
		this.response.speak(SEARCH_STATE_HELP_MESSAGE).listen(SEARCH_STATE_HELP_MESSAGE);
		this.emit(':responseReady');
	}
});


// DESCRIPTION STATE. When states.DESCRIPTION is asserted, these intents will be called
// and will dictate Alexa's responses.
let descriptionHandlers = Alexa.CreateStateHandler(states.DESCRIPTION, {
	"TellMeThisIntent": function() {
		let slots = this.event.request.intent.slots;
		let res = this.attributes.lastSearch.results[0];
		let speechOutput;
		let repromptSpeech;
		let cardContent;
		let dataType = "blank";
		dataType = this.attributes.lastSearch.lastdType;

		if(this.attributes.lastSearch){
			res =  this.attributes.lastSearch.results;
			//cardContent = generateCard(person);
			speechOutput = generateSpecificInfoMessage(slots,dataType,res);
			repromptSpeech = "Would you like to ask about another date? Say yes or no";
			this.handler.state = states.SEARCHMODE;
			this.attributes.lastSearch.lastSpeech = speechOutput;
			this.response.speak(speechOutput).listen(repromptSpeech);
		} else {
			//not a valid slot. no card needs to be set up. respond with simply a voice response.
			res = this.attributes.lastSearch.results[0]
			repromptSpeech = "I ran into some trouble. check the console log for more info.";
			this.attributes.lastSearch.lastSpeech = speechOutput;
			this.handler.state = states.SEARCHMODE;
			this.response.speak(speechOutput).listen(repromptSpeech);
		}
		this.emit(':responseReady');
	},
	"GetForDateIntent": function() {
		GetForDateIntentHandler.call(this);
	},
	"GetBetweenIntent": function() {
		GetBetweenIntentHandler.call(this);
	},
	"AMAZON.HelpIntent": function() {
		var res = this.attributes.lastSearch.results[0];
		this.response.speak(generateNextPromptMessage(res,"current")).listen(generateNextPromptMessage(res,"current"));
		this.emit(':responseReady');
	},
	"AMAZON.StopIntent": function() {
		this.response.speak(EXIT_SKILL_MESSAGE);
		this.emit(':responseReady');
	},
	"AMAZON.CancelIntent": function() {
		this.response.speak(EXIT_SKILL_MESSAGE);
		this.emit(':responseReady');
	},
	"AMAZON.NoIntent": function() {
		this.response.speak(SHUTDOWN_MESSAGE);
		this.emit(':responseReady');
	},
	"AMAZON.YesIntent": function() {
		this.emit("TellMeMoreIntent");
	},
	"AMAZON.RepeatIntent": function() {
		this.response.speak(this.attributes.lastSearch.lastSpeech).listen(this.attributes.lastSearch.lastSpeech);
		this.emit(':responseReady');
	},
	"AMAZON.StartOverIntent": function() {
		this.handler.state = states.SEARCHMODE;
		var output = "Ok, starting over. " + getGenericHelpMessage();
		this.response.speak(output).listen(output);
		this.emit(':responseReady');
	},
	"SessionEndedRequest": function() {
		this.emit("AMAZON.StopIntent");
	},
	"Unhandled": function() {
		let res = this.attributes.lastSearch.results[0];
		console.log("Unhandled intent in DESCRIPTION state handler");
		this.response.speak("Sorry, I don't know that" + generateNextPromptMessage(res,"general"))
		.listen("Sorry, I don't know that" + generateNextPromptMessage(res,"general"));
		this.emit(':responseReady');
	}
});

// ------------------------- END of Intent Handlers  ---------------------------------

function getPeriodEntry(dataType, startdate, enddate, starttime, endtime){
	if (dataType == "body temperature"){
		return request.get(url+"/temp2?startdate="+startdate+"&enddate="+enddate+"&starttime="+starttime+"&endtime="+endtime+"&dtype=temperature");
	}else if (dataType == "humidity index"){
		return request.get(url+"/temp2?startdate="+startdate+"&enddate="+enddate+"&starttime="+starttime+"&endtime="+endtime+"&dtype=humidity");
	}else if (dataType == "accelerometer reading"){
		return request.get(url+"/temp2?startdate="+startdate+"&enddate="+enddate+"&starttime="+starttime+"&endtime="+endtime+"&dtype=accelerometer");
	}else if (dataType == "gyroscope reading"){
		return request.get(url+"/temp2?startdate="+startdate+"&enddate="+enddate+"&starttime="+starttime+"&endtime="+endtime+"&dtype=gyroscope");
	}else if (dataType == "peak ppg"){
		return request.get(url+"/ppg?startdate="+startdate+"&enddate="+enddate+"&starttime="+starttime+"&endtime="+endtime+"&dtype=ppg");
	}
}


function getDateEntry(dateArg, dataType){
	if (dataType == "body temperature"){
		return request.get(url+"/command?date="+dateArg+"&dtype=temperature");
	}else if (dataType == "humidity index"){
		return request.get(url+"/command?date="+dateArg+"&dtype=humidity");
	}else if (dataType == "accelerometer reading"){
		return request.get(url+"/command?date="+dateArg+"&dtype=accelerometer");	
	}else if (dataType == "gyroscope reading"){
		return request.get(url+"/command?date="+dateArg+"&dtype=gyroscope");
	}
}

function GetForDateIntentHandler(){
	let date = isSlotValid(this.event.request, "date");
	let dataType = isSlotValid(this.event.request, "dataType");
	if (date){
		getDateEntry(date,dataType).then(
			(response) => {
				console.log(response);
				var searchQuery = date
				var result = {date: date, value:response}
				var searchResults = {count: 1, results: result};

				// saving lastSearch results to the current session
				var lastSearch = this.attributes.lastSearch = searchResults;
				var output;

				// saving last intent to session attributes
				this.attributes.lastSearch.lastIntent = "GetForDateIntent";
				this.attributes.lastSearch.lastdType = dataType;
				if (searchResults.count == 1){
					this.handler.state = states.DESCRIPTION; // change state to description
					console.log("one match was found");
					this.emitWithState("TellMeThisIntent");
				}
				else{
					console.log("no match found");
					console.log("searchQuery was  = " + searchQuery);
					console.log("searchResults.results was  = " + searchResults.results);
					output = generateSearchResultsMessage(searchQuery,searchResults.results);
					this.attributes.lastSearch.lastSpeech = output;
					// this.emit(":ask", generateSearchResultsMessage(searchQuery,searchResults.results));
					this.response.speak(output).listen(output);
				}
				this.emit(':responseReady');
			},
			(error) => {
				console.error("oops" + error)
			});
	}
}

function GetBetweenIntentHandler(){
	let startdate = isSlotValid(this.event.request, "startdate");
	let enddate = isSlotValid(this.event.request, "enddate");
	let starttime = isSlotValid(this.event.request, "starttime");
	let endtime = isSlotValid(this.event.request, "endtime");
	let dataType = isSlotValid(this.event.request, "dataType");
	
	if (startdate && enddate){
		getPeriodEntry(dataType, startdate, enddate, starttime, endtime).then(
			(response) => {
				console.log(response);
				var searchQuery = startdate
				var result = {date: "that period", value:response}
				var searchResults = {count: 1, results: result};

				// saving lastSearch results to the current session
				var lastSearch = this.attributes.lastSearch = searchResults;
				// saving last intent to session attributes
				this.attributes.lastSearch.lastIntent = "GetBetweenIntent";
				this.attributes.lastSearch.lastdType = dataType;
				if (searchResults.count == 1){
					this.handler.state = states.DESCRIPTION; // change state to description
					console.log("one match was found");
					this.emitWithState("TellMeThisIntent");
				}
				else{
					console.log("no match found");
					output = generateSearchResultsMessage(searchQuery,searchResults.results);
					this.attributes.lastSearch.lastSpeech = output;
					// this.emit(":ask", generateSearchResultsMessage(searchQuery,searchResults.results));
					this.response.speak(output).listen(output);
				}
				this.emit(':responseReady');
			},
			(error) => {
				console.error("oops" + error)
			});
	}
}


// =====================================================================================================
// ------------------------------- Section 3. Generating Speech Messages -------------------------------
// =====================================================================================================

function generateNextPromptMessage(res,mode){
	let prompt;

	if (mode == "current"){
		// if the mode is current, we should give more informaiton about the current contact
		prompt = ". You can say - tell me my average body temperature for " + res["date"];
	}
	//if the mode is general, we should provide general help information
	else if (mode == "general"){
		prompt = ". " + getGenericHelpMessage();
	}
	return prompt;
}


function generateSearchResultsMessage(searchQuery,results){
	let sentence;
	let details;
	let prompt;

	if (results){
		switch (true) {
		case (results.length == 0):
			sentence = "Hmm. I couldn't find " + searchQuery + ". " + getGenericHelpMessage();
			break;
		case (results.length == 1):
			let res = results[0];
			details = "Your average temperature for " + res.date + " is " + res.temperature;
			prompt = generateNextPromptMessage(res,"current");
			sentence = details + prompt;
			console.log(sentence);
			break;
		}
	}
	else{
		sentence = "Sorry, I didn't quite get that. " + getGenericHelpMessage();
	}
	return sentence;
}

//let counter = -1;
function getGenericHelpMessage(){
    //console.log("Called generic help message")
    //console.log(data[0]);
    //counter = (counter+1) % 4;
	let sentences = ["ask - what was my average body temperature for yesterday?",
					"ask - what was my peak ppg reading between march 13th 2019 and march 14th 2019?",
					"ask - what was my average gyroscope reading for yesterday?",
					"ask - what was my average humidity index for march 16th 2019?"]
	return "You can " + sentences[getRandom(0,3)];
}

function generateSpecificInfoMessage(slots,dtype,result){
	let infoTypeValue;
	let sentence;

	console.log(result);
	//sentence = person.firstName + "'s " + infoTypeValue.toLowerCase() + " is - " + person["say" + infoTypeValue.toLowerCase()] + " . Would you like to find another evangelist? " + getGenericHelpMessage(data);
	
	if (dtype == "peak ppg"){
		result['value'] = JSON.parse(result['value']);
		console.log(result);
		var incdec;
		var pct = result['value']["percent"];
		if (result["value"]["up"] == '1'){
			incdec = " a "+pct+"% increase from the previous hour. ";
		}else if(result['value']["up"] == '0'){
			incdec = " the same as the previous hour. ";
		}else{
			incdec = " a "+pct+"% decrease from the previous hour. ";
		}

		sentence = "Your " + dtype + " for " + result["date"] + " was " + result["value"]["temp"] + " millivolts."
		sentence += " This is "+incdec;
		sentence += "What else can I help you with?  " + getGenericHelpMessage();
	}else{
		sentence = "Your average " + dtype + " for " + result["date"] + " was " + result["value"] + ". What else can I help you with?  " + getGenericHelpMessage();
	
	}
	return sentence;
}

exports.handler = function(event, context, callback) {
	let alexa = Alexa.handler(event, context);
	alexa.appId = APP_ID;
	alexa.registerHandlers(newSessionHandlers, startSearchHandlers, descriptionHandlers);
	alexa.execute();
};

// =====================================================================================================
// ------------------------------------ Section 4. Helper Functions  -----------------------------------
// =====================================================================================================
// For more helper functions, visit the Alexa cookbook at https://github.com/alexa/alexa-cookbook
//======================================================================================================

function getRandom(min, max) {
	return Math.floor(Math.random() * (max - min + 1) + min);
}

function titleCase(str) {
	return str.replace(str[0], str[0].toUpperCase());
}


function loopThroughArrayOfObjects(arrayOfStrings) {
	let joinedResult = "";
	// Looping through the each object in the array
	for (let i = 0; i < arrayOfStrings.length; i++) {
		//concatenating names (firstName + lastName ) for each item
		joinedResult = joinedResult + ", " + arrayOfStrings[i].firstName + " " + arrayOfStrings[i].lastName;
	}
	return joinedResult;
}


function sanitizeSearchQuery(searchQuery){
	searchQuery = searchQuery.replace(/’s/g, "").toLowerCase();
	searchQuery = searchQuery.replace(/'s/g, "").toLowerCase();
	return searchQuery;
}


function isSlotValid(request, slotName){
	let slot = request.intent.slots[slotName];
	//console.log("request = "+JSON.stringify(request)); //uncomment if you want to see the request
	let slotValue;

	//if we have a slot, get the text and store it into speechOutput
	if (slot && slot.value) {
		//we have a value in the slot
		slotValue = slot.value.toLowerCase();
		return slotValue;
	} else {
		//we didn't get a value in the slot.
		slotValue = "";
		return slotValue;
	}
}

