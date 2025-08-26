#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include "../include/IoAggregateMap.hh"

int testIoStatFillData();
int testIoStatCleaning();
int testIoStatExactValue();
int testIoStatIOPS();
int testIoStatCopy();

int testInteractiveIoMap();
int testIoMapBigVolume();
int testIoMapSpecificCase();
int testIoMapExactValue();
int testIoMapData();
int testIoMapSummary();
int testIoMapIds();
int testIoMapCopy();


int testIoAggregateMap();
int testIoAggregateMapInteract();
int testIoAggregateMapWindow();
int testIoAggregateMapCopy();

int testIoAggregateCopy();


void fillData(IoAggregateMap &map);
void fillData(IoMap *map);

std::ostream& operator<<(std::ostream &os, const std::pair<double, double> &other);
std::ostream& operator<<(std::ostream &os, const std::optional<IoStatSummary> &opt);
