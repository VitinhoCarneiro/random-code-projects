#!/usr/bin/env python3

def merge_sort(elements, reverse=False, key=lambda i: i, comp=lambda i, j: i > j):
	if len(elements) > 2:
		list1 = merge_sort(elements[:int(len(elements) / 2)], reverse=reverse, key=key, comp=comp)
		list2 = merge_sort(elements[int(len(elements) / 2):], reverse=reverse, key=key, comp=comp)
		result = merge(list1, list2, reverse=reverse, key=key, comp=comp)
		return result
	else:
		if len(elements) < 2: return elements
		if(comp(key(elements[0]), key(elements[1])) ^ reverse): elements = elements[::-1]
		return elements

def merge(list1, list2, reverse=False, key=lambda i: i, comp=lambda i, j: i > j):
	result = []
	while(len(list1) > 0 and len(list2) > 0):
		if (comp(key(list1[0]), key(list2[0]))) ^ reverse: result.append(list2.pop(0))
		else: result.append(list1.pop(0))
	if(len(list1) > 0): result.extend(list1)
	else: result.extend(list2)
	return result

num_inputs = int(input())
even = []
odd = []
for i in range(num_inputs):
	num = int(input())
	if(num & 1 > 0): odd.append(num)
	else: even.append(num)
even = merge_sort(even, reverse=False)
odd = merge_sort(odd, reverse=True)
for i in even: print(i)
for i in odd: print(i)
