#include <iostream>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <queue>
#include <math.h>

using namespace std;

bool operator<(path_datatype a, path_datatype b) {return a.len < b.len ? false:true;}
bool operator<(comb_datatype a, comb_datatype b) {return a.cost < b.cost ? false:true;}

vector<vector<arc_datatype> > gen_paths(flight_datatype flight, vector<vector<arc_datatype> > map)
{
	priority_queue<path_datatype> allpathsq;
	vector<vector<arc_datatype> > allpaths; 
	vector<vector<arc_datatype> > splitpaths;
	vector<int> splitnodes;
	vector<arc_datatype> open;
	vector<arc_datatype> path;
	int curr_node = flight.start;
    for(int i=0;i<map[curr_node].size();i++) open.push_back(map[curr_node][i]);
	do
	{
        if(map[curr_node].size()>1)
        {
            int newsplitnode = 1;
            for(int i=0;i<splitnodes.size();i++)
            {
                if(splitnodes[i] == curr_node)
                {
                    splitpaths[i] = path;
                    newsplitnode = 0;
                    break;
                }
            }
            if(newsplitnode == 1)
            {
                splitnodes.push_back(curr_node);
                splitpaths.push_back(path);
            }
        }
        int jump;
        do
        {
            jump = 0;
            for(int i=0;i<path.size();i++)
            {
                if(open[open.size()-1].node2 == path[i].node1 && open[open.size()-1].node1 == path[path.size()-1].node2)
                {
                    open.pop_back();
                    i = -1;
                    jump = 1;
                }
            }
            if(path.size()!=0)
            {
                if(map[path[path.size()-1].node2].size()==0)
                {
                    jump = 1;
                }
            }
            if(open.size()!=0)
            {
                if(jump == 1)
                {
                    for(int i=0;i<splitnodes.size();i++)
                    {
                        if(open[open.size()-1].node1 == splitnodes[i])
                        {
                            path = splitpaths[i];
                        }
                    }
                }
            }
        }
        while(jump==1);
        int change = 0;
        if(open.size()!=0)
        {
            curr_node = open[open.size()-1].node2;
            path.push_back(open[open.size()-1]);
            open.pop_back();
            for(int i=0;i<map[curr_node].size();i++) open.push_back(map[curr_node][i]);
            change = 1;
        }
		if(path.size()!=0)
		{
			if(path[path.size()-1].node2 == flight.end && change == 1)
			{
				float runlen = 0;
				bool valid = true;
				bool cont = true;
				int k, l;
				for(int i=0;i<path.size();i++)
				{
					if(path.front().type[0]=='R')
					{
						k = i;
						l = 1;
					}
					else if(path.back().type[0]=='R')
					{
						k = path.size()-i-1;
						l = -1;
					}
					else
					{
						valid = false;
						break;
					}
					if(cont==true)
					{
						if(path[k].type[0]=='R')
						{
							if(i!=0)
							{
								if(path[k].type!=path[k-l].type)
								{
									valid = false;
									break;
								}
							}
							runlen += path[k].length;
						}
						else
						{
							cont = false;
							if(runlen<flight.runlen)
							{
								valid = false;
								break;
							}
						}
					}
					else if(path[k].type[0]=='R')
					{
						valid = false;
						break;
					}
				}
				if(valid==true)
				{
					float length = 0;
					for(int i=0;i<path.size();i++) length+=path[i].length;
					path_datatype newpath;
					newpath.path = path;
					newpath.len = length;
					allpathsq.push(newpath);
				}
			}
		}
	}while(open.size()!=0);
	while(allpathsq.size()>0)
	{
		cout << allpathsq.top().len << " : ";
		for(int i=0;i<allpathsq.top().path.size();i++)
		{
			cout << "(" << allpathsq.top().path[i].node1+1 << ", " << allpathsq.top().path[i].node2+1 << ") ";
		}
		cout << endl;
		allpaths.push_back(allpathsq.top().path);
		allpathsq.pop();
	}
	return allpaths;
}

priority_queue<comb_datatype> allcombs(vector<int> elems)
{
    if(elems.size()==1)
    {
		priority_queue<comb_datatype> newcombs;
        for(int i=0;i<elems[0];i++)
        {
            comb_datatype comb;
            comb.comb.push_back(i);
            comb.cost = pow(i,3);
            newcombs.push(comb);
        }
        return newcombs;
    }
    else
    {
        vector<int> elems1 (elems.begin(), elems.end()-1);
        priority_queue<comb_datatype> combs = allcombs(elems1);
        priority_queue<comb_datatype> newcombs;
        while(combs.size()>0)
        {
            for(int i=0;i<elems[elems.size()-1];i++)
            {
                comb_datatype comb = combs.top();
                comb.comb.push_back(i);
                comb.cost+=pow(i,3);
                newcombs.push(comb);
            }
            combs.pop();
        }
        return newcombs;
    }
}
