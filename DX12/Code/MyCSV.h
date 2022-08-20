#pragma once

#include <iostream>
#include <fstream>
#include <deque>
#include <string>

using namespace std;

const int MaxLen = 1024;

class MyCSV {
private:
    deque<deque<wstring>> csvfile;
    wchar_t comma;  //区切りとなるカンマ。
public:
    bool Load(string fname);
    wstring w(int y, int x);
    int i(int y, int x);
    int size_y();
    int size_x(int y);
    void SetComma(wchar_t c) { comma = c; }
    MyCSV() { comma = L','; }
    MyCSV(string fname) { comma = L','; Load(fname); }
};

bool MyCSV::Load(string fname) {

    //ワイド文字変換用のロケール設定
    std::locale::global(std::locale("japanese"));

    //ファイル
    wifstream ifs;
    ifs.open(fname.c_str());
    while (!ifs.eof()) {
        wchar_t c = 0;
        wstring line;
        getline(ifs, line);
        line.push_back(comma);
        line.push_back('\0');

        deque<wstring> cell;

        for (int i = 0, k = 0; i < line.size(); i++) {
            if (line[i] == comma) {
                wstring s = line.substr(k, i - k);
                i++;
                k = i;
                cell.push_back(s);
            }
        }

        csvfile.push_back(cell);
    }

    ifs.close();

    return 1;
}

wstring MyCSV::w(int y, int x) {
    if (x < 0 || y < 0) return L"";
    if (y >= csvfile.size()) return L"";
    if (x >= csvfile[y].size()) return L"";

    return csvfile[y][x];
}

int MyCSV::i(int y, int x) {
    wstring s = w(y, x);
    if (s.size() == 0) return -1;
    int n = _wtoi(s.c_str());
    return n;
}

int MyCSV::size_y() {
    return csvfile.size();
}

int MyCSV::size_x(int y) {
    if (y < 0 || y >= csvfile.size()) return -1;
    return csvfile[y].size();
}
