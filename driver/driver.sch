EESchema Schematic File Version 4
LIBS:driver-cache
EELAYER 26 0
EELAYER END
$Descr A2 23386 16535
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
L Device:R_Small R?
U 1 1 5C3D5DD5
P 9150 8050
F 0 "R?" V 8954 8050 50  0000 C CNN
F 1 "5mR 1W" V 9045 8050 50  0000 C CNN
F 2 "" H 9150 8050 50  0001 C CNN
F 3 "~" H 9150 8050 50  0001 C CNN
	1    9150 8050
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R?
U 1 1 5C3D5E81
P 9250 8050
F 0 "R?" V 9054 8050 50  0001 C CNN
F 1 "5mR 1W" V 9145 8050 50  0001 C CNN
F 2 "" H 9250 8050 50  0001 C CNN
F 3 "~" H 9250 8050 50  0001 C CNN
	1    9250 8050
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R?
U 1 1 5C3D5FC2
P 9350 8050
F 0 "R?" V 9154 8050 50  0001 C CNN
F 1 "5mR 1W" V 9245 8050 50  0001 C CNN
F 2 "" H 9350 8050 50  0001 C CNN
F 3 "~" H 9350 8050 50  0001 C CNN
	1    9350 8050
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R?
U 1 1 5C3D600E
P 9450 8050
F 0 "R?" V 9254 8050 50  0001 C CNN
F 1 "5mR 1W" V 9345 8050 50  0001 C CNN
F 2 "" H 9450 8050 50  0001 C CNN
F 3 "~" H 9450 8050 50  0001 C CNN
	1    9450 8050
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R?
U 1 1 5C3D603D
P 9550 8050
F 0 "R?" V 9354 8050 50  0001 C CNN
F 1 "5mR 1W" V 9445 8050 50  0001 C CNN
F 2 "" H 9550 8050 50  0001 C CNN
F 3 "~" H 9550 8050 50  0001 C CNN
	1    9550 8050
	1    0    0    -1  
$EndComp
$Comp
L Analog_ADC:INA226 U?
U 1 1 5C3D6130
P 10800 8050
F 0 "U?" H 10550 8500 50  0000 C CNN
F 1 "INA226" H 10950 8500 50  0000 C CNN
F 2 "Package_SO:MSOP-10_3x3mm_P0.5mm" H 10850 8150 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/ina226.pdf" H 11150 7950 50  0001 C CNN
	1    10800 8050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5C3D6C90
P 10800 8550
F 0 "#PWR?" H 10800 8300 50  0001 C CNN
F 1 "GND" H 10805 8377 50  0000 C CNN
F 2 "" H 10800 8550 50  0001 C CNN
F 3 "" H 10800 8550 50  0001 C CNN
	1    10800 8550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5C3D6E59
P 11300 8550
F 0 "#PWR?" H 11300 8300 50  0001 C CNN
F 1 "GND" H 11305 8377 50  0000 C CNN
F 2 "" H 11300 8550 50  0001 C CNN
F 3 "" H 11300 8550 50  0001 C CNN
	1    11300 8550
	1    0    0    -1  
$EndComp
Wire Wire Line
	11300 8550 11300 7850
Wire Wire Line
	11200 7750 11300 7750
Wire Wire Line
	11300 7750 11300 7850
Connection ~ 11300 7850
Wire Wire Line
	11300 7850 11200 7850
$Comp
L Connector_Generic:Conn_01x02 J?
U 1 1 5C3D7CA4
P 6000 6350
F 0 "J?" H 5920 6025 50  0000 C CNN
F 1 "Conn_01x02" H 5920 6116 50  0000 C CNN
F 2 "" H 6000 6350 50  0001 C CNN
F 3 "~" H 6000 6350 50  0001 C CNN
	1    6000 6350
	-1   0    0    1   
$EndComp
Wire Wire Line
	9150 8150 9150 8250
Wire Wire Line
	9150 8250 9250 8250
Wire Wire Line
	9550 8250 9550 8150
Wire Wire Line
	9450 8150 9450 8250
Connection ~ 9450 8250
Wire Wire Line
	9450 8250 9550 8250
Wire Wire Line
	9350 8250 9350 8150
Connection ~ 9350 8250
Wire Wire Line
	9350 8250 9450 8250
Wire Wire Line
	9250 8150 9250 8250
Connection ~ 9250 8250
Wire Wire Line
	9250 8250 9350 8250
Wire Wire Line
	9150 7950 9150 7850
Wire Wire Line
	9150 7850 9250 7850
Wire Wire Line
	9550 7850 9550 7950
Wire Wire Line
	9450 7950 9450 7850
Connection ~ 9450 7850
Wire Wire Line
	9450 7850 9550 7850
Wire Wire Line
	9350 7850 9350 7950
