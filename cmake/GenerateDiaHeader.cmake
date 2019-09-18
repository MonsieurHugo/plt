
# Global cmake target responsible for the generation of all the cpp headers
add_custom_target(generate-headers)

# Fonction that generate cpp header files from dia files using dia2code.
# Dia files must contain only one namespace and must be named by this
# namespace.
#
# Arguments:
#
# dia_file
#   Name of the dia file. Must also be the name of the namespace contained
#   inside the dia file.
#
function(generate_dia_header dia_file)
  # Path to the .dia file used to generate the cpp headers
  get_filename_component(namespace ${dia_file} NAME_WE)
  # Path to the output directory
  set(output_dir ${CMAKE_CURRENT_SOURCE_DIR})

  # Stamp file that take care of the dependency chain
  set(stamp ${CMAKE_CURRENT_BINARY_DIR}/generate_header_${namespace}.stamp)

  # Custom command that generate the cpp headers and create the stamp file
  add_custom_command(
    OUTPUT ${stamp}
    COMMAND $<TARGET_FILE:dia2code> -ns ${namespace} -d ${output_dir} -t cpp ${dia_file}
    COMMAND ${CMAKE_COMMAND} -E touch ${stamp}
    DEPENDS ${dia_file}
    )

  # Create cmake intermediate target to generate the cpp headers
  add_custom_target(generate-header-${namespace} DEPENDS ${stamp})

  # Add the intermediate target as a dependency of the global cmake target for header
  # generation.
  add_dependencies(generate-headers generate-header-${namespace})
endfunction()

# vim: set sw=2 sts=2 et:

