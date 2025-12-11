#include <cstdlib>
#include <vector>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>

#include "../token_types.h"
#include "file_handling.h"

void generate_intermediate_file(
        const std::string &file_header,
        const std::vector<Token> &tokens,
        const std::map<std::string, int16_t> &label_table
) {
        std::ofstream sink_file("intermediate_" + file_header + ".txt");
        if (sink_file.fail()) {
                std::cerr << "Failed to open token sink file\n";
                std::exit(1);
        }
        // print tokens and their types
        for (const Token &i: tokens) {
                sink_file << std::left << std::setw(20);
                switch (i.type) {
                case T_INTEGER_LIT:
                        sink_file << "integer literal";  break;
                case T_LABEL_DEF:
                        sink_file << "label definition"; break;
                case T_LABEL_REF:
                        sink_file << "label reference";  break;
                case T_RAM_ADDR:
                        sink_file << "ram addr";         break;
                case T_MNEMONIC:
                        sink_file << "mnemonic";         break;
                case T_REGISTER:
                        sink_file << "register";         break;
                case T_STACK_OFF:
                        sink_file << "stack offset";     break;
                case T_STRING_LIT:
                        sink_file << "string literal";   break;
                }
                sink_file << std::left << std::setw(25) << i.data;
                sink_file << " (" << i.line_num << ")\n";
        }
        std::map<std::string, int16_t>::const_iterator it;
        size_t max_label_size = 0;
        // get formatting sizes
        for (it = label_table.cbegin(); it != label_table.cend(); ++it) {
                size_t label_len = (it->first).length();
                if (label_len > max_label_size)
                        max_label_size = label_len;
        }
        // print labels and their addresses
        for (it = label_table.cbegin(); it != label_table.cend(); ++it) {
                sink_file << std::left << std::setw((int)max_label_size);
                sink_file << (it->first) << " = ";
                sink_file << (it->second) << "\n";
        }
        sink_file.close();
}

std::string get_source_buffer(
        const std::string &source_path,
        const bool &use_stdin
) {
        std::string source_buffer = "";
        if (use_stdin) {
                std::string aux_string = "";
                do {
                        std::getline(std::cin, aux_string);
                        source_buffer += aux_string + "\n";
                } while (aux_string != "");
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
        std::vector<int16_t> &program,
        const std::string &file_path
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

bool write_program_to_sink(
        const std::vector<int16_t> &program,
        const std::string &header
) {
        std::string file_path = "program_" + header + ".bin";
        std::ofstream sink_file(file_path, std::ios::binary);
        if (sink_file.fail())
                return false;
        // interpret integer as c style string, and write
        for (int16_t i : program)
                sink_file.write((char*) &i, sizeof(int16_t));
        return true;
}
