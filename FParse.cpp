// f_flat Grammar 2022
// 
// Parser Class Implementation File

// SHELL GENERATED BY PARSERGEN VERSION 5.0.4

#include <iostream>
#include <string>
using namespace std;

#include "FScan.h"
#include "FParse.h"
#include "FCodeGen.h"

extern Scanner scan;
extern CodeGen code;

Parser::Parser()
{
	tokenAvailable = false;
}

void Parser::SyntaxError(Token t, string msg)
{
	cout << "Syntax Error: " + msg << endl;
	exit(1); // abort on any syntax error
}

Token Parser::NextToken()
{
	if (!tokenAvailable)
	{
		
		savedToken = scan.GetNextToken();
		tokenAvailable = true;
	}
	return savedToken;
}

void Parser::Match(Token t)
{
	
	if (t != NextToken()){
	cout << "no token\n";
		SyntaxError(t, "");
	}
	else
		tokenAvailable = false;
}


void Parser::VarDec()
{
	Match(ID);
	VarDecTail();
}

void Parser::VarDecList()
{
	VarDec();
	VarDecListTail();
}

void Parser::VarDecListTail()
{
	switch (NextToken())
	{
	case COMMA:
		Match(COMMA);
		VarDec();
		VarDecListTail();
		break;
	case SEMICOLON:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::VarDecTail()
{
	switch (NextToken())
	{
	case LSTAPLE:
		Match(LSTAPLE);
		Match(INT_LITERAL);
		Match(RSTAPLE);
		break;
	case SEMICOLON:
	case COMMA:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::DecTail()
{
	switch (NextToken())
	{
	case ASSIGN_OP:
		Match(ASSIGN_OP);
		Literal();
		// code.ProcessLiteral();
		cout << "Assign OP\n";
		cout << scan.tokenBuffer;
		break;
	case LSTAPLE:
	case SEMICOLON:
	case COMMA:
		VarDecTail();
		VarDecListTail();
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::Type()
{
	switch (NextToken())
	{
	case INT_SYM:
		Match(INT_SYM);
		break;
	case BOOL_SYM:
		Match(BOOL_SYM);
		break;
	case FAKE_SYM:
		Match(FAKE_SYM);
		break;
	case STR_SYM:
		Match(STR_SYM);
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::Literal()
{
	switch (NextToken())
	{
	case INT_LITERAL:
		Match(INT_LITERAL);
		break;
	case BOOL_LITERAL:
		Match(BOOL_LITERAL);
		break;
	case FAKE_LITERAL:
		Match(FAKE_LITERAL);
		break;
	case STR_LITERAL:
		Match(STR_LITERAL);
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::MultOp()
{
	switch (NextToken())
	{
	case MUL_OP:
		Match(MUL_OP);
		break;
	case DIV_OP:
		Match(DIV_OP);
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::FactorTail()
{
	switch (NextToken())
	{
	case MUL_OP:
	case DIV_OP: //Real Div
		MultOp();
		Primary();
		// code.GenInfix();
		cout << "Mul or div\n";
		FactorTail();
		break;
	case AND_SYM:
	case NOT_SYM:
	case RSTAPLE:
	case RPAREN:
	case SEMICOLON:
	case COMMA:
	case ADD_OP:
	case SUB_OP:
	case LT_OP:
	case LE_OP:
	case GT_OP:
	case GE_OP:
	case EQ_OP:
	case NE_OP:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::Primary()
{
	switch (NextToken())
	{
	case INT_LITERAL:
	case FAKE_LITERAL:
	case STR_LITERAL:
	case BOOL_LITERAL:
		Literal();
		// code.ProcessLiteral();
		cout << "Process Literal\n";
		break;
	case ID:
		Variable();
		break;
	case LPAREN:
		Match(LPAREN);
		Condition();
		Match(RPAREN);
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::AddOp()
{
	switch (NextToken())
	{
	case ADD_OP:
		Match(ADD_OP);
		break;
	case SUB_OP:
		Match(SUB_OP);
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::ExprTail()
{
	switch (NextToken())
	{
	case ADD_OP:
	case SUB_OP:
		AddOp();
		Factor();
		ExprTail();
		break;
	case AND_SYM:
	case NOT_SYM:
	case RSTAPLE:
	case RPAREN:
	case SEMICOLON:
	case COMMA:
	case LT_OP:
	case LE_OP:
	case GT_OP:
	case GE_OP:
	case EQ_OP:
	case NE_OP:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::Factor()
{
	Primary();
	// code.GenInfix();
	cout << "Get infix\n";
	FactorTail();
}

void Parser::RelOp()
{
	switch (NextToken())
	{
	case LT_OP:
		Match(LT_OP);
		break;
	case LE_OP:
		Match(LE_OP);
		break;
	case GT_OP:
		Match(GT_OP);
		break;
	case GE_OP:
		Match(GE_OP);
		break;
	case EQ_OP:
		Match(EQ_OP);
		break;
	case NE_OP:
		Match(NE_OP);
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::RelTail()
{
	switch (NextToken())
	{
	case LT_OP:
	case LE_OP:
	case GT_OP:
	case GE_OP:
	case EQ_OP:
	case NE_OP:
		RelOp();
		Expression();
		break;
	case AND_SYM:
	case NOT_SYM:
	case RPAREN:
	case SEMICOLON:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::Relational()
{
	Expression();
	RelTail();
}

void Parser::AndTail()
{
	switch (NextToken())
	{
	case AND_SYM:
		Match(AND_SYM);
		Negation();
		AndTail();
		break;
	case NOT_SYM:
	case RPAREN:
	case SEMICOLON:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::Negation()
{
	switch (NextToken())
	{
	case NOT_SYM:
		Match(NOT_SYM);
		Relational();
		break;
	case AND_SYM:
	//case NOT_SYM:
	case RPAREN:
	case SEMICOLON:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::CondTail()
{
	switch (NextToken())
	{
	case NOT_SYM:
		Match(NOT_SYM);
		AndCond();
		break;
	case RPAREN:
	case SEMICOLON:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::AndCond()
{
	Negation();
	AndTail();
}

void Parser::VarInit()
{
	switch (NextToken())
	{
	case INT_SYM:
		Match(INT_SYM);
		Match(ID);
		break;
	case ID:
		Variable();
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::FelseClause()
{
	switch (NextToken())
	{
	case FELSE_SYM:
		Match(FELSE_SYM);
		StmtList();
		break;
	case FENDIF_SYM:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::Condition()
{
	cout << "Condition";
	AndCond();
	CondTail();
}

void Parser::ForStmt()
{
	Match(FOR_SYM);
	Match(LPAREN);
	VarInit();
	Match(ASSIGN_OP);
	Expression();
	Match(SEMICOLON);
	Condition();
	Match(SEMICOLON);
	Variable();
	Match(ASSIGN_OP);
	Expression();
	Match(RPAREN);
	StmtList();
	Match(ENDFOR_SYM);
}

void Parser::DoFwhileStmt()
{
	Match(DO_SYM);
	StmtList();
	Match(FWHILE_SYM);
	Match(LPAREN);
	Condition();
	Match(RPAREN);
}

void Parser::WhileStmt()
{
	Match(WHILE_SYM);
	Match(LPAREN);
	Condition();
	Match(RPAREN);
	StmtList();
	Match(ENDWHILE_SYM);
}

void Parser::FifStmt()
{
	Match(FIF_SYM);
	Match(LPAREN);
	Condition();
	Match(RPAREN);
	StmtList();
	FelseClause();
	Match(FENDIF_SYM);
}

void Parser::ItemListTail()
{
	switch (NextToken())
	{
	case COMMA:
		Match(COMMA);
		Expression();
		// code.WriteExpr();
		cout << "write Expr\n";
		ItemListTail();
		break;
	case RPAREN:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::ItemList()
{
	Expression();
	// code.WriteExpr();
	cout << "write expr\n";
	ItemListTail();
}

void Parser::VariableTail()
{
	switch (NextToken())
	{
	case LSTAPLE:
		Match(LSTAPLE);
		Expression();
		Match(RSTAPLE);
		break;
	case AND_SYM:
	case NOT_SYM:
	case RSTAPLE:
	case RPAREN:
	case SEMICOLON:
	case COMMA:
	case ASSIGN_OP:
	case ADD_OP:
	case SUB_OP:
	case MUL_OP:
	case DIV_OP:
	case LT_OP:
	case LE_OP:
	case GT_OP:
	case GE_OP:
	case EQ_OP:
	case NE_OP:
		break;
	default:
		
		SyntaxError(NextToken(), "");
	}
}

void Parser::VarListTail()
{
	switch (NextToken())
	{
	case COMMA:
		Match(COMMA);
		Variable();
		// code.ReadValue();
		cout << "Read value\n";
		VarListTail();
		break;
	case RPAREN:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::VarList()
{
	Variable();
	// code.ReadValue();
	cout << "Read Value\n";
	VarListTail();
}

void Parser::Expression()
{
	Factor();
	ExprTail();
}

void Parser::Variable()
{
	Match(ID);
	VariableTail();
}

void Parser::FoutlnStmt()
{
	Match(FOUTLN_SYM);
	Match(LPAREN);
	// code.NewLine();
	cout << "fout new line\n";
	ItemList();
	Match(RPAREN);
	Match(SEMICOLON);
}

void Parser::FoutStmt()
{
	Match(FOUT_SYM);
	Match(LPAREN);
	ItemList();
	Match(RPAREN);
	Match(SEMICOLON);
}

void Parser::FinStmt()
{
	Match(FIN_SYM);
	Match(LPAREN);
	VarList();
	Match(RPAREN);
	Match(SEMICOLON);
}

void Parser::AssignStmt()
{
	Variable();
	Match(ASSIGN_OP);
	Expression();
	// code.Assign();
	cout << "Assignment op\n";
	Match(SEMICOLON);
}

void Parser::Declaration()
{
	Type();
	Match(ID);
	DecTail();
	// code.ProcessID();
	cout << "Processed ID\n";
	Match(SEMICOLON);
}

void Parser::StructStmt()
{
	switch (NextToken())
	{
	case FIF_SYM:
		FifStmt();
		break;
	case WHILE_SYM:
		WhileStmt();
		break;
	case DO_SYM:
		DoFwhileStmt();
		break;
	case FOR_SYM:
		ForStmt();
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::SimpleStmt()
{
	switch (NextToken())
	{
	case ID:
		AssignStmt();
		break;
	case FIN_SYM:
		FinStmt();
		break;
	case FOUT_SYM:
		FoutStmt();
		break;
	case FOUTLN_SYM:
		FoutlnStmt();
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::StmtTail()
{
	switch (NextToken())
	{
	case BOOL_SYM:
	case DO_SYM:
	case FAKE_SYM:
	case FIF_SYM:
	case FIN_SYM:
	case FOR_SYM:
	case FOUT_SYM:
	case FOUTLN_SYM:
	case INT_SYM:
	case STR_SYM:
	case WHILE_SYM:
	case ID:
		Statement();
		StmtTail();
		break;
	case ENDFOR_SYM:
	case ENDWHILE_SYM:
	case END_SYM:
	case FELSE_SYM:
	case FENDIF_SYM:
	case FWHILE_SYM:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::Statement()
{
	cout << "statement\n";
	switch (NextToken())
	{
	case FIN_SYM:
	case FOUT_SYM:
	case FOUTLN_SYM:
	case ID:
		SimpleStmt();
		break;
	case DO_SYM:
	case FIF_SYM:
	case FOR_SYM:
	case WHILE_SYM:
		StructStmt();
		break;
	case BOOL_SYM:
	case FAKE_SYM:
	case INT_SYM:
	case STR_SYM:
		Declaration();
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::StmtList()
{
	Statement();
	StmtTail();
}

void Parser::Program()
{
	// code.Start();
	cout << "Started\n";
	Match(BEGIN_SYM);
	cout <<"started\n";
	StmtList();
	Match(END_SYM);
	// code.Finish();
	cout << "Finished\n";
}

void Parser::SystemGoal()
{
	Program();
	Match(EOF_SYM);
}
