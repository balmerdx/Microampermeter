EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L MCU_ST_STM32H7:STM32H753VITx U4
U 1 1 5D8E6AB4
P 3750 3850
F 0 "U4" H 3700 4350 50  0000 C CNN
F 1 "STM32H750VITx" H 3700 4200 50  0000 C CNN
F 2 "Package_QFP:LQFP-100_14x14mm_P0.5mm" H 3050 1250 50  0001 R CNN
F 3 "http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00388325.pdf" H 3750 3850 50  0001 C CNN
	1    3750 3850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR037
U 1 1 5D8F2993
P 3450 6650
F 0 "#PWR037" H 3450 6400 50  0001 C CNN
F 1 "GND" H 3455 6477 50  0001 C CNN
F 2 "" H 3450 6650 50  0001 C CNN
F 3 "" H 3450 6650 50  0001 C CNN
	1    3450 6650
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 6650 3550 6650
Connection ~ 3450 6650
Wire Wire Line
	3550 6650 3650 6650
Connection ~ 3550 6650
Wire Wire Line
	3650 6650 3750 6650
Connection ~ 3650 6650
Wire Wire Line
	3750 6650 3850 6650
Connection ~ 3750 6650
Wire Wire Line
	3850 6650 3950 6650
Connection ~ 3850 6650
$Comp
L Device:C C7
U 1 1 5D8F6B4F
P 2400 1550
F 0 "C7" H 2515 1596 50  0000 L CNN
F 1 "100n" H 2515 1505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 2438 1400 50  0001 C CNN
F 3 "~" H 2400 1550 50  0001 C CNN
	1    2400 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	2400 1400 2850 1400
Wire Wire Line
	2850 1400 2850 1450
Wire Wire Line
	2400 1700 2850 1700
Wire Wire Line
	2850 1700 2850 1650
Wire Wire Line
	3950 1150 3850 1150
Wire Wire Line
	3850 1150 3750 1150
Connection ~ 3850 1150
Wire Wire Line
	3750 1150 3650 1150
Connection ~ 3750 1150
Wire Wire Line
	3650 1150 3550 1150
Connection ~ 3650 1150
$Comp
L power:+3.3V #PWR08
U 1 1 5D8F8C9F
P 3650 1050
F 0 "#PWR08" H 3650 900 50  0001 C CNN
F 1 "+3.3V" H 3665 1223 50  0000 C CNN
F 2 "" H 3650 1050 50  0001 C CNN
F 3 "" H 3650 1050 50  0001 C CNN
	1    3650 1050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 1050 3650 1150
$Comp
L power:GND #PWR021
U 1 1 5D8FAD92
P 2050 2500
F 0 "#PWR021" H 2050 2250 50  0001 C CNN
F 1 "GND" H 2055 2327 50  0001 C CNN
F 2 "" H 2050 2500 50  0001 C CNN
F 3 "" H 2050 2500 50  0001 C CNN
	1    2050 2500
	1    0    0    -1  
$EndComp
$Comp
L Device:C C10
U 1 1 5D9000C5
P 2050 2350
F 0 "C10" H 2165 2396 50  0000 L CNN
F 1 "10u" H 2165 2305 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric" H 2088 2200 50  0001 C CNN
F 3 "~" H 2050 2350 50  0001 C CNN
	1    2050 2350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C9
U 1 1 5D9018C1
P 1700 2350
F 0 "C9" H 1815 2396 50  0000 L CNN
F 1 "10u" H 1815 2305 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric" H 1738 2200 50  0001 C CNN
F 3 "~" H 1700 2350 50  0001 C CNN
	1    1700 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	2050 2500 1700 2500
Connection ~ 2050 2500
$Comp
L power:GND #PWR016
U 1 1 5D905164
P 2400 1700
F 0 "#PWR016" H 2400 1450 50  0001 C CNN
F 1 "GND" H 2405 1527 50  0001 C CNN
F 2 "" H 2400 1700 50  0001 C CNN
F 3 "" H 2400 1700 50  0001 C CNN
	1    2400 1700
	1    0    0    -1  
$EndComp
Connection ~ 2400 1700
$Comp
L ampermeter-rescue:USB_B_Micro-Connector J2
U 1 1 5D907AF4
P 9650 1100
F 0 "J2" H 9707 1567 50  0000 C CNN
F 1 "USB_B_Micro" H 9707 1476 50  0000 C CNN
F 2 "Connector_USB:USB_Micro-B_Molex-105017-0001" H 9800 1050 50  0001 C CNN
F 3 "~" H 9800 1050 50  0001 C CNN
	1    9650 1100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR015
U 1 1 5D91031E
P 9550 1500
F 0 "#PWR015" H 9550 1250 50  0001 C CNN
F 1 "GND" H 9555 1327 50  0001 C CNN
F 2 "" H 9550 1500 50  0001 C CNN
F 3 "" H 9550 1500 50  0001 C CNN
	1    9550 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	9650 1500 9550 1500
Connection ~ 9550 1500
Text Label 2500 1400 0    50   ~ 0
NRST
Text Label 4750 2750 0    50   ~ 0
SWD_IO
Text Label 4750 2850 0    50   ~ 0
SWD_SCK
Wire Wire Line
	4650 2750 5000 2750
Wire Wire Line
	4650 2850 5100 2850
Wire Wire Line
	3550 1150 3450 1150
Connection ~ 3550 1150
$Comp
L balmer:ISL9204 U1
U 1 1 5D8F80B2
P 6900 1200
F 0 "U1" H 6900 1575 50  0000 C CNN
F 1 "ISL9204" H 6900 1484 50  0000 C CNN
F 2 "balmer:DFN-8-1EP_3x2mm_P0.5mm_EP1.75x1.65mm" H 7050 750 50  0001 C CNN
F 3 "" H 6850 1250 50  0001 C CNN
	1    6900 1200
	1    0    0    -1  
$EndComp
$Comp
L power:+5VP #PWR03
U 1 1 5D8FBF5D
P 6400 800
F 0 "#PWR03" H 6400 650 50  0001 C CNN
F 1 "+5VP" H 6415 973 50  0000 C CNN
F 2 "" H 6400 800 50  0001 C CNN
F 3 "" H 6400 800 50  0001 C CNN
	1    6400 800 
	1    0    0    -1  
$EndComp
Wire Wire Line
	6450 1050 6400 1050
