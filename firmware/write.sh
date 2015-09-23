#!/bin/bash
sudo st-flash erase
sudo st-flash --reset write ./program.bin 0x08000000
