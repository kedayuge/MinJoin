//  g++ edit.cpp timerec.cpp hash.cpp minjoin.cpp partition.cpp -std=c++11 -o minjoin -O3
#include "minjoin.h"

int k_input = 50;
string filename = "trec50k.txt";

vector<string> oridata; //original strings
vector<int> indices;
vector<string> oridata_modified; //modified strings for edit distance computation

int seg = 10;
int W = 2;
bool outputresult = false;

int method = 1;
double chunkrate = 0.7; 

unordered_map<string, list<pair<int, int>>> hash_table;
vector<pair<int, int>> candidate;
vector<pair<int, int>> outputs;

#ifdef _WIN32 
clock_t tstart, tend, tstart1, tend1;
#else
struct timeval tstart, tend, tstart1, tend1;
#endif

float partitime = 0.0; // time for partion strings

struct compare {
	bool operator()(const std::string& first, const std::string& second) {
		return first.size() < second.size();
	}
};

void readdata()
{
	ifstream  data(filename);
	string cell;
	int number_string = 0;

	while (getline(data, cell))
	{
		number_string++;
		oridata.push_back(cell);
	}
	for (int i = 0; i < oridata.size(); i++)
		indices.push_back(i);

	compare c;
	sort(indices.begin(), indices.end(), [&](int i1, int i2) { return oridata[i1].size() <  oridata[i2].size(); });
	sort(oridata.begin(), oridata.end(), c);

	oridata_modified = oridata;		// append distinguisher at the end
	for (int j = 0; j < oridata_modified.size(); j++){
		for (int k = 0; k < 8; k++) oridata_modified[j].push_back(j >> (8 * k));
	}

	fprintf(stderr, "Read:%s \n", filename.c_str());
	fprintf(stderr, "Threshold:%d \n", k_input);
	fprintf(stderr, "Number of string:%d \n", oridata.size());
}

void join()
{
	for (int i = 0; i < oridata.size(); ++i)
	{
		recordtime(tstart1);
		vector<pair<string, int>> par = partition(oridata[i], chunkrate);
		recordtime(tend1);
		updatetime(partitime, tstart1, tend1);

		unordered_set<int> set;

		for (auto & x : par)
		{
			if (hash_table.find(x.first) != hash_table.end())
			{
				for (auto it = hash_table[x.first].begin(); it != hash_table[x.first].end();)
				{
					if (abs((int)oridata[i].size() - (int)oridata[(*it).first].size()) > k_input)
					{
						it = hash_table[x.first].erase(it);
						continue;
					}

					if ( (*it).first != i && (abs(x.second - (*it).second) + abs((int)oridata[i].size() - x.second - (int)oridata[(*it).first].size() + (*it).second)) <= k_input)
					{
						candidate.push_back({ (*it).first, i });
					}
					++it;
				}
				
			}
			hash_table[x.first].push_back({ i, x.second });
		}
	}
}

bool cmppair(const pair<string, int> & a,  const pair<string, int> & b)
{
	return a.first < b.first;
}

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // Mainly for demonstration purposes, i.e. works but is overly simple
        // In the real world, use sth. like boost.hash_combine
        return h1 ^ h2;  
    }
};

void verify()
{

	sort(candidate.begin(), candidate.end());
	candidate.erase(unique(candidate.begin(), candidate.end()), candidate.end());
	for (auto x : candidate)
	{
		#ifdef _WIN32
			int ed = edit_dp(oridata[x.first], oridata[x.second], k_input);
		#else
			int ed = edit_dp(oridata_modified[x.first].data(), oridata[x.first].size(), oridata_modified[x.second].data(), oridata[x.second].size(), k_input);
		#endif
		if (ed != -1)
		{
			outputs.push_back(x);
			if (outputresult)
				cout << x.first << " " << x.second << endl;
		}
	}
	fprintf(stderr, "Num of candidate:%d\n", candidate.size());
	fprintf(stderr, "Num of outputs:%d\n", int(outputs.size()));
}

int main(int argc, char **argv)
{
	if (argc > 1)
	{
		filename = argv[1];
		k_input = atoi(argv[2]);
		seg = atoi(argv[3]);
		W = atoi(argv[4]);
		if (argc > 5)
			dictsize = atoi(argv[5]);
	}



	float readtime, jointime, veritime;
	recordtime(tstart);
	initiHash();
	readdata();
	recordtime(tend);
	updatetime(readtime, tstart, tend);
	recordtime(tstart);
	join();
	recordtime(tend);
	updatetime(jointime, tstart, tend);
	recordtime(tstart);
	verify();
	recordtime(tend);
	updatetime(veritime, tstart, tend);
				
	cout << "time for read:" << readtime << endl;
	cout << "time for partition strings:" << partitime << endl;
	cout << "time for join:" << jointime - partitime << endl;
	cout << "time for verification:" << veritime << endl;
	cout << "totoal time:" << readtime + jointime + veritime << endl;

	#ifdef _WIN32
		while (1);
	#endif
}

