#!/bin/sh

IPADDR=192.168.1.219

curl -v --request POST --data '{"length": 60.0, "loop":true, "tracks": [
    {"type": "fastled", "indexes":[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24], "frames": [
        { "effect":"rainbow", "time":0.0, "speed":80, "size":10, "offset":1 }
    ] }
] }' http://$IPADDR/api/sequence?id=1

curl -v --request POST --data '{"length": 60.0, "loop":true, "tracks": [
    {"type": "fastled", "indexes":[0, 3, 4, 9, 16, 17, 20], "frames": [
        { "effect":"rainbow", "time":0.0, "speed":50, "size":15, "offset":1 }
    ] },
    {"type": "fastled", "indexes":[1, 5, 8, 10, 15, 18, 21, 23], "frames": [
        { "effect":"rainbow", "time":0.0, "speed":50, "size":15, "offset":2 }
    ] },
    {"type": "fastled", "indexes":[2, 6, 7, 11, 14, 19, 22], "frames": [
        { "effect":"rainbow", "time":0.0, "speed":50, "size":15, "offset":3 }
    ] },
    {"type": "fastled", "indexes":[12], "frames": [
        { "effect":"rainbow", "time":0.0, "speed":50, "size":15, "offset":4 }
    ] },
    {"type": "fastled", "indexes":[13], "frames": [
        { "effect":"rainbow", "time":0.0, "speed":50, "size":15, "offset":5 }
    ] }
] }' http://$IPADDR/api/sequence?id=2

curl -v --request POST --data '{"length": 60.0, "loop":true, "tracks": [
    {"type": "fastled", "indexes":[0, 1, 2], "frames": [
        { "effect":"rainbow", "time":0.0, "speed":50, "size":15, "offset":1 }
    ] },
    {"type": "fastled", "indexes":[3, 4, 5, 6], "frames": [
        { "effect":"rainbow", "time":0.0, "speed":50, "size":15, "offset":2 }
    ] },
    {"type": "fastled", "indexes":[7, 8, 9], "frames": [
        { "effect":"rainbow", "time":0.0, "speed":50, "size":15, "offset":3 }
    ] },
    {"type": "fastled", "indexes":[10, 11, 12, 13], "frames": [
        { "effect":"rainbow", "time":0.0, "speed":50, "size":15, "offset":4 }
    ] },
    {"type": "fastled", "indexes":[14, 15, 16], "frames": [
        { "effect":"rainbow", "time":0.0, "speed":50, "size":15, "offset":5 }
    ] },
    {"type": "fastled", "indexes":[17, 18, 19], "frames": [
        { "effect":"rainbow", "time":0.0, "speed":50, "size":15, "offset":5 }
    ] },
    {"type": "fastled", "indexes":[20, 21, 22], "frames": [
        { "effect":"rainbow", "time":0.0, "speed":50, "size":15, "offset":5 }
    ] },
    {"type": "fastled", "indexes":[23], "frames": [
        { "effect":"rainbow", "time":0.0, "speed":50, "size":15, "offset":5 }
    ] }
] }' http://$IPADDR/api/sequence?id=3

curl -v --request POST --data '{"length": 60.0, "loop":true, "tracks": [
    {"type": "fastled", "indexes":[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24], "frames": [
        { "effect":"flame", "color":"#ff5500", "time":0.0, "shrink":0.1, "flash":0.1 }
    ] }
] }' http://$IPADDR/api/sequence?id=4

curl -v --request POST --data '{"length": 60.0, "loop":true, "tracks": [
    {"type": "fastled", "indexes":[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24], "frames": [
        { "color":"#ffffff", "time":0.0 }
    ] },
    {"type": "led1", "frames": [
        { "value":1.0, "time":0.0 }
    ] }
] }' http://$IPADDR/api/sequence?id=5

curl -v --request POST --data '{"length": 60.0, "loop":true, "tracks": [
    {"type": "fastled", "indexes":[0, 3, 4, 9, 16, 17, 20], "frames": [
        { "effect":"rainbow", "time":0.0, "speed":60, "size":30, "offset":5 }
    ] },
    {"type": "fastled", "indexes":[1, 5, 8, 10, 15, 18, 21, 23], "frames": [
        { "effect":"rainbow", "time":0.0, "speed":60, "size":30, "offset":4 }
    ] },
    {"type": "fastled", "indexes":[2, 6, 7, 11, 14, 19, 22], "frames": [
        { "effect":"rainbow", "time":0.0, "speed":60, "size":30, "offset":3 }
    ] },
    {"type": "fastled", "indexes":[12], "frames": [
        { "effect":"rainbow", "time":0.0, "speed":60, "size":30, "offset":2 }
    ] },
    {"type": "fastled", "indexes":[13], "frames": [
        { "effect":"rainbow", "time":0.0, "speed":60, "size":30, "offset":1 }
    ] }
] }' http://$IPADDR/api/sequence?id=6


sleep 1

curl -v --request POST http://$IPADDR/api/play?id=1
