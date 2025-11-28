#include <cstdlib>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>

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

std::string get_source_buffer(std::string source_path, bool use_stdin) {
        std::string source_buffer = "";
        if (use_stdin) {
                std::string aux_string = "";
                do {
                        std::getline(std::cin, aux_string);
                        source_buffer += aux_string + "\n";
                } while (aux_string != "; EOF");
        } else {
                std::ifstream source_file(source_path);
                if (source_file.fail()) {
                        std::cerr << "Failed to open input file\n";
                        std::exit(1);
                }
                source_file.seekg(0, std::ios_base::end);
                size_t file_size = source_file.tellg();
                source_file.seekg(0, std::ios_base::beg);
                for (size_t i = 0; i < file_size; ++i)
                        source_buffer += (char)source_file.get();
                source_file.close();

        }
        return source_buffer;
}

void populate_program_from_binary(
        std::deque<int16_t> &program,
        std::string file_path
) {
        std::ifstream source_bin(file_path, std::ios::binary);
        if (source_bin.fail()) {
                std::cerr << "Failed to open input file\n";
                std::exit(1);
        }
        // move ptr to end of file, read ptr pos, move ptr to start of file
        source_bin.seekg(0, std::ios_base::end);
        size_t file_size = source_bin.tellg();
        source_bin.seekg(0, std::ios::beg);
        for (size_t i = 0; i < file_size; i += 2) {
                int16_t lower = (int16_t)source_bin.get();
                int16_t upper = (int16_t)source_bin.get();
                int16_t final = (upper << 8) | lower;
                program.push_back(final);
        }
}

bool write_labels_to_sink(
        std::map<std::string, int16_t> label_table,
        std::string header
) {
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

bool write_program_to_sink(std::deque<int16_t> program, std::string header) {
        std::string file_path = "program_" + header + ".bin";
        std::ofstream sink_file(file_path, std::ios::binary);
        if (sink_file.fail())
                return false;
        // interpret integer as c style string, and write
        for (int16_t i : program)
                sink_file.write((char*) &i, sizeof(int16_t));
        return true;
}

bool write_tokens_to_sink(std::deque<std::string> tokens, std::string header) {
        std::ofstream sink_file("intermediate_tokens_" + header + ".txt");
        if (sink_file.fail())
                return false;
        for (std::string i: tokens)
                sink_file << i << "\n";
        sink_file.close();
        return true;
}
