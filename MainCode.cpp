#include<bits/stdc++.h>
using namespace std;

set<string> terminals,non_terminals;
map <string , list <string> > grammar;
map <string , set <string> > first,follow;
map <string , map <string, pair<string,string> > > table;

pair<string,int> tokenize(string s,int pos,char ch)
{
	int i,flag=0;
	string str;
	for(i=pos; (s[i] != ch || flag)  && i<s.length(); i++)
	{
		if(s[i]=='/' && s[i+1] =='/')	return make_pair(str,-1);
		if(s[i]!='\t' && s[i]!=' ')
			str+=s[i];
		if(s[i]=='"')	flag^=1;
	}
	return make_pair(str,i+1);
}

void print(map<string,set <string> > code)
{	
	for(map<string,set <string> > ::iterator i=code.begin();i!=code.end();i++)
	{
		cout<<i->first<<" ->  ";
		for(set <string> ::iterator j=(i->second).begin(); j!=(i->second).end() && j!=--(i->second).end(); j++)
		{
			cout<<*j<<" | ";
		}
		cout<<*(--(i->second).end());
		cout<<endl;
	}
}

void print(map<string,list <string> > code)
{	
	for(map<string,list <string> > ::iterator i=code.begin();i!=code.end();i++)
	{
		cout<<i->first<<" ->  ";
		for(list <string> ::iterator j=(i->second).begin(); j!=(i->second).end() && j!=--(i->second).end(); j++)
		{
			cout<<*j<<" | ";
		}
		cout<<*(--(i->second).end());
		cout<<endl;
	}
}

void print(map<string,map<string,pair<string,string> > > table)
{
	for(map<string,map<string, pair<string,string> > >::iterator i=table.begin();i!=table.end();i++)
	{
		cout<<(i)->first<<"->>	";
		for(map<string,pair<string,string> > ::iterator j=(i->second).begin();j!=(i->second).end();j++)
		{
			cout<<j->first<<":<"<<(j->second).first<<" , "<<(j->second).second<<">	";
		}
		cout<<endl;
	}
}

pair<string, bool> checkLR(string b, string a)				//Check left Recursive
{
	pair<string, bool> temp;
	temp = make_pair("", false);
	string c = b.substr(0, a.size());
	if(c == a)
	{
		temp.second = true;
		temp.first = b.substr(a.size(), b.size()- a.size());
	}
	return temp;
}

void  evaluateFirst(string non_terminal,set<string> terminals,set<string> non_terminals)	//Evaluate First
{
	string str;
	vector<string> s;
	s.push_back(non_terminal);
	while(!s.empty())
	{
		string token=s.back();
		s.pop_back();
		for(list<string>::iterator itr=grammar[token].begin();itr!=grammar[token].end();itr++)
		{
			if(token == non_terminal)
			{
				str=*itr;
			}
			for(int i=1;i<=(*itr).length();i++)
			{
				string temp=(*itr).substr(0,i);
				if(non_terminals.find(temp)!=non_terminals.end())
				{
					s.push_back(temp);break;
				}
				if(terminals.find(temp)!=terminals.end())
				{
					first[non_terminal].insert(temp);
					table[non_terminal][temp]=make_pair(non_terminal,str);
					break;
				}
			}
		}
	}
}