Connection ~ 9350 7850
Wire Wire Line
	9350 7850 9450 7850
Wire Wire Line
	9250 7950 9250 7850
Connection ~ 9250 7850
Wire Wire Line
	9250 7850 9350 7850
$Comp
L Transistor_FET:IRF7480M Q?
U 1 1 5C3D9DBF
P 8750 6600
F 0 "Q?" H 8955 6646 50  0000 L CNN
F 1 "IRF7480M" H 8955 6555 50  0000 L CNN
F 2 "Package_DirectFET:DirectFET_ME" H 8750 6600 50  0001 C CIN
F 3 "https://www.infineon.com/dgdl/Infineon-IRF7480M-DS-v01_02-EN.pdf?fileId=5546d462533600a4015355ff8fa41c30" H 8750 6600 50  0001 L CNN
	1    8750 6600
	1    0    0    -1  
$EndComp
$Comp
L Transistor_FET:IRF7480M Q?
U 1 1 5C3DA5D0
P 8750 5650
F 0 "Q?" H 8955 5696 50  0000 L CNN
F 1 "IRF7480M" H 8950 5800 50  0000 L CNN
F 2 "Package_DirectFET:DirectFET_ME" H 8750 5650 50  0001 C CIN
F 3 "https://www.infineon.com/dgdl/Infineon-IRF7480M-DS-v01_02-EN.pdf?fileId=5546d462533600a4015355ff8fa41c30" H 8750 5650 50  0001 L CNN
	1    8750 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	8850 5850 8850 6150
$Comp
L Transistor_FET:IRF7480M Q?
U 1 1 5C3DAAC4
P 9950 5650
F 0 "Q?" H 10156 5696 50  0000 L CNN
F 1 "IRF7480M" H 10100 5800 50  0000 L CNN
F 2 "Package_DirectFET:DirectFET_ME" H 9950 5650 50  0001 C CIN
F 3 "https://www.infineon.com/dgdl/Infineon-IRF7480M-DS-v01_02-EN.pdf?fileId=5546d462533600a4015355ff8fa41c30" H 9950 5650 50  0001 L CNN
	1    9950 5650
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J?
U 1 1 5C3DB289
P 9300 5950
F 0 "J?" V 9266 5762 50  0000 R CNN
F 1 "Conn_01x02" V 9400 6150 50  0000 R CNN
F 2 "" H 9300 5950 50  0001 C CNN
F 3 "~" H 9300 5950 50  0001 C CNN
	1    9300 5950
	0    -1   -1   0   
$EndComp
$Comp
L Transistor_FET:IRF7480M Q?
U 1 1 5C3DB5E6
P 9950 6600
F 0 "Q?" H 10156 6646 50  0000 L CNN
F 1 "IRF7480M" H 10156 6555 50  0000 L CNN
F 2 "Package_DirectFET:DirectFET_ME" H 9950 6600 50  0001 C CIN
F 3 "https://www.infineon.com/dgdl/Infineon-IRF7480M-DS-v01_02-EN.pdf?fileId=5546d462533600a4015355ff8fa41c30" H 9950 6600 50  0001 L CNN
	1    9950 6600
	-1   0    0    -1  
$EndComp
$Comp
L Device:R_Small R?
U 1 1 5C3DB9A3
P 8250 5650
F 0 "R?" V 8054 5650 50  0000 C CNN
F 1 "0R" V 8145 5650 50  0000 C CNN
F 2 "" H 8250 5650 50  0001 C CNN
F 3 "~" H 8250 5650 50  0001 C CNN
	1    8250 5650
	0    1    1    0   
$EndComp
Wire Wire Line
	8050 5650 8150 5650
Wire Wire Line
	8350 5650 8450 5650
Wire Wire Line
	8450 5700 8450 5650
Wire Wire Line
	8450 5650 8550 5650
$Comp
L power:GND #PWR?
U 1 1 5C3E47F9
P 7350 6500
F 0 "#PWR?" H 7350 6250 50  0001 C CNN
F 1 "GND" H 7355 6327 50  0000 C CNN
F 2 "" H 7350 6500 50  0001 C CNN
F 3 "" H 7350 6500 50  0001 C CNN
	1    7350 6500
	1    0    0    -1  
$EndComp
Wire Wire Line
	8850 6150 9300 6150
Connection ~ 8850 6150
Wire Wire Line
	8850 6150 8850 6400
Wire Wire Line
	9400 6150 9850 6150
Wire Wire Line
	9850 6150 9850 6400
Wire Wire Line
	9850 6150 9850 5850
Connection ~ 9850 6150
Wire Wire Line
	8850 5450 8850 5350
Wire Wire Line
	8850 5350 9350 5350
Wire Wire Line
	9850 5350 9850 5450
