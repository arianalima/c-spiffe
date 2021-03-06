#ifndef INCLUDE_INTERNAL_JWTUTIL_UTIL_H
#define INCLUDE_INTERNAL_JWTUTIL_UTIL_H

#include "../../../utils/src/util.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Copy a stb string hash map of jwt authorities.
 *
 * \param hash [in] stb string hash map of public keys.
 * \returns A copy of hash. Must be freed iterating over hash using
 * EVP_PKEY_free and then shfree on the map.
 */
map_string_EVP_PKEY *jwtutil_CopyJWTAuthorities(map_string_EVP_PKEY *hash);

/**
 * Compares two stb string hash maps of jwt authorities.
 *
 * \param hash1 [in] First stb string hash map of public keys.
 * \param hash2 [in] Second stb string hash map of public keys.
 * \returns <tt>true</tt> if the maps have the same values for the same
 * keys, <tt>false</tt> otherwise.
 */
bool jwtutil_JWTAuthoritiesEqual(map_string_EVP_PKEY *hash1,
                                map_string_EVP_PKEY *hash2);

#ifdef __cplusplus
}
#endif

#endif