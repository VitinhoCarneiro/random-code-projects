#!/usr/bin/env python3
(lines, cols, m, n) = list(map(int, input().split()))
matrix = []
scratch = []
for l in range(lines):
	matrix.append(list(map(int, input().split())))
for i in range(int(lines/m)):
	scratch.append([])
	for j in range(int(cols/n)):
		runningcount = 0
		for l in range(m):
			runningcount += sum(matrix[i*m + l][j*n:(j + 1)*n])
		scratch[i].append(runningcount)
print(max([max(lin) for lin in scratch]))
