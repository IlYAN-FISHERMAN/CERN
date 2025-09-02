//  File: testIoBuffer.cc
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

#include "tester.hh"


int testIoBuffer(){
	IoAggregateMap map;
	IoBuffer::data proto;
	
	// Add window
	map.addWindow(3600);

	// set Tracks
	map.setTrack(3600, "eos");
	map.setTrack(3600, io::TYPE::UID, 12);
	map.setTrack(3600, io::TYPE::GID, 11);

	map.setTrack(3600, "eos");

	for (size_t i = 0; i < 11; i++){
		map.addWrite(1, "eos", 12, 11, std::abs(rand())%10000);
		map.addWrite(1, "eos", 1, 11, std::abs(rand())%10000);
		map.addWrite(1, "mgm", 1, 11, std::abs(rand())%10000);
		map.addWrite(1, "fdf", 12, 1, std::abs(rand())%10000);
		map.addRead(1, "eos", 12, 11, std::abs(rand())%10000);
		map.addRead(1, "eos", 1, 11, std::abs(rand())%10000);
		map.addRead(1, "mgm", 1, 11, std::abs(rand())%10000);
		map.addRead(1, "fdf", 12, 1, std::abs(rand())%10000);
		std::cout << "fill sleep" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}


	for (size_t i = 0; i < 2; i++){
		{
			IoBuffer::Summary *protoBuf = new IoBuffer::Summary();
			auto &&eos(map.getSummary(3600, "eos"));

			proto.add_apps();
			auto *index = proto.mutable_apps(i);
			index->set_name("eos");
			if (eos.has_value())
				index->set_allocated_summary(&eos->Serialize(*protoBuf));
			else
				return -1;
		}
		{
			IoBuffer::Summary *protoBuf = new IoBuffer::Summary();
			auto &&uid(map.getSummary(3600, io::TYPE::UID, 12));

			proto.add_uids();
			auto *index = proto.mutable_uids(i);
			index->set_uid(12);
			if (uid.has_value())
				index->set_allocated_summary(&uid->Serialize(*protoBuf));
			else
				return -1;
		}
		{
			IoBuffer::Summary *protoBuf = new IoBuffer::Summary();
			auto &&gid(map.getSummary(3600, io::TYPE::GID, 11));

			proto.add_gids();
			auto *index = proto.mutable_gids(i);
			index->set_gid(12);
			if (gid.has_value())
				index->set_allocated_summary(&gid->Serialize(*protoBuf));
			else
				return -1;
		}
		std::cout << "proto sleep" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	// std::cout << "DEBUG: "<< proto.DebugString() << std::endl;

	std::string output;
	{
		google::protobuf::util::JsonPrintOptions options;
		options.add_whitespace = true;
		options.always_print_primitive_fields = true;
		options.preserve_proto_field_names = true;
		auto it = google::protobuf::util::MessageToJsonString(proto, &output, options);
		if (!it.ok())
			return -1;
		std::cout << "JSON:" << std::endl << output << std::endl;
	}
	// {
	// 	IoBuffer::data back;
	// 	google::protobuf::util::JsonParseOptions options2;
	// 	auto ite = google::protobuf::util::JsonStringToMessage(output, &back, options2);
	// 	if (!ite.ok())
	// 		return -1;
	// 	std::cout << back.DebugString() << std::endl;
	//
	// }

	std::cout << "end" << std::endl;
	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
