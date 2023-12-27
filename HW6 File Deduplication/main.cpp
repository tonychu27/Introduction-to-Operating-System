#include <iostream>
#include <filesystem>
#include <map>
#include <vector>
#include <openssl/sha.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
#define BUFFSIZE 102000
using namespace std;
using namespace filesystem;
typedef long long ll;


string compute(const string& filePath) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA_CTX sha;
    SHA1_Init(&sha);

    ifstream file(filePath, ifstream::binary);

    char* buffer = new char[BUFFSIZE];

    while (file.good()) {
        file.read(buffer, BUFFSIZE);
        SHA1_Update(&sha, buffer, file.gcount());
    }

    file.close();
    delete[] buffer;

    SHA1_Final(hash, &sha);

    stringstream ss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
        ss << hex << setw(2) << setfill('X') << (int)hash[i];

    // cout << ss.str() << endl;
    return ss.str();
}

void deduplicate(const string& directory) {
    
    map<string, vector<string> > hashing;
    map<ll, vector<string> > group;

    /*** Group the files by size ***/
    for (const auto& entry : recursive_directory_iterator(directory)) {
        if (is_regular_file(entry)) {
            auto fileSize = file_size(entry);
            group[fileSize].push_back(entry.path());
        }
    }

    /*** Group the files by hash ***/
    for (const auto& [size, paths] : group) {
        for (const auto& path : paths) {
            string hash = compute(path);
            hashing[hash].push_back(path);
        }
    }

    for (const auto& [hash, paths] : hashing) {

        const auto& target = paths.front();

        for (size_t i = 1; i < paths.size(); i++) {
            // cout << "Linking: " << linkTarget.c_str() << " and " << paths[i].c_str() << endl;
            if (unlink(paths[i].c_str()) == -1) {
                cerr << "Error unlinking\n";
                return;
            }

            if (link(target.c_str(), paths[i].c_str()) == -1) {
                cerr << "Error creating hard link\n";
                return;
            }

            // cout << "Hard link create successfully!\n";

        }
    }
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        cerr << "No directoay given..\n";
        exit(1);
    }

    string directory(argv[1]);

    deduplicate(directory);

    return 0;
}