$Comp
L power:+BATT #PWR010
U 1 1 5D8FD273
P 8950 850
F 0 "#PWR010" H 8950 700 50  0001 C CNN
F 1 "+BATT" H 8965 1023 50  0000 C CNN
F 2 "" H 8950 850 50  0001 C CNN
F 3 "" H 8950 850 50  0001 C CNN
	1    8950 850 
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR014
U 1 1 5D8FE884
P 7400 1500
F 0 "#PWR014" H 7400 1250 50  0001 C CNN
F 1 "GND" H 7405 1327 50  0001 C CNN
F 2 "" H 7400 1500 50  0001 C CNN
F 3 "" H 7400 1500 50  0001 C CNN
	1    7400 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	7350 1450 7400 1450
Wire Wire Line
	7400 1450 7400 1500
$Comp
L power:GND #PWR013
U 1 1 5D8FFCF0
P 6400 1450
F 0 "#PWR013" H 6400 1200 50  0001 C CNN
F 1 "GND" H 6405 1277 50  0001 C CNN
F 2 "" H 6400 1450 50  0001 C CNN
F 3 "" H 6400 1450 50  0001 C CNN
	1    6400 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	6450 1350 6400 1350
Wire Wire Line
	6400 1350 6400 1450
Wire Wire Line
	7350 1350 7400 1350
Wire Wire Line
	7400 1350 7400 1450
Connection ~ 7400 1450
Text Notes 6550 1700 0    50   ~ 0
200mA charge current
$Comp
L Device:R R1
U 1 1 5D906896
P 7650 1150
F 0 "R1" V 7550 1150 50  0000 C CNN
F 1 "22k" V 7650 1150 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 7580 1150 50  0001 C CNN
F 3 "~" H 7650 1150 50  0001 C CNN
	1    7650 1150
	0    1    1    0   
$EndComp
$Comp
L Device:R R3
U 1 1 5D907D99
P 7650 1350
F 0 "R3" V 7550 1350 50  0000 C CNN
F 1 "180k" V 7650 1350 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 7580 1350 50  0001 C CNN
F 3 "~" H 7650 1350 50  0001 C CNN
	1    7650 1350
	0    1    1    0   
$EndComp
Wire Wire Line
	7350 1150 7500 1150
Wire Wire Line
	7350 1250 7500 1250
Wire Wire Line
	7500 1250 7500 1350
Wire Wire Line
	7400 1450 7800 1450
Wire Wire Line
	7800 1450 7800 1350
Wire Wire Line
	7800 1350 7800 1150
Connection ~ 7800 1350
$Comp
L Device:R R2
U 1 1 5D910332
P 6200 1250
F 0 "R2" V 6100 1250 50  0000 C CNN
F 1 "1k" V 6200 1250 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 6130 1250 50  0001 C CNN
F 3 "~" H 6200 1250 50  0001 C CNN
	1    6200 1250
	0    1    1    0   
$EndComp
Wire Wire Line
	6450 1250 6350 1250
$Comp
L Device:LED D1
U 1 1 5D911BF7
P 6050 950
F 0 "D1" V 6089 833 50  0000 R CNN
F 1 "LED" V 5998 833 50  0000 R CNN
F 2 "LED_SMD:LED_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 6050 950 50  0001 C CNN
F 3 "~" H 6050 950 50  0001 C CNN
	1    6050 950 
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6050 800  6400 800 
Wire Wire Line
	6400 800  6400 1050
Connection ~ 6400 800 
Wire Wire Line
	6050 1100 6050 1250
$Comp
L Connector:Conn_01x05_Female J1
U 1 1 5D9301A9
P 5750 1000
F 0 "J1" H 5778 1026 50  0000 L CNN
F 1 "SWD" H 5778 935 50  0000 L CNN
F 2 "Connector_PinSocket_1.27mm:PinSocket_1x05_P1.27mm_Vertical" H 5750 1000 50  0001 C CNN
F 3 "~" H 5750 1000 50  0001 C CNN
	1    5750 1000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR012
U 1 1 5D93239C
P 5500 1250
F 0 "#PWR012" H 5500 1000 50  0001 C CNN
F 1 "GND" H 5505 1077 50  0001 C CNN
F 2 "" H 5500 1250 50  0001 C CNN
F 3 "" H 5500 1250 50  0001 C CNN
	1    5500 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5550 1200 5500 1200
Wire Wire Line
	5500 1200 5500 1250
$Comp
L power:+3.3V #PWR011
U 1 1 5D9354E8
P 5400 1100
F 0 "#PWR011" H 5400 950 50  0001 C CNN
F 1 "+3.3V" V 5400 1350 50  0000 C CNN
F 2 "" H 5400 1100 50  0001 C CNN
F 3 "" H 5400 1100 50  0001 C CNN
	1    5400 1100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5550 1100 5400 1100
Wire Wire Line
	5550 1000 5200 1000
Wire Wire Line
	5550 900  5200 900 
Wire Wire Line
	5550 800  5200 800 
Text Label 5200 1000 0    50   ~ 0
NRST
Text Label 5200 800  0    50   ~ 0
SWD_IO
Text Label 5200 900  0    50   ~ 0
SWD_SCK
$Comp
L Device:Battery_Cell BT1
U 1 1 5D94A730
P 8350 1250
F 0 "BT1" H 8468 1346 50  0000 L CNN
F 1 "400 mA-h?" H 8468 1255 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x02_P2.54mm_Vertical" V 8350 1310 50  0001 C CNN
F 3 "~" V 8350 1310 50  0001 C CNN
	1    8350 1250
	1    0    0    -1  
$EndComp
$Comp
L Device:C C6
U 1 1 5D95CD23
P 8000 1200
F 0 "C6" H 8115 1246 50  0000 L CNN
F 1 "10u" H 8115 1155 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric" H 8038 1050 50  0001 C CNN
F 3 "~" H 8000 1200 50  0001 C CNN
	1    8000 1200
	1    0    0    -1  
$EndComp
Wire Wire Line
	7800 1450 8000 1450
Wire Wire Line
	8350 1450 8350 1350
Connection ~ 7800 1450
Wire Wire Line
	8000 1350 8000 1450
Connection ~ 8000 1450
Wire Wire Line
	8000 1450 8350 1450
Wire Wire Line
	8000 1000 8000 1050
Wire Wire Line
	8350 1050 8350 1000
Wire Wire Line
	8350 1000 8000 1000
Connection ~ 8000 1000
Wire Wire Line
	7350 1000 7350 1050
Wire Wire Line
	7350 1000 8000 1000
$Comp
L power:+3.3VA #PWR09
U 1 1 5D97E3DC
P 4050 1050
F 0 "#PWR09" H 4050 900 50  0001 C CNN
F 1 "+3.3VA" H 3900 1200 50  0000 L CNN
F 2 "" H 4050 1050 50  0001 C CNN
F 3 "" H 4050 1050 50  0001 C CNN
	1    4050 1050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4050 1150 4050 1050
