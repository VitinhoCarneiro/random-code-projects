#!/usr/bin/env python3

(h, w) = list(map(int, input().split()))
matrix = []
for i in range(h):
	matrix.append(list(input()))
coastsize = 0
for i in range(h):
	for j in range(w):
		if(i > 0 and i < (h - 1) and j > 0 and j < (w - 1)):
			if(matrix[i][j] == "#" and (matrix[i + 1][j] == "." or matrix[i - 1][j] == "." or matrix[i][j + 1] == "." or matrix[i][j - 1] == ".")): coastsize += 1
		elif(matrix[i][j] == "#"): coastsize += 1
print(coastsize)
