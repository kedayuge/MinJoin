#include "hash.h"
#include "minjoin.h"

int hashbit;
int hashmask;

int dictsize = 37;
unordered_map<char, int> dict;

void builddict()
{
	if (dictsize == 5)
	{
		unordered_map<char, int> dict1({ { 'A', 0 }, { 'C', 1 }, { 'G', 2 }, { 'T', 3 }, { 'N', 4 } });
		dict = dict1;
	}
	else if (dictsize == 26)
	{
		for (int i = 0; i < 26; ++i)
		{
			dict['A' + i] = i + 1;
			dict['a' + i] = i + 1;
		}
	}
	else
	{
		for (int i = 0; i < 26; ++i)
		{
			dict['A' + i] = i + 1;
			dict['a' + i] = i + 1;
		}
		for (int i = 0; i < 10; ++i)
			dict['0' + i] = i + 27;
		dict[' '] = 37;
	}
}

void initiHash()
{
	hashbit = ceil(log2(dictsize));
	hashmask = 0;
	for (int i = 0; i < hashbit; ++i)
	{
		++hashmask;
		hashmask <<= 1;
	}
	hashmask <<= (hashbit * (W - 1));
	hashmask = ~hashmask;

	builddict();
}


vector<size_t> hashstring(string &s)
{
	vector<size_t> output;
	int pos = (W - 1) * hashbit;

	if (hashbit * W > 32 || k_input > 200)
	{
		hash<string> hash_fn;
		for (int i = 0; i < s.size() - W + 1; ++i)
			output.push_back(hash_fn(s.substr(i, W)));
		return output;
	}

	int value = 0;
	for (int i = 0; i < W; ++i)
	{
		value <<= hashbit;
		value |= dict[s[i]];
	}
	output.push_back(value*2654435761 % UINT_MAX);
	for (int i = W; i < s.size() - W + 1; ++i)
	{
		value &= hashmask;
		value <<= hashbit;
		value |= dict[s[i]];
		output.push_back(value*2654435761 % UINT_MAX);
	}

	//for (auto x : output)
		//cout << x << endl;

	return output;
}