#pragma once

#include "preprocessor.hpp"
#include "apiWrapper.hpp"

enum IMPORTER_TYPE{ LSMAS, BESTSOURCE, FFMS2 };

class Importer{
public:
    VSPlugin* plugin = NULL;
    IMPORTER_TYPE type;
    Api& api;
    VSCore* core;
    Importer(Api& api, VSCore* core) : api(api), core(core){
        //choose the default importer
        plugin = api.getPluginByID("com.vapoursynth.bestsource", core);
        if (plugin != NULL){
            type = BESTSOURCE;
            return;
        } 
        plugin = api.getPluginByID("com.vapoursynth.ffms2", core);
        if (plugin != NULL){
            type = FFMS2;
            return;
        }
        plugin = api.getPluginByID("systems.innocent.lsmas", core);
        if (plugin != NULL){
            type = LSMAS;
            return;
        }
        std::cerr << "ERROR: NO IMPORTER WAS FOUND IN " << std::endl;
        api.listPlugin(core);
    }
    Importer(Api& api, VSCore* core, IMPORTER_TYPE type) : api(api), type(type){
        switch (type) {
            case BESTSOURCE:
                plugin = api.getPluginByID("com.vapoursynth.bestsource", core);
                if (plugin != NULL){
                    type = BESTSOURCE;
                    return;
                } 
                std::cerr << "ERROR: Could not find Bestsource" << std::endl;
                break;
            case FFMS2:
                plugin = api.getPluginByID("com.vapoursynth.ffms2", core);
                if (plugin != NULL){
                    type = FFMS2;
                    return;
                }
                std::cerr << "ERROR: Could not find FFMS2" << std::endl;
                break;
            case LSMAS:
                plugin = api.getPluginByID("systems.innocent.lsmas", core);
                if (plugin != NULL){
                    type = LSMAS;
                    return;
                }
                std::cerr << "ERROR: Could not find LSMAS" << std::endl;
                break;
        }
    }
    bool loaded(){
        return plugin != NULL;
    }
    std::shared_ptr<VSNodeWrapper> openVideo(std::string filename){
        VSMap *args = api.createMap();
        api.mapSetData(args, "source", filename.data(), filename.size(), dtUtf8, maReplace);
        VSMap* ret;
        switch (type) {
            case BESTSOURCE:
                ret = api.invoke(plugin, "VideoSource", args);
                break;
            case FFMS2:
                ret = api.invoke(plugin, "Source", args);
                break;
            case LSMAS:
                ret = api.invoke(plugin, "LWLibavSource", args);
                break;
        }
        if (ret == NULL){
            std::cerr << "Error: failed to open video " << filename << " with importer" << std::endl;
            api.freeMap(args);
            return NULL;
        }
        VSNode* out = api.mapGetNode(ret, "clip", 0, NULL);
        if (out == NULL){
            std::cerr << "Error: failed to get the clip " << filename << " from importer" << std::endl;
            api.freeMap(args);
            api.freeMap(ret);
            return NULL;
        }

        api.freeMap(args);
        api.freeMap(ret);
        return std::shared_ptr<VSNodeWrapper>(new VSNodeWrapper(api, out));
    }
    ~Importer(){
        //api.freeCore(core);
    }
};