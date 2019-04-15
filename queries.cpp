/* Libraries */
#include<bits/stdc++.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/time.h>
#include<time.h>
#include<pqxx/pqxx>
#include<mutex>
#include<thread>
using namespace std;

/* -------------------- Class LRU_cache -------------------- */
/* Implements a hashtable to save queries recetly used.      */
/* Criteria for substitution is Least Recently Used (LRU).   */
class LRUcache{
	private:
		unordered_map<string, vector<vector<string>>> hashtable; // hashtable
	public:
		bool hasKey(string query){ // returns true if query is found in hash table
			return (hashtable.find(query) != hashtable.end());
		}

		vector<vector<string>> get(string query){ // returns correspondent output for a query
			if(hasKey(query))
				return hashtable[query];
			else
				return vector<vector<string>>();
		}

		void put(string query, vector<vector<string>> &response){ // puts new element in hash table
			hashtable[query] = response;
		}
};

/* -------------------- Class Postgres --------------------- */
/* Implements a connection to PostgresSQL database.          */
/* The goal here is to execute a query and get its output.   */
/* If query type is 'write', it returns the whole database.  */
class Postgres{
	private:
		string connection_str; // holds settings to access PostgreSQL database
		bool cache; // variable that determines if cache option is enabled
		LRUcache lru_cache; // variable that implements cache (a hashtable, in this case)
	public:
		Postgres(bool _cache){ // constructor: establishes connection to database
			try{
				string host = getenv ("HOST_NODECRUD");
				string dbname = getenv ("DATABASE_NODECRUD");
				string user = getenv ("USER_NODECRUD");
				string password = getenv ("PASSWORD_NODECRUD");
				connection_str = "host='"+ host + "' port='5432' dbname='"+ dbname +"' user='"+ user +"' password='" + password +"'";
			}
    			catch(const std::exception &e){
      				cerr << "ERROR: environment variables not set!" << std::endl;
    			}
    			cache = _cache;
  		}

		vector<vector<string>> executeQuery(char type, string query){ // executes a query, returning its output
			if(type == 'r' and cache and lru_cache.hasKey(query)){
				return lru_cache.get(query);
			}

			vector<vector<string>> output;
			vector<string> single_output;
			try {
				pqxx::connection c(connection_str);
				pqxx::work txn(c);
				pqxx::result R;
				if(type == 'w'){
					pqxx::connection d(connection_str);
					pqxx::work write(d);
        				write.exec(query);
					write.commit();
					R = txn.exec("SELECT * from users");
				}
				else R = txn.exec(query);
       				for (pqxx::result::const_iterator cc = R.begin(); cc != R.end(); ++cc) {
         				single_output = vector<string>();
         				for(auto ccc:cc)
            					single_output.push_back(to_string(ccc));
          				output.push_back(single_output);
        			}
       				if(cache)
         				lru_cache.put(query, output);
    			}
    			catch(const exception &e){
     				cerr << e.what() << endl;
    			}
    			return output;
		}

		void printQueryOutput(vector<vector<string>> &output){ // prints an output from a query
			cout << "Printing query output..." << endl;
			for(vector<string> arr:output){
				for(string str:arr)
					cout << str << " ";
				cout << endl;
			}
			cout << endl;
		}
};

/* -------------------- Global variables -------------------- */
std::mutex mtx; // mutex locker variable
Postgres postgres_seq(true); // postgress acces for sequential application
Postgres postgres_thr(true); // postgress acces for multithread application

/* ------------------ Functions 'ask query' ----------------- */
/* Function responsable to call executeQuery from Postgres.   */
/* There are two functions: one for multithread and other for */
/* sequential.                                                */
/* It implements mutex lock() for multithread scenario.       */
void askquery_thr(char type, string query) {
	mtx.lock();
	vector<vector<string>> response;
	response = ::postgres_thr.executeQuery(type, query);
      	::postgres_thr.printQueryOutput(response);
	mtx.unlock();
}

void askquery_seq(char type, string query) {
	vector<vector<string>> response;
	response = ::postgres_seq.executeQuery(type, query);
      	::postgres_seq.printQueryOutput(response);
}

/* ----------------- Class thread_readdata ----------------- */
/* Thread in charge of queries like 'SELECT x from y'.       */
class thread_readdata {
	public:
		void operator()(int number, string query) {
			cout << endl << "Starting thread: reading data number " << number << endl;
      			askquery_thr('r', query);
			cout << "Ending thread: reading data number " << number << endl << endl;
		}
};

/* ----------------- Class thread_insertdata --------------- */
/* Thread in charge of queries like 'INSERT INTO x VALUES y'.*/
class thread_insertdata {
	public:
		void operator()(int number, string name, string email, string table) {
			cout << endl << "Starting thread: inserting data number " << number << endl;
			string query = "INSERT INTO " + table + "(name,email)" + " VALUES" + "('" + name + "', '" + email + "')";
      			askquery_thr('w', query);
			cout << "Ending thread: inserting data number " << number << endl << endl;
    		}
};

