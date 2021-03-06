#ifndef MRINTER_RESULT_DISPATCHER
#define MRINTER_RESULT_DISPATCHER

#include "hiconfig.h"

#include "mapreduce.h"
#include "hiaux/threads/threadpool.h"
#include "hiaux/threads/tasklauncher.h"
#include "InterResultReader.h"

typedef std::vector<uint64_t> Int64Vec;
typedef boost::shared_ptr< std::vector<uint64_t> > Int64VecPtr;

class WriteQueue: public std::queue< std::pair<uint64_t, std::string> >,
		public hLock
{
};

#define IR_WRITING 0
#define IR_READING 1

#define KeyType uint64_t
//template <class KeyType>
class MRInterResult
{
	bool mode;
	
	std::string m_filename;
	int m_fd;

	MapReduce *m_MR;
	
	InterResultLoaderPtr m_reader;
	
	// key, offset
	FileMapPtr m_file_map;
	
	hiaux::hashtable<KeyType, EmitType*> m_emit_cache0;
	hiaux::hashtable<KeyType, EmitType*> m_emit_cache1;
	
	boost::atomic<bool> m_cache0_ready;
	boost::atomic<bool> m_cache1_ready;
	hCondWaiter m_cache0_ready_lock;
	hCondWaiter m_cache1_ready_lock;
	
	// key / dump
	//std::queue< std::pair<int64_t, std::string> > write_queue;
	//boost::lockfree::queue< std::pair<KeyType, std::string>* > write_queue;
	WriteQueue write_queue;
	uint8_t *wbuffer;
	size_t m_wbuffer_size; // offset in wbuffer
	size_t m_wbuffer_cap;
	size_t w_offset; // offset in file
	
	size_t m_size;
	
	//hLock wbuffer_lock;
	boost::atomic<bool> no_more_writes;
	
	bool flush_finished; // buffer empty && nomore
	hCondWaiter flush_finish_lock;
	
	// not thread safe
	EmitType *restore(off_t offset);
	
	//void flush(std::pair<uint64_t, std::string> dump);
	void flush_wbuffer();
public:
	
	MRInterResult(std::string filename,
				MapReduce *MR,
				TaskLauncher &flush_launcher,
				const size_t wbuffer_cap = 5000000);
	
	~MRInterResult();
	
	//bool checkWriteBufferNotEmpty();
	TaskLauncher::TaskRet flushBuffer();
	
	bool checkCacheReady(bool cid);
	bool FlushFinished();
	
	// add to m_file_map and dump. not thread safe with itself and other methods
	void addEmit(uint64_t key, EmitType *emitvec);
	void waitFlushFinished();
	void waitInitReading();
	Int64VecPtr getKeys();
	
	// preload & getEmit thread safe when cid's are different
	void preload(uint64_t key, bool cid); 
	void condWaitCache(bool cid);
	// get emit from cache
	EmitType* getEmit(uint64_t key, bool cid);
	
	void deleteFile();
	void clearCache(bool cid);
	void setCacheReady(bool cid);
	
	std::string getFileName();
	size_t size();
};

typedef boost::shared_ptr<MRInterResult> MRInterResultPtr;

#endif

/*
 *  f (keys, nkeys, i1, i2, cid)				|		preload(keys, i1, i2, cid)
 *		addTask preload(keys[nkeys..2*nkeys], i1, i2, !cid)
 *		i1.condWaitCache(cid)
 *		i2.condWaitCache(cid)
 *		merge (keys[0..nkeys], i1, i2)
 *		i1.clearCache(cid)
 *		i2.clearCache(cid)
 *		f (keys[nkeys..], nkeys, i1, i2, !cid)
 * 
 * 
 * 
 */
