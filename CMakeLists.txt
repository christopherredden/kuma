cmake_minimum_required(VERSION 3.2)
set( CMAKE_VERBOSE_MAKEFILE on )
project(kuma)

find_package(LLVM REQUIRED CONFIG)

#message(WARNING "Found LLVM ${LLVM_PACKAGE_VERSION}")
#message(WARNING "Using LLVMConfig.cmake in: ${LLVM_DIR}")
#message(WARNING "LLVM libs: ${LLVM_LIBRARY_DIRS}")
#message(WARNING "LLVM libs: ${LLVM_AVAILABLE_LIBS}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(OTHER_FILES
    kuma.l
    kuma.y
    )

set(SOURCE_FILES
    kuma.h
    kuma_parser.cpp
    kuma_parser.h
    kuma_lexer.h
    kuma_lexer.cpp
    kuma_string.h
    kuma_string.cpp
    main.cpp
    nodes.h
    nodes.cpp
    codegen.h
    codegen.cpp
    )

set(LLVM_CORE
    LLVMCore
    LLVMSupport
    )

set(LLVM_EXECUTIONENGINE
    LLVMExecutionEngine
    LLVMRuntimeDyld
    LLVMObject
    LLVMMCParser
    LLVMBitReader
    LLVMMC
    LLVMCore
    LLVMSupport
    )

set(LLVM_MCJIT
    LLVMMCJIT
    LLVMTarget
    LLVMExecutionEngine
    LLVMRuntimeDyld
    LLVMObject
    LLVMMCParser
    LLVMBitReader
    LLVMMC
    LLVMCore
    LLVMSupport
    )

set(LLVM_NATIVE
    LLVMX86Disassembler
    LLVMX86AsmParser
    LLVMX86CodeGen
    LLVMSelectionDAG
    LLVMAsmPrinter
    LLVMCodeGen
    LLVMScalarOpts
    LLVMProfileData
    LLVMInstCombine
    LLVMTransformUtils
    LLVMipa
    LLVMAnalysis
    LLVMTarget
    LLVMX86Desc
    LLVMObject
    LLVMMCParser
    LLVMBitReader
    LLVMMCDisassembler
    LLVMX86Info
    LLVMX86AsmPrinter
    LLVMMC
    LLVMX86Utils
    LLVMCore
    LLVMSupport
    )

add_executable(kuma ${SOURCE_FILES})
target_link_libraries(kuma ${LLVM_CORE} ${LLVM_EXECUTIONENGINE} ${LLVM_MCJIT} ${LLVM_NATIVE})