#!/bin/sh

msp430-gcc -mmcu=msp430f2001 blink.c -o blink
sudo mspdebug rf2500 "prog blink"
