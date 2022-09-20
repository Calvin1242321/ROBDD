#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>


using namespace std;

struct node
{
	char variable;
	int else_edge, then_edge;
	bool redundant;
	bool isomorphic;
};


void obdd(ifstream&);
void initialnode(int, node*);
void simplize(node*, int);
void save(string, string, node*, int);
bool nisredundant(node* n, int size);
bool nisomorphic(node* n, int size);

bool isPowerBy2_(int n)
{
	for (int i = 1; i <= n; i *= 2)
		if (i == n) return true;

	return false;
}


int main(int argc, char* argv[])
{
	if(argc == 2)
	{
		ifstream fin(argv[1]);
		if (!fin)
		{
			cout << "file error!" << endl;
			return -1;
		}
		else
		{
			obdd(fin);
		}

	}
	return 0;
}

void initialnode(int input_n, node* node_n)
{
	for (int i = 0; i < (int)pow(2, input_n) + 1; i++)
	{
		node_n[i].redundant = false;
		node_n[i].isomorphic = false;
	}
	for (int i = 1; i < pow(2, input_n - 1); i++)
	{
		node_n[i].else_edge = i * 2;
		node_n[i].then_edge = i * 2 + 1;
	}
	for (int i = (int)pow(2, input_n - 1); i < (int)pow(2, input_n); i++)	// least significant bit
	{
		node_n[i].else_edge = 0;
		node_n[i].then_edge = 0;
	}
	node_n[0].variable = '0';
	node_n[(int)pow(2, input_n)].variable = '1';
}

void obdd(ifstream& fin)
{
	string str;
	char ch_trash;
	node* node_n = NULL;
	int input_n = 0, output_n = 0;



	// block of '.i' 
	fin >> str;
	fin >> input_n;
	node_n = new node[(int)pow(2, input_n) + 1];
	initialnode(input_n, node_n);

	//block of '.o'
	fin >> str;
	fin >> output_n;
	

	// block of '.ilb'
	fin >> str;
	for (int i = 0; i < input_n; i++)
	{
		fin >> ch_trash;
	}
	
	char ttemp = 'a';

	node_n[1].variable = 'a';
	if(input_n >= 2)
	{
		for (int j = 2;j < 4;j++)
			node_n[j].variable = 'b';
			
	}
	if(input_n >= 3)
	{
		for (int j = 4;j < 8;j++)
			node_n[j].variable = 'c';
			
	}
	if(input_n >= 4)
	{
		for (int j = 8;j < 16;j++)
			node_n[j].variable = 'd';
	}
	if(input_n >= 5)
	{
		for (int j = 16;j < 32;j++)
			node_n[j].variable = 'e';	
	}
		
	

	// block of '.ob'
	fin >> str;
	fin >> ch_trash;

	// block of '.p'
	fin >> str;
	int line_n, tempI;
	bool* bool_index = new bool[(int)pow(2, input_n)];

	fin >> line_n;
	for (int i = 0;i < line_n;i++)
	{
		string tempS;
		fin >> tempS;
		for (int j = 0; j < (int)pow(2, input_n); j++)
		{
			bool_index[j] = false;
		}
		for (int j = 0; j < (int)tempS.length(); j++)
		{
			if (j == 0)
			{
				if (tempS[0] == '1')
				{
					bool_index[node_n[1].then_edge] = true;
				}
				else if (tempS[0] == '0')
				{
					bool_index[node_n[1].else_edge] = true;
				}
				else if (tempS[0] == '-')
				{
					bool_index[node_n[1].else_edge] = true;
					bool_index[node_n[1].then_edge] = true;
				}
			}
			else if (j == ((int)tempS.length() - 1))
			{
				if (tempS[j] == '1')
				{

					for (int k = (int)pow(2, j);k < (int)pow(2, j + 1);k++)
					{
						if (bool_index[k])
						{
							node_n[k].then_edge = (int)pow(2, input_n);
							node_n[k].else_edge = 0;
						}
					}
				}
				else if (tempS[j] == '0')
				{
					for (int k = (int)pow(2, j);k < (int)pow(2, j + 1);k++)
					{
						if (bool_index[k])
						{
							node_n[k].else_edge = (int)pow(2, input_n);
							node_n[k].then_edge = 0;
						}
					}
				}
				else if (tempS[j] == '-')
				{
					for (int k = (int)pow(2, j);k < (int)pow(2, j + 1);k++)
					{
						if (bool_index[k])
						{
							node_n[k].then_edge = (int)pow(2, input_n);
							node_n[k].else_edge = (int)pow(2, input_n);
						}
					}
				}
			}
			else
			{
				if (tempS[j] == '1')
				{

					for (int k = (int)pow(2, j);k < (int)pow(2, j + 1);k++)
					{
						if (bool_index[k])
						{
							bool_index[node_n[k].then_edge] = true;
						}
					}
				}
				else if (tempS[j] == '0')
				{
					for (int k = (int)pow(2, j);k < (int)pow(2, j + 1);k++)
					{
						if (bool_index[k])
						{
							bool_index[node_n[k].else_edge] = true;
						}
					}
				}
				else if (tempS[j] == '-')
				{
					for (int k = (int)pow(2, j);k < (int)pow(2, j + 1);k++)
					{
						if (bool_index[k])
						{
							bool_index[node_n[k].else_edge] = true;
							bool_index[node_n[k].then_edge] = true;
						}
					}
				}
			}
		}
		fin >> tempI;
	}
	
	// block of '.e'
	fin >> str;
	simplize(node_n, (int)pow(2, input_n));
	save("robdd.dot", "robdd", node_n, (int)pow(2, input_n));
}

