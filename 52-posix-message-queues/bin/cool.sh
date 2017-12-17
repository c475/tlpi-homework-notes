#!/bin/bash

cool () {
    for ((i=0; i<=1000000; i++)); do ./52-2-client $i; done    
}

cool &
