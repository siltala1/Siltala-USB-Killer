#include <iostream>
#include <fstream>
#include <unistd.h> //sleep
#include <vector>
#include <memory>
#include <cstring> //strcmp

using namespace std;

string getUsbDevices()
{
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen("lsusb", "r"), pclose);
    if (!pipe) {
        throw runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int createWhiteList()
{
	cout << "Whitelisting current devices\n";
	string result = getUsbDevices();
	ofstream configfile("config");

	if(!configfile.is_open()) {
		cout << "Config file not found\n";
		return 1;
	}

        int i = 0;
        while(i != string::npos)
	{
        	i = result.find("ID ", i+1);

        	if(i != string::npos)
       		{
        		string device = result.substr(i+3,9);

			configfile << device << endl;
        	}
        }

	return 0;
}

		
void Shutdown()
{
	cout << "Unauthorized USB device found" << endl;
	system("poweroff -ff");
}

int main(int argc, char *argv[])
{
	cout << "Siltala USB Killer\n";

	if(argc > 1) {
		cout << argv[0] << argv[1] << endl;
		if(strcmp(argv[1],"whitelist") == 0 ) {
			createWhiteList();
		}
	}

	ifstream configfile("config");

	if(!configfile.is_open()) {
		cout << "Configuration file not found" << endl;
		return 1;
	}

	string line;
	getline(configfile, line);
	int interval = stoi(line);

	cout << "Interval: " << interval << endl;
        cout << "Whitelisted devices:\n";
	
	vector<string> whitelist;

	while(getline(configfile, line))
	{
		cout << line << endl;
		whitelist.push_back(line);
	}
	
	while(true)
	{
		sleep(interval);
		string result = getUsbDevices();

		int i = 0;
		while(i != string::npos) {
			i = result.find("ID ", i+1);

			if(i != string::npos)
			{
				string device = result.substr(i+3,9);
				
				bool found = false;	
				for(int j = 0; j < whitelist.size()-1; j++) {
					if(whitelist[j] == device)
					{
						found = true;
					}
				}
				if(found == false) {
					Shutdown();
				}
			}
		}
	}
}

