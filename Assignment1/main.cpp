#include "main.h"
#include "processData.h"
#include <chrono> //-----------------------

using namespace std;
using namespace std::chrono; //-------------
void usage();


int main(int argc, char** argv) {
    //----------------------------------------
    auto start = high_resolution_clock::now();
    //----------------------------------------
	ifstream inFile;
	ofstream outFile;
	
	if (argc != 3)
		usage();
	
	inFile.open(argv[1]);
	if (!inFile) {
        cout << "Unable to open file " << argv[1];
        exit(1); // terminate with error
    }
	
    outFile.open(argv[2]);	
	if (!outFile) {
        cout << "Unable to open file " << argv[2];
        exit(1); // terminate with error
    }
    
    ProcessData * data = new ProcessData();
	
    string line;
    while (getline(inFile, line)) {
    	outFile << data->process(line) << endl;
    }


    
    
    
    inFile.close();
    outFile.close();
    delete data;

    //-------------------------------------------------------
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start); 
    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;
    //-------------------------------------------------------
	return 0;
}
void usage() {
	cout << "Usage: main <input file> <output file>" << endl;
	exit(1);
}