#!/usr/bin/env python3

def merge_sort(elements, reverse=False):
	if len(elements) > 2:
		list1 = merge_sort(elements[:int(len(elements) / 2)], reverse=reverse)
		list2 = merge_sort(elements[int(len(elements) / 2):], reverse=reverse)
		result = merge(list1, list2, reverse=reverse)
		return result
	else:
		if len(elements) < 2: return elements
		if((elements[0] > elements[1]) ^ reverse): elements = elements[::-1]
		return elements

def merge(list1, list2, reverse=False):
	result = []
	while(len(list1) > 0 and len(list2) > 0):
		if (list1[0] < list2[0]) ^ reverse: result.append(list1.pop(0))
		else: result.append(list2.pop(0))
	if(len(list1) > 0): result.extend(list1)
	else: result.extend(list2)
	return result

num_tests = int(input())
for test in range(num_tests):
	input()
	arrival_queue = list(map(int, input().split()))
	grade_queue = merge_sort(arrival_queue, reverse=True)
	unchanged_count = 0
	for i in range(len(arrival_queue)):
		if(arrival_queue[i] == grade_queue[i]): unchanged_count += 1
	print(unchanged_count)

