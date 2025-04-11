#pragma once

#include "preprocessor.hpp"

void toRGBS(std::shared_ptr<VSNodeWrapper> source, VSCore *core, Api vsapi){
    const VSVideoInfo *vi = source->videoinfo;

    if ((vi->format.bitsPerSample != 32) && (vi->format.colorFamily == cfRGB) && vi->format.sampleType == stFloat){
        return;
    }

    const int matrix = (vi->height > 650)? 1 : 6;
    VSMap* args = vsapi.createMap();
    vsapi.mapConsumeNode(args, "clip", source->node, maReplace);
    vsapi.mapSetInt(args, "matrix_in", matrix, maReplace);
    vsapi.mapSetInt(args, "format", pfRGBS, maReplace);

    VSPlugin* vsplugin = vsapi.getPluginByID(VSH_RESIZE_PLUGIN_ID, core);
    VSMap* ret = vsapi.invoke(vsplugin, "Bicubic", args);
    source->node = vsapi.mapGetNode(ret, "clip", 0, NULL);

    vsapi.freeMap(ret);
    vsapi.freeMap(args);

    return;
}