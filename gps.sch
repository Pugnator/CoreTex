EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:rfcom
LIBS:dc-dc
LIBS:stm32
LIBS:diy_diodes
LIBS:relays
LIBS:sensors
LIBS:diy_connectors
LIBS:con-usb
LIBS:con-usb-2
LIBS:con-usb-3
LIBS:tracker-cache
EELAYER 27 0
EELAYER END
$Descr User 5512 4016
encoding utf-8
Sheet 4 5
Title ""
Date "15 apr 2015"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CONN_01X04 P4
U 1 1 550F0DC2
P 2950 1800
F 0 "P4" H 2950 2050 50  0000 C CNN
F 1 "CONN_01X04" V 3050 1800 50  0000 C CNN
F 2 "" H 2950 1800 60  0000 C CNN
F 3 "" H 2950 1800 60  0000 C CNN
	1    2950 1800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR56
U 1 1 550F0DE1
P 2650 1750
F 0 "#PWR56" H 2650 1500 60  0001 C CNN
F 1 "GND" H 2650 1600 60  0000 C CNN
F 2 "" H 2650 1750 60  0000 C CNN
F 3 "" H 2650 1750 60  0000 C CNN
	1    2650 1750
	0    1    1    0   
$EndComp
Text GLabel 2750 1850 0    60   Input ~ 0
GPSRX
Text GLabel 2750 1950 0    60   Input ~ 0
GPSTX
Text Notes 2350 1450 0    60   ~ 0
External GPS board
$Comp
L +3V3 #PWR55
U 1 1 55208650
P 2650 1650
F 0 "#PWR55" H 2650 1500 60  0001 C CNN
F 1 "+3V3" H 2650 1790 60  0000 C CNN
F 2 "" H 2650 1650 60  0000 C CNN
F 3 "" H 2650 1650 60  0000 C CNN
	1    2650 1650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2650 1650 2750 1650
Wire Wire Line
	2750 1750 2650 1750
$EndSCHEMATC
