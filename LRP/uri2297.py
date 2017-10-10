#!/usr/bin/env python3
test_count = 0
while True:
	test_count += 1
	rounds = int(input())
	if(rounds == 0): break
	cards = [0, 0]
	for r in range(rounds):
		result = list(map(int, input().split()))
		cards[0] += result[0]
		cards[1] += result[1]
	print("Teste {}".format(test_count))
	if(cards[0] > cards[1]): print("Aldo\n")
	else: print("Beto\n")
