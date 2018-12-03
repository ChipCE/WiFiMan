#ifndef __CUSTOM_CONFIG_H
#define __CUSTOM_CONFIG_H

typedef struct CustomArg
{
    String key;
    String value;
};

typedef struct CustomConfig
{
    int count = 0;
    CustomArg args[10];
};

#endif