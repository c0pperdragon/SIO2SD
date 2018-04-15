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
LIBS:components
LIBS:sio2sd-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "SIO2SD single board"
Date ""
Rev "1"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ATMEGA328PB-A IC1
U 1 1 5ACA2866
P 3500 3100
F 0 "IC1" H 3100 4575 50  0000 R BNN
F 1 "ATMEGA328PB-A" H 3100 4500 50  0000 R BNN
F 2 "Housings_QFP:TQFP-32_7x7mm_Pitch0.8mm" H 3450 1700 50  0001 L CIN
F 3 "" H 3200 3150 50  0000 C CNN
	1    3500 3100
	1    0    0    -1  
$EndComp
$Comp
L LED2DIGIT LED1
U 1 1 5ACA39B2
P 9150 2000
F 0 "LED1" H 9150 2600 50  0000 C CNN
F 1 "LED2DIGIT" H 9150 1450 50  0000 C CNN
F 2 "footprints:led2digit" H 9150 2000 50  0001 C CNN
F 3 "" H 9150 2000 50  0000 C CNN
	1    9150 2000
	1    0    0    -1  
$EndComp
$Comp
L LM1117IMPX-3.3 REG1
U 1 1 5ACA3AE1
P 2300 6300
F 0 "REG1" H 1950 6250 60  0000 C CNN
F 1 "LM1117IMPX-3.3" H 2150 6350 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-223" V 2400 5900 60  0001 C CNN
F 3 "" V 2400 5900 60  0001 C CNN
	1    2300 6300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR01
U 1 1 5ACA3B60
P 1500 6400
F 0 "#PWR01" H 1500 6150 50  0001 C CNN
F 1 "GND" H 1500 6250 50  0000 C CNN
F 2 "" H 1500 6400 50  0000 C CNN
F 3 "" H 1500 6400 50  0000 C CNN
	1    1500 6400
	-1   0    0    1   
$EndComp
$Comp
L +5V #PWR02
U 1 1 5ACA3B7A
P 1500 6850
F 0 "#PWR02" H 1500 6700 50  0001 C CNN
F 1 "+5V" H 1500 6990 50  0000 C CNN
F 2 "" H 1500 6850 50  0000 C CNN
F 3 "" H 1500 6850 50  0000 C CNN
	1    1500 6850
	-1   0    0    1   
$EndComp
$Comp
L +3.3V #PWR03
U 1 1 5ACA3C07
P 2650 6500
F 0 "#PWR03" H 2650 6350 50  0001 C CNN
F 1 "+3.3V" H 2650 6640 50  0000 C CNN
F 2 "" H 2650 6500 50  0000 C CNN
F 3 "" H 2650 6500 50  0000 C CNN
	1    2650 6500
	1    0    0    -1  
$EndComp
$Comp
L C_Small C1
U 1 1 5ACA3C2B
P 2650 6900
F 0 "C1" H 2660 6970 50  0000 L CNN
F 1 "10uF" H 2660 6820 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 2650 6900 50  0001 C CNN
F 3 "" H 2650 6900 50  0000 C CNN
	1    2650 6900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR04
U 1 1 5ACA3C8F
P 2650 7100
F 0 "#PWR04" H 2650 6850 50  0001 C CNN
F 1 "GND" H 2650 6950 50  0000 C CNN
F 2 "" H 2650 7100 50  0000 C CNN
F 3 "" H 2650 7100 50  0000 C CNN
	1    2650 7100
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X05 P2
U 1 1 5ACA4270
P 6950 3600
F 0 "P2" H 6950 3900 50  0000 C CNN
F 1 "SIO" V 7050 3600 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Angled_1x05_Pitch2.54mm" H 6950 3600 50  0001 C CNN
F 3 "" H 6950 3600 50  0000 C CNN
	1    6950 3600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR05