$Comp
L Switch:SW_SPDT SW1
U 1 1 5D9F4E13
P 8650 900
F 0 "SW1" H 8650 1185 50  0000 C CNN
F 1 "PowerOn" H 8650 1094 50  0000 C CNN
F 2 "balmer:SW_Angled_with_GND" H 8650 900 50  0001 C CNN
F 3 "~" H 8650 900 50  0001 C CNN
	1    8650 900 
	-1   0    0    -1  
$EndComp
Wire Wire Line
	8450 1000 8350 1000
Connection ~ 8350 1000
$Comp
L power:GND #PWR01
U 1 1 5DB63BF5
P 5250 -50
F 0 "#PWR01" H 5250 -300 50  0001 C CNN
F 1 "GND" H 5255 -223 50  0001 C CNN
F 2 "" H 5250 -50 50  0001 C CNN
F 3 "" H 5250 -50 50  0001 C CNN
	1    5250 -50 
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR017
U 1 1 5D9FF40B
P 10350 1900
F 0 "#PWR017" H 10350 1750 50  0001 C CNN
F 1 "+3.3V" H 10365 2073 50  0000 C CNN
F 2 "" H 10350 1900 50  0001 C CNN
F 3 "" H 10350 1900 50  0001 C CNN
	1    10350 1900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR020
U 1 1 5DA00CB2
P 9850 2400
F 0 "#PWR020" H 9850 2150 50  0001 C CNN
F 1 "GND" H 9855 2227 50  0001 C CNN
F 2 "" H 9850 2400 50  0001 C CNN
F 3 "" H 9850 2400 50  0001 C CNN
	1    9850 2400
	1    0    0    -1  
$EndComp
$Comp
L power:+BATT #PWR018
U 1 1 5DA02BEC
P 9450 1950
F 0 "#PWR018" H 9450 1800 50  0001 C CNN
F 1 "+BATT" H 9465 2123 50  0000 C CNN
F 2 "" H 9450 1950 50  0001 C CNN
F 3 "" H 9450 1950 50  0001 C CNN
	1    9450 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	10350 2000 10350 2050
$Comp
L Device:Rotary_Encoder_Switch SW2
U 1 1 5D9C2BDD
P 5600 7050
F 0 "SW2" H 5600 7417 50  0000 C CNN
F 1 "EVQWKL001" H 5600 7326 50  0000 C CNN
F 2 "balmer:Encoder_EVQWK4" H 5450 7210 50  0001 C CNN
F 3 "~" H 5600 7310 50  0001 C CNN
	1    5600 7050
	1    0    0    -1  
$EndComp
$Comp
L Device:C C15
U 1 1 5D9D807B
P 4900 6900
F 0 "C15" H 5015 6946 50  0000 L CNN
F 1 "22n" H 5015 6855 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4938 6750 50  0001 C CNN
F 3 "~" H 4900 6900 50  0001 C CNN
	1    4900 6900
	1    0    0    -1  
$EndComp
$Comp
L Device:C C17
U 1 1 5D9DD258
P 4900 7200
F 0 "C17" H 5015 7246 50  0000 L CNN
F 1 "22n" H 5015 7155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4938 7050 50  0001 C CNN
F 3 "~" H 4900 7200 50  0001 C CNN
	1    4900 7200
	1    0    0    -1  
$EndComp
$Comp
L Device:C C16
U 1 1 5D9DD6BF
P 6100 7050
F 0 "C16" H 6215 7096 50  0000 L CNN
F 1 "22n" H 6215 7005 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 6138 6900 50  0001 C CNN
F 3 "~" H 6100 7050 50  0001 C CNN
	1    6100 7050
	1    0    0    -1  
$EndComp
Wire Wire Line
	5900 6950 5900 6900
Wire Wire Line
	5900 6900 6000 6900
Wire Wire Line
	5900 7150 5900 7200
Wire Wire Line
	5900 7200 6100 7200
Wire Wire Line
	4900 7350 5300 7350
Wire Wire Line
	5300 7350 5300 7150
Wire Wire Line
	5300 7050 5200 7050
Connection ~ 4900 7050
Wire Wire Line
	4900 6750 5300 6750
Wire Wire Line
	5300 6750 5300 6950
$Comp
L power:GND #PWR038
U 1 1 5DA0647D
P 6000 6900
F 0 "#PWR038" H 6000 6650 50  0001 C CNN
F 1 "GND" H 6005 6727 50  0001 C CNN
F 2 "" H 6000 6900 50  0001 C CNN
F 3 "" H 6000 6900 50  0001 C CNN
	1    6000 6900
	1    0    0    -1  
$EndComp
Connection ~ 6000 6900
Wire Wire Line
	6000 6900 6100 6900
$Comp
L power:GND #PWR040
U 1 1 5DA06A5A
P 5200 7050
F 0 "#PWR040" H 5200 6800 50  0001 C CNN
F 1 "GND" H 5205 6877 50  0001 C CNN
F 2 "" H 5200 7050 50  0001 C CNN
F 3 "" H 5200 7050 50  0001 C CNN
	1    5200 7050
	1    0    0    -1  
$EndComp
Connection ~ 5200 7050
Wire Wire Line
	5200 7050 4900 7050
$Comp
L Device:R R4
U 1 1 5DA124D2
P 4700 6750
F 0 "R4" V 4600 6750 50  0000 C CNN
F 1 "10k" V 4700 6750 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 4630 6750 50  0001 C CNN
F 3 "~" H 4700 6750 50  0001 C CNN
	1    4700 6750
	0    1    1    0   
$EndComp
$Comp
L Device:R R6
U 1 1 5DA15634
P 4700 7350
F 0 "R6" V 4600 7350 50  0000 C CNN
F 1 "10k" V 4700 7350 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 4630 7350 50  0001 C CNN
F 3 "~" H 4700 7350 50  0001 C CNN
	1    4700 7350
	0    1    1    0   
$EndComp
Wire Wire Line
	4550 7350 4550 7050
Wire Wire Line
	4900 6750 4850 6750
Connection ~ 4900 6750
Wire Wire Line
	4900 7350 4850 7350
Connection ~ 4900 7350
$Comp
L power:+3.3V #PWR039
U 1 1 5DA37693
P 4400 7050
F 0 "#PWR039" H 4400 6900 50  0001 C CNN
F 1 "+3.3V" H 4415 7223 50  0000 C CNN
F 2 "" H 4400 7050 50  0001 C CNN
F 3 "" H 4400 7050 50  0001 C CNN
	1    4400 7050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4550 7050 4400 7050
Connection ~ 4550 7050
Wire Wire Line
	4550 7050 4550 6750