void evaluateFollow(set<string> terminals,set<string> non_terminals)			//Evaluates Follow Pos
{
	for(map<string, list<string> >::iterator itr=grammar.begin();itr!=grammar.end();itr++)
	{
		for(list<string> ::iterator ptr=itr->second.begin();ptr!=itr->second.end();ptr++)
		{
			for(int i=(*ptr).length()-1;i>0;)
			{
				bool jflag=false;
				for(int j=i;j>=0 && !jflag;j--)
				{
					if(terminals.find((*ptr).substr(j,i-j+1))!=terminals.end())
					{
						for(int k=j;k>=0;k--)
						{
							if(non_terminals.find((*ptr).substr(k,j-k))!=non_terminals.end())
							{
								follow[(*ptr).substr(k,j-k)].insert((*ptr).substr(j,i-j+1));
								i=j-1;
								jflag=true;
								break;
							}
							else if(terminals.find((*ptr).substr(k,j-k))!=terminals.end())
							{
								i=j-1;
								jflag=true;
								break;
							}
							else if(k==0)
							{
								i=j-1;
								jflag=true;
							}
						}
					}
					if(i==(*ptr).length()-1 && non_terminals.find((*ptr).substr(j,i-j+1))!=non_terminals.end())
					{
						for(set<string> ::iterator it=first[itr->first].begin();it!=first[itr->first].end();it++)
						{
							follow[(*ptr).substr(j,i-j+1)].insert(*it);
						}
					}
					if(non_terminals.find((*ptr).substr(j,i-j+1))!=non_terminals.end())
					{
						for(int k=j;k>=0;k--)
						{
							if(terminals.find((*ptr).substr(k,j-k))!=terminals.end())
							{
								i=j-1;
								jflag=true;
								break;
							}
							else if(non_terminals.find((*ptr).substr(k,j-k))!=non_terminals.end())
							{
								for(set<string> ::iterator it=first[(*ptr).substr(j,i-j+1)].begin();it!=first[(*ptr).substr(j,i-j+1)].end();it++)
								{
									follow[(*ptr).substr(k,j-k)].insert(*it);
								}
								i=j-1;
								jflag=true;
								break;
							}
							else if(k==0)
							{
								i=j-1;
								jflag=true;
							}
						}
					}
					//if(jflag)	i=j-1;
				}
			}
		}
	}
}