U 1 1 5ACA466E
P 6700 3950
F 0 "#PWR05" H 6700 3700 50  0001 C CNN
F 1 "GND" H 6700 3800 50  0000 C CNN
F 2 "" H 6700 3950 50  0000 C CNN
F 3 "" H 6700 3950 50  0000 C CNN
	1    6700 3950
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR06
U 1 1 5ACA469B
P 6700 3200
F 0 "#PWR06" H 6700 3050 50  0001 C CNN
F 1 "+5V" H 6700 3340 50  0000 C CNN
F 2 "" H 6700 3200 50  0000 C CNN
F 3 "" H 6700 3200 50  0000 C CNN
	1    6700 3200
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR07
U 1 1 5ACA48C3
P 3400 1250
F 0 "#PWR07" H 3400 1100 50  0001 C CNN
F 1 "+3.3V" H 3400 1390 50  0000 C CNN
F 2 "" H 3400 1250 50  0000 C CNN
F 3 "" H 3400 1250 50  0000 C CNN
	1    3400 1250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR08
U 1 1 5ACA4959
P 3400 4850
F 0 "#PWR08" H 3400 4600 50  0001 C CNN
F 1 "GND" H 3400 4700 50  0000 C CNN
F 2 "" H 3400 4850 50  0000 C CNN
F 3 "" H 3400 4850 50  0000 C CNN
	1    3400 4850
	1    0    0    -1  
$EndComp
Text Label 4950 3200 0    60   ~ 0
Digit0
Text Label 4950 3900 0    60   ~ 0
Digit1
Text Label 6400 3600 0    60   ~ 0
RX
Text Label 6400 3500 0    60   ~ 0
TX
Text Label 6400 3700 0    60   ~ 0
CMD
$Comp
L R R2
U 1 1 5ACA4C04
P 5150 2800
F 0 "R2" V 5230 2800 50  0000 C CNN
F 1 "1k" V 5150 2800 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 5080 2800 50  0001 C CNN
F 3 "" H 5150 2800 50  0000 C CNN
	1    5150 2800
	0    1    1    0   
$EndComp
Text Label 5600 2800 0    60   ~ 0
Seg_A0
$Comp
L R R7
U 1 1 5ACA4C9A
P 5400 2900
F 0 "R7" V 5480 2900 50  0000 C CNN
F 1 "1k" V 5400 2900 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 5330 2900 50  0001 C CNN
F 3 "" H 5400 2900 50  0000 C CNN
	1    5400 2900
	0    1    1    0   
$EndComp
Text Label 5600 2900 0    60   ~ 0
Seg_A1
$Comp
L R R3
U 1 1 5ACA4FD7
P 5150 3000
F 0 "R3" V 5230 3000 50  0000 C CNN
F 1 "1k" V 5150 3000 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 5080 3000 50  0001 C CNN
F 3 "" H 5150 3000 50  0000 C CNN
	1    5150 3000
	0    1    1    0   
$EndComp
Text Label 5600 3000 0    60   ~ 0
Seg_A2
$Comp
L R R8
U 1 1 5ACA5072
P 5400 3100
F 0 "R8" V 5480 3100 50  0000 C CNN
F 1 "1k" V 5400 3100 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 5330 3100 50  0001 C CNN
F 3 "" H 5400 3100 50  0000 C CNN
	1    5400 3100
	0    1    1    0   
$EndComp
Text Label 5600 3100 0    60   ~ 0
Seg_A3
$Comp
L R R4
U 1 1 5ACA5329
P 5150 3300
F 0 "R4" V 5230 3300 50  0000 C CNN
F 1 "1k" V 5150 3300 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 5080 3300 50  0001 C CNN
F 3 "" H 5150 3300 50  0000 C CNN
	1    5150 3300
	0    1    1    0   
$EndComp
Text Label 5600 3300 0    60   ~ 0
Seg_A5
$Comp
L R R5
U 1 1 5ACA63FC
P 5150 4000
F 0 "R5" V 5230 4000 50  0000 C CNN
F 1 "1k" V 5150 4000 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 5080 4000 50  0001 C CNN
F 3 "" H 5150 4000 50  0000 C CNN
	1    5150 4000
	0    1    1    0   
$EndComp
Text Label 5600 4000 0    60   ~ 0
Seg_4
$Comp
L R R9
U 1 1 5ACA67E5
P 5400 4100
F 0 "R9" V 5480 4100 50  0000 C CNN
F 1 "1k" V 5400 4100 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 5330 4100 50  0001 C CNN
F 3 "" H 5400 4100 50  0000 C CNN
	1    5400 4100
	0    1    1    0   
