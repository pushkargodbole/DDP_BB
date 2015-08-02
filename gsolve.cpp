#include <iostream>
#include <vector>
#include <queue>
#include <math.h>

void gen_rootmatrix(vector<vector<arc_datatype> > paths, vector<flight_datatype> flights, vector<int> djnode_offset, vector<vector<cost_datatype> > &costmatrix, float &cost)
{
	for(int i=0;i<costmatrix.size();i++)
	{
		costmatrix[i][i].val = 0;
	}
	for(int i=0;i<paths.size();i++)
	{
		costmatrix[0][djnode_offset[i]].val = flights[i].start_time;
	}
	for(int i=0;i<paths.size();i++)
	{
		for(int j=0;j<paths[i].size();j++)
		{
			int I = djnode_offset[i]+j;
			int J = djnode_offset[i]+j+1;
			costmatrix[I][J].val = paths[i][j].length/flights[i].speed;
		}
	}
	for(int i=0;i<paths.size();i++)
	{
		for(int j=djnode_offset[i+1]-2; j>=djnode_offset[i];j--)
		{
			for(int k=j+1;k<djnode_offset[i+1];k++)
			{
				costmatrix[j][k].val = costmatrix[j][j+1].val + costmatrix[j+1][k].val;
			}
		}
		for(int j=djnode_offset[i]+1;j<djnode_offset[i+1];j++)
		{
			costmatrix[0][j].val = costmatrix[0][djnode_offset[i]].val + costmatrix[djnode_offset[i]][j].val;
		}
	}
	cost = 0;
	for(int i=1; i<djnode_offset.size();i++)
	{
		cost += costmatrix[0][djnode_offset[i]-1].val;
	}
}

bool operator<(con_datatype a, con_datatype b) {return a.dj2_j < b.dj2_j ? false : true;}

