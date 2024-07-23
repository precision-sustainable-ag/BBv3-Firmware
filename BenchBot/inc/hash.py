from ctypes import c_int32
import inquirer
import os
import math

os.chdir(os.path.dirname(__file__))

states = {}

def hash(input_str: str):
    input_str = [int(x) for x in input_str.encode()]
    hash_out = 0
    for char in input_str:
        hash_out = (hash_out * 256) + (char)
    return c_int32(hash_out).value

def load_file():
    global states
    file = open("commands.h")
    lines = file.readlines()
    print(lines[0])
    lines.remove("#ifndef COMMANDS_H_\n")
    lines.remove("#define COMMANDS_H_\n")
    lines.remove("\n")
    lines.remove("#endif\n")
    print(len(lines))
    if (len(lines) % 3) != 0:
        raise ValueError("Line Formatting wrong")
    for i in range(0,math.floor(len(lines) / 3)):
        states[lines[i*3].split(" ")[1]] = "".join([lines[(i*3)], lines[(i*3) + 1], "\n"])
   
def add_state():
    global states
    name = input("Enter state name: ").upper()
    str_key = input("Enter string key: ")
    states[name] = f"#define {name}_KEY 0x{hash(str_key):X}\n#define {name}_STR \"{str_key}\"\n\n"

def write_states():
    global states
    holder = ["#ifndef COMMANDS_H_\n","#define COMMANDS_H_\n","\n"] + [str(x) for x in states.values()] + ["#endif"]
    file = open("commands.h", "w")
    file.writelines(holder)

base_actions = [inquirer.List("Next Action", "Action",
            choices= ["Add Item", "Remove Item", "Save to File", "Load from File", "KILL"]
)]

load_file()

while True:
    action = inquirer.prompt(base_actions).get("Next Action")
    if action == "Add Item":
        add_state()
    elif action == "Remove Item":
        holder = inquirer.prompt([inquirer.List("Remove", "Select State for Removal", choices = (list(states.keys()) + ["@cancel"]))]).get("Remove")
        if holder != "@cancel": states.pop(holder)
    elif action == "Save to File":
        write_states()
    elif action == "Load from File":
        load_file()
    elif action == "KILL":
        break
