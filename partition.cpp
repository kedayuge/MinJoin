#include "partition.h"


vector<pair<string, int>> findPart(string& s, vector<int>& anchors,double rate = 0.5)
{
	int T = (int)s.size() * rate / seg;
	vector<pair<string, int>> output;
	for (int i = 1; i < anchors.size(); ++i)
		if (anchors[i] - anchors[i - 1] > T)
			output.push_back({ s.substr(anchors[i - 1], anchors[i] - anchors[i - 1]), anchors[i - 1] });
	return output;
}

vector<int> findAnch(string& s, double rate = 1.0)
{
	vector<int> anchors;
	int L = (int)s.size() / seg;
	int z = max(1, int(L * rate /2));

	auto value = hashstring(s);
	anchors.push_back(0);

	for (int i = z; i + z + W - 1 < s.size(); )
	{
		int d = 1;
		for (;d <= z && value[i] < value[i - d] && value[i] < value[i + d]; ++d);
		if (d == z + 1)
			anchors.push_back(i);
		i += d;	
	}
	anchors.push_back(s.size());
	return anchors;
}

vector<pair<string, int>> partition(string& s, double rate = 1.0)
{
	auto x = findAnch(s, rate);
	return findPart(s, x, 0.5);
}

