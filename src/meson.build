project('42sh', 'c',
  version : 'v1.0',
  default_options : [
    'warning_level=3',
    'c_std=c99',
    'c_args=-D_POSIX_SOURCE=200809L',
])

add_project_arguments('-D_POSIX_C_SOURCE=200809L', language: 'c')

inc = include_directories('include')

src = []
src_file = files('main.c')
src += src_file

subdir('ast')
subdir('builtin')
subdir('execute')
subdir('lexer')
subdir('parser')
subdir('utils')

dependencies_temp = [
  parser_lib,
  lexer_lib,
]

exe = executable('42sh',
  sources : src,
  include_directories : inc,
  link_with: dependencies_temp,
)
