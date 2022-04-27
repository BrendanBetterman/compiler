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
	cout << endl<<"line #" <<scan.lineNumber+1;
	cout << "position "<<scan.lineBuffer.size();
	
	cout << "Char" << scan.tokenBuffer;
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
	cerr << "no token\n";
		SyntaxError(t, "");
	}
	else{
		tokenAvailable = false;
		//cerr<< "token got\n";
	}
	
		
}
string Parser::kindToStr(ExprKind& k){
	switch(k){
		case LITERAL_INT:
			return "Literal int";
		case LITERAL_FAKE:
			return "Literal fake";
		case LITERAL_BOOL:
			return "literal bool";
		case LITERAL_STR:
			return "literal string";
		case TEMP_EXPR:
			return "temp expr";
		case ID_EXPR:
			return "id expr";
		case IDF_EXPR:
			return "idf expr";
		default:
			return "default";
	}
}

void Parser::VarDec(ExprKind& kind)
{
	ExprRec expr;//should get type from previous
	expr.kind =kind;
	Match(ID);
	expr.name = scan.tokenBuffer;//new 4-20
	code.ProcessId(expr);
	VarDecTail();
}

void Parser::VarDecList(ExprKind& kind)
{
	VarDec(kind);
	VarDecListTail(kind);
}

