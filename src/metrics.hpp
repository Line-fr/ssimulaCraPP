#include "preprocessor.hpp"
#include "apiWrapper.hpp"
#include "torgbs.hpp"

enum METRICS{SSIMULACRA2, BUTTERAUGLI};
enum METRICSPLUGINS{JXL, VSZIP, VSHIP};

class MetricdatabackStruct{
public:
    std::mutex lock;
    float avg = 0;
    int currentnum = 0;
    std::counting_semaphore<semasize>& sema;
    std::vector<float>& out;
    Api& api;
    METRICSPLUGINS plugintype;
    METRICS type;
    int start;
    int skip;
    MetricdatabackStruct(std::counting_semaphore<semasize>& sema, std::vector<float>& out, Api& api, METRICSPLUGINS plugintype, METRICS type, int start, int skip) : sema(sema), out(out), api(api), plugintype(plugintype), type(type), start(start), skip(skip) {}
};

void MetricAcquireCallback(void* data, const VSFrame* f, int n, VSNode* node, const char* errorMsg){
    MetricdatabackStruct* dat = (MetricdatabackStruct*)data;
    const VSMap* props = dat->api.getFramePropertiesRO(f);
    float res;
    switch (dat->type){
        case SSIMULACRA2:
            res = dat->api.mapGetFloat(props, "_SSIMULACRA2", 0, NULL);
            break;
        case BUTTERAUGLI:
            switch (dat->plugintype){
                case VSHIP:
                    res = dat->api.mapGetFloat(props, "_BUTTERAUGLI_3Norm", 0, NULL);
                    break;
                case JXL:
                    res = dat->api.mapGetFloat(props, "_FrameButteraugli", 0, NULL);
                    break;
            }
            break;
    }

    dat->api.freeFrame(f);

    dat->out[(n-dat->start)/dat->skip] = res;
    dat->lock.lock();
    dat->avg = ((dat->currentnum*dat->avg)+res)/(dat->currentnum+1);
    dat->currentnum++;
    dat->lock.unlock();
    dat->sema.release();
}