Wire Wire Line
	8850 6800 8850 6900
Wire Wire Line
	8850 6900 9350 6900
Wire Wire Line
	9850 6900 9850 6800
Wire Wire Line
	9350 7850 9350 6900
Connection ~ 9350 6900
Wire Wire Line
	9350 6900 9850 6900
Wire Wire Line
	10400 8250 9550 8250
Connection ~ 9550 8250
Wire Wire Line
	10400 8150 9750 8150
Wire Wire Line
	9750 8150 9750 7850
Wire Wire Line
	9750 7850 9550 7850
Connection ~ 9550 7850
Wire Wire Line
	10400 7750 10300 7750
Wire Wire Line
	10300 7750 10300 5350
Wire Wire Line
	10300 5350 9850 5350
Connection ~ 9850 5350
Wire Wire Line
	6200 6250 6200 4100
Wire Wire Line
	6200 4100 9350 4100
Wire Wire Line
	9350 4100 9350 5350
Connection ~ 9350 5350
Wire Wire Line
	9350 5350 9850 5350
Wire Wire Line
	9350 8250 9350 8500
Wire Wire Line
	9350 8500 6200 8500
Wire Wire Line
	6200 8500 6200 6350
$Comp
L power:+15V #PWR?
U 1 1 5C3FCAC5
P 7350 5700
F 0 "#PWR?" H 7350 5550 50  0001 C CNN
F 1 "+15V" H 7365 5873 50  0000 C CNN
F 2 "" H 7350 5700 50  0001 C CNN
F 3 "" H 7350 5700 50  0001 C CNN
	1    7350 5700
	1    0    0    -1  
$EndComp
Connection ~ 8450 5650
$Comp
L Device:R_Small R?
U 1 1 5C405205
P 8250 6600
F 0 "R?" V 8450 6600 50  0000 C CNN
F 1 "0R" V 8350 6600 50  0000 C CNN
F 2 "" H 8250 6600 50  0001 C CNN
F 3 "~" H 8250 6600 50  0001 C CNN
	1    8250 6600
	0    1    1    0   
$EndComp
Wire Wire Line
	8050 6600 8150 6600
$Comp
L components:TC4427 U?
U 1 1 5C406E47
P 7350 6100
F 0 "U?" H 7050 6450 50  0000 C CNN
F 1 "TC4427" H 7600 6450 50  0000 C CNN
F 2 "" H 7350 6100 50  0001 C CNN
F 3 "" H 7350 6100 50  0001 C CNN
	1    7350 6100
	1    0    0    -1  
$EndComp
Wire Wire Line
	7850 5950 8050 5950
Wire Wire Line
	8050 5950 8050 5650
Wire Wire Line
	7850 6250 8050 6250
Wire Wire Line
	8050 6250 8050 6600
$Comp
L Device:C_Small C?
U 1 1 5C40F6B4
P 8450 5800
F 0 "C?" H 8250 5850 50  0000 L CNN
F 1 "0" H 8300 5700 50  0000 L CNN
F 2 "" H 8450 5800 50  0001 C CNN
F 3 "~" H 8450 5800 50  0001 C CNN
	1    8450 5800
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C?
U 1 1 5C40F928
P 8450 6450
F 0 "C?" H 8250 6550 50  0000 L CNN
F 1 "0" H 8300 6400 50  0000 L CNN
F 2 "" H 8450 6450 50  0001 C CNN
F 3 "~" H 8450 6450 50  0001 C CNN
	1    8450 6450
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R?
U 1 1 5C41E378
P 10700 5650
F 0 "R?" V 10504 5650 50  0000 C CNN
F 1 "0R" V 10595 5650 50  0000 C CNN
F 2 "" H 10700 5650 50  0001 C CNN
F 3 "~" H 10700 5650 50  0001 C CNN
	1    10700 5650
	0    -1   1    0   
$EndComp
Wire Wire Line
	10900 5650 10800 5650
Wire Wire Line
	10600 5650 10500 5650
$Comp
L power:GND #PWR?
U 1 1 5C41E380
P 10700 6200
F 0 "#PWR?" H 10700 5950 50  0001 C CNN
F 1 "GND" H 10705 6027 50  0000 C CNN
F 2 "" H 10700 6200 50  0001 C CNN
F 3 "" H 10700 6200 50  0001 C CNN
	1    10700 6200
	-1   0    0    -1  
$EndComp
Wire Wire Line
	10500 5700 10500 5650
$Comp
L power:GND #PWR?
U 1 1 5C41E390
P 11600 6500
F 0 "#PWR?" H 11600 6250 50  0001 C CNN
F 1 "GND" H 11605 6327 50  0000 C CNN
F 2 "" H 11600 6500 50  0001 C CNN
F 3 "" H 11600 6500 50  0001 C CNN
	1    11600 6500
	-1   0    0    -1  
