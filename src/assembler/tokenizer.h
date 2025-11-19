#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <cstddef> // size_t
#include <deque>
#include <string>

/**
 * @brief finds the size of the next token to extract
 * @details tokenize_buffer helper function. Will return SIZE_MAX if
 * it hits an invalid token, but won't signal an error immediatly. Also,
 * if a literal string is not terminated before a newline, the token won't
 * be added at all, which allows a reliable "MISSING_ARGUMENTS" for SPRINT
 */
size_t get_token_len(size_t buffer_idx, const std::string source_buffer);

/**
 * @brief checks if i matches equivalent regex pattern: [$%|a-zA-Z0-9_-]
 * @details tokenize_buffer helper function
 */
bool is_identifier_char(const char i);

/**
 * @brief moves buffer_idx in tokenize_buffer past comments
 * @details tokenize_buffer helper function
 */
void skip_comment(size_t &buffer_idx, const std::string source_buffer);

/**
 * @brief moves buffer_idx in tokenize_buffer past whitespace
 * @details tokenize_buffer helper function
 */
void skip_whitespace(size_t &buffer_idx, const std::string source_buffer);

/**
 * @brief tokenizes user assembly program into tokens
 * @brief invalid tokens won't signal an error immediately, but will cause
 * MISSING_ARGUMENTS or INVALID_ATOM down the line
 */
std::deque<std::string> tokenize_buffer(const std::string source_buffer);

#endif
