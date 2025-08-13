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
int testIoMapSummary();
int testIoMapIds();


int testIoAggregateMap();
int testIoAggregateMapWindow();


void fillData(IoAggregateMap &map);
void fillData(IoMap *map);

std::ostream& operator<<(std::ostream &os, const std::pair<double, double> &other);
std::ostream& operator<<(std::ostream &os, const std::optional<IoStatSummary> &opt);
