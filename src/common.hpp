#pragma once
#include <format>

/* macro to throw runtime error when not returning VK_SUCCESS */
#define vkc(x) if( x != VK_SUCCESS) do {                                                    \
    VkResult ret = (x);                                                                     \
    if (ret != VK_SUCCESS) {                                                                \
        throw std::runtime_error(std::format("ERROR: {} at {}:{}", #x, __FILE__, __LINE__));\
    }                                                                                       \
} while(0);
