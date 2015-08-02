#include <iostream>
#include <vector>
#include <fstream>
#include <string.h>
#include <sstream>
#include <limits>
#include <time.h>
#include "mystringfuncs.cpp"
#include "datatypes.cpp"
#include "getdata.cpp"
#include "gen_paths.cpp"
#include "gsolve.cpp"

using namespace std;

bool operator<(treenode_ptr_datatype1 a, treenode_ptr_datatype1 b) {return a.level < b.level ? true:false;}
bool operator<(treenode_ptr_datatype2 a, treenode_ptr_datatype2 b) {return a.cost < b.cost ? false:true;}

int main(int argc, char* argv[])
{
    if (argc == 5)
    {
		float tolerance = -1;
		try
		{
			string tolerance_str = stringstrip(argv[4]);
			if(tolerance_str == "0") tolerance = 0;
			else
			{
				stringstream(tolerance_str) >> tolerance;
				if(tolerance==0) throw 1;
			}	
		}
		catch(int e)
		{
			cout << "Tolerance (4th argument) must be a number." << endl;
			return 1;
		}
		cout << "Tolerance : " << tolerance << endl;
        file_datatype file = getdata(argv[1], argv[2]);
        //Finding all paths for all flights
        vector<vector<vector<arc_datatype> > > allpaths;
        for(int i=0;i<file.flights.size();i++)
        {
			cout << "Flight " << i+1 << " : " << endl; 
            vector<vector<arc_datatype> > paths = gen_paths(file.flights[i], file.map);
            cout << endl;
            if(paths.size()==0)
            {
				cout << "No feasible paths found for flight " << i+1 << endl;
				return 1;
			}
            allpaths.push_back(paths);
        }
        //Generating all possible combinations of path allocation
        vector<int> paths_count;
        for(int i=0;i<file.flights.size();i++)
        {
            paths_count.push_back(allpaths[i].size());
            cout << i<< " : " << allpaths[i].size() << endl;
        }
        priority_queue<comb_datatype> combs = allcombs(paths_count);
        int ncombs = combs.size();
        cout << "Combs : " << ncombs << endl;
        float UB = numeric_limits<float>::max();
        ofstream solfile;
        int i=0;
        time_t t0 = time(NULL);
        bool solfound = false;
        int ten_percent = (ncombs*0.01 < 10 ? 10:ncombs*0.01); 
        while(combs.size()>0)
        {
			if(i==ten_percent) UB -= tolerance*file.flights.size();
			if(solfound==false) //Sort by level (Find a feasible solution fast, to reduce UB from infinity to a finite cost and begin pruning)
			{
				vector<vector<arc_datatype> > chosenpaths;
				vector<int> djnode_offset;
				for(int j=0;j<file.flights.size();j++)
				{
					chosenpaths.push_back(allpaths[j][combs.top().comb[j]]);
					if(j==0) djnode_offset.push_back(1);
					else djnode_offset.push_back(chosenpaths[j-1].size()+1+djnode_offset[j-1]);
				}
				djnode_offset.push_back(chosenpaths.back().size()+1+djnode_offset.back());
				vector<int> r_entry, r_exit;
				for(int j=0;j<file.flights.size();j++)
				{
					if(chosenpaths[j].front().type[0]=='R')
					{
						r_entry.push_back(0);
						for(int k=0;k<chosenpaths[j].size();k++)
						{
							if(chosenpaths[j][k].type=="T")
							{
								r_exit.push_back(k-1);
								break;
							}
							else if(k==chosenpaths[j].size()-1) r_exit.push_back(k);
						}
					}
					else
					{
						r_exit.push_back(chosenpaths[j].size()-1);
						for(int k=chosenpaths[j].size()-1;k>=0;k--)
						{
							if(chosenpaths[j][k].type=="T")
							{
								r_entry.push_back(k+1);
								break;
							}
							else if(k==0) r_entry.push_back(k);
						}
					}
				}
				treenode_datatype * rootnode = new (std::nothrow) treenode_datatype(djnode_offset.back());
				treenode_ptr_datatype1 rootnode_ptr1;
				treenode_ptr_datatype2 rootnode_ptr2;
				gen_rootmatrix(chosenpaths, file.flights, djnode_offset, rootnode->costmatrix, rootnode->cost);
				rootnode->level = 0;
				float rootcost = rootnode->cost;
				if(rootnode->cost <= UB)
				{
					vector<vector<con_datatype> > cons = gen_cons(chosenpaths, file.flights, djnode_offset, r_entry, r_exit);
					priority_queue<treenode_ptr_datatype1> active1;
					priority_queue<treenode_ptr_datatype2> active2;
					if(solfound==false)
					{
						rootnode_ptr1.level = rootnode->level;
						rootnode_ptr1.ptr = rootnode;
						active1.push(rootnode_ptr1);
					}
					rootnode_ptr2.cost = rootnode->cost;
					rootnode_ptr2.ptr = rootnode;
					active2.push(rootnode_ptr2);
					treenode_ptr_datatype1 treenode_ptr1;
					treenode_ptr_datatype2 treenode_ptr2;
					while((solfound==false && active1.size()>0) || (solfound==true && active2.size()>0))
					{
						treenode_datatype * treenode1 = new (std::nothrow) treenode_datatype(djnode_offset.back());
						treenode_datatype * treenode2 = new (std::nothrow) treenode_datatype(djnode_offset.back());
						if(solfound==false) cout << "Elapsed time : " << (time(NULL)-t0)/60 << " mins " << (time(NULL)-t0)%60 << " secs | Upper bound: " << UB << " | Root cost: " << rootcost << " (" << i+1 << "/" << ncombs << ") " << active1.size() << "      \xd";
						else cout << "Elapsed time : " << (time(NULL)-t0)/60 << " mins " << (time(NULL)-t0)%60 << " secs | Upper bound: " << UB << " | root cost: " << rootcost << " (" << i+1 << "/" << ncombs << ") " << active2.size() << "      \r";
						//t1 = clock();
						if(solfound==false)
						{
							*treenode1 = *active1.top().ptr;
							*treenode2 = *active1.top().ptr;
						}
						else
						{
							*treenode1 = *active2.top().ptr;
							*treenode2 = *active2.top().ptr;
						}
						treenode1->level += 1;
						treenode2->level += 1;
						if(cons.size()>0)
						{
							if(solfound==false)
							{
								update_matrix(cons[active1.top().ptr->level], djnode_offset, treenode1->costmatrix, treenode2->costmatrix, treenode1->cost, treenode2->cost);
							}
							else update_matrix(cons[active2.top().ptr->level], djnode_offset, treenode1->costmatrix, treenode2->costmatrix, treenode1->cost, treenode2->cost);
						}
						//t2 = clock();
						//update = update + t2 - t1;
						//t1 = clock();
						if(solfound==false)
						{
							//delete active1.top().ptr; This can not be done as doing this will also free the memory pointed by a pointer in the active2 queue thus potentially leading to a seg fault later
							active1.pop();
						}
						else
						{
							delete active2.top().ptr;
							active2.pop();
						}
						int newsol = 0;
						if(treenode1->cost!=-1 && treenode1->cost < UB)
						{
							if(treenode1->level==cons.size() || cons.size()==0)
							{
								UB = treenode1->cost;
								newsol = 1;
							}
							else
							{
								if(solfound==false)
								{
									treenode_ptr1.level = treenode1->level;
									treenode_ptr1.ptr = treenode1;
									active1.push(treenode_ptr1);
								}
								treenode_ptr2.cost = treenode1->cost;
								treenode_ptr2.ptr = treenode1;
								active2.push(treenode_ptr2);
							}
						}
						if(treenode2->cost!=-1 && treenode2->cost < UB)
						{
							if(treenode2->level==cons.size() || cons.size()==0)
							{
								UB = treenode2->cost;
								newsol = 2;
							}
							else
							{
								if(solfound==false)
								{
									treenode_ptr1.level = treenode2->level;
									treenode_ptr1.ptr = treenode2;
									active1.push(treenode_ptr1);
								}
								treenode_ptr2.cost = treenode2->cost;
								treenode_ptr2.ptr = treenode2;
								active2.push(treenode_ptr2);
							}
						}
						if(newsol>0)
						{
							solfound = true;
							vector<vector<cost_datatype> > solcostmatrix;
							if(newsol==1) solcostmatrix = treenode1->costmatrix;
							if(newsol==2) solcostmatrix = treenode2->costmatrix;
							solfile.open(argv[3], ofstream::trunc);
							solfile << "Time : " << (time(NULL)-t0)/60 << " mins " << (time(NULL)-t0)%60 << " secs | Cost : " << UB << endl; 
							cout << endl << "Time : " << (time(NULL)-t0)/60 << " mins " << (time(NULL)-t0)%60 << " secs | Cost : " << UB << endl;
							for(int j=0;j<file.flights.size();j++)
							{
								solfile << j << " : ";
								cout << j << " : ";
								int K;
								float t;
								for(int k=0;k<chosenpaths[j].size();k++)
								{
									K = djnode_offset[j] + k;
									t = solcostmatrix[0][K].val;
									solfile << chosenpaths[j][k].node1+1 << " " << t << " - ";
									cout << chosenpaths[j][k].node1+1 << " " << t << " - ";
								}
								K = djnode_offset[j] + chosenpaths[j].size();
								t = solcostmatrix[0][K].val;
								solfile << chosenpaths[j].back().node2+1 << " " << t << endl;
								cout << chosenpaths[j].back().node2+1 << " " << t << endl;
							}
							solfile.close();
						}
					}
				}
			}
			else //Sort by partial cost (Explore promising nodes [nodes with lower lower-bounds => higher potential to deliver an optimal solution] first)
			{
				vector<vector<arc_datatype> > chosenpaths;
				vector<int> djnode_offset;
				for(int j=0;j<file.flights.size();j++)
				{
					chosenpaths.push_back(allpaths[j][combs.top().comb[j]]);
					if(j==0) djnode_offset.push_back(1);
					else djnode_offset.push_back(chosenpaths[j-1].size()+1+djnode_offset[j-1]);
				}
				djnode_offset.push_back(chosenpaths.back().size()+1+djnode_offset.back());
				vector<int> r_entry, r_exit;
				for(int j=0;j<file.flights.size();j++)
				{
					if(chosenpaths[j].front().type[0]=='R')
					{
						r_entry.push_back(0);
						for(int k=0;k<chosenpaths[j].size();k++)
						{
							if(chosenpaths[j][k].type=="T")
							{
								r_exit.push_back(k-1);
								break;
							}
							else if(k==chosenpaths[j].size()-1) r_exit.push_back(k);
						}
					}
					else
					{
						r_exit.push_back(chosenpaths[j].size()-1);
						for(int k=chosenpaths[j].size()-1;k>=0;k--)
						{
							if(chosenpaths[j][k].type=="T")
							{
								r_entry.push_back(k+1);
								break;
							}
							else if(k==0) r_entry.push_back(k);
						}
					}
				}
				treenode_datatype * rootnode = new (std::nothrow) treenode_datatype(djnode_offset.back());
				treenode_ptr_datatype2 rootnode_ptr;
				gen_rootmatrix(chosenpaths, file.flights, djnode_offset, rootnode->costmatrix, rootnode->cost);
				rootnode->level = 0;
				float rootcost = rootnode->cost;
				rootnode_ptr.cost = rootnode->cost;
				if(rootnode->cost <= UB)
				{
					vector<vector<con_datatype> > cons = gen_cons(chosenpaths, file.flights, djnode_offset, r_entry, r_exit);
					priority_queue<treenode_ptr_datatype2> active;
					rootnode_ptr.ptr = rootnode;
					active.push(rootnode_ptr);
					treenode_ptr_datatype2 treenode_ptr;
					while(active.size()>0)
					{
						treenode_datatype * treenode1 = new (std::nothrow) treenode_datatype(djnode_offset.back());
						treenode_datatype * treenode2 = new (std::nothrow) treenode_datatype(djnode_offset.back());
						cout << "Elapsed time : " << (time(NULL)-t0)/60 << " mins " << (time(NULL)-t0)%60 << " secs | Upper bound: " << UB << " | root cost: " << rootcost << " (" << i+1 << "/" << ncombs << ") " << active.size() << "      \xd";
						//t1 = clock();
						*treenode1 = *active.top().ptr;
						*treenode2 = *active.top().ptr;
						treenode1->level += 1;
						treenode2->level += 1;
						if(cons.size()>0) update_matrix(cons[active.top().ptr->level], djnode_offset, treenode1->costmatrix, treenode2->costmatrix, treenode1->cost, treenode2->cost);
						delete active.top().ptr;
						active.pop();
						int newsol = 0;
						if(treenode1->cost!=-1 && treenode1->cost < UB)
						{
							if(treenode1->level==cons.size() || cons.size()==0)
							{
								UB = treenode1->cost;
								newsol = 1;
							}
							else
							{
								treenode_ptr.cost = treenode1->cost;
								treenode_ptr.ptr = treenode1;
								active.push(treenode_ptr);
							}
						}
						else delete treenode1;
						if(treenode2->cost!=-1 && treenode2->cost < UB)
						{
							if(treenode2->level==cons.size() || cons.size()==0)
							{
								UB = treenode2->cost;
								newsol = 2;
							}
							else
							{
								treenode_ptr.cost = treenode2->cost;
								treenode_ptr.ptr = treenode2;
								active.push(treenode_ptr);
							}
						}
						else delete treenode2;
						if(newsol>0)
						{
							vector<vector<cost_datatype> > solcostmatrix;
							if(newsol==1) solcostmatrix = treenode1->costmatrix;
							if(newsol==2) solcostmatrix = treenode2->costmatrix;
							solfile.open(argv[3], ofstream::trunc);
							solfile << "Time : " << (time(NULL)-t0)/60 << " mins " << (time(NULL)-t0)%60 << " secs | Cost : " << UB << endl; 
							cout << endl << "Time : " << (time(NULL)-t0)/60 << " mins " << (time(NULL)-t0)%60 << " secs | Cost : " << UB << endl;
							for(int j=0;j<file.flights.size();j++)
							{
								solfile << j << " : ";
								cout << j << " : ";
								int K;
								float t;
								for(int k=0;k<chosenpaths[j].size();k++)
								{
									K = djnode_offset[j] + k;
									t = solcostmatrix[0][K].val;
									solfile << chosenpaths[j][k].node1+1 << " " << t << " - ";
									cout << chosenpaths[j][k].node1+1 << " " << t << " - ";
								}
								K = djnode_offset[j] + chosenpaths[j].size();
								t = solcostmatrix[0][K].val;
								solfile << chosenpaths[j].back().node2+1 << " " << t << endl;
								cout << chosenpaths[j].back().node2+1 << " " << t << endl;
							}
							solfile.close();
						}
					}
				}
			}
			combs.pop();
			i+=1;
		}
		solfile.open(argv[3], ofstream::app);
		solfile << endl << "Total time : " << (time(NULL)-t0)/60 << " mins " << (time(NULL)-t0)%60 << " secs | Tolerance : " << tolerance << endl;
		solfile.close();
		cout << endl;
        return 0;
    }
    else
    {
        cout << "4 arguments expected, (map file, flights file, output file, tolernace)" << argc-1 << " given." << endl;
        return 1;
    }
}