$Comp
L power:+3.3V #PWR043
U 1 1 5DA445DC
P 6800 7200
F 0 "#PWR043" H 6800 7050 50  0001 C CNN
F 1 "+3.3V" H 6815 7373 50  0000 C CNN
F 2 "" H 6800 7200 50  0001 C CNN
F 3 "" H 6800 7200 50  0001 C CNN
	1    6800 7200
	1    0    0    -1  
$EndComp
Text Label 5000 6750 0    50   ~ 0
ENC_A
Text Label 5000 7350 0    50   ~ 0
ENC_B
$Comp
L Device:R R5
U 1 1 5DA649A2
P 6600 7200
F 0 "R5" V 6500 7200 50  0000 C CNN
F 1 "10k" V 6600 7200 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 6530 7200 50  0001 C CNN
F 3 "~" H 6600 7200 50  0001 C CNN
	1    6600 7200
	0    1    1    0   
$EndComp
Wire Wire Line
	6100 7200 6450 7200
Connection ~ 6100 7200
Text Label 6200 7200 0    50   ~ 0
ENC_S
Wire Wire Line
	6800 7200 6750 7200
Text Label 4750 3850 0    50   ~ 0
ENC_A
Text Label 4750 3750 0    50   ~ 0
ENC_B
$Comp
L Device:C C5
U 1 1 5DA0211D
P 2500 850
F 0 "C5" H 2615 896 50  0000 L CNN
F 1 "100n" H 2615 805 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 2538 700 50  0001 C CNN
F 3 "~" H 2500 850 50  0001 C CNN
	1    2500 850 
	1    0    0    -1  
$EndComp
$Comp
L Device:C C4
U 1 1 5DA0292E
P 2100 850
F 0 "C4" H 2215 896 50  0000 L CNN
F 1 "100n" H 2215 805 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 2138 700 50  0001 C CNN
F 3 "~" H 2100 850 50  0001 C CNN
	1    2100 850 
	1    0    0    -1  
$EndComp
$Comp
L Device:C C3
U 1 1 5DA02D82
P 1700 850
F 0 "C3" H 1815 896 50  0000 L CNN
F 1 "100n" H 1815 805 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 1738 700 50  0001 C CNN
F 3 "~" H 1700 850 50  0001 C CNN
	1    1700 850 
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 5DA0307B
P 900 850
F 0 "C1" H 1015 896 50  0000 L CNN
F 1 "100n" H 1015 805 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 938 700 50  0001 C CNN
F 3 "~" H 900 850 50  0001 C CNN
	1    900  850 
	1    0    0    -1  
$EndComp
$Comp
L Device:C C2
U 1 1 5DA03EA5
P 1300 850
F 0 "C2" H 1415 896 50  0000 L CNN
F 1 "100n" H 1415 805 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 1338 700 50  0001 C CNN
F 3 "~" H 1300 850 50  0001 C CNN
	1    1300 850 
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR02
U 1 1 5DA05199
P 2500 700
F 0 "#PWR02" H 2500 550 50  0001 C CNN
F 1 "+3.3V" H 2515 873 50  0000 C CNN
F 2 "" H 2500 700 50  0001 C CNN
F 3 "" H 2500 700 50  0001 C CNN
	1    2500 700 
	1    0    0    -1  
$EndComp
Wire Wire Line
	2500 700  2100 700 
Connection ~ 2500 700 
Wire Wire Line
	2100 700  1700 700 
Connection ~ 2100 700 
Wire Wire Line
	1300 700  1700 700 
Connection ~ 1700 700 
Wire Wire Line
	900  700  1300 700 
Connection ~ 1300 700 
Wire Wire Line
	900  1000 1300 1000
Wire Wire Line
	1700 1000 1300 1000
Connection ~ 1300 1000
Wire Wire Line
	2100 1000 1700 1000
Connection ~ 1700 1000
Wire Wire Line
	2500 1000 2100 1000
Connection ~ 2100 1000
$Comp
L power:GND #PWR05
U 1 1 5DA77DE3
P 2500 1000
F 0 "#PWR05" H 2500 750 50  0001 C CNN
F 1 "GND" H 2505 827 50  0001 C CNN
F 2 "" H 2500 1000 50  0001 C CNN
F 3 "" H 2500 1000 50  0001 C CNN
	1    2500 1000
	1    0    0    -1  
$EndComp
Connection ~ 2500 1000
Text Notes 5250 3650 0    50   ~ 0
PB3 - SPI1_SCK \nPB4 - SPI1_MISO\nPB5 - SPI1_MOSI
Text Notes 5450 6050 0    50   ~ 0
PC10 - SPI3_SCK\nPC11 - SPI3_MISO\nPC12 - SPI3_MOSI
Text Notes 5250 3850 0    50   ~ 0
PB6 - TIM4_CH1\nPB7 - TIM4_CH2
$Comp
L balmer:HT7333 U2
U 1 1 5DE03297
P 9900 2100
F 0 "U2" H 9875 2447 60  0000 C CNN
F 1 "HT7333" H 9875 2341 60  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-89-3" H 9850 2100 60  0001 C CNN
F 3 "" H 9850 2100 60  0001 C CNN
	1    9900 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	9850 2400 10350 2400
Wire Wire Line
	10350 2400 10350 2350
Connection ~ 9850 2400
Wire Wire Line
	10300 2000 10350 2000
Wire Wire Line
	10350 1900 10350 2000
Connection ~ 10350 2000
Wire Wire Line
	9450 1950 9450 2000
$Comp
L Device:C C11
U 1 1 5DE6A08D
P 2650 2350
F 0 "C11" H 2765 2396 50  0000 L CNN
F 1 "2.2u" H 2765 2305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 2688 2200 50  0001 C CNN
F 3 "~" H 2650 2350 50  0001 C CNN
	1    2650 2350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR022
U 1 1 5DE7BB40
P 2650 2500
F 0 "#PWR022" H 2650 2250 50  0001 C CNN
F 1 "GND" H 2655 2327 50  0001 C CNN
F 2 "" H 2650 2500 50  0001 C CNN
F 3 "" H 2650 2500 50  0001 C CNN
	1    2650 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2850 2050 2650 2050
Wire Wire Line
	2050 1950 2050 2200
Wire Wire Line
	1700 1850 2850 1850
Wire Wire Line
	1700 1850 1700 2200
Wire Wire Line
	2050 1950 2850 1950
Wire Wire Line
	2650 2050 2650 2200
