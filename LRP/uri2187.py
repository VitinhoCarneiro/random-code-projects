#!/usr/bin/env python3
import math
bills = (50.0, 10.0, 5.0, 1.0)
test_index = 0
while True:
	test_index += 1
	value = int(input())
	if(value == 0): break
	amounts = []
	for bill in bills:
		amt = math.floor(value / bill)
		value -= bill * amt
		amounts.append(amt)
	print("Teste {}\n{} {} {} {}\n".format(test_index, amounts[0], amounts[1], amounts[2], amounts[3]))
