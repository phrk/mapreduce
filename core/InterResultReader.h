#ifndef INTER_RESULT_LOADER_H
#define INTER_RESULT_LOADER_H

#include "hiconfig.h"

#include "mapreduce.h"

typedef hiaux::hashtable<uint64_t, uint64_t> FileMap;
typedef boost::shared_ptr<FileMap> FileMapPtr;

class InterResultLoader
{
	int m_fd; 
	size_t m_len;
	uint8_t *p;
	MapReduce *m_MR;
	bool closed;
	std::string m_filename;
	
	InterResultLoader();
	InterResultLoader(InterResultLoader&);
	InterResultLoader& operator=(InterResultLoader&);
public:
	InterResultLoader(std::string filename, MapReduce *MR);
	~InterResultLoader();
	
	FileMapPtr getFileMap();
	EmitType *readEmit(off_t offset);
	void deleteFile();
};

typedef boost::shared_ptr<InterResultLoader> InterResultLoaderPtr; 

#endif