$Comp
L Device:C C8
U 1 1 5DE60A32
P 10350 2200
F 0 "C8" H 10465 2246 50  0000 L CNN
F 1 "100u" H 10465 2155 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric" H 10388 2050 50  0001 C CNN
F 3 "~" H 10350 2200 50  0001 C CNN
	1    10350 2200
	1    0    0    -1  
$EndComp
$Comp
L Device:Crystal Y1
U 1 1 5F43E07A
P 1750 2850
F 0 "Y1" H 1550 2900 50  0000 C CNN
F 1 "8 MHz" H 2000 2900 50  0000 C CNN
F 2 "Crystal:Crystal_SMD_5032-2Pin_5.0x3.2mm" H 1750 2850 50  0001 C CNN
F 3 "~" H 1750 2850 50  0001 C CNN
	1    1750 2850
	1    0    0    -1  
$EndComp
$Comp
L Device:C C13
U 1 1 5F43E858
P 1450 3050
F 0 "C13" H 1565 3096 50  0000 L CNN
F 1 "22" H 1565 3005 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 1488 2900 50  0001 C CNN
F 3 "~" H 1450 3050 50  0001 C CNN
	1    1450 3050
	1    0    0    -1  
$EndComp
$Comp
L Device:C C14
U 1 1 5F44D9E8
P 2000 3050
F 0 "C14" H 2115 3096 50  0000 L CNN
F 1 "22" H 2115 3005 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 2038 2900 50  0001 C CNN
F 3 "~" H 2000 3050 50  0001 C CNN
	1    2000 3050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR028
U 1 1 5F44DC76
P 1750 3200
F 0 "#PWR028" H 1750 2950 50  0001 C CNN
F 1 "GND" H 1755 3027 50  0001 C CNN
F 2 "" H 1750 3200 50  0001 C CNN
F 3 "" H 1750 3200 50  0001 C CNN
	1    1750 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	2000 3200 1750 3200
Wire Wire Line
	1450 3200 1750 3200
Connection ~ 1750 3200
Wire Wire Line
	1450 2900 1450 2850
Wire Wire Line
	1450 2850 1600 2850
Wire Wire Line
	1900 2850 2000 2850
Wire Wire Line
	2000 2850 2000 2900
Wire Wire Line
	1450 2850 1450 2700
Wire Wire Line
	1450 2700 2850 2700
Wire Wire Line
	2850 2700 2850 2850
Connection ~ 1450 2850
Wire Wire Line
	2850 2950 2750 2950
Wire Wire Line
	2750 2950 2750 2850
Wire Wire Line
	2750 2850 2000 2850
Connection ~ 2000 2850
$Comp
L power:+5VP #PWR04
U 1 1 5F43B0F9
P 10100 850
F 0 "#PWR04" H 10100 700 50  0001 C CNN
F 1 "+5VP" H 10115 1023 50  0000 C CNN
F 2 "" H 10100 850 50  0001 C CNN
F 3 "" H 10100 850 50  0001 C CNN
	1    10100 850 
	1    0    0    -1  
$EndComp
Wire Wire Line
	10100 900  10100 850 
Wire Wire Line
	9950 900  10100 900 
$Comp
L Connector_Generic:Conn_01x04 J3
U 1 1 5F4CCE7E
P 6850 2250
F 0 "J3" H 6930 2242 50  0000 L CNN
F 1 "UART" H 6930 2151 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Horizontal" H 6850 2250 50  0001 C CNN
F 3 "~" H 6850 2250 50  0001 C CNN
	1    6850 2250
	1    0    0    -1  
$EndComp
Text Label 4750 2350 0    50   ~ 0
UART1_TX
Text Label 4750 2450 0    50   ~ 0
UART1_RX
$Comp
L power:GND #PWR023
U 1 1 5F4E9F54
P 6600 2500
F 0 "#PWR023" H 6600 2250 50  0001 C CNN
F 1 "GND" H 6605 2327 50  0001 C CNN
F 2 "" H 6600 2500 50  0001 C CNN
F 3 "" H 6600 2500 50  0001 C CNN
	1    6600 2500
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR019
U 1 1 5F4EA3D6
P 6600 2100
F 0 "#PWR019" H 6600 1950 50  0001 C CNN
F 1 "+3.3V" H 6615 2273 50  0000 C CNN
F 2 "" H 6600 2100 50  0001 C CNN
F 3 "" H 6600 2100 50  0001 C CNN
	1    6600 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	6650 2150 6600 2150
Wire Wire Line
	6600 2150 6600 2100
Wire Wire Line
	6650 2250 6250 2250
Wire Wire Line
	6650 2350 6250 2350
Wire Wire Line
	6650 2450 6600 2450
Wire Wire Line
	6600 2450 6600 2500
Text Label 4750 2550 0    50   ~ 0
USB_DM
Text Label 4750 2650 0    50   ~ 0
USB_DP
Wire Wire Line
	4650 2550 5050 2550
Wire Wire Line
	4650 2650 5050 2650
Wire Wire Line
	4650 2350 5100 2350
Wire Wire Line
	4650 2450 5100 2450
Text Label 10000 1200 0    50   ~ 0
USB_DM
Text Label 10000 1100 0    50   ~ 0
USB_DP
Wire Wire Line
	9950 1100 10300 1100
Wire Wire Line
	9950 1200 10300 1200
Text Notes 5350 2050 0    50   ~ 0
PA5 - SPI1_SCK
Text Notes 5350 2150 0    50   ~ 0
PA7 - SPI1_MOSI
Text Notes 600  3100 0    50   ~ 0
PE12 - SPI4_SCK\nPE13 - SPI4_MISO\nPE14 - SPI4_MOSI
$Comp
L Connector_Generic:Conn_02x18_Odd_Even J7
U 1 1 5FB92F1B
P 1200 6200
F 0 "J7" H 1250 7217 50  0000 C CNN
F 1 "Display" H 1250 7126 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x18_P2.54mm_Vertical" H 1200 6200 50  0001 C CNN
F 3 "~" H 1200 6200 50  0001 C CNN
	1    1200 6200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR041
U 1 1 5FB96488
P 950 7150
F 0 "#PWR041" H 950 6900 50  0001 C CNN
F 1 "GND" H 955 6977 50  0001 C CNN
F 2 "" H 950 7150 50  0001 C CNN
F 3 "" H 950 7150 50  0001 C CNN
	1    950  7150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR042
U 1 1 5FB9675C
P 1550 7150
F 0 "#PWR042" H 1550 6900 50  0001 C CNN
F 1 "GND" H 1555 6977 50  0001 C CNN
F 2 "" H 1550 7150 50  0001 C CNN
F 3 "" H 1550 7150 50  0001 C CNN
	1    1550 7150
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 7100 950  7100
Wire Wire Line
	950  7100 950  7150
