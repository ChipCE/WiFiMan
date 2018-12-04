#ifndef __CUSTOM_CONFIG_H
#define __CUSTOM_CONFIG_H

#define MAX_CUSTOM_CONFIG 12

typedef struct CustomArg
{
    String key;
    String value;
};

typedef struct CustomConfig
{
    int count = 0;
    CustomArg args[MAX_CUSTOM_CONFIG];
};

#endif