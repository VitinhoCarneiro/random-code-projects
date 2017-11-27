#!/usr/bin/env python3
#from collections import namedtuple

def graph_mark(adj_lists, nodes_visited, n):
	nodes_visited[n] = True
	for adj in adj_lists[n]:
		if not nodes_visited[adj]:
			graph_mark(adj_lists, nodes_visited, adj)

test_index = 0
while(True):
	test_index += 1
	(n, e) = list(map(int, input().split()))
	if(n == 0 and e == 0): break
	adj_lists = [[]] * n
	nodes_visited = [False] * n
	for i in range(e):
		(a, b) = list(map(int, input().split()))
		#print("read", a, b)
		adj_lists[a - 1].append(b - 1)
		adj_lists[b - 1].append(a - 1)
	
	graph_mark(adj_lists, nodes_visited, 0)
	print("Teste {}".format(test_index))
	if(all(nodes_visited)):	print("normal\n")
	else: print("falha\n")
