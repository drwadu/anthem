#include <anthem/Translation.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include <clingo.hh>

#include <anthem/Completion.h>
#include <anthem/Context.h>
#include <anthem/Simplification.h>
#include <anthem/StatementVisitor.h>
#include <anthem/output/AST.h>

namespace anthem
{

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Translation
//
////////////////////////////////////////////////////////////////////////////////////////////////////

void translate(const std::vector<std::string> &fileNames, Context &context)
{
	for (const auto &fileName : fileNames)
	{
		std::ifstream file(fileName, std::ios::in);

		if (!file.is_open())
			throw LogicException("could not read file “" + fileName + "”");

		translate(fileName.c_str(), file, context);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void translate(const char *fileName, std::istream &stream, Context &context)
{
	context.logger.log(output::Priority::Info) << "reading " << fileName;

	auto fileContent = std::string(std::istreambuf_iterator<char>(stream), {});

	std::vector<ast::ScopedFormula> scopedFormulas;

	const auto translateStatement =
		[&scopedFormulas, &context](const Clingo::AST::Statement &statement)
		{
			statement.data.accept(StatementVisitor(), statement, scopedFormulas, context);
		};

	const auto logger =
		[&context](const Clingo::WarningCode, const char *text)
		{
			context.logger.log(output::Priority::Error) << text;
		};

	Clingo::parse_program(fileContent.c_str(), translateStatement, logger);

	ast::PrintContext printContext(context);

	if (!context.performCompletion)
	{
		// Simplify output if specified
		if (context.performSimplification)
			for (auto &scopedFormula : scopedFormulas)
				simplify(scopedFormula.formula);

		if (context.visiblePredicateSignatures)
			context.logger.log(output::Priority::Warning) << "#show statements are ignored because completion is not enabled";

		if (context.externalPredicateSignatures)
			context.logger.log(output::Priority::Warning) << "#external statements are ignored because completion is not enabled";

		for (const auto &scopedFormula : scopedFormulas)
		{
			ast::print(context.logger.outputStream(), scopedFormula.formula, printContext);
			context.logger.outputStream() << std::endl;
		}

		return;
	}

	// Perform completion
	auto completedFormulas = complete(std::move(scopedFormulas), context);

	// Check for #show statements with undeclared predicates
	if (context.visiblePredicateSignatures)
		for (const auto &predicateSignature : context.visiblePredicateSignatures.value())
			if (!predicateSignature.used)
				context.logger.log(output::Priority::Warning)
					<< "#show declaration of “"
					<< predicateSignature.predicateSignature.name
					<< "/" << predicateSignature.predicateSignature.arity
					<< "” does not match any eligible predicate";

	// Check for #external statements with undeclared predicates
	if (context.externalPredicateSignatures)
		for (const auto &predicateSignature : context.externalPredicateSignatures.value())
			if (!predicateSignature.used)
				context.logger.log(output::Priority::Warning)
					<< "#external declaration of “"
					<< predicateSignature.predicateSignature.name
					<< "/" << predicateSignature.predicateSignature.arity
					<< "” does not match any eligible predicate";

	// Simplify output if specified
	if (context.performSimplification)
		for (auto &completedFormula : completedFormulas)
			simplify(completedFormula);

	// TODO: remove variables that are not referenced after simplification

	for (const auto &completedFormula : completedFormulas)
	{
		ast::print(context.logger.outputStream(), completedFormula, printContext);
		context.logger.outputStream() << std::endl;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

}