$EndComp
$Comp
L power:+15V #PWR?
U 1 1 5C41E396
P 11600 5700
F 0 "#PWR?" H 11600 5550 50  0001 C CNN
F 1 "+15V" H 11615 5873 50  0000 C CNN
F 2 "" H 11600 5700 50  0001 C CNN
F 3 "" H 11600 5700 50  0001 C CNN
	1    11600 5700
	-1   0    0    -1  
$EndComp
Connection ~ 10500 5650
$Comp
L Device:R_Small R?
U 1 1 5C41E39D
P 10700 6600
F 0 "R?" V 10750 6450 50  0000 C CNN
F 1 "0R" V 10800 6600 50  0000 C CNN
F 2 "" H 10700 6600 50  0001 C CNN
F 3 "~" H 10700 6600 50  0001 C CNN
	1    10700 6600
	0    -1   1    0   
$EndComp
Wire Wire Line
	10900 6600 10800 6600
$Comp
L components:TC4427 U?
U 1 1 5C41E3A6
P 11600 6100
F 0 "U?" H 11300 6450 50  0000 C CNN
F 1 "TC4427" H 11850 6450 50  0000 C CNN
F 2 "" H 11600 6100 50  0001 C CNN
F 3 "" H 11600 6100 50  0001 C CNN
	1    11600 6100
	-1   0    0    -1  
$EndComp
Wire Wire Line
	11100 5950 10900 5950
Wire Wire Line
	10900 5950 10900 5650
Wire Wire Line
	11100 6250 10900 6250
Wire Wire Line
	10900 6250 10900 6600
$Comp
L Device:C_Small C?
U 1 1 5C41E3B0
P 10500 5800
F 0 "C?" H 10592 5846 50  0000 L CNN
F 1 "0" H 10592 5755 50  0000 L CNN
F 2 "" H 10500 5800 50  0001 C CNN
F 3 "~" H 10500 5800 50  0001 C CNN
	1    10500 5800
	-1   0    0    -1  
$EndComp
$Comp
L Device:C_Small C?
U 1 1 5C41E3B6
P 10500 6450
F 0 "C?" H 10592 6496 50  0000 L CNN
F 1 "0" H 10592 6405 50  0000 L CNN
F 2 "" H 10500 6450 50  0001 C CNN
F 3 "~" H 10500 6450 50  0001 C CNN
	1    10500 6450
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5C3E155E
P 8600 6200
F 0 "#PWR?" H 8600 5950 50  0001 C CNN
F 1 "GND" H 8605 6027 50  0000 C CNN
F 2 "" H 8600 6200 50  0001 C CNN
F 3 "" H 8600 6200 50  0001 C CNN
	1    8600 6200
	1    0    0    -1  
$EndComp
Wire Wire Line
	8350 6600 8450 6600
Wire Wire Line
	8450 6600 8450 6550
Connection ~ 8450 6600
Wire Wire Line
	8450 6600 8550 6600
Wire Wire Line
	8450 5900 8450 6150
Wire Wire Line
	8450 6150 8600 6150
Wire Wire Line
	8600 6150 8600 6200
Connection ~ 8450 6150
Wire Wire Line
	8450 6150 8450 6350
Wire Wire Line
	10150 5650 10500 5650
Wire Wire Line
	10150 6600 10500 6600
Wire Wire Line
	10500 6550 10500 6600
Connection ~ 10500 6600
Wire Wire Line
	10500 6600 10600 6600
Wire Wire Line
	10500 6150 10700 6150
Wire Wire Line
	10700 6150 10700 6200
Wire Wire Line
	10500 5900 10500 6150
Connection ~ 10500 6150
Wire Wire Line
	10500 6150 10500 6350
Text Label 6650 5950 0    50   ~ 0
AH
Wire Wire Line
	6650 5950 6850 5950
Wire Wire Line
	6650 6250 6850 6250
Text Label 6650 6250 0    50   ~ 0
AL
Wire Wire Line
	12250 5950 12100 5950
Wire Wire Line
	12250 6250 12100 6250
Text Label 12250 5950 2    50   ~ 0
BH
Text Label 12250 6250 2    50   ~ 0
BL
$Comp
L power:+3V3 #PWR?
U 1 1 5C4568B2
P 10800 7550
F 0 "#PWR?" H 10800 7400 50  0001 C CNN
F 1 "+3V3" H 10815 7723 50  0000 C CNN
F 2 "" H 10800 7550 50  0001 C CNN
F 3 "" H 10800 7550 50  0001 C CNN
	1    10800 7550
	1    0    0    -1  
