src_file = files('execute.c', 'parse_execute_loop.c', 'execute_tools.c', 'redirection.c', 'pipe.c', 'my_string.c', 'expand_tools.c')
src += src_file

execute_lib = static_library('parse_execute_loop',
  src_file,
  include_directories : inc)