vector<vector<con_datatype> > gen_cons(vector<vector<arc_datatype> > paths, vector<flight_datatype> flights, vector<int> djnode_offset, vector<int> runway_entry, vector<int> runway_exit)
{
	vector<priority_queue<con_datatype> > con_queues;
	vector<vector<con_datatype> > cons;
	for(int i=0;i<flights.size();i++)
    {
        for(int j=i+1;j<flights.size();j++)
        {
			vector<con_datatype> runcons;
			int kmax = runway_entry[i];
			int lmax = runway_entry[j];
			int l_kmax, k_lmax;
            for(int k=runway_entry[i];k<=runway_exit[i];k++)
            {
                for(int l=runway_entry[j];l<=runway_exit[j];l++)
                {
					if(paths[i][k].node1 == paths[j][l].node1 || paths[i][k].node2 == paths[j][l].node2 || (paths[i][k].node1 == paths[j][l].node2 && paths[i][k].node2 == paths[j][l].node1))
                    {
						if(k>=kmax)
						{
							kmax = k;
							l_kmax = l;
						}
						if(l>=lmax)
						{
							lmax = l;
							k_lmax = k;
						}
                    }
				}
			}
			if(kmax>runway_entry[i] || lmax>runway_entry[j])
			{
				con_datatype con;
				con.dj1_i = djnode_offset[i] + kmax+1;
				con.dj1_j = djnode_offset[j] + runway_entry[j];
				con.dj1_len = 0;
				con.dj2_i = djnode_offset[j] + l_kmax+1;
				con.dj2_j = djnode_offset[i] + runway_entry[i];
				con.dj2_len = 0;
				runcons.push_back(con);
				if(kmax!=k_lmax || lmax!=l_kmax)
				{
					con.dj1_i = djnode_offset[i] + k_lmax+1;
					con.dj1_j = djnode_offset[j] + runway_entry[j];
					con.dj1_len = 0;
					con.dj2_i = djnode_offset[j] + lmax+1;
					con.dj2_j = djnode_offset[i] + runway_entry[i];
					con.dj2_len = 0;
					runcons.push_back(con);
				}
			}
			cons.push_back(runcons);
			priority_queue<con_datatype> con_queue;
			int k1, k2, l1, l2;
			if(runway_entry[i]==0)
			{
				k1 = runway_exit[i]+1;
				k2 = paths[i].size()-1;
			}
			else
			{
				k1 = 0;
				k2 = runway_entry[i]-1;
			}
			if(runway_entry[j]==0)
			{
				l1 = runway_exit[j]+1;
				l2 = paths[j].size()-1;
			}
			else
			{
				l1 = 0;
				l2 = runway_entry[j]-1;
			}
            for(int k=k1;k<=k2;k++)
            {
                for(int l=l1;l<=l2;l++)
                {
					if(paths[i][k].node1 == paths[j][l].node1)
                    {
						con_datatype con;
                        if(paths[i][k].length <= flights[i].sep)
                        {
                            con.dj1_i = djnode_offset[i] + k+1;
							con.dj1_j = djnode_offset[j] + l;
							con.dj1_len = 0;
                        }
                        else
                        {
                            con.dj1_i = djnode_offset[i] + k;
							con.dj1_j = djnode_offset[j] + l;
							con.dj1_len = ceil(flights[i].sep/flights[i].speed);
                        }
                        if(paths[j][l].length <= flights[j].sep)
                        {
                            con.dj2_i = djnode_offset[j] + l+1;
							con.dj2_j = djnode_offset[i] + k;
							con.dj2_len = 0;
                        }
                        else
                        {
                            con.dj2_i = djnode_offset[j] + l;
							con.dj2_j = djnode_offset[i] + k;
							con.dj2_len = ceil(flights[j].sep/flights[j].speed);
                        }
                        con_queue.push(con);
                    }
                    if(paths[i][k].node2 == paths[j][l].node2)
                    {
						con_datatype con;
                        if(paths[j][l].length <= flights[i].sep)
                        {
							con.dj1_i = djnode_offset[i] + k+1;
							con.dj1_j = djnode_offset[j] + l;
							con.dj1_len = 0;
                        }
                        else
                        {
							con.dj1_i = djnode_offset[i] + k+1;
							con.dj1_j = djnode_offset[j] + l+1;
							con.dj1_len = ceil(flights[i].sep/flights[j].speed);
                        }
                        if(paths[i][k].length <= flights[j].sep)
                        {
							con.dj2_i = djnode_offset[j] + l+1;
							con.dj2_j = djnode_offset[i] + k;
							con.dj2_len = 0;
                        }
                        else
                        {
							con.dj2_i = djnode_offset[j] + l+1;
							con.dj2_j = djnode_offset[i] + k+1;
							con.dj2_len = ceil(flights[j].sep/flights[i].speed);
                        }
                        con_queue.push(con);
                    }
                    if(paths[i][k].node1 == paths[j][l].node2 && paths[i][k].node2 == paths[j][l].node1)
                    {
						con_datatype con;
						con.dj1_i = djnode_offset[i] + k+1;
						con.dj1_j = djnode_offset[j] + l;
						con.dj2_i = djnode_offset[j] + l+1;
						con.dj2_j = djnode_offset[i] + k;
						con.dj1_len = 0;
						con.dj2_len = 0;
						con_queue.push(con);
                    }
				}
			}
			con_queues.push_back(con_queue);
		}
	}
	for(int i=0;i<con_queues.size();i++)
	{
		bool flag = true;
		while(con_queues[i].size()>0)
		{
			if(flag==true)
			{
				vector<con_datatype> con_group;
				cons.push_back(con_group);
			}
			cons.back().push_back(con_queues[i].top());
			//cout << con_queue.top().dj1_i << " " << con_queue.top().dj1_j << " " << con_queue.top().dj1_len << " " << con_queue.top().dj2_i << " " << con_queue.top().dj2_j << " " << con_queue.top().dj2_len << endl;
			con_queues[i].pop();
			if(con_queues[i].size()>0)
			{
				if(con_queues[i].top().dj2_j <= cons.back().back().dj2_j + 1) flag = false;
				else flag = true;
			}
		}
	}
	//int ncons = 0;
	//for(int i=0;i<cons.size();i++) ncons += cons[i].size();
	//cout << endl << ncons << endl;
	/*
	cout << "--------------------------------------------" << endl;
	for(int i=0;i<cons.size();i++)
	{
		for(int j=0;j<cons[i].size(); j++)
		{
			cout << cons[i][j].dj1_i << " " << cons[i][j].dj1_j << " " << cons[i][j].dj1_len << " " << cons[i][j].dj2_i << " " << cons[i][j].dj2_j << " " << cons[i][j].dj2_len << endl;
		}
		cout << endl;
	}
	*/
	return cons;
}

