#include<bits/stdc++.h>
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
    if(cache and lru_cache.hasKey(query))
      return lru_cache.get(query);

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
    {
        cerr << e.what() << std::endl;
    }
    return output;
  }

  void printQueryOutput(vector<vector<string> > &output){
    for(vector<string> arr:output){
      for(string str:arr)
        cout << str << " ";
      cout << endl;
    }
  }

  void enableCache(){
    cache = true;
  }
};


int main()
{
    string query = "SELECT id, name, email  FROM users";
    Postgres postgres(false);
    vector<vector<string> > response;
    clock_t time_a, time_b;
    unsigned int total_time_ticks;

    cout << "Starting no caching queries" << endl;
    time_a = clock();
    response = postgres.executeQuery(query);
    // postgres.printQueryOutput(response);
    time_b = clock();
    total_time_ticks = (unsigned int)(time_b - time_a);
    cout << "Query without cache took " << total_time_ticks << " ms" << endl;

    time_a = clock();
    response = postgres.executeQuery(query);
    // postgres.printQueryOutput(response);
    time_b = clock();
    total_time_ticks = (unsigned int)(time_b - time_a);
    cout << "Query without cache took " << total_time_ticks << " ms" << endl;

    cout << "Starting caching queries" << endl;
    postgres.enableCache();
    time_a = clock();
    response = postgres.executeQuery(query);
    // postgres.printQueryOutput(response);
    time_b = clock();
    total_time_ticks = (unsigned int)(time_b - time_a);
    cout << "Query with cache took " << total_time_ticks << " ms" << endl;

    time_a = clock();
    response = postgres.executeQuery(query);
    // postgres.printQueryOutput(response);
    time_b = clock();
    total_time_ticks = (unsigned int)(time_b - time_a);
    cout << "Query with cache took " << total_time_ticks << " ms" << endl;

    // OUTPUT EXAMPLE - data can be seen also at https://goo.gl/oeRbiQ
    // 17 Luis Alberto Bragaia luis.bragaia@terra.com.br
    // 18 Mexicano mexicano@palantir.com
    // 20 Pianista vitor.arruda@ga.ita.br
    // 22 rfdgsd fsdf@gfmdl

    return 0;
}
