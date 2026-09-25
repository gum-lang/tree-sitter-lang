{
  "targets": [
    {
      "target_name": "tree_sitter_gum_binding",
      "dependencies": [
        "<!@(node -p \"require('node-addon-api').targets\")"
      ],
      "include_dirs": [
        "src"
      ],
      "sources": [
        "bindings/node/binding.cc",
        "src/parser.c"
      ],
      "cflags_c": [
        "-std=c11"
      ],
      "configurations": {
        "Release": {
          "cflags": ["-O3"],
          "xcode_settings": {
            "OTHER_CFLAGS": ["-O3"]
          }
        }
      }
    }
  ]
}
