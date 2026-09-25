#include <tree_sitter/parser.h>
#include <string.h>
#include <stdlib.h>

enum TokenType {
  MULTILINE_STRING,
  LITERAL_STRING,
  ERROR_SENTINEL,
};

void *tree_sitter_gum_external_scanner_create(void) {
  return calloc(1, 1);
}

void tree_sitter_gum_external_scanner_destroy(void *payload) {
  free(payload);
}

unsigned tree_sitter_gum_external_scanner_serialize(void *payload, char *buffer) {
  return 0;
}

void tree_sitter_gum_external_scanner_deserialize(void *payload, const char *buffer, unsigned length) {
  (void)payload;
  (void)buffer;
  (void)length;
}

static void advance(TSLexer *lexer) {
  lexer->advance(lexer, false);
}

bool tree_sitter_gum_external_scanner_scan(void *payload, TSLexer *lexer, const bool *valid_symbols) {
  (void)payload;

  // Skip leading whitespace
  while (lexer->lookahead == ' ' || lexer->lookahead == '\t' || lexer->lookahead == '\r') {
    advance(lexer);
  }

  // Multiline string: """..."""
  if (valid_symbols[MULTILINE_STRING] && lexer->lookahead == '"') {
    advance(lexer);
    if (lexer->lookahead == '"') {
      advance(lexer);
      if (lexer->lookahead == '"') {
        advance(lexer);
        // Skip optional newline after opening """
        if (lexer->lookahead == '\n') {
          advance(lexer);
        } else if (lexer->lookahead == '\r') {
          advance(lexer);
          if (lexer->lookahead == '\n') {
            advance(lexer);
          }
        }

        // Scan content until closing """
        while (lexer->lookahead != 0) {
          if (lexer->lookahead == '"') {
            advance(lexer);
            if (lexer->lookahead == '"') {
              advance(lexer);
              if (lexer->lookahead == '"') {
                advance(lexer);
                lexer->mark_end(lexer);
                lexer->result_symbol = MULTILINE_STRING;
                return true;
              }
            }
          } else if (lexer->lookahead == '\\') {
            advance(lexer);
            if (lexer->lookahead == '\n') {
              advance(lexer);
              while (lexer->lookahead == ' ' || lexer->lookahead == '\t') {
                advance(lexer);
              }
            } else if (lexer->lookahead == '\r') {
              advance(lexer);
              if (lexer->lookahead == '\n') {
                advance(lexer);
                while (lexer->lookahead == ' ' || lexer->lookahead == '\t') {
                  advance(lexer);
                }
              }
            } else if (lexer->lookahead != 0) {
              advance(lexer);
            }
          } else {
            advance(lexer);
          }
        }
        lexer->mark_end(lexer);
        lexer->result_symbol = MULTILINE_STRING;
        return true;
      }
    }
    return false;
  }

  // Literal string (single or multiline): '...' or '''...'''
  if (valid_symbols[LITERAL_STRING] && lexer->lookahead == '\'') {
    advance(lexer);

    // Check if this is a multiline literal string '''
    bool is_multiline = (lexer->lookahead == '\'');
    if (is_multiline) {
      advance(lexer);
      is_multiline = (lexer->lookahead == '\'');
      if (is_multiline) {
        advance(lexer);
        // Skip optional newline after opening '''
        if (lexer->lookahead == '\n') {
          advance(lexer);
        } else if (lexer->lookahead == '\r') {
          advance(lexer);
          if (lexer->lookahead == '\n') {
            advance(lexer);
          }
        }

        // Scan multiline content until closing '''
        while (lexer->lookahead != 0) {
          if (lexer->lookahead == '\'') {
            advance(lexer);
            if (lexer->lookahead == '\'') {
              advance(lexer);
              if (lexer->lookahead == '\'') {
                advance(lexer);
                lexer->mark_end(lexer);
                lexer->result_symbol = LITERAL_STRING;
                return true;
              }
            }
          } else if (lexer->lookahead == '\\') {
            advance(lexer);
            if (lexer->lookahead == '\n') {
              advance(lexer);
              while (lexer->lookahead == ' ' || lexer->lookahead == '\t') {
                advance(lexer);
              }
            } else if (lexer->lookahead == '\r') {
              advance(lexer);
              if (lexer->lookahead == '\n') {
                advance(lexer);
                while (lexer->lookahead == ' ' || lexer->lookahead == '\t') {
                  advance(lexer);
                }
              }
            } else if (lexer->lookahead != 0) {
              advance(lexer);
            }
          } else {
            advance(lexer);
          }
        }
        lexer->mark_end(lexer);
        lexer->result_symbol = LITERAL_STRING;
        return true;
      }
    }

    // Single-line literal string: '...'
    // is_multiline is false here, and we've consumed the first '
    // If we saw a second ', we consumed it but it's not a triple
    if (is_multiline) {
      // We consumed '' but not ''' - treat as empty single-line string
      // The second ' is actually the closing quote
      lexer->mark_end(lexer);
      lexer->result_symbol = LITERAL_STRING;
      return true;
    }

    // Scan single-line content until closing '
    while (lexer->lookahead != 0 && lexer->lookahead != '\'' && lexer->lookahead != '\n' && lexer->lookahead != '\r') {
      if (lexer->lookahead == '\\') {
        advance(lexer);
        if (lexer->lookahead != 0 && lexer->lookahead != '\n' && lexer->lookahead != '\r') {
          advance(lexer);
        }
      } else {
        advance(lexer);
      }
    }

    if (lexer->lookahead == '\'') {
      advance(lexer);
      lexer->mark_end(lexer);
      lexer->result_symbol = LITERAL_STRING;
      return true;
    }

    // Unterminated string - still return what we have
    lexer->mark_end(lexer);
    lexer->result_symbol = LITERAL_STRING;
    return true;
  }

  return false;
}
