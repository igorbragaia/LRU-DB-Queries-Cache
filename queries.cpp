#include<bits/stdc++.h>
#include<pqxx/pqxx>
using namespace std;


class Postgres{
private:
  string connection_str;
public:
  Postgres(){
    string host = getenv ("HOST_NODECRUD");
    string dbname = getenv ("DATABASE_NODECRUD");
    string user = getenv ("USER_NODECRUD");
    string password = getenv ("PASSWORD_NODECRUD");
    connection_str = "host='"+ host + "' port='5432' dbname='"+ dbname +"' user='"+ user +"' password='" + password +"'";
  }

  vector<vector<string> > executeQuery(string query){
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
    }
    catch(const exception &e)
    {
        std::cerr << e.what() << std::endl;
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
};

int main()
{
    string query = "SELECT id, name, email  FROM users";
    Postgres postgres;
    vector<vector<string> > response = postgres.executeQuery(query);
    postgres.printQueryOutput(response);
    // PRINT EXAMPLE - data can be seen also at https://goo.gl/oeRbiQ
    // 17 Luis Alberto Bragaia luis.bragaia@terra.com.br
    // 18 Mexicano mexicano@palantir.com
    // 20 Pianista vitor.arruda@ga.ita.br
    // 22 rfdgsd fsdf@gfmdl
    return 0;
}
