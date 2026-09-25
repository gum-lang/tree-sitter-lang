/// <reference types="tree-sitter-cli/dsl" />
// @ts-check

module.exports = grammar({
  name: 'gum',

  extras: $ => [
    /[ \t]/,
    $.comment,
    /\n/,
    /\r\n/,
  ],

  rules: {
    document: $ => repeat($.assignment),

    assignment: $ => seq(
      $.key,
      '=',
      $.value,
    ),

    key: $ => choice(
      $.dotted_key,
      $.single_key,
    ),

    dotted_key: $ => prec.left(seq(
      $.single_key,
      repeat1(seq('.', $.single_key)),
    )),

    single_key: $ => choice(
      $.bare_key,
      $.basic_string,
    ),

    bare_key: $ => /[a-zA-Z_][a-zA-Z0-9_]*/,

    value: $ => choice(
      $.basic_string,
      $.literal_string,
      $.multiline_string,
      $.multiline_literal_string,
      $.number,
      $.boolean,
      $.null,
      $.table,
      $.list,
    ),

    basic_string: $ => seq(
      '"',
      repeat(choice(
        $.escape_sequence,
        /[^\\"\x00-\x1F]/,
      )),
      '"',
    ),

    escape_sequence: $ => seq(
      '\\',
      choice(
        '"',
        '\\',
        'b',
        'f',
        'n',
        'r',
        't',
        $.unicode_sequence,
      ),
    ),

    unicode_sequence: $ => choice(
      seq('u', /[0-9a-fA-F]{4}/),
      seq('U', /[0-9a-fA-F]{8}/),
    ),

    literal_string: $ => seq(
      "'",
      repeat(/[^']/),
      "'",
    ),

    multiline_string: $ => seq(
      '"""',
      optional('\n'),
      repeat(choice(
        $.escape_sequence,
        /[^"\\]/,
        seq('"', optional('"')),
      )),
      '"""',
    ),

    multiline_literal_string: $ => seq(
      "'''",
      optional('\n'),
      repeat(choice(
        /[^']/,
        seq("'", optional("'")),
      )),
      "'''",
    ),

    number: $ => seq(
      optional(choice('+', '-')),
      choice(
        $.float,
        $.integer,
      ),
    ),

    integer: $ => /[0-9]+/,

    float: $ => choice(
      seq(/[0-9]+/, '.', /[0-9]+/, optional($.exp)),
      seq(/[0-9]+/, $.exp),
    ),

    exp: $ => seq(
      choice('e', 'E'),
      optional(choice('+', '-')),
      /[0-9]+/,
    ),

    boolean: $ => choice('true', 'false'),

    null: $ => 'null',

    table: $ => seq(
      '{',
      optional($.table_body),
      '}',
    ),

    table_body: $ => seq(
      $.assignment,
      repeat(seq(',', $.assignment)),
    ),

    list: $ => seq(
      '[',
      optional($.list_body),
      ']',
    ),

    list_body: $ => seq(
      $.value,
      repeat(seq(',', $.value)),
    ),

    comment: $ => seq('#', /.*/),
  },
});
