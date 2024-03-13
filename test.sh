#!/bin/bash
#
curl http://127.0.0.1:8000/results -H "Content-Type: plain/text" -d '{"1": "0:54:39"}' -X POST -v
sleep 1
curl http://127.0.0.1:8000/results -H "Content-Type: plain/text" -d '{"2": "0:55:17"}' -X POST -v
sleep 1
curl http://127.0.0.1:8000/results -H "Content-Type: plain/text" -d '{"3": "0:60:69"}' -X POST -v
curl http://127.0.0.1:8000/results -H "Content-Type: plain/text" -d '{"4": "0:53:89"}' -X POST -v
