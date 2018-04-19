#include <anthem/IntegerVariableDetection.h>

#include <anthem/ASTCopy.h>
#include <anthem/ASTUtils.h>
#include <anthem/ASTVisitors.h>
#include <anthem/Exception.h>
#include <anthem/Simplification.h>
#include <anthem/output/AST.h>

namespace anthem
{

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IntegerVariableDetection
//
////////////////////////////////////////////////////////////////////////////////////////////////////

ast::VariableDeclaration::Domain domain(ast::Term &term);

////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OperationResult
{
	Unchanged,
	Changed,
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct TermDomainVisitor
{
	static ast::VariableDeclaration::Domain visit(ast::BinaryOperation &binaryOperation)
	{
		const auto leftDomain = domain(binaryOperation.left);
		const auto rightDomain = domain(binaryOperation.right);

		if (leftDomain == ast::VariableDeclaration::Domain::General || rightDomain == ast::VariableDeclaration::Domain::General)
			return ast::VariableDeclaration::Domain::General;

		if (leftDomain == ast::VariableDeclaration::Domain::Integer || rightDomain == ast::VariableDeclaration::Domain::Integer)
			return ast::VariableDeclaration::Domain::Integer;

		return ast::VariableDeclaration::Domain::Unknown;
	}

	static ast::VariableDeclaration::Domain visit(ast::Boolean &)
	{
		return ast::VariableDeclaration::Domain::General;
	}

	static ast::VariableDeclaration::Domain visit(ast::Function &)
	{
		// Functions may return values of any type

		// TODO: implement explicit integer specifications
		return ast::VariableDeclaration::Domain::General;
	}

	static ast::VariableDeclaration::Domain visit(ast::Integer &)
	{
		return ast::VariableDeclaration::Domain::Integer;
	}

	static ast::VariableDeclaration::Domain visit(ast::Interval &interval)
	{
		const auto fromDomain = domain(interval.from);
		const auto toDomain = domain(interval.to);

		if (fromDomain == ast::VariableDeclaration::Domain::General || toDomain == ast::VariableDeclaration::Domain::General)
			return ast::VariableDeclaration::Domain::General;

		if (fromDomain == ast::VariableDeclaration::Domain::Integer || toDomain == ast::VariableDeclaration::Domain::Integer)
			return ast::VariableDeclaration::Domain::Integer;

		return ast::VariableDeclaration::Domain::Unknown;
	}

	static ast::VariableDeclaration::Domain visit(ast::SpecialInteger &)
	{
		// TODO: check correctness
		return ast::VariableDeclaration::Domain::Integer;
	}

	static ast::VariableDeclaration::Domain visit(ast::String &)
	{
		return ast::VariableDeclaration::Domain::General;
	}

	static ast::VariableDeclaration::Domain visit(ast::UnaryOperation &unaryOperation)
	{
		return domain(unaryOperation.argument);
	}

	static ast::VariableDeclaration::Domain visit(ast::Variable &variable)
	{
		return variable.declaration->domain;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////

ast::VariableDeclaration::Domain domain(ast::Term &term)
{
	return term.accept(TermDomainVisitor());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool isVariable(const ast::Term &term, const ast::VariableDeclaration &variableDeclaration)
{
	if (!term.is<ast::Variable>())
		return false;

	auto &variable = term.get<ast::Variable>();

	return (variable.declaration == &variableDeclaration);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

struct VariableDomainInFormulaVisitor
{
	static ast::VariableDeclaration::Domain visit(ast::And &and_, ast::VariableDeclaration &variableDeclaration)
	{
		bool integer = false;

		for (auto &argument : and_.arguments)
		{
			const auto domain = argument.accept(VariableDomainInFormulaVisitor(), variableDeclaration);

			if (domain == ast::VariableDeclaration::Domain::General)
				return ast::VariableDeclaration::Domain::General;

			if (domain == ast::VariableDeclaration::Domain::Integer)
				integer = true;
		}

		if (integer)
			return ast::VariableDeclaration::Domain::Integer;

		return ast::VariableDeclaration::Domain::Unknown;
	}

	static ast::VariableDeclaration::Domain visit(ast::Biconditional &biconditional, ast::VariableDeclaration &variableDeclaration)
	{
		const auto leftDomain = biconditional.left.accept(VariableDomainInFormulaVisitor(), variableDeclaration);
		const auto rightDomain = biconditional.left.accept(VariableDomainInFormulaVisitor(), variableDeclaration);

		if (leftDomain == ast::VariableDeclaration::Domain::General || rightDomain == ast::VariableDeclaration::Domain::General)
			return ast::VariableDeclaration::Domain::General;

		if (leftDomain == ast::VariableDeclaration::Domain::Integer || rightDomain == ast::VariableDeclaration::Domain::Integer)
			return ast::VariableDeclaration::Domain::Integer;

		return ast::VariableDeclaration::Domain::Unknown;
	}

	static ast::VariableDeclaration::Domain visit(ast::Boolean &, ast::VariableDeclaration &)
	{
		// Variable doesn’t occur in Booleans, hence it’s still considered integer until the contrary is found
		return ast::VariableDeclaration::Domain::Unknown;
	}

	static ast::VariableDeclaration::Domain visit(ast::Comparison &comparison, ast::VariableDeclaration &variableDeclaration)
	{
		const auto leftIsVariable = isVariable(comparison.left, variableDeclaration);
		const auto rightIsVariable = isVariable(comparison.right, variableDeclaration);

		// TODO: implement more cases
		if (!leftIsVariable && !rightIsVariable)
			return ast::VariableDeclaration::Domain::Unknown;

		auto &otherSide = (leftIsVariable ? comparison.right : comparison.left);

		return domain(otherSide);
	}

	static ast::VariableDeclaration::Domain visit(ast::Exists &exists, ast::VariableDeclaration &variableDeclaration)
	{
		return exists.argument.accept(VariableDomainInFormulaVisitor(), variableDeclaration);
	}

	static ast::VariableDeclaration::Domain visit(ast::ForAll &forAll, ast::VariableDeclaration &variableDeclaration)
	{
		return forAll.argument.accept(VariableDomainInFormulaVisitor(), variableDeclaration);
	}

	static ast::VariableDeclaration::Domain visit(ast::Implies &implies, ast::VariableDeclaration &variableDeclaration)
	{
		const auto antecedentDomain = implies.antecedent.accept(VariableDomainInFormulaVisitor(), variableDeclaration);
		const auto consequentDomain = implies.antecedent.accept(VariableDomainInFormulaVisitor(), variableDeclaration);

		if (antecedentDomain == ast::VariableDeclaration::Domain::General || consequentDomain == ast::VariableDeclaration::Domain::General)
			return ast::VariableDeclaration::Domain::General;

		if (antecedentDomain == ast::VariableDeclaration::Domain::Integer || consequentDomain == ast::VariableDeclaration::Domain::Integer)
			return ast::VariableDeclaration::Domain::Integer;

		return ast::VariableDeclaration::Domain::Unknown;
	}

	static ast::VariableDeclaration::Domain visit(ast::In &in, ast::VariableDeclaration &variableDeclaration)
	{
		const auto elementIsVariable = isVariable(in.element, variableDeclaration);
		const auto setIsVariable = isVariable(in.set, variableDeclaration);

		// TODO: implement more cases
		if (!elementIsVariable && !setIsVariable)
			return ast::VariableDeclaration::Domain::Unknown;

		auto &otherSide = (elementIsVariable ? in.set : in.element);

		return domain(otherSide);
	}

	static ast::VariableDeclaration::Domain visit(ast::Not &not_, ast::VariableDeclaration &variableDeclaration)
	{
		return not_.argument.accept(VariableDomainInFormulaVisitor(), variableDeclaration);
	}

	static ast::VariableDeclaration::Domain visit(ast::Or &or_, ast::VariableDeclaration &variableDeclaration)
	{
		bool integer = false;

		for (auto &argument : or_.arguments)
		{
			const auto domain = argument.accept(VariableDomainInFormulaVisitor(), variableDeclaration);

			if (domain == ast::VariableDeclaration::Domain::General)
				return ast::VariableDeclaration::Domain::General;

			if (domain == ast::VariableDeclaration::Domain::Integer)
				integer = true;
		}

		if (integer)
			return ast::VariableDeclaration::Domain::Integer;

		return ast::VariableDeclaration::Domain::Unknown;
	}

	static ast::VariableDeclaration::Domain visit(ast::Predicate &, ast::VariableDeclaration &)
	{
		// TODO: implement correctly
		return ast::VariableDeclaration::Domain::Unknown;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////

// Recursively finds every variable declaration and executes functor to the formula in the scope of the declaration
struct DetectIntegerVariablesVisitor
{
	static OperationResult visit(ast::And &and_)
	{
		auto operationResult = OperationResult::Unchanged;

		for (auto &argument : and_.arguments)
			if (argument.accept(DetectIntegerVariablesVisitor()) == OperationResult::Changed)
				operationResult = OperationResult::Changed;

		return operationResult;
	}

	static OperationResult visit(ast::Biconditional &biconditional)
	{
		auto operationResult = OperationResult::Unchanged;

		if (biconditional.left.accept(DetectIntegerVariablesVisitor()) == OperationResult::Changed)
			operationResult = OperationResult::Changed;

		if (biconditional.right.accept(DetectIntegerVariablesVisitor()) == OperationResult::Changed)
			operationResult = OperationResult::Changed;

		return operationResult;
	}

	static OperationResult visit(ast::Boolean &)
	{
		return OperationResult::Unchanged;
	}

	static OperationResult visit(ast::Comparison &)
	{
		return OperationResult::Unchanged;
	}

	static OperationResult visit(ast::Exists &exists)
	{
		auto operationResult = OperationResult::Unchanged;

		if (exists.argument.accept(DetectIntegerVariablesVisitor()) == OperationResult::Changed)
			operationResult = OperationResult::Changed;

		for (auto &variableDeclaration : exists.variables)
			if (variableDeclaration->domain == ast::VariableDeclaration::Domain::Unknown
				&& exists.argument.accept(VariableDomainInFormulaVisitor(), *variableDeclaration) == ast::VariableDeclaration::Domain::Integer)
			{
				operationResult = OperationResult::Changed;
				variableDeclaration->domain = ast::VariableDeclaration::Domain::Integer;
			}

		return operationResult;
	}

	static OperationResult visit(ast::ForAll &forAll)
	{
		auto operationResult = OperationResult::Unchanged;

		if (forAll.argument.accept(DetectIntegerVariablesVisitor()) == OperationResult::Changed)
			operationResult = OperationResult::Changed;

		for (auto &variableDeclaration : forAll.variables)
			if (variableDeclaration->domain == ast::VariableDeclaration::Domain::Unknown
				&& forAll.argument.accept(VariableDomainInFormulaVisitor(), *variableDeclaration) == ast::VariableDeclaration::Domain::Integer)
			{
				operationResult = OperationResult::Changed;
				variableDeclaration->domain = ast::VariableDeclaration::Domain::Integer;
			}

		return operationResult;
	}

	static OperationResult visit(ast::Implies &implies)
	{
		auto operationResult = OperationResult::Unchanged;

		if (implies.antecedent.accept(DetectIntegerVariablesVisitor()) == OperationResult::Changed)
			operationResult = OperationResult::Changed;

		if (implies.consequent.accept(DetectIntegerVariablesVisitor()) == OperationResult::Changed)
			operationResult = OperationResult::Changed;

		return operationResult;
	}

	static OperationResult visit(ast::In &)
	{
		return OperationResult::Unchanged;
	}

	static OperationResult visit(ast::Not &not_)
	{
		return not_.argument.accept(DetectIntegerVariablesVisitor());
	}

	static OperationResult visit(ast::Or &or_)
	{
		auto operationResult = OperationResult::Unchanged;

		for (auto &argument : or_.arguments)
			if (argument.accept(DetectIntegerVariablesVisitor()) == OperationResult::Changed)
				operationResult = OperationResult::Changed;

		return operationResult;
	}

	static OperationResult visit(ast::Predicate &)
	{
		return OperationResult::Unchanged;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////

// Assumes the completed formulas to be in translated but not simplified form.
// That is, completed formulas are either variable-free or universally quantified
void detectIntegerVariables(std::vector<ast::Formula> &completedFormulas)
{
	auto operationResult{OperationResult::Changed};

	while (operationResult == OperationResult::Changed)
	{
		operationResult = OperationResult::Unchanged;

		for (auto &completedFormula : completedFormulas)
		{
			if (!completedFormula.is<ast::ForAll>())
				continue;

			auto &forAll = completedFormula.get<ast::ForAll>();

			// TODO: check that integrity constraints are also handled
			if (!forAll.argument.is<ast::Biconditional>())
				continue;

			auto &biconditional = forAll.argument.get<ast::Biconditional>();

			if (!biconditional.left.is<ast::Predicate>())
				continue;

			auto &definition = biconditional.right;

			if (definition.accept(DetectIntegerVariablesVisitor()) == OperationResult::Changed)
				operationResult = OperationResult::Changed;

			for (auto &variableDeclaration : forAll.variables)
				if (variableDeclaration->domain == ast::VariableDeclaration::Domain::Unknown
					&& definition.accept(VariableDomainInFormulaVisitor(), *variableDeclaration) == ast::VariableDeclaration::Domain::Integer)
				{
					operationResult = OperationResult::Changed;
					variableDeclaration->domain = ast::VariableDeclaration::Domain::Integer;
				}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

}
