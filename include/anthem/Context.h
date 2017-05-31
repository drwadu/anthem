#ifndef __ANTHEM__CONTEXT_H
#define __ANTHEM__CONTEXT_H

#include <anthem/output/Logger.h>

namespace anthem
{

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Context
//
////////////////////////////////////////////////////////////////////////////////////////////////////

struct Context
{
	Context() = default;

	explicit Context(output::Logger &&logger)
	:	logger{std::move(logger)}
	{
	}

	output::Logger logger;

	bool simplify = false;
	bool complete = false;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

}

#endif
