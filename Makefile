

build:
	g++ src/main.cpp -I include/ -std=c++20 -DUse_Vsscript -l:libvapoursynth-script.a $(shell python3-config --libs --embed) -o ssimulaCraPP

buildNoScript:
	g++ src/main.cpp -I include/ -std=c++20 -lvapoursynth -o ssimulaCraPP
