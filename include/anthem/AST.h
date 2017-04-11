#ifndef __ANTHEM__AST_H
#define __ANTHEM__AST_H

#include <anthem/ASTForward.h>

namespace anthem
{
namespace ast
{

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// AST
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// Terms are primitive (or arguments) if they are neither operations nor intervals
inline bool isPrimitive(const ast::Term &term)
{
	return (!term.is<ast::BinaryOperation>() && !term.is<ast::Interval>());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Primitives
////////////////////////////////////////////////////////////////////////////////////////////////////

struct BinaryOperation
{
	enum class Operator
	{
		Plus,
		Minus,
		Multiplication,
		Division,
		Modulo
	};

	BinaryOperation(const BinaryOperation &other) = delete;
	BinaryOperation &operator=(const BinaryOperation &other) = delete;
	BinaryOperation(BinaryOperation &&other) noexcept = default;
	BinaryOperation &operator=(BinaryOperation &&other) noexcept = default;

	BinaryOperation(Operator operator_, Term &&left, Term &&right)
	:	operator_{operator_},
		left{std::move(left)},
		right{std::move(right)}
	{
	}

	Operator operator_;
	Term left;
	Term right;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct Boolean
{
	Boolean(bool value)
	:	value{value}
	{
	}

	Boolean(const Boolean &other) = delete;
	Boolean &operator=(const Boolean &other) = delete;
	Boolean(Boolean &&other) noexcept = default;
	Boolean &operator=(Boolean &&other) noexcept = default;

	bool value = false;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct Comparison
{
	enum class Operator
	{
		GreaterThan,
		LessThan,
		LessEqual,
		GreaterEqual,
		NotEqual,
		Equal
	};

	Comparison(Operator operator_, Term &&left, Term &&right)
	:	operator_{operator_},
		left{std::move(left)},
		right{std::move(right)}
	{
	}

	Comparison(const Comparison &other) = delete;
	Comparison &operator=(const Comparison &other) = delete;
	Comparison(Comparison &&other) noexcept = default;
	Comparison &operator=(Comparison &&other) noexcept = default;

	Operator operator_;
	Term left;
	Term right;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct Constant
{
	Constant(std::string &&name)
	:	name{std::move(name)}
	{
	}

	Constant(const Constant &other) = delete;
	Constant &operator=(const Constant &other) = delete;
	Constant(Constant &&other) noexcept = default;
	Constant &operator=(Constant &&other) noexcept = default;

	std::string name;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct Function
{
	Function(std::string &&name)
	:	name{std::move(name)}
	{
	}

	Function(std::string &&name, std::vector<Term> &&arguments)
	:	name{std::move(name)},
		arguments{std::move(arguments)}
	{
	}

	Function(const Function &other) = delete;
	Function &operator=(const Function &other) = delete;
	Function(Function &&other) noexcept = default;
	Function &operator=(Function &&other) noexcept = default;

	std::string name;
	std::vector<Term> arguments;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: refactor (limit element type to primitive terms)
struct In
{
	In(Term &&element, Term &&set)
	:	element{std::move(element)},
		set{std::move(set)}
	{
		// While the set may be any term, the element must be primitive
		assert(isPrimitive(element));
	}

	In(const In &other) = delete;
	In &operator=(const In &other) = delete;
	In(In &&other) noexcept = default;
	In &operator=(In &&other) noexcept = default;

	Term element;
	Term set;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct Integer
{
	Integer(int value)
	:	value{value}
	{
	}

	Integer(const Integer &other) = delete;
	Integer &operator=(const Integer &other) = delete;
	Integer(Integer &&other) noexcept = default;
	Integer &operator=(Integer &&other) noexcept = default;

	int value;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct Interval
{
	Interval(Term &&from, Term &&to)
	:	from{std::move(from)},
		to{std::move(to)}
	{
	}

	Interval(const Interval &other) = delete;
	Interval &operator=(const Interval &other) = delete;
	Interval(Interval &&other) noexcept = default;
	Interval &operator=(Interval &&other) noexcept = default;

	Term from;
	Term to;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct Predicate
{
	Predicate(std::string &&name)
	:	name{std::move(name)}
	{
	}

	Predicate(std::string &&name, std::vector<Term> &&arguments)
	:	name{std::move(name)},
		arguments{std::move(arguments)}
	{
	}

	Predicate(const Predicate &other) = delete;
	Predicate &operator=(const Predicate &other) = delete;
	Predicate(Predicate &&other) noexcept = default;
	Predicate &operator=(Predicate &&other) noexcept = default;

	std::size_t arity() const
	{
		return arguments.size();
	}

	std::string name;
	std::vector<Term> arguments;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct SpecialInteger
{
	enum class Type
	{
		Infimum,
		Supremum
	};

	SpecialInteger(Type type)
	:	type{type}
	{
	}

	SpecialInteger(const SpecialInteger &other) = delete;
	SpecialInteger &operator=(const SpecialInteger &other) = delete;
	SpecialInteger(SpecialInteger &&other) noexcept = default;
	SpecialInteger &operator=(SpecialInteger &&other) noexcept = default;

	Type type;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct String
{
	String(std::string &&text)
	:	text{std::move(text)}
	{
	}

	String(const String &other) = delete;
	String &operator=(const String &other) = delete;
	String(String &&other) noexcept = default;
	String &operator=(String &&other) noexcept = default;

	std::string text;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct Variable
{
	enum class Type
	{
		UserDefined,
		Reserved
	};

	Variable(std::string &&name, Type type)
	:	name{std::move(name)},
		type{type}
	{
	}

	Variable(const Variable &other) = delete;
	Variable &operator=(const Variable &other) = delete;
	Variable(Variable &&other) noexcept = default;
	Variable &operator=(Variable &&other) noexcept = default;

	std::string name;
	Type type;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Expressions
////////////////////////////////////////////////////////////////////////////////////////////////////

struct And
{
	And() = default;

	And(std::vector<Formula> &&arguments)
	:	arguments{std::move(arguments)}
	{
	}

	And(const And &other) = delete;
	And &operator=(const And &other) = delete;
	And(And &&other) noexcept = default;
	And &operator=(And &&other) noexcept = default;

	std::vector<Formula> arguments;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct Biconditional
{
	Biconditional(Formula &&left, Formula &&right)
	:	left{std::move(left)},
		right{std::move(right)}
	{
	}

	Biconditional(const Biconditional &other) = delete;
	Biconditional &operator=(const Biconditional &other) = delete;
	Biconditional(Biconditional &&other) noexcept = default;
	Biconditional &operator=(Biconditional &&other) noexcept = default;

	Formula left;
	Formula right;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct Exists
{
	Exists(std::vector<Variable> &&variables, Formula &&argument)
	:	variables{std::move(variables)},
		argument{std::move(argument)}
	{
	}

	Exists(const Exists &other) = delete;
	Exists &operator=(const Exists &other) = delete;
	Exists(Exists &&other) noexcept = default;
	Exists &operator=(Exists &&other) noexcept = default;

	std::vector<Variable> variables;
	Formula argument;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct ForAll
{
	ForAll(std::vector<Variable> &&variables, Formula &&argument)
	:	variables{std::move(variables)},
		argument{std::move(argument)}
	{
	}

	ForAll(const ForAll &other) = delete;
	ForAll &operator=(const ForAll &other) = delete;
	ForAll(ForAll &&other) noexcept = default;
	ForAll &operator=(ForAll &&other) noexcept = default;

	std::vector<Variable> variables;
	Formula argument;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct Implies
{
	Implies(Formula &&antecedent, Formula &&consequent)
	:	antecedent{std::move(antecedent)},
		consequent{std::move(consequent)}
	{
	}

	Implies(const Implies &other) = delete;
	Implies &operator=(const Implies &other) = delete;
	Implies(Implies &&other) noexcept = default;
	Implies &operator=(Implies &&other) noexcept = default;

	Formula antecedent;
	Formula consequent;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct Not
{
	Not(Formula &&argument)
	:	argument{std::move(argument)}
	{
	}

	Not(const Not &other) = delete;
	Not &operator=(const Not &other) = delete;
	Not(Not &&other) noexcept = default;
	Not &operator=(Not &&other) noexcept = default;

	Formula argument;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct Or
{
	Or() = default;

	Or(std::vector<Formula> &&arguments)
	:	arguments{std::move(arguments)}
	{
	}

	Or(const Or &other) = delete;
	Or &operator=(const Or &other) = delete;
	Or(Or &&other) noexcept = default;
	Or &operator=(Or &&other) noexcept = default;

	std::vector<Formula> arguments;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Deep Copying
////////////////////////////////////////////////////////////////////////////////////////////////////

BinaryOperation deepCopy(const BinaryOperation &other);
Boolean deepCopy(const Boolean &other);
Comparison deepCopy(const Comparison &other);
Constant deepCopy(const Constant &other);
Function deepCopy(const Function &other);
Integer deepCopy(const Integer &other);
Interval deepCopy(const Interval &other);
Predicate deepCopy(const Predicate &other);
SpecialInteger deepCopy(const SpecialInteger &other);
String deepCopy(const String &other);
Variable deepCopy(const Variable &other);
std::vector<Variable> deepCopy(const std::vector<Variable> &other);
And deepCopy(const And &other);
Biconditional deepCopy(const Biconditional &other);
Exists deepCopy(const Exists &other);
ForAll deepCopy(const ForAll &other);
Implies deepCopy(const Implies &other);
Not deepCopy(const Not &other);
Or deepCopy(const Or &other);

Formula deepCopy(const Formula &formula);
std::vector<Formula> deepCopy(const std::vector<Formula> &formulas);
Term deepCopy(const Term &term);
std::vector<Term> deepCopy(const std::vector<Term> &terms);

////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Variant>
struct VariantDeepCopyVisitor
{
	template<class T>
	Variant visit(const T &x)
	{
		return deepCopy(x);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////

const auto deepCopyVariant =
	[](const auto &variant) -> typename std::decay<decltype(variant)>::type
	{
		using VariantType = typename std::decay<decltype(variant)>::type;

		return variant.accept(VariantDeepCopyVisitor<VariantType>());
	};

////////////////////////////////////////////////////////////////////////////////////////////////////

const auto deepCopyVariantVector =
	[](const auto &variantVector) -> typename std::decay<decltype(variantVector)>::type
	{
		using Type = typename std::decay<decltype(variantVector)>::type::value_type;

		std::vector<Type> result;
		result.reserve(variantVector.size());

		for (const auto &variant : variantVector)
			result.emplace_back(deepCopyVariant(variant));

		return result;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////

const auto deepCopyVector =
	[](const auto &vector) -> typename std::decay<decltype(vector)>::type
	{
		using Type = typename std::decay<decltype(vector)>::type::value_type;

		std::vector<Type> result;
		result.reserve(vector.size());

		for (const auto &element : vector)
			result.emplace_back(deepCopy(element));

		return result;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////

inline BinaryOperation deepCopy(const BinaryOperation &other)
{
	return BinaryOperation(other.operator_, deepCopy(other.left), deepCopy(other.right));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline Boolean deepCopy(const Boolean &other)
{
	return Boolean(other.value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline Comparison deepCopy(const Comparison &other)
{
	return Comparison(other.operator_, deepCopy(other.left), deepCopy(other.right));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline Constant deepCopy(const Constant &other)
{
	return Constant(std::string(other.name));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline Function deepCopy(const Function &other)
{
	return Function(std::string(other.name), deepCopy(other.arguments));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline Integer deepCopy(const Integer &other)
{
	return Integer(other.value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline Interval deepCopy(const Interval &other)
{
	return Interval(deepCopy(other.from), deepCopy(other.to));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline Predicate deepCopy(const Predicate &other)
{
	return Predicate(std::string(other.name), deepCopy(other.arguments));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline SpecialInteger deepCopy(const SpecialInteger &other)
{
	return SpecialInteger(other.type);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline String deepCopy(const String &other)
{
	return String(std::string(other.text));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline Variable deepCopy(const Variable &other)
{
	return Variable(std::string(other.name), other.type);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline std::vector<Variable> deepCopy(const std::vector<Variable> &other)
{
	return deepCopyVector(other);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline And deepCopy(const And &other)
{
	return And(deepCopy(other.arguments));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline Biconditional deepCopy(const Biconditional &other)
{
	return Biconditional(deepCopy(other.left), deepCopy(other.right));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline Exists deepCopy(const Exists &other)
{
	return Exists(deepCopy(other.variables), deepCopy(other.argument));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline ForAll deepCopy(const ForAll &other)
{
	return ForAll(deepCopy(other.variables), deepCopy(other.argument));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline In deepCopy(const In &other)
{
	return In(deepCopy(other.element), deepCopy(other.set));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline Implies deepCopy(const Implies &other)
{
	return Implies(deepCopy(other.antecedent), deepCopy(other.consequent));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline Not deepCopy(const Not &other)
{
	return Not(deepCopy(other.argument));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline Or deepCopy(const Or &other)
{
	return Or(deepCopy(other.arguments));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline Formula deepCopy(const Formula &formula)
{
	return deepCopyVariant(formula);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline std::vector<Formula> deepCopy(const std::vector<Formula> &formulas)
{
	return deepCopyVariantVector(formulas);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline Term deepCopy(const Term &term)
{
	return deepCopyVariant(term);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline std::vector<Term> deepCopy(const std::vector<Term> &terms)
{
	return deepCopyVariantVector(terms);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

}
}

#endif
