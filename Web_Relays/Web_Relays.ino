// Relay Switch System
// Joel Haker 2014
// joelhakerdesigns.com

// Includes
#include <SPI.h>
#include <Ethernet.h>
#include <WebServer.h>
#include <EEPROM.h>

// Pins
static const int RELAY_ONE = 3;
static const int RELAY_TWO = 4;
static const int RELAY_THREE = 5;
static const int RELAY_FOUR = 6;

// Relay enum
static const int Rly1 = 0;
static const int Rly2 = 1;
static const int Rly3 = 2;
static const int Rly4 = 3;

// Webserver
static uint8_t MAC[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
static uint8_t IP[] = {192, 168, 1, 49};
WebServer webserver("", 80);

// Variables
int switchState[4];
int	lastSwitchState[4];

// HTML for web front end UI
P(frontEnd) =
"<!doctype html>"
"\n<html lang='en'>"
"\n<head>"
"\n	<meta charset='utf-8'>"
"\n	<title>Arduino Relays</title>"
"\n	<!--[if lt IE 9]>"
"\n		<script src='http://html5shiv.googlecode.com/svn/trunk/html5.js'></script>"
"\n	<![endif]-->"
"\n	<style type='text/css'>"
"\n		html {"
"\n			background-color: #EEEEEE;"
"\n			height: 451px;"
"\n			width: 310px;"
"\n			padding: 0;"
"\n			margin: 0 auto;"
"\n		}"
"\n		body {"
"\n			height: 450px;"
"\n			width: 300px;"
"\n			font-family: Arial, Helvetica, sans-serif;"
"\n			font-size: 16px;"
"\n			font-size: 100%;"
"\n				font-size: 1em;"
"\n				padding: 0;"
"\n				margin: 0 auto;"
"\n			}"
"\n			h1{"
"\n				width: 95%;"
"\n				text-align: center;"
"\n				padding: 0;"
"\n				margin: 5px auto;"
"\n			}"
"\n			#switchForm"
"\n			{"
"\n				width: 300px;"
"\n				padding: 0;"
"\n				margin: 5px auto;"
"\n			}"
"\n			#switchForm .wrapper, .wrapper2"
"\n			{"
"\n				display: block;"
"\n				padding: 0;"
"\n				margin: 0 auto;"
"\n			}"
"\n			.wrapper"
"\n			{"
"\n				width: 150px;"
"\n				float: left;"
"\n			}"
"\n			.wrapper2"
"\n			{"
"\n				width: 150px;"
"\n				float: right;"
"\n			}"
"\n			button"
"\n			{"
"\n				display: block;"
"\n				height: 70px;"
"\n				width: 100px;"
"\n				padding: 0;"
"\n				margin: 20px auto;"
"\n			}"
"\n			.ON"
"\n			{"
"\n				-moz-box-shadow: 0 0 10px 2px #FFFF00;"
"\n				-webkit-box-shadow: 0 0 10px 2px #FFFF00;"
"\n				box-shadow: 0 0 10px 2px #FFFF00;"
"\n			}"
"\n			.OFF"
"\n			{"
"\n				-moz-box-shadow: 0 0 10px 2px #000000;"
"\n				-webkit-box-shadow: 0 0 10px 2px #000000;"
"\n				box-shadow: 0 0 10px 2px #000000;"
"\n			}"
"\n		</style>"
"\n		<script src='http://ajax.googleapis.com/ajax/libs/jquery/2.1.1/jquery.min.js'></script>"
"\n	</head>"
"\n	<body>"
"\n	<h1>Arduino Relays</h1>"
"\n	<div id='switchForm'>"
"\n		<div class='wrapper' id='switchOne-button-wrapper'>"
"\n			<button id='swOne-on-button'>Switch 1 ON</button>"
"\n			<button id='swOne-off-button'>Swich 1 OFF</button>"
"\n		</div>"
"\n		<div class='wrapper2' id='switchTwo-button-wrapper'>"
"\n			<button id='swTwo-on-button'>Switch 2 ON</button>"
"\n			<button id='swTwo-off-button'>Swich 2 OFF</button>"
"\n		</div>"
"\n		<div class='wrapper' id='switchThree-button-wrapper'>"
"\n			<button id='swThree-on-button'>Switch 3 ON</button>"
"\n			<button id='swThree-off-button'>Swich 3 OFF</button>"
"\n		</div>"
"\n		<div class='wrapper2' id='switchFour-button-wrapper'>"
"\n			<button id='swFour-on-button'>Switch 4 ON</button>"
"\n			<button id='swFour-off-button'>Swich 4 OFF</button>"
"\n		</div>"
"\n	</div>"
"\n	"
"\n	<script type='text/javascript'>"
"\n		function getRelayState() {"
"\n			$.get('http://192.168.1.49/swService')"
"\n			.done(function(data) {"
"\n				handleXMLresponse(data);"
"\n			});"
"\n		}"
"\n	"
"\n		function setRelayState(sw1, sw2, sw3, sw4) {"
"\n			$.post('http://192.168.1.49/swService', {rly1: sw1, rly2: sw2, rly3: sw3, rly4: sw4})"
"\n			.done(function(data) {"
"\n				handleXMLresponse(data);"
"\n			});"
"\n		}"
"\n		"
"\n		function handleXMLresponse(data) {"
"\n			var parser = new DOMParser();"
"\n			var doc = parser.parseFromString(data, 'text/xml');"
"\n			xmlDoc = $.parseXML(data);"
"\n			$xml = $(xmlDoc);"
"\n			var rswitch1 = $xml.find('sw1').text();"
"\n			var rswitch2 = $xml.find('sw2').text();"
"\n			var rswitch3 = $xml.find('sw3').text();"
"\n			var rswitch4 = $xml.find('sw4').text();"
"\n			if (rswitch1 == '1') {"
"\n				$('#swOne-off-button').removeClass('OFF');"
"\n				$('#swOne-on-button').addClass('ON');"
"\n			}"
"\n			else if (rswitch1 == '0') {"
"\n				$('#swOne-on-button').removeClass('ON');"
"\n				$('#swOne-off-button').addClass('OFF');"
"\n			}"
"\n			"
"\n			if (rswitch2 == '1') {"
"\n				$('#swTwo-off-button').removeClass('OFF');"
"\n				$('#swTwo-on-button').addClass('ON');"
"\n			}"
"\n			else if (rswitch2 == '0') {"
"\n				$('#swTwo-on-button').removeClass('ON');"
"\n				$('#swTwo-off-button').addClass('OFF');"
"\n			}"
"\n			"
"\n			if (rswitch3 == '1') {"
"\n				$('#swThree-off-button').removeClass('OFF');"
"\n				$('#swThree-on-button').addClass('ON');"
"\n			}"
"\n			else if (rswitch3 == '0') {"
"\n				$('#swThree-on-button').removeClass('ON');"
"\n				$('#swThree-off-button').addClass('OFF');"
"\n			}"
"\n			"
"\n			if (rswitch4 == '1') {"
"\n				$('#swFour-off-button').removeClass('OFF');"
"\n				$('#swFour-on-button').addClass('ON');"
"\n			}"
"\n			else if (rswitch4 == '0') {"
"\n				$('#swFour-on-button').removeClass('ON');"
"\n				$('#swFour-off-button').addClass('OFF');"
"\n			}"
"\n		}"
"\n		;(function($) {"
"\n			$('#swOne-on-button')"
"\n			.click(function() {"
"\n			setRelayState(1, 2, 2, 2);"
"\n			});"
"\n			$('#swOne-off-button')"
"\n			.click(function() {"
"\n			setRelayState(0, 2, 2, 2);"
"\n			});"
"\n			$('#swTwo-on-button')"
"\n			.click(function() {"
"\n			setRelayState(2, 1, 2, 2);"
"\n			});"
"\n			$('#swTwo-off-button')"
"\n			.click(function() {"
"\n			setRelayState(2, 0, 2, 2);"
"\n			});"
"\n			$('#swThree-on-button')"
"\n			.click(function() {"
"\n			setRelayState(2, 2, 1, 2);"
"\n			});"
"\n			$('#swThree-off-button')"
"\n			.click(function() {"
"\n			setRelayState(2, 2, 0, 2);"
"\n			});"
"\n			$('#swFour-on-button')"
"\n			.click(function() {"
"\n			setRelayState(2, 2, 2, 1);"
"\n			});"
"\n			$('#swFour-off-button')"
"\n			.click(function() {"
"\n				setRelayState(2, 2, 2, 0);"
"\n			});"
"\n			getRelayState();"
"\n		})(jQuery);"
"\n	</script>"
"\n	</body>"
"\n	</html>";

// Serves web front end to control light from a web browser
void webUI(WebServer &server, WebServer::ConnectionType type, char *, bool) 
{
	server.httpSuccess();
	if (type == WebServer::GET) {
		server.printP(frontEnd);
	}
}

// Provides back end to control lights from front end and other apps
void webBackend(WebServer &server, WebServer::ConnectionType type, char *, bool) 
{
	char name[16];
	char value[16];
	server.httpSuccess();
	
	if(type == WebServer::POST) {
		int colour[3];
		while (server.readPOSTparam(name, 16, value, 16)) {
			Serial.print(name);
			Serial.print(" = ");
			Serial.println(value);
			
			if(strcmp(name, "rly1") == 0) switchState[Rly1] = atoi(value);
			if(strcmp(name, "rly2") == 0) switchState[Rly2] = atoi(value);
			if(strcmp(name, "rly3") == 0) switchState[Rly3] = atoi(value);
			if(strcmp(name, "rly4") == 0) switchState[Rly4] = atoi(value);
		}
		Serial.println("-----------------------------------------");
	}
	
	// Relay 1 handle
	if (lastSwitchState[Rly1] != switchState[Rly1]) {
		if (switchState[Rly1] == 1) {
			digitalWrite(RELAY_ONE, HIGH);
			lastSwitchState[Rly1] = switchState[Rly1];
		} else if (switchState[Rly1] == 0) {
			digitalWrite(RELAY_ONE, LOW);
			lastSwitchState[Rly1] = switchState[Rly1];
		} else if (switchState[Rly1] == 2) {
			Serial.println("Relay1 = 2");
		} else {
			Serial.println("///////////////");
			Serial.println("Something went wrong with comparing Relay1's values.");
			Serial.println("///////////////");
		}
	} else if (lastSwitchState[Rly1] == switchState[Rly1]) {
		Serial.println("------------");
		Serial.println("Relay1's values are the same.");
		Serial.println("----------");
	}
	
	// Relay 2 handle
	if (lastSwitchState[Rly2] != switchState[Rly2]) {
		if (switchState[Rly2] == 1) {
			digitalWrite(RELAY_TWO, HIGH);
			lastSwitchState[Rly2] = switchState[Rly2];
		} else if (switchState[Rly2] == 0) {
			digitalWrite(RELAY_TWO, LOW);
			lastSwitchState[Rly2] = switchState[Rly2];
		} else if (switchState[Rly2] == 2) {
			Serial.println("Relay2 = 2");
		} else {
			Serial.println("///////////////");
			Serial.println("Something went wrong with comparing Relay2's values.");
			Serial.println("///////////////");
		}
	} else if (lastSwitchState[Rly2] == switchState[Rly2]) {
		Serial.println("----------");
		Serial.println("Relay2's values are the same.");
		Serial.println("----------");
	}
	
	// Relay 3 handle
	if (lastSwitchState[Rly3] != switchState[Rly3]) {
		if (switchState[Rly3] == 1) {
			digitalWrite(RELAY_THREE, HIGH);
			lastSwitchState[Rly3] = switchState[Rly3];
		} else if (switchState[Rly3] == 0) {
			digitalWrite(RELAY_THREE, LOW);
			lastSwitchState[Rly3] = switchState[Rly3];
		} else if (switchState[Rly3] == 2) {
			Serial.println("Relay3 = 2");
		} else {
			Serial.println("///////////////");
			Serial.println("Something went wrong with comparing Relay3's values.");
			Serial.println("///////////////");
		}
	} else if (lastSwitchState[Rly3] == switchState[Rly3]) {
		Serial.println("----------");
		Serial.println("Relay3's values are the same.");
		Serial.println("----------");
	}
	
	// Relay 4 handle
	if (lastSwitchState[Rly4] != switchState[Rly4]) {
		if (switchState[Rly4] == 1) {
			digitalWrite(RELAY_FOUR, HIGH);
			lastSwitchState[Rly4] = switchState[Rly4];
		} else if (switchState[Rly4] == 0) {
			digitalWrite(RELAY_FOUR, LOW);
			lastSwitchState[Rly4] = switchState[Rly4];
		} else if (switchState[Rly4] == 2) {
			Serial.println("*********************");
			Serial.println("Relay4 = 2");
			Serial.println("*********************");
		} else {
			Serial.println("///////////////");
			Serial.println("Something went wrong with comparing Relay4's values.");
			Serial.println("///////////////");
		}
	} else if (lastSwitchState[Rly4] == switchState[Rly4]) {
		Serial.println("------------");
		Serial.println("Relay4's values are the same.");
		Serial.println("----------");
	}
	
	if((type == WebServer::POST) || (type == WebServer::GET)) {
		server.println("<?xml version='1.0' encoding='UTF-8'?>");
		server.println("<xml>");
		server.println("<currentSwitchState>");
		server.print("<sw1>");
		server.print(lastSwitchState[Rly1]);
		server.println("</sw1>");
		server.print("<sw2>");
		server.print(lastSwitchState[Rly2]);
		server.println("</sw2>");
		server.print("<sw3>");
		server.print(lastSwitchState[Rly3]);
		server.println("</sw3>");
		server.print("<sw4>");
		server.print(lastSwitchState[Rly4]);
		server.println("</sw4>");
		server.println("</currentSwitchState>");
		server.println("</xml>");
	}
}

void setup() 
{
	Serial.begin(115200);
	
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);
	
	pinMode(RELAY_ONE, OUTPUT);
	pinMode(RELAY_TWO, OUTPUT);
	pinMode(RELAY_THREE, OUTPUT);
	pinMode(RELAY_FOUR, OUTPUT);
	
	digitalWrite(RELAY_ONE, LOW);
	digitalWrite(RELAY_TWO, LOW);
	digitalWrite(RELAY_THREE, LOW);
	digitalWrite(RELAY_FOUR, LOW);
	
	Ethernet.begin(MAC, IP);
	
	webserver.setDefaultCommand(&webUI);
	webserver.addCommand("index", &webUI);
	webserver.addCommand("swService", &webBackend);
	webserver.begin();
}

void loop() 
{
	char buff[64];
	int len = 64;
	webserver.processConnection(buff, &len);
}
