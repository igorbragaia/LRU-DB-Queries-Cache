#include<bits/stdc++.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<pqxx/pqxx>
using namespace std;


class LRUcache{
private:
  unordered_map<string, vector<vector<string> > >hashtable;
public:
  bool hasKey(string query){
    return hashtable.find(query) != hashtable.end();
  }

  vector<vector<string> > get(string query){
    if(hasKey(query))
      return hashtable[query];
    else
      return vector<vector<string> >();
  }

  void put(string query, vector<vector<string> > &response){
    if(!hasKey(query)){
      hashtable[query] = response;
    }
  }
};


class Postgres{
private:
  string connection_str;
  LRUcache lru_cache;
  bool cache;
  vector<unsigned int>log;
public:
  Postgres(bool _cache){
    string host = getenv ("HOST_NODECRUD");
    string dbname = getenv ("DATABASE_NODECRUD");
    string user = getenv ("USER_NODECRUD");
    string password = getenv ("PASSWORD_NODECRUD");
    connection_str = "host='"+ host + "' port='5432' dbname='"+ dbname +"' user='"+ user +"' password='" + password +"'";
    cache = _cache;
  }

  vector<vector<string> > executeQuery(string query){
    if(cache and lru_cache.hasKey(query)){
      log.push_back(clock());
      return lru_cache.get(query);
    }

    vector<vector<string> > output;
    vector<string> single_output;
    try {
        pqxx::connection c(connection_str);
        pqxx::work txn(c);
        pqxx::result R = txn.exec(query);
        for (pqxx::result::const_iterator cc = R.begin(); cc != R.end(); ++cc) {
          single_output = vector<string>();
          for(auto ccc:cc)
            single_output.push_back(to_string(ccc));
          output.push_back(single_output);
        }
        if(cache)
          lru_cache.put(query, output);
    }
    catch(const exception &e)
        cerr << e.what() << std::endl;

    log.push_back(clock());
    return output;
  }

  void printQueryOutput(vector<vector<string> > &output){
    for(vector<string> arr:output){
      for(string str:arr)
        cout << str << " ";
      cout << endl;
    }
  }

  void writeLog(string path){
    ofstream myfile;
    string file_name = cache?"logCacheEnabled.txt":"logCacheDisabled.txt";
    file_name = path + "/" + file_name;
    myfile.open (file_name);
    for(unsigned int elm:log)
      myfile << elm << endl;
    myfile.close();
  }

  int avgClockTicksPerSecond(){
    int sum=0;
    for(int i=1;i<(int)log.size();i++)
      sum += log[i] - log[0];
    return sum/(int)log.size();
  }

  void enableCache(){
    cache = true;
  }
};


int main()
{
    srand (time(NULL));
    if (mkdir("log", 0777) == -1)
      cerr << "Log directory Error :  " << strerror(errno) << endl;
    else
      cout << "Log Directory created" << endl;

    int id = rand()%2019;
    string path = "log/" + to_string(id);
    // Creating a directory
    cout << path << endl;
    if (mkdir(path.c_str(), 0777) == -1){
      cerr << "Error :  " << strerror(errno) << endl;
      return 1;
    }
    else
      cout << "Directory created" << endl;

    string query = "SELECT id, name, email  FROM users";
    Postgres postgres(false);
    vector<vector<string> > response;
    cout << "Starting no caching queries" << endl;
    for(int i=0;i<10;i++){
      response = postgres.executeQuery(query);
      // postgres.printQueryOutput(response);
      cout << "iteration " << i << " done" << endl;
    }
    postgres.writeLog(path);
    cout << "Average clock ticks: " << postgres.avgClockTicksPerSecond() << endl;

    Postgres postgres2(true);
    response =   vector<vector<string> >();
    cout << "Starting caching queries" << endl;
    for(int i=0;i<10;i++){
      response = postgres2.executeQuery(query);
      // postgres.printQueryOutput(response);
      cout << "iteration " << i << " done" << endl;
    }
    postgres2.writeLog(path);
    cout << "Average clock ticks: " << postgres2.avgClockTicksPerSecond() << endl;

    // OUTPUT EXAMPLE - data can be seen also at https://goo.gl/oeRbiQ
    // 17 Luis Alberto Bragaia luis.bragaia@terra.com.br
    // 18 Mexicano mexicano@palantir.com
    // 20 Pianista vitor.arruda@ga.ita.br
    // 22 rfdgsd fsdf@gfmdl

    return 0;
}
