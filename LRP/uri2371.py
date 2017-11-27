#!/usr/bin/env python3

def wreck_search(matrix, i, j):
	if(i < 0 or i >= len(matrix) or j < 0 or j >= len(matrix[0])): return False
	if(matrix[i][j] == "."): return False
	if(matrix[i][j] == "#"):
		return True
	matrix[i][j] = "."
	return wreck_search(matrix, i + 1, j) or wreck_search(matrix, i - 1, j) or wreck_search(matrix, i, j + 1) or wreck_search(matrix, i, j - 1)

(h, w) = list(map(int, input().split()))
matrix = []
for a in range(h):
	matrix.append(list(input()))
shots = int(input())
for a in range(shots):
	(i, j) = list(map(int, input().split()))
	#print("shot at {} {}".format(i, j))
	if(matrix[i - 1][j - 1] == "#"): 
		matrix[i - 1][j - 1] = "!"
		#print("strike at {} {}".format(i, j))
		#for i in matrix:print(i)
hit_ships = 0
for a in range(h):
	for b in range(w):
		if(matrix[a][b] == "!"):
			if not wreck_search(matrix, a, b): hit_ships += 1
print(hit_ships)
for i in matrix: print(i)
