#!/usr/bin/env python3
import math
slices = int(input())
input()
bread = list(map(int, input().split()))
total = sum(bread)
estimate = 0
got_slices = 0
binary_stride = math.ceil((total / slices) / 2)
while(not got_slices == slices):
	estimate += binary_stride
	got_slices = 0
	for b in bread:
		got_slices += math.floor(b / estimate)
	if(got_slices > slices): binary_stride = abs(math.ceil(binary_stride / 2))
	else: binary_stride = -(abs(math.ceil(binary_stride / 2)))
linear_stride = math.ceil((math.sqrt(estimate)))
#i = 0
#j = 0
while(got_slices == slices):
	#i += 1
	estimate += linear_stride	
	got_slices = 0
	for b in bread:
		got_slices += math.floor(b / estimate)
while(got_slices < slices):
	#j += 1
	estimate -= 1
	got_slices = 0
	for b in bread:
		got_slices += math.floor(b / estimate)
#print("linear tries: i={}, j={}; linear_stride={}".format(i, j, linear_stride))
print(estimate)
