//  File: IoStat.hh
//  Author: Ilkay Yanar - 42Lausanne /CERN
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

//--------------------------------------------
/// Each class has a variable define DEBUG which
/// can be redefined at compilation to print the
/// debug of each class
///
/// Number required to see debug mode: 1
//--------------------------------------------

#include "ioMonitor.hh"

//--------------------------------------------
/// The current name of the class when us
/// printInfo function
//--------------------------------------------
#define IOSTAT_NAME "IoStat"

class IoStat {
	//--------------------------------------------
	/// Enumerator that allows to keep the context
	/// of READ or WRITE to avoid duplicate functions
	//--------------------------------------------
	public: enum class Marks{
		READ,
		WRITE
	};

	private:
		//--------------------------------------------
		/// Each IoStat class keeps track of exactly
		/// where it came from, the file ID, the name
		/// of the app it came from, the user ID and
		/// the group ID
		//--------------------------------------------
		uint64_t _fileId;
		std::string _app;
		uid_t _uid;
		gid_t _gid;

		//--------------------------------------------
		/// Read deque that keeps track all of read
		/// operations on the _fileId
		//--------------------------------------------
		std::deque<IoMark> _readMarks;

		//--------------------------------------------
		/// Write deque that keeps track all of written
		/// operations on the _fileId
		//--------------------------------------------
		std::deque<IoMark> _writeMarks;


		//WARNING: --------------------------------------------
		/// The default constructor is private because
		/// the instant is required to have tracking information
		//--------------------------------------------
		IoStat();

	public:
		//--------------------------------------------
		/// Orthodoxe canonical form
		//--------------------------------------------
	
		//--------------------------------------------
		/// Destructor
		//--------------------------------------------
		~IoStat();

		//--------------------------------------------
		/// Constructor by copy constructor
		//--------------------------------------------
		IoStat(const IoStat &other);

		//--------------------------------------------
		/// Overloading copy "=" operator
		//--------------------------------------------
		IoStat& operator=(const IoStat &other);


		//--------------------------------------------
		/// @brief Main constructor to initialize the class
		///
		/// @param	fileId	Tracked file id
		/// @param	app		Name of the application where the file is linked
		/// @param	uid		Id of the user
		/// @param	gid		Id of the group
		//--------------------------------------------
		IoStat(uint64_t fileId, const std::string& app, uid_t uid, gid_t gid);

		//--------------------------------------------
		/// @brief Add read bytes to the read deque
		/// and records its execution timestamp
		///
		/// @param	rBytes Number of bytes read
		//--------------------------------------------
		void addRead(size_t rBytes);

		//--------------------------------------------
		/// @brief Add read/written bytes to the corresponding deque
		/// {read/write} and records its execution timestamp
		///
		/// @param	rBytes/wBytes Number of bytes written
		//--------------------------------------------
		void addWrite(size_t wBytes);

		//--------------------------------------------
		/// @brief Keep all I/O from the last N seconds.
		///
		/// @details
		/// 
		/// The function takes as parameters the number of
		/// seconds corresponding to the number of seconds
		/// of I/O which will be kept, the rest will be erased.
		/// 
		/// @param	enumMark READ or WRITE variable comes
		/// from the IoStat::Marks enumerator
		/// - Exemple: IoStat::Marks::READ
		/// @param seconds(optional) The number of last seconds of
		/// I/O that will be kept (by default - 10s)
		///
		/// @return -1 If the enumerator is incorrect
		/// @return 1 If there is nothing to delete
		/// @return size Else returns the size of what was erased
		//--------------------------------------------
		uint64_t cleanOldsMarks(Marks enumMark, size_t seconds = 10);


		//--------------------------------------------
		/// @brief Calculate the write or read bandwidth
		///
		/// @details The function calculates the write or
		/// read bandwidth (depending on the "enumMark" parameter)
		/// of the last N seconds (by default - 10s)
		///
		/// The function thus calculates the average and
		/// the standard deviation of the range of data found
		///
		/// Sets the "range" variable to the
		/// number of elements that were found
		/// (can be set to NULL in which case the parameter
		/// is ignored)
		/// 
		/// @param	enumMark READ or WRITE variable comes
		/// from the IoStat::Marks enumerator
		/// - Exemple: IoStat::Marks::WRITE
		/// @param seconds(optional) over how many seconds
		/// from now the function should calculate
		///
		/// @return size Returns the size of what was erased
		//--------------------------------------------
		std::pair<double, double> bandWidth(Marks EnumMark, size_t *range, size_t seconds = 10) const;

		//--------------------------------------------
		/// Static function
		/// @brief Displays the string given as a parameter
		/// in a format corresponding to the class with the
		/// current timestamp
		///
		/// @param	os The output steam
		/// @param	msg The message to display
		/// -Exemple: std::cout/std::cerr
		//--------------------------------------------
		static void	printInfo(std::ostream &os, const char *msg);

		//--------------------------------------------
		/// Static function
		/// @brief Displays the string given as a parameter
		/// in a format corresponding to the class with the
		/// current timestamp
		///
		/// @param	os The output stream
		/// @param	msg The message to display
		/// -Exemple: std::cout/std::cerr
		//--------------------------------------------
		static void	printInfo(std::ostream &os, const std::string &msg);

		//--------------------------------------------
		/// Get current uid
		//--------------------------------------------
		uid_t getUid() const;

		//--------------------------------------------
		/// Get current gid
		//--------------------------------------------
		gid_t getGid() const;

		//--------------------------------------------
		/// Get current app name
		//--------------------------------------------
		const std::string& getApp() const;

		//--------------------------------------------
		/// @brief Get the size of corresponding
		/// READ or WRITE deck
		///
		/// @param	enumMark READ or WRITE variable comes
		/// from the IoStat::Marks enumerator
		/// - Exemple: IoStat::Marks::WRITE
		//--------------------------------------------
		ssize_t getSize(Marks enumMark) const;
};


//--------------------------------------------
/// @brief Overload operator << to print the average
/// and standard deviation of the last 10 seconds
//--------------------------------------------
std::ostream& operator<<(std::ostream &os, const IoStat *other);

struct IoStatSummary {
    double avg_read = 0.0;
    double sigma_read = 0.0;
    double avg_write = 0.0;
    double sigma_write = 0.0;
};
