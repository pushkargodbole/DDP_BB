using namespace std;

file_datatype getdata(string mapfile_str, string flightsfile_str)
{
    file_datatype file;
    char * mapfile_char;
    char * flightsfile_char;
    mapfile_char = new char [mapfile_str.length()+1];
	strcpy(mapfile_char, mapfile_str.c_str());
    flightsfile_char = new char [flightsfile_str.length()+1];
	strcpy(flightsfile_char, flightsfile_str.c_str());
    
    vector<vector<arc_datatype> > map;
    string line;
    ifstream mapfile (mapfile_char);
    if (mapfile.is_open())
    {
        int count = 0;
        while ( mapfile.good() )
        {
            vector<arc_datatype> arcs;
            vector<string> elems;
            getline (mapfile, line);
            line = stringstrip(line);
            elems = stringsplit(line, ' ');
            for(int i=0;i<elems.size();i=i+3)
            {
                arc_datatype arc;
                arc.node1 = count;
                stringstream(elems[i]) >> arc.node2;
                stringstream(elems[i+1]) >> arc.length;
                arc.type = elems[i+2];
                arcs.push_back(arc);
            }
            map.push_back(arcs);
            count++;
        }
        mapfile.close();
        file.map = map;
    }
    else cout << "Unable to open file" << endl;
    
    vector<flight_datatype> flights;
    ifstream flightfile (flightsfile_char);
    if (flightfile.is_open())
    {
        while ( flightfile.good() )
        {
            vector<string> elems;
            getline (flightfile, line);
            line = stringstrip(line);
            elems = stringsplit(line, ' ');
            if(elems.size() == 7)
            {
                flight_datatype flight;
                stringstream(elems[0]) >> flight.start;
                stringstream(elems[1]) >> flight.end;
                stringstream(elems[2]) >> flight.start_time;
                stringstream(elems[3]) >> flight.speed;
                stringstream(elems[4]) >> flight.sep;
                stringstream(elems[5]) >> flight.runlen;
                stringstream(elems[6]) >> flight.priority;
                flights.push_back(flight);
            }
        }
        flightfile.close();
        file.flights = flights;
    }
    else cout << "Unable to open file" << endl;
    return file;
}
    
