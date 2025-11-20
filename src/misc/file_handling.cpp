#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <cstdlib>

#include "file_handling.h"

void generate_intermediates(std::string file_header,
                            std::deque<std::string> tokens,
                            std::map<std::string, int16_t> label_table) {
        bool res_temp;
        res_temp = write_tokens_to_sink(tokens, file_header);
        if (!res_temp) {
                std::cerr << "Failed to open token sink file\n";
                std::exit(1);
        }
        res_temp = write_labels_to_sink(label_table, file_header);
        if (!res_temp) {
                std::cerr << "Failed to open label sink file\n";
                std::exit(1);
        }
}

std::string read_file_to_buffer(std::ifstream &source_file) {
        source_file.seekg(0, std::ios_base::end);
        size_t file_size = source_file.tellg();
        source_file.seekg(0, std::ios_base::beg);
        std::string source_buffer = "";
        for (size_t i = 0; i < file_size; ++i)
                source_buffer += (char)source_file.get();
        source_file.close();
        return source_buffer;
}

bool write_labels_to_sink(std::map<std::string, int16_t> label_table,
                          std::string header) {
        std::ofstream sink_file("intermediate_labels_" + header + ".txt");
        if (sink_file.fail()) {
                return false;
        }
        std::map<std::string, int16_t>::iterator it;
        size_t max_label_size = 0;
        // get formatting sizes
        for (it = label_table.begin(); it != label_table.end(); ++it) {
                size_t label_len = (it->first).length();
                if (label_len > max_label_size)
                        max_label_size = label_len;
        }
        // print
        for (it = label_table.begin(); it != label_table.end(); ++it) {
                sink_file << std::left << std::setw((int)max_label_size);
                sink_file << (it->first) << " = ";
                sink_file << (it->second) << "\n";
        }
        sink_file.close();
        return true;
}

bool write_program_to_sink(std::deque<int16_t> program,
                           std::string header) {
        std::ofstream sink_file("program_" + header + ".bin",
                                std::ios::binary);
        if (sink_file.fail()) {
                return false;
        }
        for (int16_t i : program) {
                // interpret integer as char, and write
                sink_file.write((char*) &i, sizeof(int16_t));
        }

        return true;
}

bool write_tokens_to_sink(std::deque<std::string> tokens,
                          std::string header) {
        std::ofstream sink_file("intermediate_tokens_" + header + ".txt");
        if (sink_file.fail()) {
                return false;
        }
        for (std::string i: tokens) {
                sink_file << i << "\n";
        }
        sink_file.close();
        return true;
}
