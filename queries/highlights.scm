; Comments
(comment) @comment

; Keys
(bare_key) @property
(single_key
  (basic_string) @property)
(dotted_key
  (single_key) @property
  "." @punctuation)

; Assignment operator
"=" @operator

; Strings
(basic_string) @string
(literal_string) @string
(multiline_string) @string
(multiline_literal_string) @string

; Numbers
(integer) @number
(float) @number
(number) @number

; Booleans
(boolean) @constant.builtin

; Null
(null) @constant.builtin

; Punctuation
"{" @punctuation
"}" @punctuation
"[" @punctuation
"]" @punctuation
"," @punctuation
"." @punctuation
