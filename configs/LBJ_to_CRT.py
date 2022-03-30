#!/usr/bin/env python
# -*- coding: utf-8 -*-

import numpy as np
import pandas as pd
import sys

# Get arguments
if len(sys.argv) < 4:
	print("Not enough arguments!")
	print("1: lbj-setup.csv")
	print("2: lbj-name")
	print("3: 192.168.3.133")
	sys.exit()

# Read CSV file	
csv_file = pd.read_csv(sys.argv[1], index_col=0)

# Print header
print("Section LabJack")
print("name=" + sys.argv[2])

ch_count = 0
for i in range(0, csv_file.shape[0]):

	if ("single" in str(csv_file.iat[i, 5]).lower()) or ("diff" in str(csv_file.iat[i, 5]).lower() and "positive" in str(csv_file.iat[i, 2]).lower()):
		ch_count += 1
		
print("channel=" + str(ch_count))
print("con=2")
print("id=" + sys.argv[3])

# Print channels
ch_count = 0
for i in range(0, csv_file.shape[0]):

	if ("single" in str(csv_file.iat[i, 5]).lower()) or ("diff" in str(csv_file.iat[i, 5]).lower() and "positive" in str(csv_file.iat[i, 2]).lower()):
			
			# Set channel	
			print("c" + str(ch_count) + "n=" + str(csv_file.iat[i, 6]))

			if "single" in str(csv_file.iat[i, 5]).lower():
				print("c" + str(ch_count) + "pc=" + str(csv_file.iat[i, 1])[3:])
				print("c" + str(ch_count) + "nc=" + str(199))

			if "diff" in str(csv_file.iat[i, 5]).lower() and "positive" in str(csv_file.iat[i, 2]).lower():
				print("c" + str(ch_count) + "pc=" + str(csv_file.iat[i, 1])[3:])
				print("c" + str(ch_count) + "nc=" + str(csv_file.iat[i, 4])[3:])
						
			# Set boundary
			if len(csv_file.columns) > 7:
				print("c" + str(ch_count) + "b=" + str(csv_file.iat[i, 7]))
			else:
				print("c" + str(ch_count) + "b=" + str(0))
			
			# Set gain
			if len(csv_file.columns) > 8:
				print("c" + str(ch_count) + "g=" + str(csv_file.iat[i, 8]))
			else:
				print("c" + str(ch_count) + "g=" + str(1))
			# Set range
			if len(csv_file.columns) > 9:
				print("c" + str(ch_count) + "r=" + str(csv_file.iat[i, 9]))
			else:
				print("c" + str(ch_count) + "r=" + str(10))

			ch_count += 1			
			
#			print("(" + str(i) + ", " + str(j) + "): " + str(csv_file.iat[i, j]))

print("EndSection")
