#ifndef TIRAMISU_WRAPPER_TEST_166_H
#define TIRAMISU_WRAPPER_TEST_166_H

// Define these values for each new test
#define TEST_NAME_STR       "Nonblocking communication"
#define TEST_NUMBER_STR     "166"

// --------------------------------------------------------
// No need to modify anything in the following ------------
// --------------------------------------------------------

#include <tiramisu/utils.h>

#ifdef __cplusplus
extern "C" {
#endif
  int dist_nonblocking(halide_buffer_t *, halide_buffer_t *);

extern const struct halide_filter_metadata_t halide_pipeline_aot_metadata;
#ifdef __cplusplus
}  // extern "C"
#endif

#endif //TIRAMISU_WRAPPER_TEST_166_H