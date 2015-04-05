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
LIBS:tracker-cache
EELAYER 27 0
EELAYER END
$Descr User 5906 4016
encoding utf-8
Sheet 5 5
Title ""
Date "5 apr 2015"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text GLabel 900  2350 0    60   Input ~ 0
RELAY
$Comp
L GND #PWR60
U 1 1 550F23FC
P 850 2650
F 0 "#PWR60" H 850 2400 60  0001 C CNN
F 1 "GND" H 850 2500 60  0000 C CNN
F 2 "" H 850 2650 60  0000 C CNN
F 3 "" H 850 2650 60  0000 C CNN
	1    850  2650
	0    1    1    0   
$EndComp
Text GLabel 2150 2750 2    60   Input ~ 0
CPS_OUT
Text GLabel 2150 2450 2    60   Input ~ 0
CPS_IN
$Comp
L 74LS240 U8
U 1 1 550F2AF1
P 4200 1700
F 0 "U8" H 4250 1500 60  0000 C CNN
F 1 "74LS240" H 4300 1300 60  0000 C CNN
F 2 "~" H 4200 1700 60  0000 C CNN
F 3 "~" H 4200 1700 60  0000 C CNN
	1    4200 1700
	1    0    0    -1  
$EndComp
NoConn ~ 4900 1700
NoConn ~ 4900 1800
NoConn ~ 4900 1900
Wire Wire Line
	4900 1500 5000 1500
Wire Wire Line
	5000 1500 5000 1200
Wire Wire Line
	5000 1200 4900 1200
Wire Wire Line
	4900 1300 5250 1300
Connection ~ 5000 1300
Wire Wire Line
	4900 1400 5000 1400
Connection ~ 5000 1400
Wire Wire Line
	3500 1200 3450 1200
Wire Wire Line
	3450 1100 3450 1500
Wire Wire Line
	3450 1500 3500 1500
Wire Wire Line
	4900 1600 5050 1600
Wire Wire Line
	5050 1600 5050 1100
Wire Wire Line
	5050 1100 3450 1100
Connection ~ 3450 1200
Wire Wire Line
	3500 1300 3450 1300
Connection ~ 3450 1300
Wire Wire Line
	3500 1400 3450 1400
Connection ~ 3450 1400
Wire Wire Line
	3500 1700 3450 1700
Wire Wire Line
	3450 1700 3450 2200
Wire Wire Line
	3450 2200 3500 2200
Wire Wire Line
	3400 1900 3500 1900
Connection ~ 3450 1900
Wire Wire Line
	3500 1800 3450 1800
Connection ~ 3450 1800
Wire Wire Line
	3500 2100 3450 2100
Connection ~ 3450 2100
$Comp
L GND #PWR61
U 1 1 550F3174
P 3400 1900
F 0 "#PWR61" H 3400 1650 60  0001 C CNN
F 1 "GND" H 3400 1750 60  0000 C CNN
F 2 "" H 3400 1900 60  0000 C CNN
F 3 "" H 3400 1900 60  0000 C CNN
	1    3400 1900
	0    1    1    0   
$EndComp
$Comp
L ANTENNA ANT2
U 1 1 550F3575
P 5250 850
F 0 "ANT2" H 5250 500 60  0000 C CNN
F 1 "ANTENNA" H 5000 750 60  0000 C CNN
F 2 "~" H 5250 850 60  0000 C CNN
F 3 "~" H 5250 850 60  0000 C CNN
	1    5250 850 
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 1300 5250 1100
$Comp
L OPTO-TRANSISTOR U?
U 1 1 5521100B
P 1550 2550
F 0 "U?" H 1750 2850 60  0000 C CNN
F 1 "OPTO-TRANSISTOR" H 1650 2250 60  0000 C CNN
F 2 "" H 1550 2550 60  0000 C CNN
F 3 "" H 1550 2550 60  0000 C CNN
	1    1550 2550
	1    0    0    -1  
$EndComp
NoConn ~ 2150 2350
Wire Wire Line
	850  2650 900  2650
Text GLabel 3500 1600 0    60   Input ~ 0
SENDER
$EndSCHEMATC
