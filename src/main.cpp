#include <fstream>
#include "preprocessor.hpp"
#include "apiWrapper.hpp"
#include "videoOpener.hpp"
#include "metrics.hpp"
#include "logHandler.hpp"

void printUsage(){
    std::cout << R"(usage: ./ssimulaCraPP [-h] [--source SOURCE] [--encoded ENCODED] [--sourceScript SCRIPT OUTPUTNODE] [--encodedScript SCRIPT OUTPUTNODE]
                    [-i {ffms2,bestsource,lsmash}] [-si {ffms2,bestsource,lsmash}]
                    [-ei {ffms2,bestsource,lsmash}]
                    [-m {ssimu2_vszip,ssimu2_vship,butter_vship,ssimu2_jxl,butter_jxl}]
                    [-t THREADS] [--width WIDTH] [--height HEIGHT] [-e EVERY] [--start START] [--end END]
                    [--installed] [--version] [-v]

Use metrics to score the quality of videos compared to a source.

options:
  -h, --help            show this help message and exit
  --source SOURCE       Source video path. Can be relative to this script or a full path.
  --encoded ENCODED
                        Encoded video path. Can be relative to this script or a full path.
  -i, --importer {ffms2,bestsource,lsmash}
                        Video importer for the source and encoded videos.
  -si, --source-importer {dgdecnv,ffms2,bestsource,lsmash}
                        Source video importer. Overrides -i (--importer) for the source video.
  -ei, --encoded-importer {dgdecnv,ffms2,bestsource,lsmash}
                        Encoded video importer. Overrides -i (--importer) for the encoded video.
  -m, --metric {ssimu2_vszip,ssimu2_vship,butter_vship,ssimu2_jxl,butter_jxl}
                        Metric to use. (default: ssimu2_vszip)
  -t, --threads THREADS
                        Number of threads to use. Default: -1 (Auto).
  -g, --gpu-threads THREADS
                        Number of gpu threads to use for vship. Default: 8.
  -e, --every EVERY     Frames calculated every nth frame. Default: 1 (Every frame is calculated). For example,
                        setting this to 5 will calculate every 5th frame.
  --start START         Start frame. Default: 0 (First frame).
  --end END             End frame. Default: (Last frame).
  --installed           Print out a list of dependencies and whether they are installed then exit.
  --json PATH           Outputs a file with every score that got computed in a list)" << std::endl;
}

bool plugingIsInstalled(Api& api, VSCore* core, METRICSPLUGINS plugin){
    switch (plugin){
        case VSHIP:
        return api.getPluginByID("com.lumen.vship", core) != NULL;
        case VSZIP:
        return api.getPluginByID("com.julek.vszip", core) != NULL;
        case JXL:
        return api.getPluginByID("com.julek.plugin", core) != NULL;
    }
    return false;
}

void printInstalled(Api& api, VSCore* core){
    VSPlugin* plugin;
    std::cout << "VSSCRIPT enabled: " << api.supportVSScript() << std::endl;
    plugin = api.getPluginByID("com.lumen.vship", core);
    std::cout << "Vship : ";
    if (plugin == NULL){
        std::cout << "Not installed";
    } else {
        std::cout << "Installed";
    }
    std::cout << std::endl;
    plugin = api.getPluginByID("com.julek.vszip", core);
    std::cout << "vszip : ";
    if (plugin == NULL){
        std::cout << "Not installed";
    } else {
        std::cout << "Installed";
    }
    std::cout << std::endl;
    plugin = api.getPluginByID("com.julek.plugin", core);
    std::cout << "julek-plugin : ";
    if (plugin == NULL){
        std::cout << "Not installed";
    } else {
        std::cout << "Installed";
    }
    std::cout << std::endl;
}

bool isScript(std::string filename){
    return !(filename.substr(filename.size()-3, 3) != ".py" && filename.substr(filename.size()-4, 4) != ".vpy");
}

