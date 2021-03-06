/*
 * Copyright (c) 2010-2013  Artur Gilmutdinov

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the BSD 2-Clause License

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * BSD 2-Clause License for more details.

 * You should have received a copy of the BSD 2-Clause License
 * along with this program; if not, see <http://opensource.org/licenses>.

 * See also <http://highinit.com>
*/

#ifndef MAPREDUCE_H
#define  MAPREDUCE_H

#include "hiconfig.h"

#include <vector>
#include "hiaux/structs/hashtable.h"
#include <queue>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/atomic.hpp>
#include <boost/noncopyable.hpp>

class MRStats
{
public:
	boost::atomic<size_t> nmaps;
	boost::atomic<size_t> nemits;
	boost::atomic<size_t> nreduces;
	MRStats();
	MRStats(const MRStats &a);
	MRStats& operator+=(const MRStats &a);
	MRStats& operator=(const MRStats &a);
};

class InputType
{
public:
	virtual ~InputType() { }
};

class EmitType
{
public:
	virtual void restore(const std::string &_dump) = 0;
	virtual void dump(std::string &_dump) const = 0;
	virtual ~EmitType() { } 
};

typedef hiaux::hashtable<uint64_t, EmitType*> EmitHash;

class BatchAccessor
{
public:
	BatchAccessor() { }
	virtual bool end() = 0;
	virtual InputType *getNextInput() = 0;
	virtual ~BatchAccessor() { }
};

class MapReduce : public boost::noncopyable
{
protected:
	boost::function<void(uint64_t, EmitType*)> emit; 

public:
	MapReduce ();
	void setEmitF(boost::function<void(uint64_t, EmitType*)> emitf);

	virtual void map(const InputType* object) const = 0;
	virtual EmitType* reduce(uint64_t key, EmitType* a, EmitType* b) const = 0;
	virtual void finilize(EmitType*) = 0;

	//virtual void dumpEmit(const EmitType *_emit, std::string &_dump) const = 0;
	virtual EmitType* restoreEmit(const std::string &_dump) const = 0;

	virtual MapReduce *create() const = 0;
	virtual ~MapReduce() { }
};


#endif
