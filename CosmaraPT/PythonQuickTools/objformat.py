def formatFaces(line):
    restof = line[2:]
    splitted = restof.split()
    lines = []
    for k in splitted:
        currentsplit = k.split("/")
        currentline = "f "
        for face in currentsplit:
            currentline += face
            currentline += " "
        lines.append(currentline)
    return lines

def formatFaces2(line):
    restof = line[2:]
    splitted = restof.split()
    currentline = "f "
    for k in range(len(splitted)):
        currentsplit = splitted[k].split("/")[0]
        currentline += currentsplit
        if (k == len(splitted) - 1):currentline += "\n"
        else:currentline += " "
    return currentline

lines_store = []
lines_out = []

with open("assets/hunter.obj", "r") as f:
    lines_store = f.readlines()

for line in lines_store:
    if line[0] == "v" and line[1] == " ":
        lines_out.append(line)

for line in lines_store:
    if line[0] == "f":
        lines_out.append(formatFaces2(line))

with open("assets/hunter.obj", "w") as f:
    f.writelines(lines_out)
