"use strict";

Pebble.addEventListener("ready",
    function(e) {
        console.log("PebbleKit JS ready!");
        sendWeather();
    });

Pebble.addEventListener("appmessage",
    function(e) {
        console.log("message received!");
        sendWeather();
    });

function sendWeather() {

    var method = "POST";
    var url = "http://pogoda.ngs.ru/json/";

    // Create the request
    var request = new XMLHttpRequest();

    // Specify the callback for when the request is completed
    request.onload = function() {
        // The request was successfully completed!
        // console.log('Got response: ' + this.responseText);
        var json = JSON.parse(this.responseText);

        var weatherData = json.result.temp_current_c;
        var trend = "";
        if (json.result.temp_current_trend === 1) {
            trend = " /";
        }

        if (json.result.temp_current_trend === -1) {
            trend = " \\";
        }
        
        var weatherDict = { 
            "weather": weatherData + trend,
            "conditions": json.result.cloud_title + ", " + json.result.precip_title
        };

        Pebble.sendAppMessage(weatherDict);

    };

    // Send the request
    request.open(method, url);
    
    var reqObj = {
        method: "getForecast",
        params: { name:"current", city: "nsk"}
    };

    var req = JSON.stringify(reqObj);

    request.send(req);

}