$EndComp
Text Label 5600 4100 0    60   ~ 0
Seg_5
$Comp
L R R6
U 1 1 5ACA6904
P 5150 4200
F 0 "R6" V 5230 4200 50  0000 C CNN
F 1 "1k" V 5150 4200 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 5080 4200 50  0001 C CNN
F 3 "" H 5150 4200 50  0000 C CNN
	1    5150 4200
	0    1    1    0   
$EndComp
Text Label 5600 4200 0    60   ~ 0
Seg_6
$Comp
L GND #PWR09
U 1 1 5ACA6C0D
P 5200 1550
F 0 "#PWR09" H 5200 1300 50  0001 C CNN
F 1 "GND" H 5200 1400 50  0000 C CNN
F 2 "" H 5200 1550 50  0000 C CNN
F 3 "" H 5200 1550 50  0000 C CNN
	1    5200 1550
	-1   0    0    1   
$EndComp
Text Label 7950 1500 0    60   ~ 0
Digit1
Text Label 10150 1500 0    60   ~ 0
Digit0
Text Label 10150 1700 0    60   ~ 0
Seg_A1
Text Label 10150 1800 0    60   ~ 0
Seg_A0
Text Label 10150 1900 0    60   ~ 0
Seg_A2
Text Label 10150 2000 0    60   ~ 0
Seg_6
Text Label 10150 2100 0    60   ~ 0
Seg_A5
Text Label 10150 2200 0    60   ~ 0
Seg_4
Text Label 10150 2300 0    60   ~ 0
Seg_5
Text Label 10150 2400 0    60   ~ 0
Seg_A3
Text Label 7950 1700 0    60   ~ 0
Seg_4
Text Label 7950 1800 0    60   ~ 0
Seg_5
Text Label 7950 1900 0    60   ~ 0
Seg_A5
Text Label 7950 2000 0    60   ~ 0
Seg_A0
Text Label 7950 2100 0    60   ~ 0
Seg_A1
Text Label 7950 2200 0    60   ~ 0
Seg_A3
Text Label 7950 2300 0    60   ~ 0
Seg_A2
Text Label 7950 2400 0    60   ~ 0
Seg_6
$Comp
L SW_PUSH SW1
U 1 1 5ACA86A1
P 5250 4900
F 0 "SW1" H 5400 5010 50  0000 C CNN
F 1 "UP" H 5250 4820 50  0000 C CNN
F 2 "footprints:button" H 5250 4900 50  0001 C CNN
F 3 "" H 5250 4900 50  0000 C CNN
	1    5250 4900
	1    0    0    -1  
$EndComp
$Comp
L SW_PUSH SW2
U 1 1 5ACA89B5
P 5250 5200
F 0 "SW2" H 5400 5310 50  0000 C CNN
F 1 "DOWN" H 5250 5120 50  0000 C CNN
F 2 "footprints:button" H 5250 5200 50  0001 C CNN
F 3 "" H 5250 5200 50  0000 C CNN
	1    5250 5200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR010
U 1 1 5ACA8D99
P 5650 5350
F 0 "#PWR010" H 5650 5100 50  0001 C CNN
F 1 "GND" H 5650 5200 50  0000 C CNN
F 2 "" H 5650 5350 50  0000 C CNN
F 3 "" H 5650 5350 50  0000 C CNN
	1    5650 5350
	1    0    0    -1  
$EndComp
Wire Wire Line
	1500 6400 1500 6550
Wire Wire Line
	1500 6550 1700 6550
Wire Wire Line
	1500 6850 1500 6750
Wire Wire Line
	1500 6750 1700 6750
Wire Wire Line
	2500 6650 3100 6650
Wire Wire Line
	2650 6500 2650 6800
Connection ~ 2650 6650
Wire Wire Line
	2650 7000 2650 7100
Wire Wire Line
	6700 3950 6700 3800
Wire Wire Line
	6400 3800 6750 3800
Wire Wire Line
	6400 3400 6750 3400
Wire Wire Line
	6700 3200 6700 3400
Wire Wire Line
	4800 3600 6750 3600
Wire Wire Line
	5700 3500 6750 3500
Wire Wire Line
	5700 3500 5700 3700
Wire Wire Line
	5700 3700 4800 3700
Wire Wire Line
	3400 1250 3400 1600
Wire Wire Line
	3200 1400 3200 1600
Wire Wire Line
	3200 1400 3400 1400
