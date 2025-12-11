#ifndef FILE_HANDLING_H
#define FILE_HANDLING_H 1

#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <fstream>

#include "../token_types.h"

/**
 * @brief generates file of program's label_table and filtered_tokens
 */
void generate_intermediate_file(
        const std::string &file_header,
        const std::vector<Token> &tokens,
        const std::map<std::string, int16_t> &label_table
);

/**
 * @brief gets user program, either from file or stdin
 * @details may exit if source_path fails
 */
std::string get_source_buffer(
        const std::string &source_path,
        const bool &use_stdin
);

/**
 * @brief populates final program from input if -b flag is given
 */
void populate_program_from_binary(
        std::vector<int16_t> &program,
        const std::string &filepath
);

/**
 * @brief writes assembled program to sink
 */
bool write_program_to_sink(
        const std::vector<int16_t> &program,
        const std::string &header
);

#endif
