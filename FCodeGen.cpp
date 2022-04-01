#include <iostream>
#include <fstream>

using namespace std;

extern ifstream sourceFile;
extern ofstream outFile, listFile;

#include "FScan.h"
#include "FCodeGen.h"


extern Scanner scan; //Global scanner

CodeGen::CodeGen(){
    maxTemp =0;
	intoff = 0;
	fakoff = 0;
	stroff = 0;
	booloff =0;
}
//-------Private-Methods-------
void CodeGen::CheckId(const string & s, ExprRec & t)
{
	if (!LookUp(s,t))  // variable not declared yet
        Enter(s,t.kind);
}
void CodeGen::Enter(const string & s, const ExprKind & t )
{
	//fakes, bools and ints
    //symbolTable.push_back(s);

    Symbol thisSym;
    cout << s << " Enter(s) before pushback symbolTable\n";
    thisSym.label = s;
    thisSym.kind = t;
	
	switch(t){
		case LITERAL_INT:
			thisSym.off = intoff;
			intoff +=2;
			break;
		case LITERAL_FAKE:
			thisSym.off = fakoff;
			fakoff +=4;
			break;
		case LITERAL_BOOL:
			thisSym.off = booloff;
			booloff +=2;
			break;
	}
	
    symbolTable.push_back(thisSym);



    /*switch(t)
    {
        case 0: intTable.push_back(s); break;
        case 1: boolTable.push_back(s); break;
        case 2: fakeTable.push_back(s); break;
        case 3: strTable.push_back(s); break;
        default: break;
    }
    */
}
int CodeGen::getOff(string& s){
	for (unsigned i=0; i< symbolTable.size(); i++){
		if(symbolTable[i].label ==s){
			return symbolTable[i].off;
		}
	}
	return 0;
}
bool CodeGen::LookUp(const string & s,ExprRec & t)
{
	//loop through all symbol tables int bool string fake
    for (unsigned i =0; i < symbolTable.size(); i++)
        if (symbolTable[i].label == s){
			t.kind = symbolTable[i].kind;
			
			return true;
		}
			
    /*for (unsigned i = 0; i < intTable.size(); i++)
        if (intTable[i] = s)
            return true;
    for (unsigned j = 0; j < boolTable.size(); j++)
        if (boolTable[j] = s)
            return true;
    for (unsigned j = 0; j < fakeTable.size(); j++)
        if (fakeTable[j] = s)
            return true;
    for (unsigned j = 0; j < strTable.size(); j++)
        if (strTable[j] = s)
            return true;
      */


	

	return false;
}
void CodeGen::IntToAlpha(int val, string& str)
{
	int k;
	char temp;

	str = "";
	if (val == 0) str = "0";
	while (val > 0)
	{
		str.append(1, (char)(val % 10 + (int)'0'));
		val /= 10;
	}
	k = int(str.length());
	for (int i = 0; i < k/2; i++)
	{
		temp = str[i];
		str[i] = str[k-i-1];
		str[k-i-1] = temp;
	}
}
void CodeGen::FakeToAlpha(float val, string& str)
{

    str = "";

    cout << "\nFakeToAlpha\n";

    string fakeStr(std::to_string(val));

    str = fakeStr;
    cout << str;



}
void CodeGen::ExtractExpr(const ExprRec & e, string& s){
	string t;
	int k, n;

	switch (e.kind)
	{
	case ID_EXPR:
		//cout<<"id expr";
	case TEMP_EXPR:  // operand form: +k(R15)
		
		s = e.name;
		n = 0;
		//cout<<"temp";
        while (symbolTable[n].label != s) n++;
		k = 2 * n;  // offset: 2 bytes per variable
		IntToAlpha(k, t);
		s = "+" + t + "(R15)";
		break;
	case LITERAL_INT:
		cout <<"lit int\n";
		IntToAlpha(e.val, t);
		s = "#" + t;
		break;
	case LITERAL_STR:
		s ="+" + to_string(StringSamDistance(stringTable.size()-1)) +"(R13)";
		//cout<<"str";
		break;
		//s = "+" + to_string(StringSamDistance(stringTable.size()-1))+ "(R14)";
		//s ="+" + to_string(StringSamDistance(stringTable.size()-1)) +"(R14)";
	//WIP case for bool and fakes
    case LITERAL_BOOL:
		IntToAlpha(e.val,t);
		s = "#" + t;
        break;
    case LITERAL_FAKE:
        cout << "---literal fake\n";
        //FakeToAlpha(e.valF, t);
		//get fake table symbol offset.
		IntToAlpha((fakeTable.size()-1)*4,s);
        //s = "+" + s + "(R14)";
        break;
	default:
		cout<<"--Default";
		break;
	}
}
void CodeGen::Generate(const string & s1, const string & s2, const string & s3){
	
	listFile.width(20);
	listFile << ' ' << s1;
	outFile << s1;
	if (s2.length() > 0)
	{
		listFile << s2;
		outFile << s2;
		if (s3.length() > 0)
		{
			listFile << ',' << s3;
			outFile << ',' << s3;
		}
	}
	listFile << endl;
	outFile << endl;
}
string CodeGen::ConvertToSam(string s){
	string tmp ="\"";
	for(int i =0; i<s.length(); i++){
		if(s[i]=='"' || s[i]==':'){
			tmp+=":";
		}
		tmp += s[i];
	}
	tmp +="\"";
	return tmp;
}
int CodeGen::StringSamDistance(int index){
	if(index ==0){
		return 0;
	}
	int tmp =0;
	for(int i=0; i<index; i++){
		tmp += stringTable[i].length();
		if(tmp%2==0){
			tmp+=2;
		}else{
			tmp+=1;
		}
	}
	stringTable[index];
	return tmp;
}
//--------Public-Methods--------
void CodeGen::Start()
{
	Generate("LDA		", "R15", "INTS");
	Generate("LDA		", "R14", "FAKES");
	Generate("LDA		", "R13", "STRS");
	Generate("LDA		", "R12", "BOOLS");
	//WIP
	//Generate load address for STRS BOOLS fakes
	//R14 , R13, R12
}
void CodeGen::Finish()
{
	string s;
	listFile.width(6);
	listFile << ++scan.lineNumber << " " << scan.lineBuffer << endl;
	Generate("HALT		", "", "");
	//repeat this chunk for fakes, bools, strings
	//keep in mind the differnt sizes of registers
	Generate("LABEL	", "INTS","");
	int tmpSize =0;
	for(int i=0; i< symbolTable.size(); i++){
		if(symbolTable[i].kind == LITERAL_INT) tmpSize +=1;
	}
    IntToAlpha(int(2*(tmpSize)),s);
	Generate("SKIP	", s, "");
	//WIP Need tables for str bool and fake
    Generate("LABEL	", "BOOLS","");
	tmpSize =0;
	for(int i=0; i< symbolTable.size(); i++){
		if(symbolTable[i].kind == LITERAL_BOOL) tmpSize +=1;
	}
    IntToAlpha(int(2*(tmpSize)),s);
	Generate("SKIP	", s, "");

    Generate("LABEL	", "FAKES", "");
	for(int i=0; i< fakeTable.size(); i++){
		Generate("REAL	",fakeTable[i],"");
	}
	tmpSize =0;
	for(int i=0; i< symbolTable.size(); i++){
		if(symbolTable[i].kind == LITERAL_FAKE) tmpSize +=1;
	}
    IntToAlpha(int(4*(tmpSize)),s);
    Generate("SKIP	", s, "");
	
	Generate("LABEL	", "STRS", "");
    //IntToAlpha(int(4*(fakeTable.size()+1)),s);
    //for loop for stings
	for(int i=0; i< stringTable.size(); i++){
		Generate("STRING	",ConvertToSam(stringTable[i]),"");
	}

	outFile.close();



    listFile << endl << endl;
    listFile << " _-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_\n";
    listFile << " ||||||||   S Y M B O L   T A B L E   ||||||||\n" << endl;
    listFile << " _____________________________________________" << endl;
    listFile << " Relative" << endl;
    listFile << " Address      Identifier" << endl;
    listFile << " --------     --------------------------------" << endl;

    int curOffSet = 0;

    for (int j = 0; j < 4; j++)
    {
        switch(j)
        {
          case 0: listFile << " INTS----" << endl; break;
          case 1: listFile << " BOOLS---" << endl; break;
          case 2: listFile << " FAKES---" << endl; break;
          case 3: listFile << " STRINGS-" << endl; break;
        }

        for (unsigned i = 0; i < symbolTable.size(); i++)
        {
            switch(j)
            {
            case 0: if (symbolTable[i].kind == LITERAL_INT)
                    {
                        listFile.width(7);
                        listFile << curOffSet << "        " << symbolTable[i].label << endl;
                        curOffSet += 2;
                    }
                    break;
            case 1: if (symbolTable[i].kind == LITERAL_BOOL)
                    {
                        listFile.width(7);
                        listFile << curOffSet << "      " << symbolTable[i].label << endl;
                        curOffSet += 2;
                    }
                    break;
            case 2: if (symbolTable[i].kind == LITERAL_FAKE)
                    {
                        listFile.width(7);
                        listFile << curOffSet << "      " << symbolTable[i].label << endl;
                        curOffSet += 4;
                    }
                    break;
            case 3: if (symbolTable[i].kind == LITERAL_STR)
                    {


                    listFile << "        " << symbolTable[i].label << endl;
                        //figure string offset here
                    }
                    break;
            default: break;
                        }

           // if ( symbolTable[i].kind == )
           // listFile.width(7);
           // listFile << 2*i << "       " << symbolTable[i].label << endl;
        }

    }
    listFile << " _-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_" << endl;
    listFile << endl;
    listFile << " Normal successful compilation." << endl;

    listFile.close();


}
void CodeGen::Assign(const ExprRec & target, const ExprRec & source)
{
//needs to check if its an int fake boolean or string
	string s,id;
	int tmp;
	string s2;
	switch(source.kind){
		case LITERAL_INT:
			cout<< "assign int";
			ExtractExpr(source, s);
			Generate("LD		", "R0", s);
			ExtractExpr(target, s);
			id =target.name;
			tmp = getOff(id);
			IntToAlpha(tmp,id);
			cout << tmp;
			Generate("STO		", "R0", "+"+id+"(R15)");
			break;
		case LITERAL_FAKE:
			cout<<"assign fake";
			ExtractExpr(source, s);
			s = "+" + s + "(R14)";
			Generate("LD		", "R0", s);
			ExtractExpr(target, s);
			IntToAlpha(stoi(s)+2,s);
			s = "+" + s + "(R14)";
			Generate("LD		", "R1", s);
			break;
		case LITERAL_BOOL:
			ExtractExpr(source, s);
			Generate("LD		", "R0", s);
			ExtractExpr(target, s);
			id =target.name;
			tmp = getOff(id);
			IntToAlpha(tmp,id);
			cout << tmp;
			Generate("STO		", "R0", "+"+id+"(R12)");
			break;
	}
	
	
}
void CodeGen::ReadValue(const ExprRec & InVal)
{

}
void CodeGen::ProcessVariable()
{

}
void CodeGen::WriteExpr(const ExprRec & outExpr)
{
	//WIP .kind of lit str bool and fake
	
	if(outExpr.kind == LITERAL_STR){
		//wrtie string
		string s;
		ExtractExpr(outExpr,s);
		Generate("WRST	", s,"");
	}
	if(outExpr.kind == TEMP_EXPR){
		cout <<"write id";
	}
	//string s;
	//ExtractExpr(outExpr,s);
	//cout << s;
	if(outExpr.kind == LITERAL_INT){
		string s;
		
		ExtractExpr(outExpr, s);
		Generate("WRI	", s, "");
	}
    if (outExpr.kind == LITERAL_FAKE){
        string f,s;
        //cout << "/nWriteExpr .kind = LITERAL_FAKE\n";
        ExtractExpr(outExpr, f);
			f = "+" + f + "(R14)";
			IntToAlpha(stoi(f)+2,s);
			s = "+" + s + "(R14)";

        Generate("WRF		", f+","+ s , "");
    }
    if (outExpr.kind == LITERAL_BOOL){
        string f;
        cout << "/nWriteExpr .kind = LITERAL_BOOL\n";
        Generate("WRI       ", "", "");
    }

	
}
void CodeGen::NewLine()
{
	Generate("WRNL		", "", "");
}
//---fif---
void CodeGen::ProcessIf()
{

}
void CodeGen::ProcessEndIf()
{

}
void CodeGen::ProcessElse()
{

}
//---do-while---
void CodeGen::ProcessDo()
{

}
void CodeGen::ProcessEndFwhile()
{

}
//---while---
void CodeGen::ProcessWhile()
{

}
void CodeGen::ProcessWhileCond()
{

}
void CodeGen::ProcessEndWhile()
{

}
//---for-loop---
void CodeGen::InitLoopCtrl()
{

}
void CodeGen::ProcessForCond()
{

}
void CodeGen::LoopUpdate()
{

}
void CodeGen::ProcessEndFor()
{

}
//---Conditions---
void CodeGen::SetOrCond()
{

}
void CodeGen::SetAndCond()
{

}
void CodeGen::SetNotCond()
{

}
//---Ops---
void CodeGen::ProcessRelOp()
{

}
void CodeGen::SetRelCond()
{

}
void CodeGen::ProcessOp(OpRec& o)
{
	string s = scan.tokenBuffer;
	if(s =="+") o.op=PLUS;
	else if (s=="-") o.op =MINUS;
	else if (s=="*") o.op =MULT;
	else if (s=="/") o.op =DIV;
	else if (s=="%") o.op =MOD;
	
}
string CodeGen::GetTemp(){
	string s;
	static string t;
	t = "Temp&";
	IntToAlpha(++maxTemp, s);
	t += s;
	ExprRec tmp;
	tmp.kind = LITERAL_INT;
    CheckId(t, tmp);  // forcing TEMP_EXPR for quick fix
	return t;
}
string ExtractOp(const OpRec& o){
	//needs it for floats and floats and ints
	switch(o.op){
		case PLUS:
			return "IA		";
		case MINUS:
			return "IS		";
		case MULT:
			return "IM		";
		case DIV:
			return "ID		";
		default:
			return "IA		";
		//WIP
		//find sam instructions for mul div mod
	}	

}
void CodeGen::GenInfix(const ExprRec & e1, const OpRec & op, const ExprRec & e2, ExprRec& e)
{
	string s;
	string tmp;
	if(e1.kind == ID_EXPR && e2.kind == ID_EXPR){
		e.kind = ID_EXPR;
		switch(op.op){
			case PLUS:
				e.val = e1.val + e2.val;
				break;
			case MINUS:
				e.val = e1.val - e2.val;
			case MULT:
				e.val = e1.val * e2.val;
			case DIV:
				e.val = e1.val / e2.val;
			case MOD:
				e.val = e1.val % e2.val;
		}
	}else{
		e.kind = TEMP_EXPR;
		e.name = GetTemp();
		ExtractExpr(e1,s);
		Generate("LD		","R0",s);
		
		ExtractExpr(e2, s);
		tmp = ExtractOp(op);
		cout<< tmp<<"here\n";
		
		Generate(tmp, "R0", s);
		ExtractExpr(e, s);
		Generate("STO       ", "R0", s);

	}
}

