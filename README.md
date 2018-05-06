# SR04T_ESP8266_json_server
Get distance from a json server on ESP8266 with SR04T ultrasonic sensor.

Sample query:
curl -i http://your_ip:8080/distance // change to your IP  

response json:

{
    "count": 1,
    "distance": 251
}

count: Number of queries performed

distance: distance in cm
