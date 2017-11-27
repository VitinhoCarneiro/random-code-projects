#!/usr/bin/env python3

def clrscr(){
	print("\033[2J")
}

(h, w) = list(map(int, input("Please type in the matrix height and width (space-separated): ").split()))
