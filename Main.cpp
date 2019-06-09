#include<iostream>
#include<limits.h>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<map>
#include<cmath>
#include<utility>

#define TRUE 1
#define FALSE 0
#define STATION 600
#define INF 10000

using namespace::std;

typedef struct Way {
	double distance;
	int way[STATION];
	int size;
	int found;
}Way;

double graph[STATION][STATION];

double average[STATION];
double deviation[STATION];

map<string, int> str2int;
map<string, string> unitLine;
string index2str[STATION];
vector<string> index2line[STATION];

void top10(double *arr, int arr_size, int *rank, double *value_arr);

void readFile();
vector<string> parsing(string str);
vector<string> split(string str, char delim, vector<string> elems);
vector<string> split(string str, char delim);

double computeAverage(vector<double> list);
double computeDiviation(vector<double> list, int index);

void dijkstra(int start, int flag, Way **path);
void printWay(int start, int des, int flag, Way **path);


int main() {
	readFile();

	int n;
	
	int mode;		// 0: Auto, 1: Master
	cin >> n >> mode;

	string dest;
	if (mode == 1) {
		cin >> dest;
	}

	string *from = new string[n];
	Way **path = new Way*[n];
	for (int i = 0; i < n; i++) {
		path[i] = new Way[STATION];
	}

	int *start = new int[n];
	for (int i = 0; i < n; i++) {
		cin >> from[i];
		start[i] = str2int[from[i]];
		dijkstra(start[i], i, path);
	}

	for (int j = 0; j < STATION; j++) {
		vector<double> temp;
		for (int k = 0; k < n; k++) {
			temp.push_back(path[k][j].distance);
		}
		average[j] = computeAverage(temp);
		deviation[j] = computeDiviation(temp, j);
	}

	int rank[10] = { 0 };
	double ave[10];

	int dev_rank[10];
	double dev[10];
	
	if (mode == 0) {
		top10(average, STATION, rank, ave);
		
		for (int i = 0; i < 10; i++) {
			dev_rank[i] = rank[i];
			dev[i] = deviation[rank[i]];
		}

		for (int i = 0; i < 10; i++) {
			int temp_index = dev_rank[i];
			int temp_rank_index = i;
			double temp_value = dev[i];
			for (int j = i; j < 10; j++) {

				if (temp_value > dev[j]) {
					temp_value = dev[j];
					temp_index = dev_rank[j];
					temp_rank_index = j;
				}
			}
			dev_rank[temp_rank_index] = dev_rank[i];
			dev[temp_rank_index] = dev[i];
			dev_rank[i] = temp_index;
			dev[i] = temp_value;
		}

		for (int i = 0; i < 10; i++) {
			cout << endl;
			cout << i + 1 << endl;
			cout << index2str[dev_rank[i]] << endl;
			cout << ave[i] << "," << dev[i] << endl;

			cout << endl;
			for (int j = 0; j < n; j++) {
				cout << index2str[start[j]] << endl;
				cout << path[j][dev_rank[i]].distance << endl;
				printWay(start[j], dev_rank[i], j, path);
				cout << endl;
			}
		}
	} 

	if (mode == 1) {
		int destCode = str2int[dest];
		cout << endl;
		cout << index2str[destCode] << endl;
		cout << average[destCode] << endl;
		cout << endl;
		for (int j = 0; j < n; j++) {
			cout << index2str[start[j]] << endl;
			cout << path[j][destCode].distance << endl;
			printWay(start[j], destCode, j, path);
			cout << endl;
		}
	}
	return 0;
}

void top10(double *arr, int arr_size,int *rank, double *value_arr) {
	for (int i = 0; i < 10; i++) {
		double m = INF;
		int m_index = -1;
		for (int j = 0; j < arr_size; j++) {
			if (i != 0 && (arr[j] <= value_arr[i - 1]) ) {
				continue;
			}
			else {
				if (arr[j] < m) {
					m = arr[j];
					m_index = j;
				}
			}
		}
		rank[i] = m_index;
		value_arr[i] = m;
	}
}



double computeAverage(vector<double> list) {
	double std_average = 0;
	for (int i = 0; i < list.size(); i++) {
		std_average += list[i];
	}
	return std_average / list.size();
}

