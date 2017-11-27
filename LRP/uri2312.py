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

def cmp_country(c1, c2):
	if not(c1[1] == c2[1]): return c1[1] < c2[1]
	if not(c1[2] == c2[2]): return c1[2] < c2[2]
	if not(c1[3] == c2[3]): return c1[3] < c2[3]
	return c1[0] > c2[0]

num_countries = int(input())
country_list = [input().split() for i in range(num_countries)]
parsed_list = []
for country in country_list:
	a = [country[0]]
	a.extend(list(map(int, country[1:])))
	parsed_list.append(a)
sorted_list = merge_sort(parsed_list, reverse=False, comp=cmp_country)
for country in sorted_list: print(' '.join(list(map(str, country))))
