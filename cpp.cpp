#include "hpp.hpp"
#define YYERR "\n\n"<<yylineno<<":"<<msg<<"["<<yytext<<"]\n\n"
void yyerror(string msg) { cout<<YYERR; cerr<<YYERR; exit(-1); }
int main() { env_init(); return yyparse(); }

Sym::Sym(string T, string V) { tag=T; val=V; }
Sym::Sym(string V):Sym("",V) {}
void Sym::push(Sym*o) { nest.push_back(o); }

string Sym::str() { return val; }
string Sym::tagval() { return "<"+tag+":"+val+">"; }
string Sym::pad(int n) { string S; for (int i=0;i<n;i++) S+='\t'; return S; }
string Sym::dump(int depth) { string S = "\n"+pad(depth)+tagval();
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
		S += (*it)->dump(depth+1);
	return S; }

Sym* Sym::eval() { 	
	Sym*E = env[val]; if (E) return E;
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
		(*it) = (*it)->eval();
	return this; }

Sym* Sym::eq(Sym*o) { env[val]=o; return this; }
Sym* Sym::at(Sym*o) { push(o); return this; }

Str::Str(string V):Sym("str",V) {}
string Sym::tagstr() { return "<"+tag+":'"+val+"'>"; }
string Str::tagval() { return tagstr(); }

List::List():Sym("[","]") {}

Op::Op(string V):Sym("op",V) {}
Sym* Op::eval() { Sym::eval();
	if (val=="=") return nest[0]->eq(nest[1]);
	if (val=="@") return nest[0]->at(nest[1]);
	return this;
}

Fn::Fn(string V,FN F):Sym("fn",V) { fn=F; }
Sym* Fn::at(Sym*o) { return fn(o); }

File::File(string V):Sym("file",V) {}
Sym* File::file(Sym*o) { return new File(o->str()); }
string File::tagval() { return tagstr(); }

map<string,Sym*> env;
void env_init() {
	env["file"] = new Fn("file",File::file);
}

