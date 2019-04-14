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
			for(vector<string> arr:output){
				for(string str:arr)
					cout << str << " ";
				cout << endl;
			}
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
		void operator()(int number, string columns, string table) { 
			cout << endl << "Starting thread: reading data number " << number << endl;        		
			string query = "SELECT " + columns + " FROM " + table;
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

/* ---------------------- Main function -------------------- */
int main()
{
    // Local variables: related to time counting and creation of threads
    struct timeval start, end, startseq, endseq; 
    std::thread threads_read[3];
    std::thread threads_insdata[3];
    std::thread threads_remdata[3];

    // Starting multithread application
    gettimeofday(&start, NULL);
    cout << "Starting time for multithread application" << endl;    

    for(int i=0;i<3;i++){
      threads_read[i] = std::thread(thread_readdata(), i+1, "Id, name", "users");
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
    cout << "Multithread application ended after " << (end.tv_sec + end.tv_usec/1000000)-(start.tv_sec + start.tv_usec/1000000)<< " seconds" << endl;

    // Starting sequential application
    gettimeofday(&startseq, NULL);
    cout << "Starting time for sequential application" << endl;

    askquery_seq('r', "SELECT Id, name from users");
    askquery_seq('w', "INSERT INTO users(name, email) VALUES ('teste_seq1', 'teste_seq1@gmail.com')");
    askquery_seq('w', "DELETE FROM users WHERE Id=90");
    askquery_seq('r', "SELECT Id, name from users");
    askquery_seq('w', "INSERT INTO users(name, email) VALUES ('teste_seq2', 'teste_seq2@gmail.com')");
    askquery_seq('w', "DELETE FROM users WHERE Id=91");
    askquery_seq('r', "SELECT Id, name from users");
    askquery_seq('w', "INSERT INTO users(name, email) VALUES ('teste_seq3', 'teste_seq3@gmail.com')");
    askquery_seq('w', "DELETE FROM users WHERE Id=77");

    // Ending sequential application
    gettimeofday(&endseq, NULL);
    cout << "Sequential application ended after " << (endseq.tv_sec + endseq.tv_usec/1000000.0)-(startseq.tv_sec + startseq.tv_usec/1000000.0)<< " seconds" << endl;

    return 0;
}