$EndComp
$Comp
L Regulator_Linear:AMS1117-3.3 U?
U 1 1 5C45B743
P 14000 4100
F 0 "U?" H 14000 4342 50  0000 C CNN
F 1 "AMS1117-3.3" H 14000 4251 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 14000 4300 50  0001 C CNN
F 3 "http://www.advanced-monolithic.com/pdf/ds1117.pdf" H 14100 3850 50  0001 C CNN
	1    14000 4100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5C45EB4A
P 9350 8500
F 0 "#PWR?" H 9350 8250 50  0001 C CNN
F 1 "GND" H 9355 8327 50  0000 C CNN
F 2 "" H 9350 8500 50  0001 C CNN
F 3 "" H 9350 8500 50  0001 C CNN
	1    9350 8500
	1    0    0    -1  
$EndComp
Connection ~ 9350 8500
$Comp
L Device:C_Small C?
U 1 1 5C460459
P 14400 4250
F 0 "C?" H 14492 4296 50  0000 L CNN
F 1 "1u" H 14500 4200 50  0000 L CNN
F 2 "" H 14400 4250 50  0001 C CNN
F 3 "~" H 14400 4250 50  0001 C CNN
	1    14400 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	14400 4350 14400 4400
Wire Wire Line
	14400 4150 14400 4100
Wire Wire Line
	14400 4100 14300 4100
$Comp
L Device:C_Small C?
U 1 1 5C46605E
P 13600 4250
F 0 "C?" H 13400 4300 50  0000 L CNN
F 1 "1u" H 13400 4200 50  0000 L CNN
F 2 "" H 13600 4250 50  0001 C CNN
F 3 "~" H 13600 4250 50  0001 C CNN
	1    13600 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	13600 4400 13600 4350
Wire Wire Line
	13600 4150 13600 4100
Wire Wire Line
	13600 4100 13700 4100
$Comp
L Device:CP_Small C?
U 1 1 5C46BEDF
P 13250 4250
F 0 "C?" H 13000 4300 50  0000 L CNN
F 1 "100u" H 13000 4200 50  0000 L CNN
F 2 "" H 13250 4250 50  0001 C CNN
F 3 "~" H 13250 4250 50  0001 C CNN
	1    13250 4250
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R?
U 1 1 5C47C915
P 12850 4100
F 0 "R?" V 12654 4100 50  0000 C CNN
F 1 "10R .5W" V 12745 4100 50  0000 C CNN
F 2 "" H 12850 4100 50  0001 C CNN
F 3 "~" H 12850 4100 50  0001 C CNN
	1    12850 4100
	0    -1   1    0   
$EndComp
Wire Wire Line
	12950 4100 13250 4100
Wire Wire Line
	13250 4100 13250 4150
Wire Wire Line
	13250 4100 13600 4100
Connection ~ 13250 4100
Connection ~ 13600 4100
$Comp
L power:GND #PWR?
U 1 1 5C45FBC3
P 14000 4450
F 0 "#PWR?" H 14000 4200 50  0001 C CNN
F 1 "GND" H 14005 4277 50  0000 C CNN
F 2 "" H 14000 4450 50  0001 C CNN
F 3 "" H 14000 4450 50  0001 C CNN
	1    14000 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	14400 4400 14000 4400
Wire Wire Line
	13250 4400 13250 4350
Connection ~ 13600 4400
Wire Wire Line
	13600 4400 13250 4400
Connection ~ 14000 4400
Wire Wire Line
	14000 4400 13600 4400
Wire Wire Line
	14000 4400 14000 4450
$Comp
L Device:CP_Small C?
U 1 1 5C48FE5D
P 15100 4250
F 0 "C?" H 15200 4300 50  0000 L CNN
F 1 "470u" H 15200 4200 50  0000 L CNN
F 2 "" H 15100 4250 50  0001 C CNN
F 3 "~" H 15100 4250 50  0001 C CNN
	1    15100 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	14750 4150 14750 4100
Wire Wire Line
	14750 4100 14400 4100
Connection ~ 14400 4100
Wire Wire Line
	14400 4400 14750 4400
Wire Wire Line
	14750 4400 14750 4350
Connection ~ 14400 4400
$Comp
L Device:C_Small C?
U 1 1 5C49E930
P 14750 4250
F 0 "C?" H 14842 4296 50  0000 L CNN
F 1 "10u" H 14850 4200 50  0000 L CNN
F 2 "" H 14750 4250 50  0001 C CNN
F 3 "~" H 14750 4250 50  0001 C CNN
	1    14750 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	14750 4100 15100 4100
Wire Wire Line
	15100 4100 15100 4150
Connection ~ 14750 4100
Wire Wire Line
	14750 4400 15100 4400
Wire Wire Line
	15100 4400 15100 4350
Connection ~ 14750 4400
Wire Wire Line
	12750 4100 12650 4100
