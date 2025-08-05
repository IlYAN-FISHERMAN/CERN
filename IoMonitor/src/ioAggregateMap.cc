//  File: ioAggregateMap.cc
//  Author: Ilkay Yanar - 42Lausanne / CERN
//  ----------------------------------------------------------------------

/*************************************************************************
 *  EOS - the CERN Disk Storage System                                   *
 *  Copyright (C) 2025 CERN/Switzerland                                  *
 *                                                                       *
 *  This program is free software: you can redistribute it and/or modify *
 *  it under the terms of the GNU General Public License as published by *
 *  the Free Software Foundation, either version 3 of the License, or    *
 *  (at your option) any later version.                                  *
 *                                                                       *
 *  This program is distributed in the hope that it will be useful,      *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 *  GNU General Public License for more details.                         *
 *                                                                       *
 *  You should have received a copy of the GNU General Public License    *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.*
 *************************************************************************/

#include "../include/ioAggregateMap.hh"

AggregatedIoMap::AggregatedIoMap(const std::vector<size_t> &aggregationWindows) : _running(true){
	(void)aggregationWindows;
	_thread = std::thread(&AggregatedIoMap::aggregationLoop, this);
}

AggregatedIoMap::~AggregatedIoMap(){}

void addRead(uint64_t inode, const std::string &app, uid_t uid, gid_t gid, size_t rbytes){
	(void)inode;
	(void)app;
	(void)uid;
	(void)gid;
	(void)rbytes;
}

void addWrite(uint64_t inode, const std::string &app, uid_t uid, gid_t gid, size_t wbytes){
	(void)inode;
	(void)app;
	(void)uid;
	(void)gid;
	(void)wbytes;
}

std::vector<size_t> AggregatedIoMap::getAvailableWindows() const{
	return (std::vector<size_t>(_aggregates.begin(), _aggregates.end()));
}
