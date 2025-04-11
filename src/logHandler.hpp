#pragma once

#include "preprocessor.hpp"
#include "apiWrapper.hpp"

static const char *messageTypeToString(int msgType) {
    switch (msgType) {
    case mtDebug: return "Debug";
    case mtInformation: return "Information";
    case mtWarning: return "Warning";
    case mtCritical: return "Critical";
    case mtFatal: return "Fatal";
    default: return "";
    }
}

template<int prefix>
static void VS_CC logMessageHandler(int msgType, const char *msg, void *userData) {
    if (msgType >= mtCritical)
        std::cerr << prefix << ": " << messageTypeToString(msgType) << " : " << msg << std::endl;
}