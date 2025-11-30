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
void generate_intermediates(
        const std::string file_header,
        const std::vector<std::string> tokens,
        const std::map<std::string, int16_t> label_table
);

/**
 * @brief gets user program, either from file or stdin
 * @details may exit if source_path fails
 */
std::string get_source_buffer(
        const std::string source_path,
        const bool use_stdin
);

/**
 * @brief populates final program from input if -b flag is given
 */
void populate_program_from_binary(
        std::vector<int16_t> &program,
        const std::string filepath
);

/**
 * @brief writes intermediate file for label
 */
bool write_labels_to_sink(
        const std::map<std::string, int16_t> label_table,
        const std::string header
);

/**
 * @brief writes intermediate file for assembled progra
 */
bool write_program_to_sink(
        const std::vector<int16_t> program,
        const std::string header
);

/**
 * @brief writes intermediate file for token
 */
bool write_tokens_to_sink(
        const std::vector<std::string> tokens,
        const std::string header
);

#endif
