#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include "../include/IoAggregateMap.hh"

int testIoStatFillData();
int testIoStatCleaning();
int testIoStatExactValue();

int testInteractiveIoMap();
int testIoMapBigVolume();
int testIoMapSpecificCase();
int testIoMapExactValue();
int testIoMapData();

int testIoAggregateMap();


void fillData(IoAggregateMap &map);
void fillData(IoMap *map);
