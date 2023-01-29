#!/bin/bash

g++ "$1" libportaudio.a -lrt -lm -lasound -ljack -pthread -o "$2"
