/**
 * \file error.h
 *
 * \author angel.castillo
 * \date   Sep 09, 2023
 *
 * Copyright 2023 Biglup Labs
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CARDANO_ERROR_H
#define CARDANO_ERROR_H

/* INCLUDES ******************************************************************/

#include <cardano/export.h>

/* DECLARATIONS **************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief Cardano C library error codes.
 */
typedef enum
{
  /**
   * \brief Successful operation.
   */
  CARDANO_SUCCESS = 0,

  /**
   * \brief Generic error.
   */
  CARDANO_ERROR_GENERIC = 1,

  /**
   * \brief Insufficient buffer size.
   */
  CARDANO_INSUFFICIENT_BUFFER_SIZE = 2,

  /**
   * \brief Operation over a null pointer.
   */
  CARDANO_POINTER_IS_NULL = 3,

  /**
   * \brief Memory could not be allocated.
   */
  CARDANO_MEMORY_ALLOCATION_FAILED = 4,

  /**
   * \brief Out of bounds memory read.
   */
  CARDANO_OUT_OF_BOUNDS_MEMORY_READ = 5,

  /**
   * \brief Out of bounds memory write.
   */
  CARDANO_OUT_OF_BOUNDS_MEMORY_WRITE = 6,

  /**
   * \brief Invalid argument.
   */
  CARDANO_ERROR_INVALID_ARGUMENT = 7,

  /* Encoding errors */

  /**
   * \brief Encoding failure.
   */
  CARDANO_ERROR_ENCODING = 10,

  /**
   * \brief Decoding failure.
   */
  CARDANO_ERROR_DECODING = 11,

  /**
   * \brief Invalid checksum.
   */
  CARDANO_ERROR_CHECKSUM_MISMATCH = 12,

  /* Serialization errors */

  /**
   * \brief The serialization or deserialization process resulted in a loss of precision.
   */
  CARDANO_ERROR_LOSS_OF_PRECISION = 100,

  /* Crypto errors */

  /**
   * \brief The hash size is invalid.
   */
  CARDANO_ERROR_INVALID_BLAKE2B_HASH_SIZE = 200,

  /**
   * \brief The Ed25519 signature size is invalid.
   */
  CARDANO_ERROR_INVALID_ED25519_SIGNATURE_SIZE = 201,

  /**
   * \brief The Ed25519 public key size is invalid.
   */
  CARDANO_ERROR_INVALID_ED25519_PUBLIC_KEY_SIZE = 202,

  /**
   * \brief The Ed25519 private key size is invalid.
   */
  CARDANO_ERROR_INVALID_ED25519_PRIVATE_KEY_SIZE = 203,

  /**
   * \brief The BIP32 public key size is invalid.
   */
  CARDANO_ERROR_INVALID_BIP32_PUBLIC_KEY_SIZE = 204,

  /**
   * \brief The BIP32 private key size is invalid.
   */
  CARDANO_ERROR_INVALID_BIP32_PRIVATE_KEY_SIZE = 205,

  /**
   * \brief The BIP32 derivation index is invalid.
   */
  CARDANO_ERROR_INVALID_BIP32_DERIVATION_INDEX = 206,

  /* CBOR errors */

  /**
   * \brief The CBOR type is unexpected.
   */
  CARDANO_ERROR_UNEXPECTED_CBOR_TYPE = 300,

  /**
   * \brief The CBOR value is of the right type, but the value is invalid (I.E out of range).
   */
  CARDANO_ERROR_INVALID_CBOR_VALUE = 301,

  /**
   * \brief The CBOR array size is invalid.
   */
  CARDANO_ERROR_INVALID_CBOR_ARRAY_SIZE = 302,

  /**
   * \brief The CBOR map size is invalid.
   */
  CARDANO_ERROR_INVALID_CBOR_MAP_SIZE = 303,

  // Address errors

  /**
   * \brief The address type is invalid.
   */
  CARDANO_INVALID_ADDRESS_TYPE = 400,

  /**
   * \brief The address format is invalid.
   */
  CARDANO_INVALID_ADDRESS_FORMAT = 401,

} cardano_error_t;

/**
 * \brief Converts error codes to their human readable form.
 *
 * \param[in] error The error code to get the string representation for.
 * \return Human readable form of the given error code.
 */
CARDANO_NODISCARD
CARDANO_EXPORT const char* cardano_error_to_string(cardano_error_t error);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // CARDANO_ERROR_H