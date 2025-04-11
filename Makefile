ifeq ($(OS),Windows_NT)
	exeend := .exe
	vapoursynthlinkscript := -L "%APPDATA%\..\Local\Programs\VapourSynth\core" -lvsscript
	vapoursynthlink := -L "%APPDATA%\..\Local\Programs\VapourSynth\core" -lvapoursynth
else
	exeend := 
	vapoursynthlinkscript := -l:libvapoursynth-script.a
	vapoursynthlink := -lvapoursynth
endif

build:
	g++ src/main.cpp -I include/ -std=c++20 -DUse_Vsscript $(vapoursynthlinkscript) $(shell python3-config --libs --embed) -o ssimulaCraPP$(exeend)

buildNoScript:
	g++ src/main.cpp -I include/ -std=c++20 $(vapoursynthlink) -o ssimulaCraPP$(exeend)