int main(int argc, char** argv){
    std::vector<std::string> args(argc-1);
    for (int i = 1; i < argc; i++){
        args[i-1] = argv[i];
    } 

    int gputhreads = 8;
    int start = 0;
    int end = -1;
    int skip = 1;
    int threads = -1;
    bool importer_set1 = false;
    IMPORTER_TYPE importer_type1 = FFMS2;
    bool importer_set2 = false;
    IMPORTER_TYPE importer_type2 = FFMS2;
    bool metric_set = false;
    METRICS metric = SSIMULACRA2;
    bool plugin_set = false;
    METRICSPLUGINS plugin = VSHIP;
    std::string file1;
    std::string file2;
    int outputnum = -1;
    int outputnum2 = -1;

    std::string JSONFILE = "";

    Api api;

    for (unsigned int i = 0; i < args.size(); i++){
        if (args[i] == "-h" || args[i] == "--help"){
            printUsage();
            return 0;
        } else if (args[i] == "--installed"){
            VSCore* coreInstalled = api.createCore(0);
            printInstalled(api, coreInstalled);
            return 0;
        } else if (args[i] == "-e" || args[i] == "--every"){
            if (i == args.size()-1){
                std::cout << "-e needs an argument" << std::endl;
                return 0;
            }
            try {
                skip = stoi(args[i+1]);
            } catch (std::invalid_argument& e){
                std::cout << "invalid value for -e" << std::endl;
                return 0;
            }
            i++;
        } else if (args[i] == "--source") {
            if (i == args.size()-1){
                std::cout << "--source needs an argument" << std::endl;
                return 0;
            }
            file1 = args[i+1];
            if (isScript(file1)){
                std::cout << "a script was specified as --source, use --sourceSCRIPT instead" << std::endl;
                return 0;
            }
            i++;
        } else if (args[i] == "--json") {
            if (i == args.size()-1){
                std::cout << "--json needs an argument" << std::endl;
                return 0;
            }
            JSONFILE = args[i+1];
            i++;
        } else if (args[i] == "--encoded") {
            if (i == args.size()-1){
                std::cout << "--encoded needs an argument" << std::endl;
                return 0;
            }
            file2 = args[i+1];
            if (isScript(file2)){
                std::cout << "a script was specified as --encoded, use --encodedSCRIPT instead" << std::endl;
                return 0;
            }
            i++;
        } else if (args[i] == "--sourceSCRIPT") {
            if (i >= args.size()-2 || args[i+2][0] == '-'){
                std::cout << "--sourceSCRIPT needs 2 arguments" << std::endl;
                return 0;
            }
            file1 = args[i+1];
            if (!isScript(file1)){
                std::cout << "a video was specified as --sourceSCRIPT, use --source instead" << std::endl;
                return 0;
            }
            try {
                outputnum = stoi(args[i+2]);
            } catch (std::invalid_argument& e){
                std::cout << "invalid output number for --sourceSCRIPT" << std::endl;
                return 0;
            }
            i += 2;
        } else 
#if Use_Vsscript==1
        if (args[i] == "--encodedSCRIPT") {
            if (i >= args.size()-2 || args[i+2][0] == '-'){
                std::cout << "--encodedSCRIPT needs 2 arguments" << std::endl;
                return 0;
            }
            file2 = args[i+1];
            if (!isScript(file2)){
                std::cout << "a video was specified as --encodedSCRIPT, use --encoded instead" << std::endl;
                return 0;
            }
            try {
                outputnum2 = stoi(args[i+2]);
            } catch (std::invalid_argument& e){
                std::cout << "invalid output number for --encodedSCRIPT" << std::endl;
                return 0;
            }
            i += 2;
        } else if (args[i] == "-i" || args[i] == "--importer"){
            if (i == args.size()-1){
                std::cout << "-i needs an argument" << std::endl;
                return 0;
            }
            if (args[i+1] == "ffms2"){
                importer_type1 = FFMS2;
                if (!importer_set2) importer_type2 = FFMS2;
                importer_set1 = true;
                importer_set2 = true;
            } else if (args[i+1] == "bestsource"){
                importer_type1 = BESTSOURCE;
                if (!importer_set2) importer_type2 = BESTSOURCE;
                importer_set1 = true;
                importer_set2 = true;
            } else if (args[i+1] == "lsmash"){
                importer_type1 = LSMAS;
                if (!importer_set2) importer_type2 = LSMAS;
                importer_set1 = true;
                importer_set2 = true;
            } else {
                std::cout << "unrecognized importer : " << args[i+1] << std::endl;
                return 0;
            }
            i++;
        } else 
#endif
        if (args[i] == "-si" || args[i] == "--source-importer"){
            if (i == args.size()-1){
                std::cout << "-si needs an argument" << std::endl;
                return 0;
            }
            if (args[i+1] == "ffms2"){
                importer_type1 = FFMS2;
                importer_set1 = true;
            } else if (args[i+1] == "bestsource"){
                importer_type1 = BESTSOURCE;
                importer_set1 = true;
            } else if (args[i+1] == "lsmash"){
                importer_type1 = LSMAS;
                importer_set1 = true;
            } else {
                std::cout << "unrecognized importer : " << args[i+1] << std::endl;
                return 0;
            }
            i++;
        } else if (args[i] == "-ei" || args[i] == "--encoded-importer"){
            if (i == args.size()-1){
                std::cout << "-ei needs an argument" << std::endl;
                return 0;
            }
            if (args[i+1] == "ffms2"){
                importer_type2 = FFMS2;
                importer_set2 = true;
            } else if (args[i+1] == "bestsource"){
                importer_type2 = BESTSOURCE;
                importer_set2 = true;
            } else if (args[i+1] == "lsmash"){
                importer_type2 = LSMAS;
                importer_set2 = true;
            } else {
                std::cout << "unrecognized importer : " << args[i+1] << std::endl;
                return 0;
            }
            i++;
        } else if (args[i] == "-t" || args[i] == "--threads"){
            if (i == args.size()-1){
                std::cout << "-t needs an argument" << std::endl;
                return 0;
            }
            try {
                threads = stoi(args[i+1]);
            } catch (std::invalid_argument& e){
                std::cout << "invalid value for -t" << std::endl;
                return 0;
            }
            i++;
        } else if (args[i] == "-g" || args[i] == "--gputhreads"){
            if (i == args.size()-1){
                std::cout << "-g needs an argument" << std::endl;
                return 0;
            }
            try {
                gputhreads = stoi(args[i+1]);
            } catch (std::invalid_argument& e){
                std::cout << "invalid value for -g" << std::endl;
                return 0;
            }
            i++;
        } else if (args[i] == "-m" || args[i] == "--metric"){
            if (i == args.size()-1){
                std::cout << "-m needs an argument" << std::endl;
                return 0;
            }
            if (args[i+1] == "ssimu2_vszip"){
                plugin_set = true;
                plugin = VSZIP;
                metric_set = true;
                metric = SSIMULACRA2;
            } else if (args[i+1] == "ssimu2_vship"){
                plugin_set = true;
                plugin = VSHIP;
                metric_set = true;
                metric = SSIMULACRA2;
            } else if (args[i+1] == "butter_vship"){
                plugin_set = true;
                plugin = VSHIP;
                metric_set = true;
                metric = BUTTERAUGLI;
            } else if (args[i+1] == "ssimu2_jxl"){
                plugin_set = true;
                plugin = JXL;
                metric_set = true;
                metric = SSIMULACRA2;
            } else if (args[i+1] == "butter_jxl"){
                plugin_set = true;
                plugin = JXL;
                metric_set = true;
                metric = BUTTERAUGLI;
            } else {
                std::cout << "unrecognized metric : " << args[i+1] << std::endl;
                return 0;
            }
            i++;
        } else if (args[i] == "--start"){
            if (i == args.size()-1){
                std::cout << "--start needs an argument" << std::endl;
                return 0;
            }
            try {
                start = stoi(args[i+1]);
            } catch (std::invalid_argument& e){
                std::cout << "invalid value for --start" << std::endl;
                return 0;
            }
            i++;
        } else if (args[i] == "--end"){
            if (i == args.size()-1){
                std::cout << "--end needs an argument" << std::endl;
                return 0;
            }
            try {
                end = stoi(args[i+1]);
            } catch (std::invalid_argument& e){
                std::cout << "invalid value for --end" << std::endl;
                return 0;
            }
            i++;
        } else {
            std::cout << "Unrecognized option: " << args[i] << std::endl;
            return 0;
        }
    }

    if (file1 == "" || file2 == "") {
        std::cout << "missing file input" << std::endl;
        return 0;
    }
    if (!metric_set){
        std::cout << "Metric is not set" << std::endl;
        return 0;
    }

    VSCore* coremetric = api.createCore(0);
    VSCore* core1 = NULL; //to use with scripts
    VSCore* core2 = NULL;
    api.addLogHandler(logMessageHandler<0>, NULL, NULL, coremetric);

    if (!plugingIsInstalled(api, coremetric, plugin)){
        std::cout << "The specified plugin is not installed" << std::endl;
        return 0;
    }

    std::shared_ptr<VSNodeWrapper> node1;
    std::shared_ptr<VSNodeWrapper> node2;

    if (isScript(file1) && file1 == file2){
        core1 = api.createCore(0);
        api.addLogHandler(logMessageHandler<1>, NULL, NULL, core1);
        #if Use_Vsscript==1
        auto [node1, node2] = clipFromScript(file1, api, core1, outputnum, outputnum2);
        #else
        std::cout << "A script was used while ssimulaCraPP was compiled without support for it" << std::endl;
        #endif
    } else {
        if (isScript(file1)){
            core1 = api.createCore(0);
            api.addLogHandler(logMessageHandler<1>, NULL, NULL, core1);
            #if Use_Vsscript==1
            node1 = clipFromScript(file1, api, core1, outputnum);
            #else
            std::cout << "A script was used while ssimulaCraPP was compiled without support for it" << std::endl;
            #endif
        } else {
            Importer* imp = NULL;

            if (importer_set1){
                imp = new Importer(api, coremetric, importer_type1);
            } else {
                imp = new Importer(api, coremetric);
            }

            node1 = clipFromFile(file1, *imp);

            delete imp;
        }
        if (isScript(file2)){
            core2 = api.createCore(0);
            api.addLogHandler(logMessageHandler<1>, NULL, NULL, core2);
            #if Use_Vsscript==1
            node2 = clipFromScript(file2, api, core2, outputnum2);
            #else
            std::cout << "A script was used while ssimulaCraPP was compiled without support for it" << std::endl;
            #endif  
        } else {
            Importer* imp = NULL;

            if (importer_set2){
                imp = new Importer(api, coremetric, importer_type2);
            } else {
                imp = new Importer(api, coremetric);
            }

            node2 = clipFromFile(file2, *imp);

            delete imp;
        }
    }

    VSCoreInfo infos;
    api.getCoreInfo(coremetric, &infos);

    MetricComputer met(api, coremetric, plugin, metric);
    if (threads == -1) threads = infos.numThreads;
    std::vector<float> result = met.compute(node1, node2, start, end, skip, threads, gputhreads=gputhreads);
    std::cout << "Computed metrics on " << result.size() << " frames: " << std::endl;

    if (JSONFILE != ""){
        std::ofstream f(JSONFILE);

        if (!f){
            std::cout << "failed to open output json file : " << JSONFILE << std::endl;
        } else {
            f << "[";
            for (int i = 0; i < result.size(); i++){
                f << result[i];
                if (i != result.size()-1) f << ", ";
            }
            f << "]";
        }
    }

    std::sort(result.begin(), result.end());

    float avg = 0;
    float stddev = 0;
    float percentile5 = result[result.size()/20];
    float percentile95 = result[result.size()*19/20];
    float median = result[result.size()/2];

    for (const auto& el: result){
        avg += el;
        stddev += el*el;
    }
    avg /= result.size();
    stddev /= result.size();
    stddev -= avg*avg;
    stddev = std::sqrt(stddev);

    switch (metric){
        case (BUTTERAUGLI):
        switch (plugin){
            case JXL:
            std::cout << "Butteraugli INF-Norm Score : " << std::endl;
            break;
            case VSHIP:
            std::cout << "Butteraugli 3-Norm Score : " << std::endl;
            break;
        }
        break;
        case (SSIMULACRA2):
        std::cout << "SSIMULACRA2 : " << std::endl;
        break;
    }

    std::cout << "Average : " << avg << std::endl;
    std::cout << "Standard Deviation : " << stddev << std::endl;
    std::cout << "Median : " << median << std::endl;
    std::cout << "5th percentile : " << percentile5 << std::endl;
    std::cout << "95th percentile : " << percentile95 << std::endl;

    node1->destroy();
    node2->destroy();
    api.freeCore(coremetric);

    return 0;
}
