#ifndef FILE_HANDLING_H
#define FILE_HANDLING_H 1

#include <deque>
#include <map>
#include <string>
#include <vector>

// reads ascii source file into 
std::string read_file_to_buffer(std::ifstream &source_file);

// writes intermediate file for labels
bool write_labels_to_sink(std::map<std::string, int16_t> label_table,
                          std::string header);

// writes intermediate file for compiled program
bool write_program_to_sink(std::vector<int16_t> program,
                           std::string header);

// writes intermediate file for tokens
bool write_tokens_to_sink(std::deque<std::string> tokens,
                          std::string header);

#endif
