#!/usr/bin/env python3
test_count = 0
while True:
	test_count += 1
	regions = int(input())
	if(regions == 0): break
	rect = list(map(int, input().split()))
	for i in range(regions - 1):
		newrect = list(map(int, input().split()))
		if newrect[0] > rect[0]: rect[0] = newrect[0]
		if newrect[1] < rect[1]: rect[1] = newrect[1]
		if newrect[2] < rect[2]: rect[2] = newrect[2]
		if newrect[3] > rect[3]: rect[3] = newrect[3]
	if(rect[0] < rect[2] and rect[1] > rect[3]): print("Teste {}\n{} {} {} {}\n".format(test_count, *rect))
	else: print("Teste {}\nnenhum\n".format(test_count))
