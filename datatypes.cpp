#include <iostream>
#include <string>
#include <queue>

using namespace std;

struct arc_datatype
{
	public:
		int node1;
        int node2;
        float length;
        string type;
};

struct flight_datatype
{
    public:
        int start;
        int end;
        float start_time;
        float speed;
        float sep;
        float runlen;
        float priority;
};

struct path_datatype
{
	public:
		float len;
		vector<arc_datatype> path;
};

struct file_datatype
{
    public:
        vector<vector<arc_datatype> > map;
        vector<flight_datatype> flights;
};

struct cost_datatype
{
	public:
		float val;
		float dj;
		cost_datatype() : val(-1), dj(0) {}
};

struct comb_datatype
{
	public:
		vector<int> comb;
		int cost;
		comb_datatype() : cost(0) {}
};

struct con_datatype
{
	public:
		int dj1_i;
		int dj1_j;
		float dj1_len;
		int dj2_i;
		int dj2_j;
		float dj2_len;
};

struct treenode_datatype
{
	public:
		float cost;
		int level;
		vector<vector<cost_datatype> > costmatrix;
		treenode_datatype(int nodes)
		{
			costmatrix.resize(nodes);
			for (int i = 0;i<nodes;i++)
			{
				costmatrix[i].resize(nodes);
			}
		}
};


struct treenode_ptr_datatype1
{
	public:
		int level;
		treenode_datatype * ptr;
};

struct treenode_ptr_datatype2
{
	public:
		float cost;
		treenode_datatype * ptr;
};
