lines = []
with open("assets/hunter.obj", "r") as file:
    lines = file.readlines()

with open("assets/hunter.obj", "w") as file:
    for line in lines:
        if ("v  " in line):
            currentLine = line.replace(" ", "", 1)
            file.write(currentLine)
        elif ("f" in line):
            currentLine = line[:-2] + "\n"
            file.write(currentLine)
        else:
            file.write(line)