/* ----------------- Class thread_removedata --------------- */
/* Thread in charge of queries like 'DELETE FROM x WHERE y'. */
class thread_removedata {
	public:
		void operator()(int number, int id, string table) {
			cout << endl << "Starting thread: removing data number " << number << endl;
			string query = "DELETE FROM " + table + " WHERE " + "Id=" + std::to_string(id);
			askquery_thr('w', query);
      			cout << "Ending thread: removing data number " << number << endl << endl;
    		}
};

/* ----------------- Function subsetsWithDup --------------- */
/* Class responsable for creating the subsets of the set of  */
/* all the columns names. in charge of                       */
vector<vector<string>> subsetsWithDup(vector<string>& nums) { 
	vector<int> bitmask{0}, aux;
	vector<string> current;
	set<vector<string>>response_set;
	vector<vector<string>>response;
	for(int i = 0; i < (int)nums.size(); i++){
		aux = bitmask;
		for(int j = 0; j < (int)aux.size(); j++)
			bitmask.push_back(aux[j] | 1<<i);
	}
	for(int i = 0; i < (int)bitmask.size(); i++){
		current = vector<string>();
		for(int j = 0; j < (int)nums.size(); j++)
			if(1 << j & bitmask[i])
				current.push_back(nums[j]);
		sort(current.begin(), current.end());				
		response_set.insert(current);
	}
	for(set<vector<string>>::iterator it=response_set.begin(); it!=response_set.end();it++)
		response.push_back(*it);
	return response;
}

/* ---------------- Function getSELECTqueries -------------- */
/* Returns vector with all the possibilities of 'SELECT      */
/* queries' and possibly prints it.                           */
vector<string> getSELECTqueries(bool print, vector<string> cols, string table){ // 
	vector<string> queries;
	vector<vector<string>> subsets = subsetsWithDup(cols);
	string query;
	for(vector<string>set:subsets){
		query = "SELECT";
		for(int i=0;i<(int)set.size();i++){
			query += " " + set[i];
			if(i != (int)set.size()-1)
				query += ",";
		}
		query += " FROM " + table;
		queries.push_back(query);
	}
	if(print){
		cout << "PRINTING " << queries.size() << " different queries for the specified database!" << endl;
		for(string query:queries)
			cout << query << endl;
	}
	cout << endl;
	return queries;
}

/* ---------------------- Main function -------------------- */
int main()
{
    // Initial declarations: variables related to time counting, possible 'SELECT queries' of database and creation of threads
    struct timeval start, end, startseq, endseq;
    std::thread threads_read[3];
    std::thread threads_insdata[3];
    std::thread threads_remdata[3];
    vector<string> columns, queries;
    string database;
    int randomquery, queries_size;

    // Initial assignments: related to getting random 'SELECT queries'
    columns = {"Id", "Name", "Email"};
    database = "users";
    queries = getSELECTqueries(true, columns, database);
    queries_size = queries.size();

    // Starting multithread application
    gettimeofday(&start, NULL);
    cout << "Starting time for multithread application" << endl;

    for(int i=0;i<3;i++){
      randomquery =  rand() % queries_size;
      threads_read[i] = std::thread(thread_readdata(), i+1, queries[randomquery]);
      threads_insdata[i] = std::thread(thread_insertdata(), i+1, "teste_thr" + std::to_string(i+1), "teste_thr" + std::to_string(i+1) + "@gmail.com", "users");
      threads_remdata[i] = std::thread(thread_removedata(), i+1, 103+i, "users");
    }

    for(int i=0;i<3;i++){
      threads_read[i].join();
      threads_insdata[i].join();
      threads_remdata[i].join();
    }

    // Ending multithread application
    gettimeofday(&end, NULL);
    cout << "Multithread application ended after " << (end.tv_sec + end.tv_usec/1000000.0)-(start.tv_sec + start.tv_usec/1000000.0)<< " seconds" << endl;

    // Starting sequential application
    gettimeofday(&startseq, NULL);
    cout << "Starting time for sequential application" << endl;

    for (int i=0; i<3; i++){
			cout << endl << "Starting job: reading data number " << i+1 << endl;
			randomquery =  rand() % queries_size;			
			askquery_seq('r', queries[randomquery]);
			cout << "Ending job: reading data number " << i+1 << endl << endl;

			cout << endl << "Starting job: inserting data number " << i+1 << endl;
		        askquery_seq('w', "INSERT INTO users(name, email) VALUES ('teste_seq" + std::to_string(i+1) + "', 'teste_seq" + std::to_string(i+1) + "@gmail.com')");
			cout << "Ending job: inserting data number " << i+1 << endl << endl;

		        cout << endl << "Starting job: inserting data number " << i+1 << endl;
			askquery_seq('w', "DELETE FROM users WHERE Id=90");
			cout << "Ending job: removing data number " << i+1 << endl << endl;
    }

    // Ending sequential application
    gettimeofday(&endseq, NULL);
    cout << "Sequential application ended after " << (endseq.tv_sec + endseq.tv_usec/1000000.0)-(startseq.tv_sec + startseq.tv_usec/1000000.0)<< " seconds" << endl;

    return 0;
}
