#ifndef INCLUDE_INTERNAL_PEMUTIL_PEM_H
#define INCLUDE_INTERNAL_PEMUTIL_PEM_H

#include "../../../utils/src/util.h"
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Parse X.509 certificates in PEM format.
 *
 * \param bytes [in] stb array of bytes in PEM format.
 * \param err [out] Variable to get information in the event of error.
 * \returns stb array of X.509 certificate object pointers. Must be freed
 * iterations over the array using X509_free and then arrfree.
 */
X509 **pemutil_ParseCertificates(const byte *bytes, err_t *err);

/**
 * Parse a private key in PEM format.
 *
 * \param bytes [in] stb array of bytes in PEM format.
 * \param err [out] Variable to get information in the event of error.
 * \returns Parsed private key object pointer if successful, <tt>NULL</tt>
 * otherwise. Must be freed using EVP_PKEY_free.
 */
EVP_PKEY *pemutil_ParsePrivateKey(const byte *bytes, err_t *err);

/**
 * Encodes a private key in DER format.
 *
 * \param pkey [in] private key object pointer.
 * \param byles_len [out] length of the encoded object in bytes.
 * \param err [out] Variable to get information in the event of error.
 * \returns Encoded private key array of bytes if successful, <tt>NULL</tt>
 * otherwise.
 */
byte *pemutil_EncodePrivateKey(EVP_PKEY *pkey, int *bytes_len, err_t *err);

/**
 * Encodes a stb array of X.509 certificates in DER format.
 *
 * \param certs [in] stb array ox X.509 certificate object pointers.
 * \returns Encoded stb array of stb arrays of bytes if successful,
 * <tt>NULL</tt> otherwise. Must be freed iterating on the array using
 * arrfree, and then arrfree on the outer stb array.
 */
byte **pemutil_EncodeCertificates(X509 **certs);

#ifdef __cplusplus
}
#endif

#endif