int main()
{
	//extract grammar from file
	fstream fin;
	string line;
	fin.open("mygrammar.txt",ios::in);
	while(getline(fin,line))
	{
		int i=0;
		pair<string,int> terminal=tokenize(line,i,':');
		i=terminal.second;
		if(i== -1)	continue;
		i+=2;
		while(i<line.length())
		{
			pair <string,int> tokens=tokenize(line,i,'|');
			grammar[terminal.first].push_back(tokens.first);
			if(tokens.second== -1)	break;
			i=tokens.second;
		}
	}
	
	//remove left recursion
	for(map<string,list<string> > ::iterator itr=grammar.begin();itr!=grammar.end();itr++)
	{
		bool flag=false;
		for(list<string> ::iterator i=itr->second.begin();i!=itr->second.end();i++)
		{
			pair<string,bool> token=checkLR(*i,itr->first);
			if(token.second==true)
			{
				flag=true;
				grammar[(itr->first).substr(0,(itr->first).size()-1) +"'>"].push_back(token.first+(itr->first).substr(0,(itr->first).size()-1) +"'>");
			}
		}
		if(flag)
		{	
			grammar[(itr->first).substr(0,(itr->first).size()-1) +"'>"].push_back("Epsilon");
			for(list<string> ::iterator i=itr->second.begin();i!=itr->second.end();i++)
			{
				pair<string,bool> token=checkLR(*i,itr->first);
				if(token.second==false)
				{
					list<string> ::iterator it=++i;--i;
					replace(i,it,*i,(*i)+ (itr->first).substr(0,(itr->first).size()-1) +"'>");
				}
			}
			for(list<string> ::iterator i=itr->second.begin();i!=itr->second.end();)
			{
				pair<string,bool> token=checkLR(*i,itr->first);
				if(token.second==true)
				{
					itr->second.erase(i++);
				}
				else i++;
			}
		}
	}
	cout<<"\n------Grammar------\n";print(grammar);
	
	//generate first follow
	for(map<string,list<string> > ::iterator itr=grammar.begin();itr!=grammar.end();itr++)
	{
		non_terminals.insert(itr->first);
	}
	for(map<string,list<string> > ::iterator itr=grammar.begin();itr!=grammar.end();itr++)
	{
		for(list<string> ::iterator i=itr->second.begin();i!=itr->second.end();i++)
		{
			bool flag=false;
			string temp;
			for(int j=0;j<(*i).size();j++)
			{
				if((*i)[j]=='"')	flag=!flag;
				if(flag)	temp+=(*i)[j];
				else
				{
					terminals.insert(temp+'"');
					temp.clear();
				}
			}
		}
	}
	terminals.erase("\"");
	terminals.insert("Epsilon");
	for(map<string,list<string> > ::iterator itr=grammar.begin();itr!=grammar.end();itr++)
	{
		for(list<string> ::iterator i=itr->second.begin();i!=itr->second.end();i++)
		{
			if(terminals.find(*i)!=terminals.end())	first[itr->first].insert(*i);
		}
	}
	for(map<string,list<string> > ::iterator itr=grammar.begin();itr!=grammar.end();itr++)
	{
		evaluateFirst(itr->first,terminals,non_terminals);
	}
	cout<<"\n------First------\n";print(first);
	evaluateFollow(terminals,non_terminals);
	cout<<"\n------Follow------\n";print(follow);
	
	//generate LL1 parse table
	for(map<string,set<string> > ::iterator itr=first.begin();itr!=first.end();itr++)
	{
		for(set<string> ::iterator ptr= (itr->second).begin();ptr!=(itr->second).end();ptr++)
		{
			if(*ptr == "Epsilon")
			{
				for(set <string> :: iterator jtr= follow[itr->first].begin();jtr != follow[itr->first].end();jtr++)
				{
					table[itr->first][*jtr]=make_pair(itr->first,"Epsilon");
				}
			}
		}
	}
	cout<<"\n\nParse Table\n";print(table);
	
	//take tokenized C code
	fstream file2;
	file2.open("output_program");
	ofstream f3;
	f3.open("output");

	std::vector<string> program;

	while ( getline (file2,str) ) 
	{
		program.push_back(str);
	}

	//parse the C code
	
	program.push_back("$");
	vector<string> stck;
	stck.push_back("$");
	stck.push_back(non_terminals[0]);
	int i=0, err = 0;
	while(i < production.size() && stck.size() > 0)
	{
		pair<string, int> term = make_pair("", 0);
		term = tokenize(program[i], term.second, ' ');
		do
		{
			f3 << "\nToken to check = "<< term.first << " " << endl;
			string y = stck.back();
			f3 << "Top of stack = " << y << endl;
			stck.pop_back();
			if(check_terminal(y) || y[0] == '$')
			{
				if(y == term.first)
				{
					term = tokenize(program[i], term.second, ' ');
					if(term.second == -2 )
					{
						break;
					}
					continue;
				}
				else
				{
					err=1;
					cout<<"Error\n";
					break;
				}

			}
			int j,k;
			for( k=0; k<terminals.size(); k++)
				if(terminals[k] == term.first)
					break;
			for( j=0; j<non_terminals.size(); j++)
				if(non_terminals[j] == y)
					break;
			string z = parser_table[j][k];
			if(z == " ")
			{
				err=1;
				break;
			}
			if(z == "Epsilon")
				continue;
			string ss="";
			std::vector<string> temp;
			for(j= 0; j < z.size(); j++)
			{
				if(z[j] == ' ')
				{
					temp.push_back(ss);
					ss = "";
				}	
				else
					ss += z[j];
			}
			temp.push_back(ss);
			for(j= temp.size()-1; j>=0; j--)
			{
				stck.push_back(temp[j]);
			}	
		}while(term.second != -2);
		i++;
		if(err)
			break;
	}
	
	if(!err)
		f3<<"\nINPUT C PROGRAM IS CORRECT\n";
	else
		f3<<"\nINPUT C PROGRAM IS INCORRECT\n";
	
	//close all open files
	file.close();
	file2.close();
	cout<<"\n\n-------finish----------";
	return 0;
}
