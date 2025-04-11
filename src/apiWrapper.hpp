#pragma once

#include "preprocessor.hpp"

class Api{
public:
    const VSAPI* vsapi = NULL;
    #if Use_Vsscript == 1
    const VSSCRIPTAPI* vssapi = NULL;
    #endif

    Api(const VSAPI* vsapi){
        this->vsapi = vsapi;
    }
    void freeNode(VSNode* node){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot freeNode" << std::endl;
        } else {
        vsapi->freeNode(node);
        }
    }
    VSCore* createCore(int flags){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot createCore" << std::endl;
            return NULL;
        } else {
            return vsapi->createCore(flags);
        }
    }
    void addLogHandler(VSLogHandler handler, VSLogHandlerFree free, void* userData, VSCore* core){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot addLogHandler" << std::endl;
        } else {
            vsapi->addLogHandler(handler, free, userData, core);
        }
    }
    VSMap* createMap(){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot createMap" << std::endl;
            return NULL;
        } else {
            return vsapi->createMap();
        }
    }
    void freeMap(VSMap* map){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot freeMap" << std::endl;
        } else {
            vsapi->freeMap(map);
        }
    }
    int mapSetData(VSMap* map, const char* key, const char* data, int size, int type, int append){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot mapSetData" << std::endl;
            return 2;
        } else {
            return vsapi->mapSetData(map, key, data, size, type, append);
        }
    }
    int mapSetInt(VSMap* map, const char* key, int integer, int append){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot mapSetInt" << std::endl;
            return 2;
        } else {
            return vsapi->mapSetInt(map, key, integer, append);
        }
    }
    int mapSetFloat(VSMap* map, const char* key, float val, int append){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot mapSetFloat" << std::endl;
            return 2;
        } else {
            return vsapi->mapSetFloat(map, key, val, append);
        }
    }
    VSPlugin* getPluginByID(std::string identifier, VSCore* core){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot getPluginByID" << std::endl;
            return NULL;
        } else {
            return vsapi->getPluginByID(identifier.data(), core);
        }
    }
    VSPlugin* getPluginByNamespace(std::string ns, VSCore* core){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot getPluginByNamespace" << std::endl;
            return NULL;
        } else {
            return vsapi->getPluginByNamespace(ns.data(), core);
        }
    }
    VSMap* invoke(VSPlugin* plugin, const char* name, const VSMap* args){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot Invoke" << std::endl;
            return NULL;
        } else {
            return vsapi->invoke(plugin, name, args);
        }
    }
    VSNode* mapGetNode(const VSMap* map, std::string key, int index, int* error){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot mapGetNode" << std::endl;
            return NULL;
        } else {
            return vsapi->mapGetNode(map, key.data(), index, error);
        }
    }
    float mapGetFloat(const VSMap* map, std::string key, int index, int* error){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot mapGetFloat" << std::endl;
            return 0.;
        } else {
            return vsapi->mapGetFloat(map, key.data(), index, error);
        }
    }
    void mapSetNode(VSMap* map, std::string key, VSNode* node, int append){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot mapSetNode" << std::endl;
        } else {
            vsapi->mapSetNode(map, key.data(), node, append);
        }
    }
    void mapConsumeNode(VSMap* map, std::string key, VSNode* node, int append){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot mapConsumeNode" << std::endl;
        } else {
            vsapi->mapConsumeNode(map, key.data(), node, append);
        }
    }
    void freeCore(VSCore* core){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot freeCore" << std::endl;
        } else {
            return vsapi->freeCore(core);
        }
    }
    const VSFrame* getFrame(int n, VSNode* node, char* errorMsg = NULL, int bufSize = 0){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot getFrame" << std::endl;
            return NULL;
        } else {
            return vsapi->getFrame(n, node, errorMsg, bufSize);
        }
    }
    void getFrameAsync(int n, VSNode* node, VSFrameDoneCallback callback, void* userdata){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot getFrameAsync" << std::endl;
        } else {
            vsapi->getFrameAsync(n, node, callback, userdata);
        }
    }
    void freeFrame(const VSFrame* frame){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot freeFrame" << std::endl;
        } else {
            return vsapi->freeFrame(frame);
        }
    }
    const VSVideoInfo * getVideoInfo(VSNode* node){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot getVideoInfo" << std::endl;
            return NULL;
        } else {
            return vsapi->getVideoInfo(node);
        }
    }
    const VSMap* getFramePropertiesRO(const VSFrame* f){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot getFramePropertiesRO" << std::endl;
            return NULL;
        } else {
            return vsapi->getFramePropertiesRO(f);
        }
    }
    void listPlugin(VSCore* core){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot listPlugin" << std::endl;
        } else {
            VSPlugin* plugin = vsapi->getNextPlugin(NULL, core);
            std::cout << "Plugin List : " << std::endl;
            while (plugin != NULL){
                std::cout << " - " << vsapi->getPluginName(plugin) << " of ID " << vsapi->getPluginID(plugin) << " and Namespace " << vsapi->getPluginNamespace(plugin) << std::endl;
                plugin = vsapi->getNextPlugin(plugin, core);
            }
        }
    }
    void getCoreInfo(VSCore* core, VSCoreInfo* infos){
        if (vsapi == NULL){
            std::cerr << "ERROR: vsapi is null. I cannot getCoreInfo" << std::endl;
            return;
        } else {
            return vsapi->getCoreInfo(core, infos);
        }
    }
    #if Use_Vsscript == 1
    Api(const VSAPI* vsapi, const VSSCRIPTAPI* vssapi){
        this->vsapi = vsapi;
        this->vssapi = vssapi;
    }
    Api(const VSSCRIPTAPI* vssapi){
        this->vssapi = vssapi;
        this->vsapi = vssapi->getVSAPI(VAPOURSYNTH_API_VERSION);
        assert(vsapi);
    }
    Api(){
        this->vssapi = getVSScriptAPI(VSSCRIPT_API_VERSION);
        if (!vssapi) {
            // VapourSynth probably isn't properly installed at all
            std::cerr << "Failed to initialize VSScript library" << std::endl;
            return;
        }
        this->vsapi = vssapi->getVSAPI(VAPOURSYNTH_API_VERSION);
        assert(vsapi);
    }
    VSScript* createScript(VSCore* core){
        if (vssapi == NULL) {
            std::cerr << "ERROR: vssapi is null. I cannot freescript" << std::endl;
            return NULL;
        } else {
            return vssapi->createScript(core);
        }
    }
    int evaluateFile(VSScript* se, std::string filename){
        if (vssapi == NULL) {
            std::cerr << "ERROR: vssapi is null. I cannot freescript" << std::endl;
            return -1;
        } else {
            return vssapi->evaluateFile(se, filename.data());
        }
    }
    void freeScript(VSScript* se){
        if (vssapi == NULL) {
            std::cerr << "ERROR: vssapi is null. I cannot freescript" << std::endl;
        } else {
            vssapi->freeScript(se);
        }
    }
    VSNode* getOutputNode(VSScript* se, int outputnode){
        if (vssapi == NULL) {
            std::cerr << "ERROR: vssapi is null. I cannot freescript" << std::endl;
            return NULL;
        } else {
            return vssapi->getOutputNode(se, outputnode);
        }
    }
    std::string getError(VSScript* se){
        if (vssapi == NULL) {
            std::cerr << "ERROR: vssapi is null. I cannot freescript" << std::endl;
            return "NULL";
        } else {
            return vssapi->getError(se);
        }
    }

    int supportVSScript() {
        return 1;
    }
    #else
    Api(){
        this->vsapi = getVapourSynthAPI(VAPOURSYNTH_API_VERSION);
    }
    int supportVSScript() {
        return 0;
    }
    #endif
};