Connection ~ 9350 4100
$Comp
L MCU_ST_STM32F1:STM32F103C8Tx U?
U 1 1 5C4C16F6
P 16650 6400
F 0 "U?" H 17050 7850 50  0000 C CNN
F 1 "STM32F103C8Tx" V 17250 7400 50  0000 C CNN
F 2 "Package_QFP:LQFP-48_7x7mm_P0.5mm" H 16050 5000 50  0001 R CNN
F 3 "http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/CD00161566.pdf" H 16650 6400 50  0001 C CNN
	1    16650 6400
	1    0    0    -1  
$EndComp
Wire Wire Line
	16850 4900 16850 4800
Wire Wire Line
	16850 4800 16750 4800
Connection ~ 15100 4100
Wire Wire Line
	16750 4800 16750 4900
Connection ~ 16750 4800
Wire Wire Line
	16750 4800 16650 4800
Wire Wire Line
	16650 4800 16650 4900
Connection ~ 16650 4800
Wire Wire Line
	16650 4800 16550 4800
Wire Wire Line
	16550 4900 16550 4800
Connection ~ 16550 4800
Wire Wire Line
	16550 4800 16450 4800
Wire Wire Line
	16450 4800 16450 4900
$Comp
L power:+3V3 #PWR?
U 1 1 5C4D85E0
P 15100 4000
F 0 "#PWR?" H 15100 3850 50  0001 C CNN
F 1 "+3V3" H 15115 4173 50  0000 C CNN
F 2 "" H 15100 4000 50  0001 C CNN
F 3 "" H 15100 4000 50  0001 C CNN
	1    15100 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	15100 4000 15100 4100
$Comp
L components:MT3608 U?
U 1 1 5C3DB03B
P 14000 3250
F 0 "U?" H 14000 3615 50  0000 C CNN
F 1 "MT3608" H 14000 3524 50  0000 C CNN
F 2 "" H 14000 3250 50  0001 C CNN
F 3 "" H 14000 3250 50  0001 C CNN
	1    14000 3250
	1    0    0    -1  
$EndComp
$Comp
L components:L_EU_small L?
U 1 1 5C3E1491
P 14000 2750
F 0 "L?" H 14000 2942 50  0000 C CNN
F 1 "10u" H 14000 2851 50  0000 C CNN
F 2 "" H 14000 2750 50  0001 C CNN
F 3 "" H 14000 2750 50  0001 C CNN
	1    14000 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	13800 2750 13600 2750
Wire Wire Line
	13600 2750 13600 3150
Wire Wire Line
	13600 3150 13650 3150
Wire Wire Line
	13650 3300 13600 3300
Wire Wire Line
	13600 3300 13600 3150
Connection ~ 13600 3150
Wire Wire Line
	12650 3150 12650 4100
Connection ~ 12650 4100
Wire Wire Line
	12650 4100 9350 4100
Wire Wire Line
	14200 2750 14450 2750
Wire Wire Line
	14450 2750 14450 3150
Wire Wire Line
	14450 3150 14350 3150
$Comp
L Device:D_Schottky_Small_ALT D?
U 1 1 5C3F3E87
P 14650 2750
F 0 "D?" H 14650 2545 50  0000 C CNN
F 1 "2A schottky" H 14650 2636 50  0000 C CNN
F 2 "" V 14650 2750 50  0001 C CNN
F 3 "~" V 14650 2750 50  0001 C CNN
	1    14650 2750
	-1   0    0    1   
$EndComp
Wire Wire Line
	14550 2750 14450 2750
Connection ~ 14450 2750
$Comp
L Device:C_Small C?
U 1 1 5C3F99AD
P 15250 3200
F 0 "C?" H 15342 3246 50  0000 L CNN
F 1 "10u" H 15342 3155 50  0000 L CNN
F 2 "" H 15250 3200 50  0001 C CNN
F 3 "~" H 15250 3200 50  0001 C CNN
	1    15250 3200
	1    0    0    -1  
$EndComp
$Comp
L Device:CP_Small C?
U 1 1 5C3FB1E5
P 15600 3200
F 0 "C?" H 15700 3250 50  0000 L CNN
F 1 "470u" H 15700 3150 50  0000 L CNN
F 2 "" H 15600 3200 50  0001 C CNN
F 3 "~" H 15600 3200 50  0001 C CNN
	1    15600 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	15600 3100 15600 2750
Wire Wire Line
	15600 2750 15250 2750
Wire Wire Line
	15250 3100 15250 2750
Connection ~ 15250 2750
Wire Wire Line
	15250 2750 14950 2750
Wire Wire Line
	15250 3650 15250 3300
Wire Wire Line
	15250 3650 15600 3650
Wire Wire Line
	15600 3650 15600 3300
