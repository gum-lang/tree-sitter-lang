; Indentation for tables
(table
  "{" @indent
  "}" @outdent)

; Indentation for lists
(list
  "[" @indent
  "]" @outdent)

; Indent table body
(table_body) @indent

; Indent list body
(list_body) @indent
