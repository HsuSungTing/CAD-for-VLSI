#ifndef LIBERTY_PARSER_H
#define LIBERTY_PARSER_H

#include <string>
#include <vector>

using namespace std;

struct gate_info {
    double A1_capcitance;
    double A2_capcitance;
    vector<vector<double>> cell_rise_TB;
    vector<vector<double>> cell_fall_TB;
    vector<vector<double>> rise_transision_TB;
    vector<vector<double>> fall_transision_TB;
};

struct liberty_file {
    vector<double> index_1; // output_net_capacitance
    vector<double> index_2; // input_transition_time
    struct gate_info NOR;
    struct gate_info INV;
    struct gate_info NAND;
};

vector<vector<string>> readBlocksFromFile(const string& filename);
vector<vector<string>> extractBlocks(const vector<string>& lines);
vector<double> extractAndSplit(string input);
bool startsWithTrimmed(const string& line, const string& keyword);
bool isClosingBrace(const string& line);

void build_index_vec(struct liberty_file &file, vector<vector<string>> result);
void build_NOR_TB(struct liberty_file &file, vector<string> result);
void build_INV_TB(struct liberty_file &file, vector<string> result);
void build_NAND_TB(struct liberty_file &file, vector<string> result);
void build_liberty_file(struct liberty_file &file);
void show_liberty_file(struct liberty_file file);

#endif // LIBERTY_PARSER_H