Wire Wire Line
	1500 7100 1550 7100
Wire Wire Line
	1550 7100 1550 7150
$Comp
L power:+3.3V #PWR035
U 1 1 5FBAAE3A
P 950 5350
F 0 "#PWR035" H 950 5200 50  0001 C CNN
F 1 "+3.3V" H 965 5523 50  0000 C CNN
F 2 "" H 950 5350 50  0001 C CNN
F 3 "" H 950 5350 50  0001 C CNN
	1    950  5350
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR036
U 1 1 5FBAB303
P 1550 5350
F 0 "#PWR036" H 1550 5200 50  0001 C CNN
F 1 "+3.3V" H 1565 5523 50  0000 C CNN
F 2 "" H 1550 5350 50  0001 C CNN
F 3 "" H 1550 5350 50  0001 C CNN
	1    1550 5350
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 5400 950  5400
Wire Wire Line
	950  5400 950  5350
Wire Wire Line
	1500 5400 1550 5400
Wire Wire Line
	1550 5400 1550 5350
Wire Wire Line
	1000 5500 750  5500
Wire Wire Line
	1000 5600 750  5600
Wire Wire Line
	1000 5700 750  5700
Wire Wire Line
	1000 5800 750  5800
Wire Wire Line
	1000 5900 750  5900
Wire Wire Line
	1000 6000 750  6000
Wire Wire Line
	1000 6100 750  6100
Wire Wire Line
	1000 6200 750  6200
Wire Wire Line
	1000 6300 750  6300
Wire Wire Line
	1000 6400 750  6400
Wire Wire Line
	1500 5500 1800 5500
Wire Wire Line
	1500 5600 1800 5600
Wire Wire Line
	1500 5700 1800 5700
Wire Wire Line
	1500 5800 1800 5800
Wire Wire Line
	1500 5900 1800 5900
Wire Wire Line
	1500 6000 1800 6000
Wire Wire Line
	1500 6100 1800 6100
Wire Wire Line
	1500 6200 1800 6200
Wire Wire Line
	1500 6300 1800 6300
Wire Wire Line
	1500 6400 1800 6400
Text Label 1550 5500 0    50   ~ 0
DB9
Text Label 850  5500 0    50   ~ 0
DB8
Text Label 800  5600 0    50   ~ 0
DB10
Text Label 1550 5600 0    50   ~ 0
DB11
Text Label 800  5700 0    50   ~ 0
DB12
Text Label 1550 5700 0    50   ~ 0
DB13
Text Label 800  5800 0    50   ~ 0
DB14
Text Label 1550 5800 0    50   ~ 0
DB15
Text Label 800  5900 0    50   ~ 0
DB7
Text Label 1550 5900 0    50   ~ 0
DB6
Text Label 800  6000 0    50   ~ 0
DB5
Text Label 1550 6000 0    50   ~ 0
DB4
Text Label 800  6100 0    50   ~ 0
DB3
Text Label 1550 6100 0    50   ~ 0
DB2
Text Label 800  6200 0    50   ~ 0
DB1
Text Label 1550 6200 0    50   ~ 0
DB0
Text Label 750  6300 0    50   ~ 0
LCD_RS
Text Label 750  6400 0    50   ~ 0
LCD_CS
Text Label 1550 6300 0    50   ~ 0
LCD_WR
Text Label 1550 6400 0    50   ~ 0
LCD_RST
Text Label 6250 2250 0    50   ~ 0
UART1_TX
Text Label 6250 2350 0    50   ~ 0
UART1_RX
Text Label 4750 3950 0    50   ~ 0
ENC_S
Wire Wire Line
	2850 4850 2550 4850
Wire Wire Line
	2850 4950 2550 4950
Wire Wire Line
	2850 5050 2550 5050
Wire Wire Line
	2850 5150 2550 5150
Wire Wire Line
	2850 5250 2550 5250
Wire Wire Line
	2850 5350 2550 5350
Wire Wire Line
	2850 5450 2550 5450
Wire Wire Line
	2850 5550 2550 5550
Wire Wire Line
	2850 5650 2550 5650
Wire Wire Line
	2850 5750 2550 5750
Wire Wire Line
	2850 5850 2550 5850
Wire Wire Line
	2850 5950 2550 5950
Wire Wire Line
	2850 6050 2550 6050
Wire Wire Line
	2850 6150 2550 6150
Wire Wire Line
	2850 6250 2550 6250
Wire Wire Line
	2850 6350 2550 6350
Text Label 2650 4850 0    50   ~ 0
DB0
Text Label 2650 4950 0    50   ~ 0
DB1
Text Label 2650 5050 0    50   ~ 0
DB2
Text Label 2650 5150 0    50   ~ 0
DB3
Text Label 2650 5250 0    50   ~ 0
DB4
Text Label 2650 5350 0    50   ~ 0
DB5
Text Label 2650 5450 0    50   ~ 0
DB6
Text Label 2650 5550 0    50   ~ 0
DB7
Text Label 2650 5650 0    50   ~ 0
DB8
Text Label 2650 5750 0    50   ~ 0
DB9
Text Label 2650 5850 0    50   ~ 0
DB10
Text Label 2650 5950 0    50   ~ 0
DB11
Text Label 2650 6050 0    50   ~ 0
DB12
Text Label 2650 6150 0    50   ~ 0
DB13
Text Label 2650 6250 0    50   ~ 0
DB14
Text Label 2650 6350 0    50   ~ 0
DB15
Text Label 4750 5750 0    50   ~ 0
LCD_RS
Text Label 4750 5650 0    50   ~ 0
LCD_WR
Text Label 4750 5550 0    50   ~ 0
LCD_CS
Text Label 4750 5450 0    50   ~ 0
LCD_RST
Wire Wire Line
	4650 5450 5100 5450
Wire Wire Line
	4650 5550 5100 5550
Wire Wire Line
	4650 5650 5100 5650
Wire Wire Line
	4650 5750 5100 5750
Text Notes 600  2800 0    50   ~ 0
PE2 - SAI_A_MCLK\nPE4 - SAI_A_FS\nPE5 - SAI_A_SCK\nPE6 - SAI_A_SD
Wire Wire Line
	2850 3350 2350 3350
Wire Wire Line
	2850 3550 2350 3550
Wire Wire Line
	2850 3650 2350 3650
Wire Wire Line
	2850 3750 2350 3750