$Comp
L Device:R_Small R?
U 1 1 5C40F95F
P 14950 3150
F 0 "R?" V 14754 3150 50  0000 C CNN
F 1 "75k" V 14845 3150 50  0000 C CNN
F 2 "" H 14950 3150 50  0001 C CNN
F 3 "~" H 14950 3150 50  0001 C CNN
	1    14950 3150
	1    0    0    1   
$EndComp
$Comp
L Device:R_Small R?
U 1 1 5C417AAA
P 14950 3450
F 0 "R?" V 14754 3450 50  0000 C CNN
F 1 "3.0k" V 14845 3450 50  0000 C CNN
F 2 "" H 14950 3450 50  0001 C CNN
F 3 "~" H 14950 3450 50  0001 C CNN
	1    14950 3450
	1    0    0    1   
$EndComp
Wire Wire Line
	15250 3650 14950 3650
Wire Wire Line
	14000 3650 14000 3500
Connection ~ 15250 3650
Wire Wire Line
	14950 3650 14950 3550
Connection ~ 14950 3650
Wire Wire Line
	14950 3650 14550 3650
Wire Wire Line
	14350 3300 14950 3300
Wire Wire Line
	14950 3300 14950 3350
Wire Wire Line
	14950 3300 14950 3250
Connection ~ 14950 3300
Wire Wire Line
	14950 3050 14950 2750
Connection ~ 14950 2750
Wire Wire Line
	14950 2750 14750 2750
$Comp
L power:GND #PWR?
U 1 1 5C4436DB
P 14550 3650
F 0 "#PWR?" H 14550 3400 50  0001 C CNN
F 1 "GND" H 14555 3477 50  0000 C CNN
F 2 "" H 14550 3650 50  0001 C CNN
F 3 "" H 14550 3650 50  0001 C CNN
	1    14550 3650
	1    0    0    -1  
$EndComp
Connection ~ 14550 3650
Wire Wire Line
	14550 3650 14000 3650
$Comp
L power:+15V #PWR?
U 1 1 5C454A8B
P 15600 2650
F 0 "#PWR?" H 15600 2500 50  0001 C CNN
F 1 "+15V" H 15615 2823 50  0000 C CNN
F 2 "" H 15600 2650 50  0001 C CNN
F 3 "" H 15600 2650 50  0001 C CNN
	1    15600 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	15600 2650 15600 2750
Connection ~ 15600 2750
$Comp
L Device:R_Small R?
U 1 1 5C45B966
P 16050 3150
F 0 "R?" V 16250 3150 50  0000 C CNN
F 1 "47k" V 16150 3150 50  0000 C CNN
F 2 "" H 16050 3150 50  0001 C CNN
F 3 "~" H 16050 3150 50  0001 C CNN
	1    16050 3150
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R?
U 1 1 5C45D762
P 16050 3450
F 0 "R?" V 16250 3450 50  0000 C CNN
F 1 "10k" V 16150 3450 50  0000 C CNN
F 2 "" H 16050 3450 50  0001 C CNN
F 3 "~" H 16050 3450 50  0001 C CNN
	1    16050 3450
	1    0    0    1   
$EndComp
Wire Wire Line
	16050 3250 16050 3300
Connection ~ 16050 3300
Wire Wire Line
	16050 3300 16050 3350
Text Label 16950 3300 2    50   ~ 0
VBOOT_MON
Wire Wire Line
	16050 3300 16400 3300
Wire Wire Line
	16050 3550 16050 3650
Wire Wire Line
	16050 3650 15600 3650
Connection ~ 15600 3650
Wire Wire Line
	15600 2750 16050 2750
Wire Wire Line
	16050 2750 16050 3050
$Comp
L Device:C_Small C?
U 1 1 5C48B1CF
P 16400 3450
F 0 "C?" H 16492 3496 50  0000 L CNN
F 1 "100n" H 16492 3405 50  0000 L CNN
F 2 "" H 16400 3450 50  0001 C CNN
F 3 "~" H 16400 3450 50  0001 C CNN
	1    16400 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	16400 3350 16400 3300
Connection ~ 16400 3300
Wire Wire Line
	16400 3550 16400 3650
Wire Wire Line
	16400 3650 16050 3650
Connection ~ 16050 3650
Wire Wire Line
	16400 3300 16950 3300
$Comp
L Device:R_Small R?
U 1 1 5C4AAEC7
P 12650 4650
F 0 "R?" V 12850 4650 50  0000 C CNN
F 1 "47k" V 12750 4650 50  0000 C CNN
F 2 "" H 12650 4650 50  0001 C CNN
F 3 "~" H 12650 4650 50  0001 C CNN
	1    12650 4650
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R?
U 1 1 5C4AAECD
P 12650 4950
F 0 "R?" V 12850 4950 50  0000 C CNN
F 1 "10k" V 12750 4950 50  0000 C CNN
F 2 "" H 12650 4950 50  0001 C CNN
F 3 "~" H 12650 4950 50  0001 C CNN
	1    12650 4950
	1    0    0    1   
