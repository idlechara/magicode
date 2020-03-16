#include "../../include/median/median.h"


/**
 * @brief Selects a median via a random criteria (doesnt select a median at all)
 * 
 * @param target_ptr 
 * @param lenght 
 * @param stride_lenght 
 * @return element_t 
 */
element_t median_random(element_t *target_ptr, size_t lenght, size_t stride_lenght) {
    return 1;
}

/**
 * @brief Selects a median by ordering the elements
 * 
 * @param target_ptr 
 * @param lenght 
 * @param stride_lenght 
 * @return element_t 
 */
element_t median_sorted(element_t *target_ptr, size_t lenght, size_t stride_lenght) {

}

/**
 * @brief Selects a median via Blum Floyd Pratt Rivest Tarjan method using a single core thread.
 * 
 * @param target_ptr 
 * @param lenght 
 * @param stride_lenght 
 * @return element_t 
 */
element_t median_bfprt(element_t *target_ptr, size_t lenght, size_t stride_lenght) {

}

/**
 * @brief Selects a median via Blum Floyd Pratt Rivest Tarjan method via a OpenCL kernels
 * 
 * @param target_ptr 
 * @param lenght 
 * @param stride_lenght 
 * @return element_t 
 */
element_t median_bfprt_opencl(element_t *target_ptr, size_t lenght, size_t stride_lenght) {

}