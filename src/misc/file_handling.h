#ifndef FILE_HANDLING_H
#define FILE_HANDLING_H 1

#include <vector>
#include <map>
#include <string>
#include <fstream>

/**
 * @brief generates token file and labels file
 * @details wrapper function of write_tokens_to_sink and write_labels_to_sink
 */
void generate_intermediates(std::string file_header,
                            std::vector<std::string> tokens,
                            std::map<std::string, int16_t> label_table);

/**
 * @brief gets user program, either from file or stdin
 * @details may exit if source_path fails
 */
std::string get_source_buffer(std::string source_path, bool use_stdin);

/**
 * @brief populates final program from input if -b flag is given
 */
void populate_program_from_binary(
        std::vector<int16_t> &program,
        std::string filepath
);

/**
 * @brief writes intermediate file for label
 */
bool write_labels_to_sink(
        std::map<std::string, int16_t> label_table,
        std::string header
);

/**
 * @brief writes intermediate file for assembled progra
 */
bool write_program_to_sink(std::vector<int16_t> program, std::string header);

/**
 * @brief writes intermediate file for token
 */
bool write_tokens_to_sink(std::vector<std::string> tokens, std::string header);

#endif