Connection ~ 3400 1400
Wire Wire Line
	3400 4600 3400 4850
Wire Wire Line
	3300 4600 3300 4700
Wire Wire Line
	3300 4700 3400 4700
Connection ~ 3400 4700
Wire Wire Line
	4800 3800 5800 3800
Wire Wire Line
	5800 3800 5800 3700
Wire Wire Line
	5800 3700 6750 3700
Wire Wire Line
	4800 2800 5000 2800
Wire Wire Line
	5300 2800 5600 2800
Wire Wire Line
	4800 2900 5250 2900
Wire Wire Line
	5550 2900 5600 2900
Wire Wire Line
	4800 3000 5000 3000
Wire Wire Line
	5300 3000 5600 3000
Wire Wire Line
	4800 3100 5250 3100
Wire Wire Line
	4800 3200 4950 3200
Wire Wire Line
	4800 3900 4950 3900
Wire Wire Line
	5550 3100 5600 3100
Wire Wire Line
	4800 3300 5000 3300
Wire Wire Line
	5300 3300 5600 3300
Wire Wire Line
	1700 6650 1700 7000
Wire Wire Line
	1700 7000 2550 7000
Wire Wire Line
	2550 7000 2550 6650
Connection ~ 2550 6650
Wire Wire Line
	4800 4000 5000 4000
Wire Wire Line
	5300 4000 5600 4000
Wire Wire Line
	4800 4100 5250 4100
Wire Wire Line
	5550 4100 5600 4100
Wire Wire Line
	4800 4200 5000 4200
Wire Wire Line
	5300 4200 5600 4200
Wire Wire Line
	4800 2000 5200 2000
Wire Wire Line
	5200 2000 5200 1550
Wire Wire Line
	7950 1500 8400 1500
Wire Wire Line
	8300 1600 8200 1600
Wire Wire Line
	8200 1600 8200 1500
Connection ~ 8200 1500
Wire Wire Line
	9900 1500 10150 1500
Wire Wire Line
	10000 1600 10050 1600
Wire Wire Line
	10050 1600 10050 1500
Connection ~ 10050 1500
Wire Wire Line
	10000 1700 10150 1700
Wire Wire Line
	10000 1800 10150 1800
Wire Wire Line
	10000 1900 10150 1900
Wire Wire Line
	10000 2000 10150 2000
Wire Wire Line
	10000 2100 10150 2100
Wire Wire Line
	10000 2200 10150 2200
Wire Wire Line
	10000 2300 10150 2300
Wire Wire Line
	9900 2400 10150 2400
Wire Wire Line
	7950 1700 8300 1700
Wire Wire Line
	7950 1800 8300 1800
Wire Wire Line
	7950 1900 8300 1900
Wire Wire Line
	7950 2000 8300 2000
Wire Wire Line
	7950 2100 8300 2100
Wire Wire Line
	7950 2200 8300 2200
Wire Wire Line
	7950 2300 8300 2300
Wire Wire Line
	7950 2400 8400 2400
Wire Wire Line
	4800 4300 4900 4300
Wire Wire Line
	4900 4300 4900 4900
Wire Wire Line
	4900 4900 4950 4900
Wire Wire Line
	5550 5200 5650 5200
Wire Wire Line
	5650 4900 5650 5350
Wire Wire Line
	5550 4900 5650 4900
Connection ~ 5650 5200
Wire Wire Line
	4800 1900 4850 1900
Wire Wire Line
	4850 1900 4850 5200
Wire Wire Line
	4850 5200 4950 5200
Wire Wire Line
	2200 2200 2150 2200
NoConn ~ 4800 2500
NoConn ~ 4800 2600
NoConn ~ 2200 3700
NoConn ~ 2200 3800
NoConn ~ 2200 3900
NoConn ~ 2200 4000
Text Label 5200 1750 0    60   ~ 0
LEDType_Common_Anode
Text Label 5300 2100 0    60   ~ 0
ChipSelectSD
Text Label 5300 2200 0    60   ~ 0
MOSI
Text Label 5300 2300 0    60   ~ 0
MISO
Text Label 5300 2400 0    60   ~ 0
SCK
Wire Wire Line
	4800 2100 5300 2100
Wire Wire Line
	4800 2200 5300 2200
Wire Wire Line
	4800 2300 5300 2300
Wire Wire Line
	4800 2400 5300 2400
