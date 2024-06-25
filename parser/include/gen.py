#!/bin/python3
file = open("commands.hpp","a")
for i in range(2,27):
	file.write(f"#define {chr(65+i)} {i+65}\n")
file.close()
