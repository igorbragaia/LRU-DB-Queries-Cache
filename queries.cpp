#include<bits/stdc++.h>
#include<pqxx/pqxx>
using namespace std;

int main()
{
    string host = getenv ("HOST_NODECRUD");
    string dbname = getenv ("DATABASE_NODECRUD");
    string user = getenv ("USER_NODECRUD");
    string password = getenv ("PASSWORD_NODECRUD");
    string connection_str = "host='"+ host + "' port='5432' dbname='"+ dbname +"' user='"+ user +"' password='" + password +"'";

    try {
        pqxx::connection c(connection_str);
        pqxx::work txn(c);
        pqxx::result R = txn.exec("SELECT id, name, email  FROM users");
        for (pqxx::result::const_iterator cc = R.begin(); cc != R.end(); ++cc) {
           cout << "ID = " << cc[0] << endl;
           cout << "Name = " << cc[1] << endl;
           cout << "Email = " << cc[2] << endl;
        }
    }
    catch(const exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
