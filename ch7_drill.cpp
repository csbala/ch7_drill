#include "std_lib_facilities.h"

constexpr char number = '8';
constexpr char quit = 'q';
constexpr char print = ';';
constexpr char result = '=';
constexpr char let = 'L';
constexpr char name = 'a';
const string declkey = "let";
constexpr char square_root = '@';
constexpr char cpow = 'P';
const string sqrtkey = "sqrt";
const string powkey = "pow";
//ezek az olvashatoság miat jók, jobban átlátható a program



//a sorrend miatt néhol nincsnek deklarálva  az értékek
//prototipusok:
double expression();
double term();
double primary();
double define_name();	
double declaration();
double statement();
double calc_sqrt();
double calc_pow();

class Variable{
public:
	string name;
	double value;
};

vector<Variable> var_table;

bool is_declared(string var)
{
	for(const auto& v: var_table){
		if(v.name==var) return true;
	}
return false;
}

double get_value(string s){//*
for(const auto& v: var_table)
	if(v.name==s) return v.value;

error("get: Variable not defined"); ///végig megyünk a vektoron a változóért, ha nincs benne hibát dob ki
return -1;
}

void set_value(string s, double d){
for(auto& v: var_table){
	if(v.name==s) 
	{
		v.value=d;
		return;
	}
 error("get: Variable not defined");///végig megyünk a vektoron a változóért, ha nincs benne hibát dob ki
	
}

}

class Token{
public:
	char kind;
	double value;
	string name;
	Token(): kind(0) {} //alap konstruktor
	Token(char ch): kind(ch), value(0) {}//konstruktór fügvények
	Token(char ch, double val): kind(ch), value(val) {}
	Token(char ch, string n): kind(ch), name(n) {}
};

class Token_stream{
public:
	Token_stream();
	Token get();
	void putback(Token t);//a számok visszatevéséhez
	void ignore(char c);//prototipus
private:
	bool full; //van e változoba valami vagy sem?
	Token buffer;

};

Token_stream::Token_stream(): full(false), buffer(0) {} //osztályon kivűl deklaráljuk[ez a Token_stream fügyvény a Token_strem osztályon belűl van]

void Token_stream::putback(Token t)//a t az amit paraméternek adunk
{		
	if(full) error("Token_stream buffer is full");//már van a bufferbe
	buffer = t; //majd a buffert rakjuk vissza 
	full=true;  //a buffer tele van
}
//legyen a token strem get fügvénye
Token Token_stream::get()
{
	if(full)
	{
		full=false;//ki fogjuk üriteni a buffert tehát visszaálitjuk false-ra
		return buffer; 
	}

 	char ch;
 	cin>>ch;//it olvassuk a bemenetet
 	switch(ch)
 	{
 		case print:
 		case quit:
 		case '=':
 		case '(':
 		case ')':
 		case '+':
 		case '-':
 		case '*':
 		case '/':
 		case '%':
 			return Token(ch); //az operátór megjelenik a kind-ba, [a konstruktor felépiti a t-ét, ilyenkor mivel nincs szám value=0 a kind pedig az operátor]
		case '.':
		case '0': case '1': case '2': case '3': case '4': 
		case '5': case '6': case '7': case '8': case '9':			
		{
			cin.putback(ch);//a karaktert amit kivettük hogy micsoda visszatesszük így nagyobb számokat egybe tudjuk olvasni
			double val=0;
			cin>>val;
			return Token(number,val);
		}
		default:
		{
			if(isalpha(ch)){//ha betű akkor igazzal tér vissza ha szám akor hamis
				string s;
				s += ch;
				while(cin.get(ch) && (isalpha(ch) || isdigit(ch))) s +=ch; ///létrehozza a hosszabb sztingeket pl: let, sqrt.pow
				cin.putback(ch);//vissza kell tennünk a nagyob számok miatt
				if(s == declkey) return Token{let}; //a statementbe ezt fogjuk vizsgálni
				else if(s == sqrtkey) return Token{square_root};
				else if(s == powkey) return Token{cpow};
				else if(is_declared(s))
					return Token(number, get_value(s));
			
				return Token{name,s};
				
			}
			error("Bad tocken");
			//return 0;
		}
		
		
 	}
 return -1;
}

void Token_stream::ignore(char c)
{
	//de a void strembe van egy buffer
	if(full && buffer.kind == c)
	{
		full=false;//így kiűritjük a buffert ha volt benne ';'
		return;
	}

	full=false;

  char ch=0;
  while(cin>>ch)
  	if(ch==c) return;//(nincs érteék mert void), addig olvasunk mig c nem kapunk tehát quitet aminek értéke ';'
  

}


Token_stream ts;//globális változo 



