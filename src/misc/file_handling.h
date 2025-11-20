#ifndef FILE_HANDLING_H
#define FILE_HANDLING_H 1

#include <deque>
#include <map>
#include <string>

/**
 * @brief generates token file and labels file
 * @details wrapper function of write_tokens_to_sink and write_labels_to_sink
 */
void generate_intermediates(std::string file_header,
                            std::deque<std::string> tokens,
                            std::map<std::string, int16_t> label_table);

/**
 * @brief reads ascii source file into
 */
std::string read_file_to_buffer(std::ifstream &source_file);

/**
 * @brief writes intermediate file for label
 */
bool write_labels_to_sink(std::map<std::string, int16_t> label_table, std::string header);

/**
 * @brief writes intermediate file for compiled progra
 */
bool write_program_to_sink(std::deque<int16_t> program, std::string header);

/**
 * @brief writes intermediate file for token
 */
bool write_tokens_to_sink(std::deque<std::string> tokens, std::string header);

#endif
