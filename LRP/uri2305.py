#!/usr/bin/env python3
(lines, cols, m, n) = list(map(int, input().split()))
matrix = []
highest = 0
laststartcount = 0
for i in range(lines):
	matrix.append(list(map(int, input().split())))
for l in range(m): laststartcount += sum(matrix[l][0:n])
for i in range(cols - n + 1):
	runningcount = laststartcount
	if i > 0:
		for l in range(m):
			runningcount += matrix[l][i + n - 1] - matrix[l][i - 1]
	laststartcount = runningcount
	for j in range(1, lines - m + 1):
		runningcount += sum(matrix[j + m - 1][i:i + n]) - sum(matrix[j - 1][i:i + n])
		if(runningcount > highest): highest = runningcount
print(highest)
