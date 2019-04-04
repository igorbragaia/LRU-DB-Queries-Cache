#include<pqxx/pqxx>
#include<iostream>
using namespace std;

int main()
{
    std::string name = "name";
    int id = 0;
    try {
        pqxx::connection c("host='ec2-184-72-238-22.compute-1.amazonaws.com' port='5432' dbname='d12f1ll1geqc36' user='nswsrcytjxtmqv' password='cf4c0209208d76e86dc8eaf42861b0ef9b545c15a316dd8d342b171a3a4e0a40'");
        // pqxx::work w(c);
        //
        // c.prepare("example", "SELECT id  FROM users");
        // pqxx::result r = w.prepared("example")(id).exec();
        // w.commit();
        //
        // std::cout << r.size() << std::endl;
        pqxx::work txn(c);

        pqxx::result R = txn.exec("SELECT id, name, email  FROM users");
        for (pqxx::result::const_iterator cc = R.begin(); cc != R.end(); ++cc) {
           cout << "ID = " << cc[0] << endl;
           cout << "Name = " << cc[1] << endl;
           cout << "Email = " << cc[2] << endl;
        }
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
