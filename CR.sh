#!/bin/bash


g++ src/main.cpp src/Game.cpp -o MyGame -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system


# Check if the compilation was successful
if [ $? -eq 0 ]; then
    # Execute the program only if the compilation was successful
    ./MyGame
fi