bool nisredundant(node* n, int size)
{
	bool need_rework = false;
	for (int i = 1; i < size; i++)
	{
		for (int j = i + 1; j < size; j++)
		{
			if (n[i].redundant || n[i].isomorphic)continue;
			if (n[j].redundant || n[j].isomorphic)continue;

			if (n[i].else_edge == n[j].else_edge &&
				n[i].then_edge == n[j].then_edge &&
				n[i].variable == n[j].variable)
			{
				need_rework = true;
				n[j].redundant = true;
				for (int k = 1; k < size; k++)
				{
					if (n[k].else_edge == j)
						n[k].else_edge = i;
					if (n[k].then_edge == j)
						n[k].then_edge = i;
				}
			}
		}
	}
	return need_rework;
}

bool nisomorphic(node* n, int size)
{
	bool need_rework = false;
	for (int i = 1; i < size; i++)
	{
		if (n[i].redundant || n[i].isomorphic) continue;
		if (n[i].else_edge == n[i].then_edge)
		{
			need_rework = true;
			n[i].isomorphic = true;
			for (int j = 1; j < size; j++)
			{
				if (n[j].else_edge == i)
					n[j].else_edge = n[i].else_edge;
				if (n[j].then_edge == i)
					n[j].then_edge = n[i].else_edge;
			}
		}
	}
	return need_rework;
}

void simplize(node* n, int size)
{
	if (nisomorphic(n, size) || nisredundant(n, size))
		simplize(n, size);
}

void save(string fileName, string filetitle, node* n, int size)
{
	ofstream fout(fileName, ofstream::out);
	fout << "digraph " << filetitle << " {" << endl;
	fout << "\t" << "{rank=same 1}\n";
	char tempc = n[1].variable;
	string str_rank = "{rank=same";
	for (int i = 2; i < size; i++)
	{
		if (isPowerBy2_(i))
		{
			tempc = n[i].variable;
			str_rank = "{rank=same";
		}
		if (!n[i].redundant && !n[i].isomorphic)
		{
			if (n[i].variable == tempc)
			{
				if (i < 10)
				{
					char c = (i + '0');
					str_rank = str_rank + ' ';
					str_rank = str_rank + c;
				}
				else if (i >= 10){
					str_rank = str_rank + ' ';
					int t1 = i / 10;
					char c = (t1 + '0');
					str_rank = str_rank + c;
					t1 = i % 10;
					c = (t1 + '0');
					str_rank = str_rank + c;
				}
			}
		}
		if (i == size - 1 || isPowerBy2_(i + 1))
		{
			fout << "\t" << str_rank << "}\n";
			str_rank = "{rank=same";
		}
	}
	fout << "\t0 [label=\"0\", shape=box];" << endl;
	for (int i = 1; i < size; i++)
	{
		if (!n[i].redundant && !n[i].isomorphic)
			fout << "\t" << i << " [label=\"" << n[i].variable << "\"];\n";
	}
	fout << "\t" << size << " [label=\"1\", shape=box];\n";
	fout << endl;
	for (int i = 1; i < size; i++)
	{
		if (!n[i].redundant && !n[i].isomorphic)
		{
			fout << "\t" << i << " -> " << n[i].else_edge << " [label=\"0\", style=dotted]\n";
			fout << "\t" << i << " -> " << n[i].then_edge << " [label=\"1\", style=solid]\n";
		}
	}
	fout << "}";

	fout.close();
}