double computeDiviation(vector<double> list, int index) {
	double std_diviation = 0;
	
	for (int i = 0; i < list.size(); i++) {
		double t = (list[i] - average[index]);
		std_diviation += pow(t, 2);
	}
	std_diviation /= list.size();
	std_diviation = pow(std_diviation, 0.5);

	return std_diviation;
}

void readFile() {

	for (int i = 0; i < STATION; i++) {
		for (int j = 0; j < STATION; j++) {
			if (i != j) {
				graph[i][j] = INF;
			}
			else {
				graph[i][j] = 0;
			}
		}
	}
	string fpath = "edge_int.txt";
	ifstream openFile(fpath);
	if (openFile.is_open()) {

		string line;
		vector<string> token;
		while (getline(openFile, line)) {
			token = parsing(line);
			string line = token[0];
			int from = stoi(token[1]);
			int to = stoi(token[2]);
			double timeDist = stod(token[3]);
			string key1 = token[1] + token[2];
			string key2 = token[2] + token[1];
			unitLine.insert(make_pair(key1, line));
			unitLine.insert(make_pair(key2, line));
			index2line[from].push_back(line);
			index2line[to].push_back(line);

			graph[to][from] = timeDist;
			graph[from][to] = timeDist;
		}
		openFile.close();
	}
	string path1 = "vertex.txt";
	ifstream open2File(path1);
	if (open2File.is_open()) {
		string line;
		int count = 0;
		while (getline(open2File, line)) {
			index2str[count] = line;
			str2int.insert(make_pair(line, count));
			count++;
		}
		open2File.close();
	}

}
void dijkstra(int start, int flag, Way **path)
{
	double min;

	int roc = start;
	int k;

	path[flag][start].way[0] = start;

	for (int i = 0; i < STATION; i++) {
		path[flag][start].way[i] = -1;
	}
	for (int i = 0; i < STATION; i++)
	{
		path[flag][i].distance = graph[start][i];
		path[flag][i].size = 1;
		path[flag][i].found = FALSE;
		path[flag][i].way[0] = start;
	}
	path[flag][start].found = TRUE;
	path[flag][start].size++;

	int trans_count[STATION] = { 0 };

	for (int i = 0; i < STATION; i++) {

		min = INF;
		for (int j = 0; j < STATION; j++)
		{
			if (!path[flag][j].found && min > path[flag][j].distance)
			{
				min = path[flag][j].distance;
				roc = j;
			}
		}

		path[flag][roc].found = TRUE;
		path[flag][roc].size++;
		path[flag][roc].way[path[flag][roc].size - 1] = roc;

		for (int j = 0; j < STATION; j++) {
			if (!path[flag][j].found) {
				if (path[flag][j].distance > (min + graph[roc][j])) {
					path[flag][j].distance = min + graph[roc][j] + 0.7;
					path[flag][j].size = path[flag][roc].size;
					for (k = 0; k < path[flag][roc].size; k++) {
						path[flag][j].way[k] = path[flag][roc].way[k];
					}
				}

			}
		}
	}
}
void printWay(int start, int des, int flag, Way **path)
{

	if (start == des) {
		cout << index2str[path[flag][des].way[0]] << endl;
		return;
	}
	int from = path[flag][des].way[0];
	int to = path[flag][des].way[1];
	string key = to_string(from) + to_string(to);
	string line = unitLine[key];

	cout << index2str[path[flag][des].way[0]] << " -> (" << line << ") ";

	for (int i = 0; i < path[flag][des].size; i++)
	{
		if (i != 0) {
			string temp_line;
			from = path[flag][des].way[i - 1];
			to = path[flag][des].way[i];
			key = to_string(from) + to_string(to);
			temp_line = unitLine[key];
			if (line != temp_line) {
				cout << "-> " << index2str[path[flag][des].way[i - 1]] << " ";
				cout << "-> (" << line << "-> " << temp_line << ") ";
				line = temp_line;
			}
		}
		if (i == path[flag][des].size - 1)
			cout << "-> " << index2str[path[flag][des].way[i]];
	}
	cout << endl;
}

vector<string> parsing(string str) {
	vector<string> tokens;
	tokens = split(str, ' ', tokens);
	return tokens;
}
vector<string> split(string str, char delim, vector<string> elems) {
	stringstream ss(str);
	string item;

	while (getline(ss, item, delim))
		elems.push_back(item);
	return elems;
}
vector<string> split(string str, char delim) {
	vector<string> elems;
	split(str, delim, elems);
	return elems;
}
