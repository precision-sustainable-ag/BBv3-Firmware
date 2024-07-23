#!/bin/python3
import os
os.chdir(os.path.dirname(__file__))
file = open("commands1.hpp","a")
for i in range(2,27):
	file.write(f"#define {chr(65+i)} {i+65}\n")
file.close()
