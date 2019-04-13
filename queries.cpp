#include<bits/stdc++.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/time.h>
#include<time.h>
#include<pqxx/pqxx>
#include<mutex>
#include<thread>
using namespace std;

std::mutex mtx;

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

		vector<vector<string>> executeQuery(char type, string query){
			if(type == 'r' and cache and lru_cache.hasKey(query)){
				log.push_back(clock());
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

		int avgClockTicks(){
			int sum=0;
			for(int i=1;i<(int)log.size();i++)
				sum += log[i] - log[0];
			return sum/(int)log.size();
		}

		void enableCache(){
			cache = true;
		}
};

void askquery(char type, string query) {
	mtx.lock(); 
	Postgres *postgres = new Postgres(true);
	vector<vector<string>> response;
	response = postgres->executeQuery(type, query);
      	postgres->printQueryOutput(response);
	mtx.unlock();
}

class thread_readdata { 
	public:
		void operator()(int number, string columns, string table) { 
			cout << endl << "Starting thread: reading data number " << number << endl;        		
			string query = "SELECT " + columns + " FROM " + table;
      			askquery('r', query);
			cout << "Ending thread: reading data number " << number << endl << endl;
		} 
};

class thread_insertdata { 
	public:
		void operator()(int number, string name, string email, string table) { 
			cout << endl << "Starting thread: inserting data number " << number << endl;        		
			string query = "INSERT INTO " + table + "(name,email)" + " VALUES" + "('" + name + "', '" + email + "')";
      			askquery('w', query);
			cout << "Ending thread: inserting data number " << number << endl << endl;
    		} 
};

class thread_removedata { 
	public:
		void operator()(int number, int id, string table) {          		
			cout << endl << "Starting thread: removing data number " << number << endl;
			string query = "DELETE FROM " + table + " WHERE " + "Id=" + std::to_string(id);
			askquery('w', query);
      			cout << "Ending thread: removing data number " << number << endl << endl;
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
      cout << "Directory created" << endl << endl;

    struct timeval start, end;
    std::thread threads_read[3];
    std::thread threads_insdata[3];
    std::thread threads_remdata[3];

    gettimeofday(&start, NULL);
    cout << "Starting time for multithread application" << endl;    

    for(int i=0;i<3;i++){
      threads_read[i] = std::thread(thread_readdata(), i+1, "*", "users");
      threads_insdata[i] = std::thread(thread_insertdata(), i+1, "teste", "teste@gmail.com", "users");
      threads_remdata[i] = std::thread(thread_removedata(), i+1, 54+i, "users");
    }

    for(int i=0;i<3;i++){
      threads_read[i].join();
      threads_insdata[i].join();
      threads_remdata[i].join();
    }

    gettimeofday(&end, NULL);
    cout << "Multihtread application ended after " << (end.tv_sec + end.tv_usec/1000000)-(start.tv_sec + start.tv_usec/1000000)<< " seconds" << endl;

    //postgres->writeLog(path);
    //cout << "Average clock ticks: " << postgres->avgClockTicks() << endl;
    
    return 0;
}
