#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
namespace fs = filesystem;

struct file_data_t {
    string filename;
    string path;
    int lines;
};

vector<file_data_t> _data;

int get_lines_code(string dir) {
    int lines = 0;
    int x;
    int flines;
    string str(1024, '\0');
    int readed;

    fs::current_path(dir);

    fstream file;

    for (auto f : fs::recursive_directory_iterator("./")) {
        if (!f.is_regular_file()) continue;

        file.open(f.path(), ios_base::in);
        if (!file) {
            cout << "error open read: " << f.path().filename() << endl;
            continue;
        }
        flines = 0;
        bool isNotEmptyLine = false;
        char filterFlag = 0;
        while (!file.eof()) {
            readed = file.read(str.data(), str.size()).gcount();
            for (x = 0; x < readed; ++x) {
                char& c = str[x];
                if (filterFlag) {
                    switch (filterFlag & 0x6) {
                        case 0x2:
                            if (c == '\n') filterFlag = 0;  // clear
                            break;
                        case 0x4:
                            if ((filterFlag & 0x8) == 0 && c == '*') {
                                filterFlag |= 0x8;
                            } else {
                                if (((filterFlag & 0x2) == 0) && c == '*')
                                    filterFlag |= 0x2;  // clear
                                else if (c == '/')
                                    filterFlag = 0;
                            }
                            break;
                        default:
                            if (c == '/') {
                                filterFlag |= 0x2;  // single line
                            } else if (c == '*') {
                                filterFlag |= 0x4;  // multi line
                            } else
                                filterFlag = 0;  // not comment line
                    }
                    continue;
                }
                if (c == '/') {
                    filterFlag = 0x1;
                } else {
                    if (!isNotEmptyLine && c != ' ' && c != '\t' && c != '\n') isNotEmptyLine = true;
                    if (c == '\n' && isNotEmptyLine) {
                        isNotEmptyLine = false;
                        ++flines;
                    }
                }
            }
        }
        file.close();
        cout << "file: " << f.path().filename() << ", lines: " << flines << endl;
        lines += flines;
    }

    return lines;
}

void showVersion() {
    std::cout << "Code Lines v1.0.0" << std::endl;
    exit(EXIT_SUCCESS);
}

void showHelp() {
    const char helps[] = {
        "Show lines from codes \n"
        "usage: <dir1, dir2, ... >\n"
        "\tshow help    --help\n"
        "\tshow version --version\n"};
    std::cout << helps << std::endl;
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) {
    if (argc == 1) {
        showHelp();
    }

    if (!std::strncmp(argv[1], "--h", 3)) showHelp();
    if (!std::strncmp(argv[1], "--v", 3)) showVersion();

    int lines = 0;
    for (; argc > 1; --argc) lines = get_lines_code(argv[argc - 1]);
    cout << "Summarize lines: " << lines << endl;

    return 0;
}
