# -----------------------------------------------
# Creates precompiled header files using clang.
# Can be used to speed up autocompletion:
# Create the precompiled header:
#   set(header a.hpp b.hpp ...)
#   add_precompiled_header(somename.pch header ${additional_flags})
# then, include it in other projects:
# .clang_complete
#   -include-pch somename.pch
# -----------------------------------------------
function(add_precompiled_header _output _input _flags)
    ##
    ## At cmake-generation-time
    ##
    # create a new c-file that includes everything.
    set(_compilestr "")
    foreach(_incl ${${_input}})
        set(_compilestr "${_compilestr}\n#include \"${_incl}\"")
    endforeach()
    set(_tmpfile ${CMAKE_CURRENT_BINARY_DIR}/${_output}_all_include.cpp)
    file(WRITE ${_tmpfile} ${_compilestr})


    ## get a list of the current include directories, and parse them to "-I/path/"
    get_property(${_output}_includedirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
    set(${_output}_iflags "")
    foreach(_dir ${${_output}_includedirs})
        list(APPEND ${_output}_iflags "-I${_dir}")
    endforeach()

    ## process CXX_FLAGS: -fopenmp in gcc must be -fopenmp=libomp in clang, if enabled at all..
    string(REPLACE " " ";" _flags_list "${CMAKE_CXX_FLAGS}")
    set(_CLANG_CXX_FLAGS "")
    foreach(_flag ${_flags_list})
        if(_flag MATCHES "-fopenmp")
            # ... just skip it.
            #list(APPEND _CLANG_CXX_FLAGS "-fopenmp=libomp" "-fopenmp")
        else()
            list(APPEND _CLANG_CXX_FLAGS ${_flag})
        endif()
    endforeach()
    message("CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}")
    message("_CLANG_CXX_FLAGS ${_CLANG_CXX_FLAGS}")

    ##
    ## At build time
    ##
    # compile the all_include.cpp to generate a precompiled header
    add_custom_command(
        OUTPUT ${_output}
        COMMAND clang++
        ARGS -xc++-header -Xclang ${_CLANG_CXX_FLAGS} -Wno-everything ${_flags} -o ${_output} ${${_output}_iflags} ${_tmpfile}
        DEPENDS ${${_input}}
        COMMENT "Building precompiled header ${_output}"
    )

    ## add the custom target to actually do build the pch
    add_custom_target(${_output}_pch_target ALL DEPENDS ${_output})
endfunction()


# add_precompiled_header(myprecompiledheader.pch ${CMAKE_CURRENT_LIST_DIR}/src/someinclude.hpp)
