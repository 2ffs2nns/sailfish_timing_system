#!/bin/bash
#
curl http://10.3.141.1:8000/results -H "Content-Type: plain/text" -d '{"1": "0:54:39"}' -X POST -v
sleep 1
curl http://10.3.141.1:8000/results -H "Content-Type: plain/text" -d '{"2": "0:55:17"}' -X POST -v
sleep 1
curl http://10.3.141.1:8000/results -H "Content-Type: plain/text" -d '{"3": "0:60:69"}' -X POST -v
curl http://10.3.141.1:8000/results -H "Content-Type: plain/text" -d '{"4": "0:53:89"}' -X POST -v
curl http://10.3.141.1:8000/results -H "Content-Type: plain/text" -d '{"5": "0:53:90"}' -X POST -v
curl http://10.3.141.1:8000/results -H "Content-Type: plain/text" -d '{"6": "0:53:92"}' -X POST -v
