#ifndef MRINTER_MERGER_H
#define MRINTER_MERGER_H

#include <cmath>
#include "MRInterResult.h"
#include "hiaux/threads/tasklauncher.h"

class MRInterMerger
{
public:

	static MRStats merge(TaskLauncher &preload_tasks_launcher,
				MRInterResultPtr inter1,
				MRInterResultPtr inter2,
				MRInterResultPtr result,
				MapReduce *MR,
				int emits_in_cache);

	static TaskLauncher::TaskRet loadCache(MRInterResultPtr inter,
				bool cid,
				Int64VecPtr keys, int b, int e, hLock &lock);

	static Int64VecPtr mergeKeys(Int64VecPtr a, Int64VecPtr b);
};

#endif
