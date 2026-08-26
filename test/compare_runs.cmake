if(NOT DEFINED TEST_BINARY OR NOT DEFINED TEST_ALGORITHM OR
   NOT DEFINED TEST_INSTANCE OR NOT DEFINED TEST_EXPECTED_DIR)
    message(FATAL_ERROR "TEST_BINARY, TEST_ALGORITHM, TEST_INSTANCE and TEST_EXPECTED_DIR are required")
endif()

set(test_work_dir "${CMAKE_CURRENT_BINARY_DIR}/test-runs/${TEST_ALGORITHM}")
file(REMOVE_RECURSE "${test_work_dir}")
file(MAKE_DIRECTORY "${test_work_dir}")

execute_process(
    COMMAND "${TEST_BINARY}" "${TEST_ALGORITHM}" "${TEST_INSTANCE}"
    WORKING_DIRECTORY "${test_work_dir}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error_output
)

if(NOT result EQUAL 0)
    message(FATAL_ERROR "Program failed with code ${result}\n${output}\n${error_output}")
endif()

get_filename_component(instance_name "${TEST_INSTANCE}" NAME)
set(failures "")
foreach(run RANGE 1 10)
    set(actual_file
        "${test_work_dir}/output_files/${run}/solution_${TEST_ALGORITHM}_${instance_name}.txt")
    set(expected_file
        "${TEST_EXPECTED_DIR}/${run}/solution_${TEST_ALGORITHM}_${instance_name}.txt")

    if(NOT EXISTS "${expected_file}")
        string(APPEND failures
            "Missing expected solution for run ${run}: ${expected_file}\n")
        continue()
    endif()
    if(NOT EXISTS "${actual_file}")
        string(APPEND failures
            "Missing generated solution for run ${run}: ${actual_file}\n")
        continue()
    endif()

    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E compare_files "${actual_file}" "${expected_file}"
        RESULT_VARIABLE compare_result
    )
    if(NOT compare_result EQUAL 0)
        string(APPEND failures
            "Solution mismatch for ${TEST_ALGORITHM}, ${instance_name}, run ${run}\n")
    endif()
endforeach()

if(failures)
    message(FATAL_ERROR "Test failures:\n${failures}")
endif()

message(STATUS "All 10 ${TEST_ALGORITHM} runs match the expected solutions for ${instance_name}")