$EndComp
Wire Wire Line
	12650 4750 12650 4800
Connection ~ 12650 4800
Wire Wire Line
	12650 4800 12650 4850
Text Label 13550 4800 2    50   ~ 0
VIN_MON
Wire Wire Line
	12650 4800 13000 4800
Wire Wire Line
	12650 5050 12650 5150
$Comp
L Device:C_Small C?
U 1 1 5C4AAEDB
P 13000 4950
F 0 "C?" H 13092 4996 50  0000 L CNN
F 1 "100n" H 13092 4905 50  0000 L CNN
F 2 "" H 13000 4950 50  0001 C CNN
F 3 "~" H 13000 4950 50  0001 C CNN
	1    13000 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	13000 4850 13000 4800
Connection ~ 13000 4800
Wire Wire Line
	13000 5050 13000 5150
Wire Wire Line
	13000 5150 12650 5150
Wire Wire Line
	13000 4800 13550 4800
Wire Wire Line
	12650 4550 12650 4100
$Comp
L power:GND #PWR?
U 1 1 5C4D989C
P 12650 5250
F 0 "#PWR?" H 12650 5000 50  0001 C CNN
F 1 "GND" H 12655 5077 50  0000 C CNN
F 2 "" H 12650 5250 50  0001 C CNN
F 3 "" H 12650 5250 50  0001 C CNN
	1    12650 5250
	1    0    0    -1  
$EndComp
Wire Wire Line
	12650 5250 12650 5150
Connection ~ 12650 5150
$Comp
L Device:CP_Small C?
U 1 1 5C4E826F
P 13100 3350
F 0 "C?" H 12850 3400 50  0000 L CNN
F 1 "470u" H 12850 3300 50  0000 L CNN
F 2 "" H 13100 3350 50  0001 C CNN
F 3 "~" H 13100 3350 50  0001 C CNN
	1    13100 3350
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C?
U 1 1 5C4E8B95
P 13450 3350
F 0 "C?" H 13250 3400 50  0000 L CNN
F 1 "10u" H 13250 3300 50  0000 L CNN
F 2 "" H 13450 3350 50  0001 C CNN
F 3 "~" H 13450 3350 50  0001 C CNN
	1    13450 3350
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R?
U 1 1 5C503EA9
P 12850 3150
F 0 "R?" V 12654 3150 50  0000 C CNN
F 1 "10R .5W" V 12745 3150 50  0000 C CNN
F 2 "" H 12850 3150 50  0001 C CNN
F 3 "~" H 12850 3150 50  0001 C CNN
	1    12850 3150
	0    -1   1    0   
$EndComp
Wire Wire Line
	12650 3150 12750 3150
Wire Wire Line
	12950 3150 13100 3150
Wire Wire Line
	13450 3250 13450 3150
Connection ~ 13450 3150
Wire Wire Line
	13450 3150 13600 3150
Wire Wire Line
	13100 3250 13100 3150
Connection ~ 13100 3150
Wire Wire Line
	13100 3150 13450 3150
Wire Wire Line
	13100 3450 13100 3650
Wire Wire Line
	13100 3650 13450 3650
Connection ~ 14000 3650
Wire Wire Line
	13450 3450 13450 3650
Connection ~ 13450 3650
Wire Wire Line
	13450 3650 14000 3650
Wire Wire Line
	16450 7900 16450 8000
Wire Wire Line
	16450 8000 16550 8000
Wire Wire Line
	16750 8000 16750 7900
Wire Wire Line
	16650 7900 16650 8000
Connection ~ 16650 8000
Wire Wire Line
	16650 8000 16750 8000
Wire Wire Line
	16550 8000 16550 7900
Connection ~ 16550 8000
Wire Wire Line
	16550 8000 16600 8000
$Comp
L power:GND #PWR?
U 1 1 5C574D2B
P 16600 8100
F 0 "#PWR?" H 16600 7850 50  0001 C CNN
F 1 "GND" H 16605 7927 50  0000 C CNN
F 2 "" H 16600 8100 50  0001 C CNN
F 3 "" H 16600 8100 50  0001 C CNN
	1    16600 8100
	1    0    0    -1  
$EndComp
Wire Wire Line
	16600 8100 16600 8000
Connection ~ 16600 8000
Wire Wire Line
	16600 8000 16650 8000
Wire Wire Line
	16650 4800 16650 4100
Wire Wire Line
	16650 4100 15100 4100
$EndSCHEMATC