$Comp
L SD_Card CON1
U 1 1 5ACAA818
P 8900 5300
F 0 "CON1" H 8250 5850 50  0000 C CNN
F 1 "SD_Card" H 9500 4750 50  0000 C CNN
F 2 "footprints:sdcardsocket" H 9100 5650 50  0000 C CNN
F 3 "" H 8900 5300 50  0000 C CNN
	1    8900 5300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR011
U 1 1 5ACAB12D
P 7800 5850
F 0 "#PWR011" H 7800 5600 50  0001 C CNN
F 1 "GND" H 7800 5700 50  0000 C CNN
F 2 "" H 7800 5850 50  0000 C CNN
F 3 "" H 7800 5850 50  0000 C CNN
	1    7800 5850
	1    0    0    -1  
$EndComp
Wire Wire Line
	7800 5200 7800 5850
Wire Wire Line
	7800 5500 8000 5500
Wire Wire Line
	7800 5200 8000 5200
Connection ~ 7800 5500
$Comp
L +3.3V #PWR012
U 1 1 5ACAB24E
P 7850 4650
F 0 "#PWR012" H 7850 4500 50  0001 C CNN
F 1 "+3.3V" H 7850 4790 50  0000 C CNN
F 2 "" H 7850 4650 50  0000 C CNN
F 3 "" H 7850 4650 50  0000 C CNN
	1    7850 4650
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 5300 7850 5300
Wire Wire Line
	7850 5300 7850 4650
Text Label 7150 5000 0    60   ~ 0
ChipSelectSD
Wire Wire Line
	7150 5000 8000 5000
Text Label 7150 5100 0    60   ~ 0
MOSI
Wire Wire Line
	7150 5100 8000 5100
Text Label 7150 5400 0    60   ~ 0
SCK
Wire Wire Line
	7150 5400 8000 5400
Text Label 7150 5600 0    60   ~ 0
MISO
Wire Wire Line
	7150 5600 8000 5600
NoConn ~ 8000 4900
NoConn ~ 8000 5700
$Comp
L C_Small C2
U 1 1 5ACAC4FC
P 3100 6900
F 0 "C2" H 3110 6970 50  0000 L CNN
F 1 "100nF" H 3110 6820 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 3100 6900 50  0001 C CNN
F 3 "" H 3100 6900 50  0000 C CNN
	1    3100 6900
	1    0    0    -1  
$EndComp
Wire Wire Line
	3100 6650 3100 6800
Wire Wire Line
	2650 7050 3100 7050
Wire Wire Line
	3100 7050 3100 7000
Connection ~ 2650 7050
Text Label 5450 3450 0    60   ~ 0
Reset
Wire Wire Line
	4800 3400 5000 3400
Wire Wire Line
	5000 3400 5000 3450
Wire Wire Line
	5000 3450 5450 3450
Text Label 4500 7000 1    60   ~ 0
MOSI
Text Label 4900 7000 1    60   ~ 0
MISO
Text Label 5250 7000 1    60   ~ 0
SCK
Text Label 5600 7000 1    60   ~ 0
Reset
NoConn ~ 2150 2200
$Comp
L PWR_FLAG #FLG013
U 1 1 5ACB20EC
P 6400 3250
F 0 "#FLG013" H 6400 3345 50  0001 C CNN
F 1 "PWR_FLAG" H 6400 3430 50  0000 C CNN
F 2 "" H 6400 3250 50  0000 C CNN
F 3 "" H 6400 3250 50  0000 C CNN
	1    6400 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	6400 3250 6400 3400
Connection ~ 6700 3400
$Comp
L PWR_FLAG #FLG014
U 1 1 5ACB2684
P 6400 3900
F 0 "#FLG014" H 6400 3995 50  0001 C CNN
F 1 "PWR_FLAG" H 6400 4080 50  0000 C CNN
F 2 "" H 6400 3900 50  0000 C CNN
F 3 "" H 6400 3900 50  0000 C CNN
	1    6400 3900
	-1   0    0    1   
$EndComp
Wire Wire Line
	6400 3800 6400 3900
