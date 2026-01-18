import math
import subprocess
import sys
scriptname = sys.argv[0]
originalfile = sys.argv[1]
filename = sys.argv[2]
result = subprocess.run(['magick', originalfile, '-background','black' ,'-alpha', 'remove','-compress', 'none', filename], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

count = 0
with open(filename, "r") as file:
    lines = file.readlines()

header = lines[:3]
data = lines[3:]

with open(filename, "w") as file:
    for line in header:
        file.write(line)
    
    for line in data:
        splitted = line.split(" ")
        currentLine = ''
        for i in range(1,len(splitted)):
            if (i%3 == 0):
                currentLine += str(math.floor(int(splitted[i-1]))) + "\n"
                file.write(currentLine)
                currentLine = ''
            else:
                currentLine += str(math.floor(int(splitted[i-1]))) + " "