#ifndef SERVERCONFIGSTRUCT_H_
#define SERVERCONFIGSTRUCT_H_

#include <Arduino.h>

struct HubConfig 
{
    String Host;
    bool UseSsl;
    int Port;
    String Hub;
};

#endif