Connection ~ 6700 3800
$Comp
L GND #PWR015
U 1 1 5ACBBC39
P 9950 5650
F 0 "#PWR015" H 9950 5400 50  0001 C CNN
F 1 "GND" H 9950 5500 50  0000 C CNN
F 2 "" H 9950 5650 50  0000 C CNN
F 3 "" H 9950 5650 50  0000 C CNN
	1    9950 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	9800 5400 9950 5400
Wire Wire Line
	9950 5200 9950 5650
Wire Wire Line
	9800 5500 9950 5500
Connection ~ 9950 5500
$Comp
L R_Small R1
U 1 1 5ACBC0B0
P 9800 4150
F 0 "R1" H 9830 4170 50  0000 L CNN
F 1 "220k" H 9830 4110 50  0000 L CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 9800 4150 50  0001 C CNN
F 3 "" H 9800 4150 50  0000 C CNN
	1    9800 4150
	-1   0    0    1   
$EndComp
Wire Wire Line
	9800 4250 9800 5100
$Comp
L R_Small R10
U 1 1 5ACBDB2D
P 9800 3550
F 0 "R10" H 9830 3570 50  0000 L CNN
F 1 "10k" H 9830 3510 50  0000 L CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 9800 3550 50  0001 C CNN
F 3 "" H 9800 3550 50  0000 C CNN
	1    9800 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	9800 3650 9800 4050
$Comp
L +3.3V #PWR016
U 1 1 5ACBE4CA
P 9800 3250
F 0 "#PWR016" H 9800 3100 50  0001 C CNN
F 1 "+3.3V" H 9800 3390 50  0000 C CNN
F 2 "" H 9800 3250 50  0000 C CNN
F 3 "" H 9800 3250 50  0000 C CNN
	1    9800 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	9800 3250 9800 3450
Text Label 9200 3900 0    60   ~ 0
Reset
$Comp
L TEST_1P W1
U 1 1 5ACC0912
P 4500 7100
F 0 "W1" H 4500 7370 50  0000 C CNN
F 1 "MOSI" H 4500 7300 50  0000 C CNN
F 2 "footprints:pad" H 4700 7100 50  0001 C CNN
F 3 "" H 4700 7100 50  0000 C CNN
	1    4500 7100
	-1   0    0    1   
$EndComp
$Comp
L TEST_1P W2
U 1 1 5ACC09DB
P 4900 7100
F 0 "W2" H 4900 7370 50  0000 C CNN
F 1 "MISO" H 4900 7300 50  0000 C CNN
F 2 "footprints:pad" H 5100 7100 50  0001 C CNN
F 3 "" H 5100 7100 50  0000 C CNN
	1    4900 7100
	-1   0    0    1   
$EndComp
$Comp
L TEST_1P W3
U 1 1 5ACC0A34
P 5250 7100
F 0 "W3" H 5250 7370 50  0000 C CNN
F 1 "SCK" H 5250 7300 50  0000 C CNN
F 2 "footprints:pad" H 5450 7100 50  0001 C CNN
F 3 "" H 5450 7100 50  0000 C CNN
	1    5250 7100
	-1   0    0    1   
$EndComp
$Comp
L TEST_1P W4
U 1 1 5ACC0A87
P 5600 7100
F 0 "W4" H 5600 7370 50  0000 C CNN
F 1 "Reset" H 5600 7300 50  0000 C CNN
F 2 "footprints:pad" H 5800 7100 50  0001 C CNN
F 3 "" H 5800 7100 50  0000 C CNN
	1    5600 7100
	-1   0    0    1   
$EndComp
Wire Wire Line
	4500 7100 4500 7000
Wire Wire Line
	4900 7100 4900 7000
Wire Wire Line
	5250 7100 5250 7000
Wire Wire Line
	5600 7100 5600 7000
Wire Wire Line
	9800 5200 9950 5200
Connection ~ 9950 5400
$Comp
L C_Small C3
U 1 1 5AD32337
P 10050 4150
F 0 "C3" H 10060 4220 50  0000 L CNN
F 1 "100nF" H 10060 4070 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 10050 4150 50  0001 C CNN
F 3 "" H 10050 4150 50  0000 C CNN
	1    10050 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	9200 3900 10050 3900
Wire Wire Line
	10050 3900 10050 4050
Connection ~ 9800 3900
Wire Wire Line
	10050 4250 10050 4400
Wire Wire Line
	10050 4400 9800 4400
Connection ~ 9800 4400
$EndSCHEMATC
