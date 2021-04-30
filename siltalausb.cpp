#include <iostream>
#include <fstream>
#include <unistd.h>
#include <vector>
#include <memory>

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

void Shutdown()
{
	cout << "Unauthorized USB device found" << endl;
	system("poweroff -ff");
}

int main()
{
	cout << "Siltala USB Killer\n";

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