void CodeGen::ProcessMulOp()
{

}
void CodeGen::ProcessId(ExprRec& e)
{
	
	CheckId(scan.tokenBuffer,e);
	//e.kind = ID_EXPR;
	//cout << scan.tokenBuffer;
	//e.name = scan.tokenBuffer;
}
void CodeGen::ProcessLiteralInit(ExprRec& e)
{

}


void CodeGen::ProcessLiteral(ExprRec& e)
{
	string s;
	switch(e.kind){
		case LITERAL_INT:
			cout << "process lit int\n";
			//cout << scan.tokenBuffer.data();
			e.kind = LITERAL_INT;
			e.val = atoi(scan.tokenBuffer.data());
			cout << e.val;
			
			//cout << e.val;
			break;
		case LITERAL_STR:
			//push to string table.
			e.kind = LITERAL_STR;
			stringTable.push_back(scan.tokenBuffer.data());
			//ExtractExpr(e,s);
			//Generate("WRST	", s,"");
			break;
		case LITERAL_BOOL:
			e.kind = LITERAL_BOOL;
			cout<<scan.tokenBuffer.data();
			
			if(scan.tokenBuffer.data()=="yay"){
				e.val = 1;
				
			}else{
				e.val =0;
				cout<<"true";
			}
			
			break;
		case LITERAL_FAKE:
			cout<<"process fake";
            //cout << scan.tokenBuffer.data()<<"\n";
			e.valF = std::stof(scan.tokenBuffer.data());
            //e.valF = atoi(scan.tokenBuffer.data());
			e.kind = LITERAL_FAKE;
			fakeTable.push_back(scan.tokenBuffer.data());
            cout << e.valF;
			break;
		case ID_EXPR:
			cout << "expr";
			break;
		case TEMP_EXPR:
			cout << "temp expr";
			break;
		default:
			cout <<"default";
			break;
	}
	
}
/*
void CodeGen::ProcessStringLiteral(ExprRec& e)
{

}
*/