Text Label 2350 3350 0    50   ~ 0
SAI_A_MCLK
Text Label 2350 3550 0    50   ~ 0
SAI_A_FS
Text Label 2350 3650 0    50   ~ 0
SAI_A_SCK
Text Label 2350 3750 0    50   ~ 0
SAI_A_SD
$Comp
L Connector_Generic:Conn_02x05_Odd_Even J6
U 1 1 5FBDC44B
P 1150 4000
F 0 "J6" H 1200 4417 50  0000 C CNN
F 1 "Data&Power" H 1200 4326 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_2x05_P2.54mm_Horizontal" H 1150 4000 50  0001 C CNN
F 3 "~" H 1150 4000 50  0001 C CNN
	1    1150 4000
	1    0    0    -1  
$EndComp
$Comp
L power:+BATT #PWR031
U 1 1 5FC0676A
P 1500 3750
F 0 "#PWR031" H 1500 3600 50  0001 C CNN
F 1 "+BATT" H 1515 3923 50  0000 C CNN
F 2 "" H 1500 3750 50  0001 C CNN
F 3 "" H 1500 3750 50  0001 C CNN
	1    1500 3750
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3VA #PWR07
U 1 1 5FC06BC9
P 4650 1000
F 0 "#PWR07" H 4650 850 50  0001 C CNN
F 1 "+3.3VA" H 4500 1150 50  0000 L CNN
F 2 "" H 4650 1000 50  0001 C CNN
F 3 "" H 4650 1000 50  0001 C CNN
	1    4650 1000
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR06
U 1 1 5FC06D98
P 4350 1000
F 0 "#PWR06" H 4350 850 50  0001 C CNN
F 1 "+3.3V" H 4365 1173 50  0000 C CNN
F 2 "" H 4350 1000 50  0001 C CNN
F 3 "" H 4350 1000 50  0001 C CNN
	1    4350 1000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 1000 4350 1000
Wire Wire Line
	1450 3800 1500 3800
Wire Wire Line
	1500 3800 1500 3750
$Comp
L power:+3.3V #PWR032
U 1 1 5FC23927
P 1750 3750
F 0 "#PWR032" H 1750 3600 50  0001 C CNN
F 1 "+3.3V" H 1765 3923 50  0000 C CNN
F 2 "" H 1750 3750 50  0001 C CNN
F 3 "" H 1750 3750 50  0001 C CNN
	1    1750 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	1450 3900 1750 3900
Wire Wire Line
	1750 3900 1750 3750
Wire Wire Line
	2850 3450 2350 3450
Text Label 2350 3450 0    50   ~ 0
MODE
Wire Wire Line
	1450 4000 1750 4000
$Comp
L power:GND #PWR034
U 1 1 5FC50AA6
P 1500 4250
F 0 "#PWR034" H 1500 4000 50  0001 C CNN
F 1 "GND" H 1505 4077 50  0001 C CNN
F 2 "" H 1500 4250 50  0001 C CNN
F 3 "" H 1500 4250 50  0001 C CNN
	1    1500 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	1450 4100 1500 4100
Wire Wire Line
	1500 4100 1500 4200
Wire Wire Line
	1450 4200 1500 4200
Connection ~ 1500 4200
Wire Wire Line
	1500 4200 1500 4250
Text Label 1550 4000 0    50   ~ 0
MODE
Text Label 500  3800 0    50   ~ 0
SAI_A_MCLK
Text Label 500  3900 0    50   ~ 0
SAI_A_SCK
Text Label 500  4000 0    50   ~ 0
SAI_A_FS
Text Label 500  4100 0    50   ~ 0
SAI_A_SD
Text Label 500  4200 0    50   ~ 0
~SYNC
Wire Wire Line
	950  3800 500  3800
Wire Wire Line
	950  3900 500  3900
Wire Wire Line
	950  4000 500  4000
Wire Wire Line
	950  4100 500  4100
Wire Wire Line
	950  4200 500  4200
Text Label 2350 3250 0    50   ~ 0
~SYNC
Wire Wire Line
	2850 3250 2350 3250
$Comp
L balmer:M25P16 U3
U 1 1 5FB70BF5
P 9800 3300
F 0 "U3" H 9825 3697 60  0000 C CNN
F 1 "M25P16 or LY68L6400" H 9825 3591 60  0000 C CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 9850 3300 60  0001 C CNN
F 3 "" H 9850 3300 60  0001 C CNN
	1    9800 3300
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR026
U 1 1 5FB7191B
P 10400 3000
F 0 "#PWR026" H 10400 2850 50  0001 C CNN
F 1 "+3.3V" H 10415 3173 50  0000 C CNN
F 2 "" H 10400 3000 50  0001 C CNN
F 3 "" H 10400 3000 50  0001 C CNN
	1    10400 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	10250 3150 10400 3150
Wire Wire Line
	10400 3150 10400 3000
$Comp
L power:GND #PWR029
U 1 1 5FB83285
P 9350 3500
F 0 "#PWR029" H 9350 3250 50  0001 C CNN
F 1 "GND" H 9355 3327 50  0001 C CNN
F 2 "" H 9350 3500 50  0001 C CNN
F 3 "" H 9350 3500 50  0001 C CNN
	1    9350 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	9400 3450 9350 3450
Wire Wire Line
	9350 3450 9350 3500
Wire Wire Line
	10400 3000 10450 3000
Connection ~ 10400 3000
Wire Wire Line
	9400 3150 9000 3150
Wire Wire Line
	9400 3250 9000 3250
Wire Wire Line
	9400 3350 9000 3350
Wire Wire Line
	10250 3350 10700 3350
Wire Wire Line
	10250 3450 10700 3450
Wire Wire Line
	10250 3250 10700 3250
Text Label 9000 3150 0    50   ~ 0
~QSPI_CS
Text Label 9000 3250 0    50   ~ 0
QSPI_IO1
Text Label 9000 3350 0    50   ~ 0
QSPI_IO2
Text Label 10350 3250 0    50   ~ 0
QSPI_IO3
Text Label 10350 3350 0    50   ~ 0
QSPI_CLK
Text Label 10350 3450 0    50   ~ 0
QSPI_IO0
Text Label 2350 3850 0    50   ~ 0
QSPI_IO0
Text Label 2350 3950 0    50   ~ 0
QSPI_IO1
Text Label 2350 4050 0    50   ~ 0
QSPI_IO2
Text Label 2350 4150 0    50   ~ 0
QSPI_IO3
Text Label 4750 3350 0    50   ~ 0
QSPI_CLK
Text Label 4750 4150 0    50   ~ 0
~QSPI_CS
Wire Wire Line
	4650 4150 5050 4150
Wire Wire Line
	4650 3350 5100 3350
Wire Wire Line
	4650 3750 5050 3750
Wire Wire Line
	4650 3850 5050 3850
Wire Wire Line
	4650 3950 5050 3950
Wire Wire Line
	2850 3850 2350 3850
