#include <iostream> 
#include <fstream> 
#include <string> 
#include <vector>  
#include <thread> 
#include <mutex>

std::mutex mutex;

class Line {
	std::string name;
	
	int value;
	
	char mod;
	
	static long sum_value;
public:
	Line(std::string name, int value, char mod) : name{ name }, value{ value }, mod{ mod } { sum_value += value; }

	Line() : name{ "undefine" }, value{ 0 }, mod{ ' ' } {}

	std::string getName() const { return name; }

	int getValue() const { return value; }

	int getSum() const { return sum_value; }

	std::string getLine() const { return name + " " + std::to_string(value) + " " + mod; }

	friend std::istream& operator>>(std::istream& in, Line& line);
};

std::istream& operator>> (std::istream& in, Line& line) {
	in >> line.name >> line.value >> line.mod;

	Line::sum_value += line.value;

	return in;
}

long Line::sum_value = 0;

void splitByObjects(std::vector<Line>& lines_split, std::string filename) {
	std::ifstream file(filename);

	while (!file == '\0') {
		Line line;
		
		file >> line;
		
		mutex.lock();
		
		lines_split.push_back(line);
		
		mutex.unlock();
	}

	file.close();
}

std::vector<Line> splitWithStreams() {
	std::vector<Line> lines_split;

	std::vector<std::thread> streams;

	for (int i = 0; i < 4; i++) {
		std::string filename = "file" + std::to_string(i) + ".txt";
		
		streams.push_back(std::thread(splitByObjects, std::ref(lines_split), filename));
	}

	for (int i = 0; i < streams.size(); i++) streams.at(i).join();

	return lines_split;
}

void writeAvgFile(const std::vector<Line>& lines) {
	std::ofstream avg("more_than_avg.txt");

	long average = lines.at(0).getSum() / lines.size();

	for (int i = 0; i < lines.size(); i++) if (lines.at(i).getValue() > average) avg << lines.at(i).getLine() << "\n";

	avg.close();
}

void searchName(const std::vector<Line>& lines_split, std::string search) {
	std::ofstream name("name.txt");

	for (int i = 0; i < lines_split.size(); i++) if (lines_split.at(i).getName() == search) name << lines_split.at(i).getLine() << "\n";

	name.close();
}

int main() {

	std::time_t time = clock();
	
	std::string text = "";

	for (int i = 0; i < 4; i++) {
		std::string filename = "file" + std::to_string(i) + ".txt";
		
		std::string temp_text = "";
		
		std::ifstream text_file(filename);
		
		std::getline(text_file, temp_text, '\0');
		
		text += temp_text;
		
		text_file.close();
	}

	std::ofstream out("output.txt");
	
	out << text;
	
	out.close();

	std::cout << "Time: " << (clock() - time) / 1000.0 << "sec\n";
	
	std::vector<Line> lines = splitWithStreams();

	std::cout << "Enter name: "; std::string search; std::cin >> search;
	
	std::time_t start_find = clock(); searchName(lines, search); std::cout << "Time: " << (clock() - start_find) / 1000.0 << "sec\n";
	
	std::time_t start_avg = clock(); writeAvgFile(lines); std::cout << "Time: " << (clock() - start_avg) / 1000.0 << "sec\n";
}