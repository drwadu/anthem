#ifndef __ANTHEM__VERIFY_PROPERTIES__TRANSLATION_H
#define __ANTHEM__VERIFY_PROPERTIES__TRANSLATION_H

#include <string>
#include <vector>

#include <anthem/Context.h>

namespace anthem
{
namespace verifyProperties
{

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Translation
//
////////////////////////////////////////////////////////////////////////////////////////////////////

void translate(const std::vector<std::string> &fileNames, Context &context);
void translate(const char *fileName, std::istream &stream, Context &context);

////////////////////////////////////////////////////////////////////////////////////////////////////

}
}

#endif