class MetricComputer{
public:
    VSPlugin* plugin;
    METRICSPLUGINS plugintype;
    METRICS type;
    Api& api;
    VSCore* core;
    MetricComputer(Api& api, VSCore* core, METRICSPLUGINS plugintype, METRICS type = SSIMULACRA2) : api(api), plugintype(plugintype), type(type), core(core){
        switch (plugintype){
            case VSHIP:
                plugin = api.getPluginByID("com.lumen.vship", core);
                if (plugin == NULL){
                    std::cerr << "Error: failed to find vship" << std::endl;
                }
                break;
            case VSZIP:
                plugin = api.getPluginByID("com.julek.vszip", core);
                if (plugin == NULL){
                    std::cerr << "Error: failed to find vszip" << std::endl;
                }
                break;
            case JXL:
                plugin = api.getPluginByID("com.julek.plugin", core);
                if (plugin == NULL){
                    std::cerr << "Error: failed to find jxl" << std::endl;
                }
                break;
        }
    }
    MetricComputer(Api& api, VSCore* core, METRICS type = SSIMULACRA2) : api(api), type(type), core(core){
        switch (type){
            case SSIMULACRA2:
                plugin = api.getPluginByID("com.lumen.vship", core);
                if (plugin != NULL){
                    plugintype = VSHIP;
                    return;
                }
                plugin = api.getPluginByID("com.julek.vszip", core);
                if (plugin != NULL){
                    plugintype = VSZIP;
                    return;
                }
                plugin = api.getPluginByID("com.julek.plugin", core);
                if (plugin != NULL){
                    plugintype = JXL;
                    return;
                }

                break;

            case BUTTERAUGLI:
                plugin = api.getPluginByID("com.lumen.vship", core);
                if (plugin != NULL){
                    plugintype = VSHIP;
                    return;
                }
                plugin = api.getPluginByID("com.julek.plugin", core);
                if (plugin != NULL){
                    plugintype = JXL;
                    return;
                }

                break;
        }
    }
    std::vector<float> compute(std::shared_ptr<VSNodeWrapper> node1, std::shared_ptr<VSNodeWrapper> node2, int start= 0, int end = -1, int skip = 1, int prefetch = 32, int gputhreads=4){
        if (end < 0) end += node1->videoinfo->numFrames;
        if (end > node1->videoinfo->numFrames) end = node1->videoinfo->numFrames;
        if (start > end) start = end;
        if (end-start < prefetch) prefetch = end-start;
        VSMap* args = api.createMap();
        if (plugintype == JXL){
            toRGBS(node1, core, api);
            toRGBS(node2, core, api);
        }
        switch (plugintype){
            case VSHIP:
                api.mapSetInt(args, "numStream", gputhreads, maReplace);
            case JXL:
            case VSZIP:
                api.mapConsumeNode(args, "reference", node1->node, maReplace);
                api.mapConsumeNode(args, "distorted", node2->node, maReplace);
                node1->node = NULL;
                node2->node = NULL;
                break;
        }
        VSMap* ret = NULL;
        switch (plugintype){
            case VSHIP:
                switch (type){
                    case SSIMULACRA2:
                        ret = api.invoke(plugin, "SSIMULACRA2", args);
                        break;
                    case BUTTERAUGLI:
                        api.mapSetFloat(args, "intensity_multiplier", 203., maReplace);
                        ret = api.invoke(plugin, "BUTTERAUGLI", args);
                        break;
                }
                break;
            case JXL:
                switch (type){
                    case SSIMULACRA2:
                        ret = api.invoke(plugin, "SSIMULACRA", args);
                        break;
                    case BUTTERAUGLI:
                        api.mapSetFloat(args, "intensity_target", 203., maReplace);
                        ret = api.invoke(plugin, "Butteraugli", args);
                        break;
                }
                break;
            case VSZIP:
                switch (type){
                    case SSIMULACRA2:
                        ret = api.invoke(plugin, "Metrics", args);
                        break;
                }
                break;
        }
        api.freeMap(args);

        if (ret == NULL){
            std::cerr << "Failed to compute metric with the given plugin" << std::endl;
            return {};
        }

        int error;
        VSNodeWrapper resnode(api, api.mapGetNode(ret, "clip", 0, &error));
        api.freeMap(ret);

        if (error != peSuccess){
            std::cerr << "Failure to get back the node from the metric plugin with error : " << error << std::endl;
            return {};
        }
        
        std::counting_semaphore<semasize> sema(prefetch);
        int totalframe = (end-start)/skip;
        std::vector<float> res(totalframe);
        MetricdatabackStruct data(sema, res, api, plugintype, type, start, skip);
        int print_refresh_min = 50;
        int last_refresh = start;
        int length = 50;
        auto now = std::chrono::system_clock::now();
        size_t init = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        std::cout << "[";
        for (int i = 0; i < length; i++){
            std::cout << " ";
        }
        std::cout << "] 0% Avg: NONE FPS: 0.";
        for (int i = start; i < end; i+=skip){
            if ((i+1)*length/((end-start)/skip) != (i)*length/((end-start)/skip) || print_refresh_min <= (i-last_refresh)){ //switch!
                std::cout << '\r' << "[";
                for (int j = 0; j < length; j++){
                    if (j < (i+1)*length/((end-start))) {
                        std::cout << "|";
                    } else {
                        std::cout << " ";
                    }
                }
                data.lock.lock();
                int completed = data.currentnum+1;
                data.lock.unlock();
                now = std::chrono::system_clock::now();
                int totalmilli = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count()-init;
                float fps = (float)completed*1000/totalmilli;
                std::cout << "] " << (int)((float)completed*100/((end-start)/skip)) << "% Avg: " << data.avg << " FPS: " << fps <<  "                 ";
                std::cout << std::flush;
                last_refresh = i;
            }
            sema.acquire();
            api.getFrameAsync(i, resnode.node, MetricAcquireCallback, &data);
        }
        for (int i = 0; i < prefetch; i++){
            sema.acquire();
        }
        std::cout << "\r" << "[";
        for (int i = 0; i < length; i++){
            std::cout << "|";
        }
        now = std::chrono::system_clock::now();
        int totalmilli = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count()-init;
        int completed = data.currentnum+1;
        float fps = (float)completed*1000/totalmilli;
        std::cout << "] 100% Avg: " << data.avg << " FPS: " << fps;
        std::cout << std::endl;
        return res;

    }
    ~MetricComputer(){
        //api.freeCore(core);
    }
};
