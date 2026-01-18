import os
import subprocess

def retrieveData():
    threads = input("Enter the number of threads you would like the renderer to use: ")
    while not threads.isnumeric() or "." in threads or " " in threads:
        threads = input("Only natural number values are allowed try again: ")
    samples = input("Enter the rays per pixel the renderer should use: ")
    while not samples.isnumeric() or "." in samples or " " in samples:
        samples = input("Only natural number values are allowed try again: ")
    return threads, samples

def pathTraceScene():
    rendererData = retrieveData()
    compile = subprocess.Popen(["clang++", "-std=c++17", "main.cpp"])
    compile.wait()
    subprocess.run(["./a.out", rendererData[0], rendererData[1]])

pathTraceScene()