Wire Wire Line
	2850 3950 2350 3950
Wire Wire Line
	2850 4050 2350 4050
Wire Wire Line
	2850 4150 2350 4150
Wire Wire Line
	4650 1450 4900 1450
Wire Wire Line
	4650 1550 4900 1550
Wire Wire Line
	4650 1650 4900 1650
Wire Wire Line
	4650 1750 4900 1750
Wire Wire Line
	4650 1850 4900 1850
Wire Wire Line
	4650 1950 4900 1950
Wire Wire Line
	4650 2050 4900 2050
Wire Wire Line
	4650 2150 4900 2150
Text Label 4700 1450 0    50   ~ 0
PA0
Text Label 4700 1550 0    50   ~ 0
PA1
Text Label 4700 1650 0    50   ~ 0
PA2
Text Label 4700 1750 0    50   ~ 0
PA3
Text Label 4700 1850 0    50   ~ 0
PA4
Text Label 4700 1950 0    50   ~ 0
PA5
Text Label 4700 2050 0    50   ~ 0
PA6
Text Label 4700 2150 0    50   ~ 0
PA7
$Comp
L Connector_Generic:Conn_02x05_Odd_Even J4
U 1 1 5FD34C6D
P 7900 2300
F 0 "J4" H 7950 2717 50  0000 C CNN
F 1 "PA0-7" H 7950 2626 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_2x05_P2.54mm_Horizontal" H 7900 2300 50  0001 C CNN
F 3 "~" H 7900 2300 50  0001 C CNN
	1    7900 2300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR024
U 1 1 5FD363C7
P 7650 2550
F 0 "#PWR024" H 7650 2300 50  0001 C CNN
F 1 "GND" H 7655 2377 50  0001 C CNN
F 2 "" H 7650 2550 50  0001 C CNN
F 3 "" H 7650 2550 50  0001 C CNN
	1    7650 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	7700 2500 7650 2500
Wire Wire Line
	7650 2500 7650 2550
$Comp
L power:GND #PWR025
U 1 1 5FD48BC9
P 8250 2550
F 0 "#PWR025" H 8250 2300 50  0001 C CNN
F 1 "GND" H 8255 2377 50  0001 C CNN
F 2 "" H 8250 2550 50  0001 C CNN
F 3 "" H 8250 2550 50  0001 C CNN
	1    8250 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	8200 2500 8250 2500
Wire Wire Line
	8250 2500 8250 2550
Wire Wire Line
	7700 2100 7450 2100
Wire Wire Line
	7700 2200 7450 2200
Wire Wire Line
	7700 2300 7450 2300
Wire Wire Line
	7700 2400 7450 2400
Wire Wire Line
	8200 2100 8450 2100
Wire Wire Line
	8200 2200 8450 2200
Wire Wire Line
	8200 2300 8450 2300
Wire Wire Line
	8200 2400 8450 2400
Text Label 7500 2100 0    50   ~ 0
PA0
Text Label 8250 2100 0    50   ~ 0
PA1
Text Label 7500 2200 0    50   ~ 0
PA2
Text Label 8250 2200 0    50   ~ 0
PA3
Text Label 7500 2300 0    50   ~ 0
PA4
Text Label 8250 2300 0    50   ~ 0
PA5
Text Label 7500 2400 0    50   ~ 0
PA6
Text Label 8250 2400 0    50   ~ 0
PA7
$Comp
L Connector_Generic:Conn_01x06 J5
U 1 1 5FDF605E
P 7150 3400
F 0 "J5" H 7230 3392 50  0000 L CNN
F 1 "SPI3" H 7230 3301 50  0000 L CNN
F 2 "Connector_PinSocket_1.27mm:PinSocket_1x06_P1.27mm_Vertical" H 7150 3400 50  0001 C CNN
F 3 "~" H 7150 3400 50  0001 C CNN
	1    7150 3400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR033
U 1 1 5FDF6EB1
P 6900 3750
F 0 "#PWR033" H 6900 3500 50  0001 C CNN
F 1 "GND" H 6905 3577 50  0001 C CNN
F 2 "" H 6900 3750 50  0001 C CNN
F 3 "" H 6900 3750 50  0001 C CNN
	1    6900 3750
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR030
U 1 1 5FDF7256
P 6750 3600
F 0 "#PWR030" H 6750 3450 50  0001 C CNN
F 1 "+3.3V" V 6750 3850 50  0000 C CNN
F 2 "" H 6750 3600 50  0001 C CNN
F 3 "" H 6750 3600 50  0001 C CNN
	1    6750 3600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6950 3600 6750 3600
Wire Wire Line
	6950 3700 6900 3700
Wire Wire Line
	6900 3700 6900 3750
Wire Wire Line
	4650 5850 5200 5850
Wire Wire Line
	4650 5950 5200 5950
Wire Wire Line
	4650 6050 5200 6050
Text Label 4750 5850 0    50   ~ 0
PC10_SPI3_SCK
Text Label 4750 5950 0    50   ~ 0
PC11_SPI3_MISO
Text Label 4750 6050 0    50   ~ 0
PC12_SPI3_MOSI
Wire Wire Line
	4650 2950 5150 2950
Text Label 4750 2950 0    50   ~ 0
PA15
Text Label 6350 3200 0    50   ~ 0
PA15
Text Label 6350 3300 0    50   ~ 0
PC10_SPI3_SCK
Text Label 6350 3400 0    50   ~ 0
PC11_SPI3_MISO
Text Label 6350 3500 0    50   ~ 0
PC12_SPI3_MOSI
Wire Wire Line
	6350 3300 6950 3300
Wire Wire Line
	6350 3400 6950 3400
Wire Wire Line
	6350 3500 6950 3500
Wire Wire Line
	6350 3200 6950 3200
$Comp
L power:GND #PWR027
U 1 1 5FBB7955
P 10750 3000
F 0 "#PWR027" H 10750 2750 50  0001 C CNN
F 1 "GND" H 10755 2827 50  0001 C CNN
F 2 "" H 10750 3000 50  0001 C CNN
F 3 "" H 10750 3000 50  0001 C CNN
	1    10750 3000
	1    0    0    -1  
$EndComp
$Comp
L Device:C C12
U 1 1 5FB94E6D
P 10600 3000
F 0 "C12" V 10450 2950 50  0000 L CNN
F 1 "100n" V 10550 3100 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 10638 2850 50  0001 C CNN
F 3 "~" H 10600 3000 50  0001 C CNN
	1    10600 3000
	0    1    1    0   
$EndComp
Wire Wire Line
	8850 900  8950 900 
Wire Wire Line
	8950 900  8950 850 
$EndSCHEMATC
