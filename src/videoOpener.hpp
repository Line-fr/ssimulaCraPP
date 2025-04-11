#pragma once

#include "preprocessor.hpp"
#include "apiWrapper.hpp"
#include "importer.hpp"

#if Use_Vsscript == 1
std::shared_ptr<VSNodeWrapper> clipFromScript(std::string scriptName, Api& api, VSCore* core, int outputnumber = 0){
    VSScript* se = api.createScript(core);

    //evaluate the script
    if (api.evaluateFile(se, scriptName.data())){
        std::cerr << "Script Evaluation Failed : " << std::endl;
        std::cerr << api.getError(se) << std::endl;
        api.freeScript(se);
        return NULL;
    }
    VSNode* node = api.getOutputNode(se, outputnumber);
    if (!node){
        std::cerr << "Failed to retrieve the output node for vs script " << scriptName << " with outputnumber : " << outputnumber << std::endl;
        api.freeScript(se);
        return NULL;
    }

    return std::shared_ptr<VSNodeWrapper>(new VSNodeWrapper(api, node, se));
}

std::pair<std::shared_ptr<VSNodeWrapper>, std::shared_ptr<VSNodeWrapper>> clipFromScript(std::string scriptName, Api& api, VSCore* core, int outputnumber, int outputnumber2){
    VSScript* se = api.createScript(core);

    //evaluate the script
    if (api.evaluateFile(se, scriptName.data())){
        std::cerr << "Script Evaluation Failed : " << std::endl;
        std::cerr << api.getError(se) << std::endl;
        api.freeScript(se);
        return std::make_pair<std::shared_ptr<VSNodeWrapper>, std::shared_ptr<VSNodeWrapper>>(NULL, NULL);
    }
    VSNode* node = api.getOutputNode(se, outputnumber);
    if (!node){
        std::cerr << "Failed to retrieve the output node for vs script " << scriptName << " with outputnumber : " << outputnumber << std::endl;
        api.freeScript(se);
        return std::make_pair<std::shared_ptr<VSNodeWrapper>, std::shared_ptr<VSNodeWrapper>>(NULL, NULL);
    }
    VSNode* node2 = api.getOutputNode(se, outputnumber2);
    if (!node2){
        std::cerr << "Failed to retrieve the output node for vs script " << scriptName << " with outputnumber : " << outputnumber2 << std::endl;
        api.freeScript(se);
        return std::make_pair<std::shared_ptr<VSNodeWrapper>, std::shared_ptr<VSNodeWrapper>>(NULL, NULL);
    }

    return std::make_pair(std::shared_ptr<VSNodeWrapper>(new VSNodeWrapper(api, node, se)), std::shared_ptr<VSNodeWrapper>(new VSNodeWrapper(api, node2, se)));
}
#endif

std::shared_ptr<VSNodeWrapper> clipFromFile(std::string filename, Importer& importer){
    return importer.openVideo(filename);
}