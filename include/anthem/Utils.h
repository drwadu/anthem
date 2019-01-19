#ifndef __ANTHEM__UTILS_H
#define __ANTHEM__UTILS_H

namespace anthem
{

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Utils
//
////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr const auto HeadVariablePrefix = "V";
constexpr const auto BodyVariablePrefix = "X";
constexpr const auto UserVariablePrefix = "U";
constexpr const auto IntegerVariablePrefix = "N";

////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr const auto TPTPFunctionNamePrefix = "f__";
constexpr const auto TPTPFunctionNameSuffix = "__";
constexpr const auto AuxiliaryPredicateNameEven = "p__is_even__";
constexpr const auto AuxiliaryPredicateNameOdd = "p__is_odd__";

////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Tristate
{
	True,
	False,
	Unknown,
};

////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OperationResult
{
	Unchanged,
	Changed,
};

////////////////////////////////////////////////////////////////////////////////////////////////////

enum class EvaluationResult
{
	True,
	False,
	Unknown,
	Error,
};

////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Domain
{
	Program,
	Integer,
	Unknown,
};

////////////////////////////////////////////////////////////////////////////////////////////////////

enum class SetSize
{
	Empty,
	Unit,
	Multi,
	Unknown,
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct Type
{
	Domain domain{Domain::Unknown};
	SetSize setSize{SetSize::Unknown};
};

////////////////////////////////////////////////////////////////////////////////////////////////////

}

#endif
