#ifndef __MOVIE_H_
#define __MOVIE_H_

#ifdef __cplusplus

#include <vector>
#include <map>
#include <string>
#include <ostream>
#include <istream>
#include "../../core.h"
//#include "utils/guid.h"
//#include "utils/md5.h"

typedef struct
{
	int movie_version;					// version of the movie format in the file
	u32 num_frames;
	u32 rerecord_count;
	bool poweron;
	u32 emu_version_used;
//	MD5DATA md5_of_rom_used;
	std::string name_of_rom_used;

	std::vector<std::wstring> comments;
	std::vector<std::string> subtitles;
} MOVIE_INFO;

enum EMOVIEMODE
{
	MOVIEMODE_INACTIVE = 1,
	MOVIEMODE_RECORD = 2,
	MOVIEMODE_PLAY = 4,
};

enum EMOVIECMD
{
	MOVIECMD_RESET = 1,
};

//RLDUTSBAYXWEG

class MovieData;
class MovieRecord
{

public:
	u16 pad;
	
	union {
		struct {
			u8 x, y;
			u8 touch;
		};

		u32 padding;
	} touch;
	
	//misc commands like reset, etc.
	//small now to save space; we might need to support more commands later.
	//the disk format will support up to 64bit if necessary
	u8 commands;
	bool command_reset() { return (commands&MOVIECMD_RESET)!=0; }

	void toggleBit(int bit)
	{
		pad ^= mask(bit);
	}

	void setBit(int bit)
	{
		pad |= mask(bit);
	}

	void clearBit(int bit)
	{
		pad &= ~mask(bit);
	}

	void setBitValue(int bit, bool val)
	{
		if(val) setBit(bit);
		else clearBit(bit);
	}

	bool checkBit(int bit)
	{
		return (pad & mask(bit))!=0;
	}

	void clear();
	
	//a waste of memory in lots of cases..  maybe make it a pointer later?
	//std::vector<char> savestate;

	void parse(MovieData* md, std::istream* is);
	bool parseBinary(MovieData* md, std::istream* is);
	void dump(MovieData* md, std::ostream* os, int index);
	void dumpBinary(MovieData* md, std::ostream *os, int index);
	void parsePad(std::istream* is, u16& pad);
	void dumpPad(std::ostream* os, u16 pad);
	
	static const char mnemonics[13];

private:
	int mask(int bit) { return 1<<bit; }
};


class MovieData
{
public:
	MovieData();
	

	int version;
	int emuVersion;
	//todo - somehow force mutual exclusion for poweron and reset (with an error in the parser)
	
//	MD5DATA romChecksum;
	std::string romFilename;
	std::vector<char> savestate;
	std::vector<MovieRecord> records;
	std::vector<std::wstring> comments;
	
	int rerecordCount;
//	Desmume_Guid guid;

	//was the frame data stored in binary?
	bool binaryFlag;

	int getNumRecords() { return records.size(); }

	class TDictionary : public std::map<std::string,std::string>
	{
	public:
		bool containsKey(std::string key)
		{
			return find(key) != end();
		}

		void tryInstallBool(std::string key, bool& val)
		{
			if(containsKey(key))
				val = atoi(operator [](key).c_str())!=0;
		}

		void tryInstallString(std::string key, std::string& val)
		{
			if(containsKey(key))
				val = operator [](key);
		}

		void tryInstallInt(std::string key, int& val)
		{
			if(containsKey(key))
				val = atoi(operator [](key).c_str());
		}

	};

	void truncateAt(int frame);
	void installValue(std::string& key, std::string& val);
	int dump(std::ostream *os, bool binary);
	void clearRecordRange(int start, int len);
	void insertEmpty(int at, int frames);
	
	static bool loadSavestateFrom(std::vector<char>* buf);
	static void dumpSavestateTo(std::vector<char>* buf, int compressionLevel);
	//void TryDumpIncremental();

private:
	void installInt(std::string& val, int& var)
	{
		var = atoi(val.c_str());
	}

	void installBool(std::string& val, bool& var)
	{
		var = atoi(val.c_str())!=0;
	}
};

//extern int currFrameCounter;
extern EMOVIEMODE movieMode;		//adelikat: main needs this for frame counter display
extern MovieData currMovieData;		//adelikat: main needs this for frame counter display

bool FCEUI_MovieGetInfo(std::istream* fp, MOVIE_INFO& info, bool skipFrameCount);
void FCEUI_SaveMovie(const char *fname, std::wstring author);
void FCEUI_LoadMovie(const char *fname, bool _read_only, bool tasedit, int _pauseframe);
void FCEUI_StopMovie();
void NDS_setTouchFromMovie(void);
//void mov_savestate(std::ostream* os);
//bool mov_loadstate(std::istream* is, int size);
void LoadFM2_binarychunk(MovieData& movieData, std::istream* fp, int size);
extern bool movie_readonly;
void FCEUI_MakeBackupMovie(bool dispMessage);
void ToggleReadOnly();
#endif

//C stuff

#ifdef __cplusplus
extern "C" {
#endif

int MovieIsActive();
void HardResetGame();
void MakeMovieStateName(const char *filename);
void FCEUMOV_AddInputState();

void PauseOrUnpause(void);
void MakeMovieStateName(const char *filename);
char* GetMovieLengthStr();

#ifdef __cplusplus
}
#endif
#endif

