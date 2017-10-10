#!/usr/bin/env python3
iteration = 0
while(True):
	iteration += 1
	(numsamples, averaging) = list(map(int, input().split()))
	if(numsamples == 0 and averaging == 0): break
	samples = [int(input()) for s in range(numsamples)]
	scratch = [0 for i in range(int(numsamples - averaging + 1))]
	scratch[0] = int(sum(samples[0:averaging]))
	for i in range(int(numsamples - averaging)):
		scratch[i + 1] = scratch[i] - samples[i] + samples[i + averaging]
	results = [int(s/averaging) for s in scratch]
	print("Teste {}".format(iteration))
	print("{} {}".format(min(results), max(results)))
	print()