void Parser::VarDecListTail(ExprKind& kind)
{
	switch (NextToken())
	{
	case COMMA:
		Match(COMMA);
		VarDec(kind);
		VarDecListTail(kind);
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

void Parser::DecTail(ExprRec& expr)
{
	ExprRec source;
	switch (NextToken())
	{
	case ASSIGN_OP:
		Match(ASSIGN_OP);
		Literal(source);
		
		code.ProcessLiteral(source);
		cout << "Assign OP\n";

		code.Assign(expr,source);
		//cout << scan.tokenBuffer;
		break;
	case LSTAPLE:
	case SEMICOLON:
	case COMMA:
		VarDecTail();
		VarDecListTail(expr.kind);
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::Type(ExprRec& expr)
{
	switch (NextToken())
	{
	case INT_SYM:
		Match(INT_SYM);
		expr.kind=ID_EXPR;//was lit int
		cout<<"type is literal int";
		break;
	case BOOL_SYM:
		Match(BOOL_SYM);
		expr.kind = IDB_EXPR;//LITERAL_BOOL;
		break;
	case FAKE_SYM:
		Match(FAKE_SYM);
		expr.kind = IDF_EXPR;//was lit fake
		break;
	case STR_SYM:
		Match(STR_SYM);
		expr.kind = IDS_EXPR;//LITERAL_STR;
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::Literal(ExprRec& expr)
{
	switch (NextToken())
	{
	case INT_LITERAL:
		Match(INT_LITERAL);
		expr.kind = LITERAL_INT;
		break;
	case BOOL_LITERAL:
		Match(BOOL_LITERAL);
		expr.kind = LITERAL_BOOL;
		break;
	case FAKE_LITERAL:
		Match(FAKE_LITERAL);
		expr.kind = LITERAL_FAKE;
		break;
	case STR_LITERAL:
		Match(STR_LITERAL);
		expr.kind = LITERAL_STR;
		break;
	case ID:
		Match(ID);
		expr.kind = ID_EXPR;
		expr.name = scan.tokenBuffer;//get scanner data
		cout<< "ID to id\n";
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::MultOp(OpRec& op)
{
	switch (NextToken())
	{
	case MUL_OP:
		Match(MUL_OP);
		op.op = MULT;
		code.ProcessOp(op);
		break;
	case DIV_OP:
		Match(DIV_OP);
		op.op = DIV;
		code.ProcessOp(op);
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::FactorTail()
{
	ExprRec expr;
	OpRec op;
	switch (NextToken())
	{
	case MUL_OP:
	case DIV_OP: //Real Div
		MultOp(op);
		Primary(expr);
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

void Parser::Primary(ExprRec& result)
{
	ExprRec identifier;
	ConRec con;
	switch (NextToken())
	{
	case INT_LITERAL:
		result.kind = LITERAL_INT;
		Literal(result);
		code.ProcessLiteral(result);
		cout << "Process Literal INT\n";
		
		break;
	case FAKE_LITERAL:
		result.kind = LITERAL_FAKE;
		Literal(result);
		code.ProcessLiteral(result);
		cout << "Process Literal Fake\n";
		break;
	case STR_LITERAL:
		result.kind = LITERAL_STR;
		Literal(result);
		code.ProcessLiteral(result);
		cout << "Process Literal STR\n";
		break;
	case BOOL_LITERAL:
		result.kind = LITERAL_BOOL;
		Literal(result);
		code.ProcessLiteral(result);
		cout << "Process Literal Bool\n";
		break;
	case ID:
		//Literal(result);
		
		Variable(result); //for arrays
		break;
	case LPAREN:
		Match(LPAREN);
		Condition(result,con,identifier);
		Match(RPAREN);
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::AddOp(OpRec& op)
{
	switch (NextToken())
	{
	case ADD_OP:
		Match(ADD_OP);
		op.op = PLUS;
		code.ProcessOp(op);
		break;
	case SUB_OP:
		Match(SUB_OP);
		op.op = MINUS;
		code.ProcessOp(op);
		break;
	
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::ExprTail()
{
	OpRec op;
	ExprRec expr;
	switch (NextToken())
	{
	case ADD_OP:
	case SUB_OP:
	case MUL_OP:
		AddOp(op);
		Factor(expr);
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

void Parser::Factor(ExprRec& expr)
{	
	Primary(expr);// leftside
	
	//code.GenInfix(expr);
	cout << "Get infix\n";
	FactorTail();
}

void Parser::RelOp(ConRec& con)
{
	switch (NextToken())
	{
	case LT_OP:
		Match(LT_OP);
		con.con = LT;
		break;
	case LE_OP:
		Match(LE_OP);
		con.con = LE;
		break;
	case GT_OP:
		Match(GT_OP);
		con.con = GT;
		break;
	case GE_OP:
		Match(GE_OP);
		con.con = GE;
		break;
	case EQ_OP:
		Match(EQ_OP);
		con.con = EQ;
		break;
	case NE_OP:
		Match(NE_OP);
		con.con = NE;
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::RelTail()
{
	ExprRec expr;
	ConRec con;
	switch (NextToken())
	{
	case LT_OP:
	case LE_OP:
	case GT_OP:
	case GE_OP:
	case EQ_OP:
	case NE_OP:
		RelOp(con);
		Expression(expr);
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
	ExprRec expr;
	Expression(expr);
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
	ExprRec expr,source;
	
	switch (NextToken())
	{
	case INT_SYM:
		Match(INT_SYM);
		expr.kind = ID_EXPR;
		Match(ID);
		expr.name = scan.tokenBuffer;
		code.ProcessId(expr);
		NextToken();
		Match(ASSIGN_OP);
		Literal(expr);
		code.ProcessLiteral(expr);
		code.Assign(expr,expr);
		Match(SEMICOLON);
		break;
	case ID:
		Match(ID);
		expr.name = scan.tokenBuffer;
		switch(NextToken()){
			case ASSIGN_OP:
				//assign;
				Match(ASSIGN_OP);
				Literal(source);
				code.ProcessLiteral(source);
				code.Assign(expr,source);
				
				break;
            default: break;
		}
		//Variable(expr);
		Match(SEMICOLON);
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::FelseClause()
{
	ExprRec expr;
	switch (NextToken())
	{
	case FELSE_SYM:
		Match(FELSE_SYM);
		code.ProcessElse();
		StmtList(expr);
		break;
	case FENDIF_SYM:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::Condition(ExprRec& Lexpr,ConRec& con, ExprRec& Rexpr)
{
	cout << "Condition";
	//Variable(expr);
	
	Primary(Lexpr);
	RelOp(con);
	Primary(Rexpr);
	
		
	AndCond();
	CondTail();
}

void Parser::ForStmt()
{
	ExprRec Lexpr,Rexpr;
	ConRec con;
	Match(FOR_SYM);
	Match(LPAREN);

	VarInit();
	
	//Match(ASSIGN_OP);
	//Expression(Lexpr);
	//Match(SEMICOLON);
	//init var 
	Condition(Lexpr,con,Rexpr);
	code.ProcessForCond(Lexpr,con,Rexpr);
	//code.forcond
	Match(SEMICOLON);
	Variable(Lexpr);
	Match(ASSIGN_OP);
	Expression(Lexpr);
	code.LoopUpdate();
	Match(RPAREN);
	StmtList(Lexpr);
	
	Match(ENDFOR_SYM);
	code.ProcessEndFor();
}

void Parser::DoFwhileStmt()
{
	ExprRec expr,Lexpr,Rexpr;
	ConRec con;
	Match(DO_SYM);
	code.ProcessDo();
	StmtList(expr);
	Match(FWHILE_SYM);
	Match(LPAREN);
	Condition(Lexpr,con,Rexpr);
	Match(RPAREN);
	code.ProcessEndFwhile(Lexpr,con,Rexpr);
}

void Parser::WhileStmt()
{
	ExprRec Lexpr,Rexpr;
	ConRec con;
	Match(WHILE_SYM);
	Match(LPAREN);
	Condition(Lexpr,con,Rexpr);
	code.ProcessWhile(Lexpr,con,Rexpr);
	Match(RPAREN);
	StmtList(Lexpr);
	Match(ENDWHILE_SYM);
	code.ProcessEndWhile();
}

void Parser::FifStmt()
{
	ExprRec Lexpr,Rexpr;
	ConRec con;
	Match(FIF_SYM);
	Match(LPAREN);
	Condition(Lexpr,con,Rexpr);
	code.ProcessIf(Lexpr,con,Rexpr);//should go in if
	Match(RPAREN);
	StmtList(Lexpr);
	FelseClause();
	Match(FENDIF_SYM);
	code.ProcessEndIf();
}

void Parser::ItemListTail()
{
	ExprRec expr;
	switch (NextToken())
	{
	case COMMA:
		Match(COMMA);
		Expression(expr);
		code.WriteExpr(expr);
		
		ItemListTail();
		break;
	case RPAREN:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::ItemList(ExprRec& expr)
{

	Expression(expr);
	
	code.WriteExpr(expr);
	ItemListTail();
}

void Parser::VariableTail(ExprRec& expr)
{
	switch (NextToken())
	{
	case LSTAPLE:
		Match(LSTAPLE);
		Expression(expr);
		Match(RSTAPLE);
		break;
	case SEMICOLON:
		
	case AND_SYM:
	case NOT_SYM:
	case RSTAPLE:
	case RPAREN:
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
		//cout<<"conditional stmt";
		break;
	default:
		
		SyntaxError(NextToken(), "");
	}
}

void Parser::VarListTail(ExprRec& expr)
{
	switch (NextToken())
	{
	case COMMA:
		Match(COMMA);
		Variable(expr);
		cout <<"varlisttail" <<expr.name <<endl;
		code.ReadValue(expr);
		cout << "Read value\n";
		VarListTail(expr);
		break;
	case RPAREN:
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::VarList(ExprRec& expr)
{
	Variable(expr);
	code.ReadValue(expr);
	cout << "Read Value\n";
	VarListTail(expr);
}

void Parser::Expression(ExprRec& result)//
{
	ExprRec leftOperand, rightOperand;
	OpRec op;
	

	//old non order
	//Primary(result);
	/*
	Primary(leftOperand);
	for (;;){
		switch(NextToken()){
			case ADD_OP:
			case SUB_OP:
				AddOp(op);
				Primary(rightOperand);
				code.GenInfix(leftOperand, op, rightOperand, result);
				cerr<<result.name<<"\n";
				//wip temp fix
				break;
			case DIV_OP:
			case MUL_OP:
				MultOp(op);
				Primary(rightOperand);
				code.GenInfix(leftOperand, op, rightOperand, result);
				break;
			
			default:
				result = leftOperand;
				return;
		}
	}*/
	Factor(result);
	ExprTail();
}

void Parser::Variable(ExprRec& expr)
{
	
	Match(ID);
	expr.name = scan.tokenBuffer;

    string st = std::to_string(expr.kind);
    cout << "\nVariable(expr.kind)=" << st << "\n";


    //expr.kind = ID_EXPR;//should check idf or id
	VariableTail(expr);
}

void Parser::FoutlnStmt(ExprRec& expr)
{
	Match(FOUTLN_SYM);
	Match(LPAREN);
	code.NewLine();
	//cout << "fout new line\n";
	ItemList(expr);
	Match(RPAREN);
	Match(SEMICOLON);
}

void Parser::FoutStmt(ExprRec& expr)
{
	Match(FOUT_SYM);
	Match(LPAREN);
	ItemList(expr);
	Match(RPAREN);
	Match(SEMICOLON);
}

void Parser::FinStmt(ExprRec& expr)
{
	Match(FIN_SYM);
	Match(LPAREN);
	VarList(expr);
	Match(RPAREN);
	Match(SEMICOLON);
}

void Parser::AssignStmt(ExprRec& expr)
{
	ExprRec identifier;
	Variable(expr);
	Match(ASSIGN_OP);
	identifier.kind = expr.kind;
	identifier.name = expr.name;
	
	Expression(identifier);
	//WIP identifier should be the tmp var
	//identifier.name = expr.name;
	cout<<identifier.name<<expr.name<<"\n";
	if(identifier.kind ==ID_EXPR){
		identifier.name ="Temp";
	}else if(identifier.kind ==IDF_EXPR){
		identifier.name = "TempF";
	}else{
		
		identifier.name ="Temp";
	}
	code.Assign( expr,identifier);
	
	cout << "Assignment op\n";
	Match(SEMICOLON);
}
//get type,match id,set name,code.processid,dectail
void Parser::Declaration(ExprRec& expr)
{
	Type(expr);
	Match(ID);
	expr.name = scan.tokenBuffer;
	code.ProcessId(expr);
	DecTail(expr);
    string st = std::to_string(expr.kind);
    cout << "Processed ID->expr.kind=" << st << "\n";
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

void Parser::SimpleStmt(ExprRec& expr)
{
	switch (NextToken())
	{
	case ID:
		AssignStmt(expr);
		break;
	case FIN_SYM:
		FinStmt(expr);
		break;
	case FOUT_SYM:
		FoutStmt(expr);
		break;
	case FOUTLN_SYM:
		FoutlnStmt(expr);
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::StmtTail(ExprRec& expr)
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
		Statement(expr);
		StmtTail(expr);
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

void Parser::Statement(ExprRec& expr)
{
	cout << "statement\n";
	switch (NextToken())
	{
	case FIN_SYM:
	case FOUT_SYM:
	case FOUTLN_SYM:
	case ID:
		SimpleStmt(expr);
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
		
		Declaration(expr);//needs expr
		break;
	default:
		SyntaxError(NextToken(), "");
	}
}

void Parser::StmtList(ExprRec& expr)
{
	Statement(expr);
	StmtTail(expr);
}

void Parser::Program()
{
	
	code.Start();
	cout << "Started\n";
	
	Match(BEGIN_SYM);
	ExprRec expr;
	StmtList(expr);
	Match(END_SYM);
	
}

void Parser::SystemGoal()
{
	Program();
	//Match(EOF_SYM);
	code.Finish();
}