class VSFrameWrapper{
    public:
        Api& api;
    
        const VSFrame* frame = NULL;
    
        VSFrameWrapper(Api& api, const VSFrame* frame) : api(api), frame(frame) {
    
        }
        ~VSFrameWrapper(){
            if (frame != NULL) api.freeFrame(frame);
        }
    };

class VSNodeWrapper{
public:
    Api& api;

    #if Use_Vsscript == 1
    VSScript* se = NULL;
    #endif
    
    VSNode* node = NULL;

    const VSVideoInfo * videoinfo = NULL;

    VSNodeWrapper(Api& api, VSNode* node) : api(api){
        this->node = node;
        videoinfo = api.getVideoInfo(node);
    }
    std::shared_ptr<VSFrameWrapper> getFrame(int i, char* errorMsg = NULL, int bufSize = 0){
        if (i >= videoinfo->numFrames){
            std::cerr << "ERROR: you tried accessing a frame at index " << i << " while the clip if of length " << videoinfo->numFrames << std::endl;
            return NULL;
        }
        return std::make_shared<VSFrameWrapper>(api, api.getFrame(i, node, errorMsg, bufSize));
    }
    void debug(){
        if (videoinfo == NULL){
            std::cout << "Cannot print Video Info because it was not initialized" << std::endl;
            return;
        }
        std::cout << "VSNode with " << videoinfo->numFrames << " frames of size " << videoinfo->width << "x" << videoinfo->height << " with " << videoinfo->fpsNum << "/" << videoinfo->fpsDen << " fps" << std::endl;
    }
    #if Use_Vsscript == 1
    VSNodeWrapper(Api& api, VSNode* node, VSScript* se) : api(api){
        this->node = node;
        this->se = se;
        videoinfo = api.getVideoInfo(node);
    }
    #endif
    void destroy(){
        if (node != NULL) api.freeNode(node);
        node = NULL;
        #if Use_Vsscript
        if (se != NULL) api.freeScript(se);
        se = NULL;
        #endif
    }
    ~VSNodeWrapper(){
        if (node != NULL) api.freeNode(node);
        
        #if Use_Vsscript
        if (se != NULL) api.freeScript(se);
        #endif
    }
};