double expression()
{
	double left= term(); //expression rögtön hivja a term fügvényt,a  nyelvtani szabály miatt[megnézi a baloldalt,hogy *,/,% van e]
	Token t = ts.get();//Tokenekre szedi
	while(true) //expression feladata itt lesz, az összeadás és kivonás[kifejezés végéig megy a while ciklus]
	{
		switch(t.kind){
			case '+':
				left += term();//a jobb oldalra is meghivja a term fügvényt
				t =  ts.get();
				break;
			case '-':
				left -= term();//a jobb oldalra is meghivja a term fügvényt
				t =  ts.get();
				break;
			default:
			ts.putback(t);
			return left;//visszaadja az végeredményt az int mainbe 



		}
	}
}

double term()
{
	double left = primary();//a term röftön hivja a primary fügyvényt[zárójel,szám van e]
	Token t =ts.get();
	while(true)//a term itt csinálja a feladatát ami: szorzás, osztás, modulós osztás[kifejezés végéig megy a while ciklus]
	{
		switch(t.kind){
			case '*':
			left *=primary();//a jobb oldalra hivja meg a primary fügvényt
			t = ts.get();
			break;
			case '/':
			{//deklarálás miatt kell a zárójel(mindig ki lehet tenni,de általába felesleges lenne,ide kell)
			double d =primary();
			if(d==0) error("divide by zero");//ellenőrizük hogy a primary nem e tért e vissza nullával az osztásnál 
			left /=d; //itt nem a primaryit hivtuk meg, mert már meghivtuk a d-be nem kell meghini mégegyszer mert ez csak lasitja a programot
			t = ts.get();
			break;
			}
			case '%':
			{
			double d =primary();
			if(d==0) error("divide by zero");
			left = fmod(left, d);//fmod a maradekos osztas
			t = ts.get();
			break;
			}
			default:
			ts.putback(t);
			return left; //visszaküldi az eredményt az expressionbe
		}

	}	


}


double primary()
{
	Token t = ts.get();//megnézi mi következő token
	switch(t.kind)
	{
		case '('://ha zárojel van hivjuk az expression fügvényt: '('Expression')'
		{
			double d = expression();
			t = ts.get();
			if(t.kind != ')')error("')' expected"); //nincs bezáró zárójel
			return d;
		}
		case number:
			return t.value; //token értékét adja vissza
		case '-':
			return - primary();//a primary negatívba fogja visszaadni
		case '+':
			return primary();
		case square_root: ///sqroot(9)
			return calc_sqrt();
		case cpow: 
			return calc_pow();
		default:
		error("Primary expected");//se nem szám sem nem zárójel érkezett
		return 0;
	}
}

void clean_u_mess(){
	ts.ignore(print);//a pontos vesszőig törlünk/ignorálunk

}

double calc_pow(){
	char ch;
	if(cin.get(ch) && ch != '(') error("'(' expected");
	//cin.putback(ch);
	double d=0;
	double d2=0;
	Token t = ts.get();
	d=t.value;
	if(cin.get(ch) && ch != ',') error("',' expected");
	Token t2 = ts.get();
	d2=t2.value;
	if(cin.get(ch) && ch != ')') error("')' expected");
	d=pow(d,d2);
	return d;
}

double calc_sqrt(){
	char ch;
	if(cin.get(ch) && ch != '(') error("'(' expected");
	cin.putback(ch);
	double d = expression();
	if(d<0) error("sqrt: negative value");
	return sqrt(d);
}


void define_name(string var,double val)
{
	if(is_declared(var)) error("Variable is declared:", var);
	Variable y;
	y.name=var;
	y.value=val;
	var_table.push_back(y);
	//return val;

}

double declaration()
{
	Token t = ts.get();
	if (t.kind != name) error("name expected in declaration");//ha nem név
	string var_name = t.name;
	
	Token t2 = ts.get();
	if(t2.kind != '=') error("= expected in declaration");
	
	double d = expression();
	define_name(var_name,d);
	return d;
}

double statement()//deklaráció e vagy sem
{
	Token t=ts.get();
	switch(t.kind){
		case let:
			return declaration(); 
		default:
			ts.putback(t);
			return expression();
	}

}

void calculate(){

	while(cin)
	try{
		Token t = ts.get();

		while(t.kind == print) t=ts.get();
				if(t.kind==quit) return;//viszatérünk a mainbe és így kilép			
			
				ts.putback(t);
				cout<< result << statement() << endl;

	}catch( exception& e){
		cerr << e.what() << endl;
		clean_u_mess();
}
}

int main()
try{
	cout<<"Welcome to our simple calculator.\n";
	cout<<"Please enter expressions using floa ting-point numbers.\n";

 	calculate();

 	return 0;

}catch( exception& e){
	cerr << e.what() << endl;
	return 1;
}catch(...){
 cerr <<"exeption\n";
 return 2;
}





