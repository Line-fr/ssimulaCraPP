#pragma once

#include <stdio.h>
#include <assert.h>
#include<string>
#include<vector>
#include<iostream>
#include<memory>
#include <semaphore>
#include <sstream>
#include <algorithm>

#include "VSHelper4.h"
#include "VapourSynth4.h"

#ifndef Use_Vsscript
#define Use_Vsscript 0
#endif

#if Use_Vsscript == 1
#include "VSScript4.h"
#endif

#define semasize 64