void update_matrix(vector<con_datatype> cons, vector<int> djnode_offset, vector<vector<cost_datatype> > &costmatrix1, vector<vector<cost_datatype> > &costmatrix2, float &cost1, float &cost2)
{
	//cout << "woohoo2" << endl;
	for(int i=0;i<cons.size();i++)
	{
		if(costmatrix1[cons[i].dj1_i][cons[i].dj1_j].val==-1)
		{
			costmatrix1[cons[i].dj1_i][cons[i].dj1_j].val = cons[i].dj1_len;
			costmatrix1[cons[i].dj1_i][cons[i].dj1_j].dj = 1;
		}
		if(costmatrix2[cons[i].dj2_i][cons[i].dj2_j].val==-1)
		{
			costmatrix2[cons[i].dj2_i][cons[i].dj2_j].val = cons[i].dj2_len;
			costmatrix2[cons[i].dj2_i][cons[i].dj2_j].dj = 1;
		}
		
	}
	//cout << "* " << I << " " << J << endl;
	int i1, j1, i2, j2;
	for(int i=0; i<costmatrix1.size();i++)
	{
		for(int j=0;j<costmatrix1.size();j++)
		{
			for(int k=0;k<cons.size();k++)
			{
				i1 = cons[k].dj1_i;
				j1 = cons[k].dj1_j;
				i2 = cons[k].dj2_i;
				j2 = cons[k].dj2_j;
				if(costmatrix1[i][i1].val!=-1 && costmatrix1[j1][j].val!=-1)
				{
					if(costmatrix1[i][j].val < costmatrix1[i][i1].val + costmatrix1[i1][j1].val + costmatrix1[j1][j].val)
					{
						costmatrix1[i][j].val = costmatrix1[i][i1].val + costmatrix1[i1][j1].val + costmatrix1[j1][j].val;
					}
					costmatrix1[i][j].dj = costmatrix1[i][j].dj + costmatrix1[i][i1].dj + costmatrix1[i1][j1].dj + costmatrix1[j1][j].dj;
				}
				if(costmatrix2[i][i2].val!=-1 && costmatrix2[j2][j].val!=-1)
				{
					if(costmatrix2[i][j].val < costmatrix2[i][i2].val + costmatrix2[i2][j2].val + costmatrix2[j2][j].val)
					{
						costmatrix2[i][j].val = costmatrix2[i][i2].val + costmatrix2[i2][j2].val + costmatrix2[j2][j].val;
					}
					costmatrix2[i][j].dj = costmatrix2[i][j].dj + costmatrix2[i][i2].dj + costmatrix2[i2][j2].dj + costmatrix2[j2][j].dj;
				}
			}
		}
	}
		
		/*
		for(int a=0;a<costmatrix1.size();a++)
		{
			for(int b=0;b<costmatrix1[a].size();b++)
			{
				cout << costmatrix1[a][b].val << " ";
			}
			cout << endl;
		}
		
		for(int a=0;a<costmatrix.size();a++)
		{
			for(int b=0;b<costmatrix[a].size();b++)
			{
				cout << costmatrix[a][b].dj << " ";
			}
			cout << endl;
		}
		*/
	cost1 = 0;
	cost2 = 0;
	for(int i=1; i<djnode_offset.size();i++)
	{
		cost1 = cost1 + costmatrix1[0][djnode_offset[i]-1].val;
		cost2 = cost2 + costmatrix2[0][djnode_offset[i]-1].val;
	}
	for(int i=0;i<costmatrix1.size();i++)
	{
		if(costmatrix1[i][i].dj!=0)	cost1 = -1;
		if(costmatrix2[i][i].dj!=0) cost2 = -1;
	}
}
