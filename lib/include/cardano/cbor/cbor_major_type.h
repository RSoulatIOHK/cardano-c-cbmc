/**
 * \file cbor_reader_state.h
 *
 * \author angel.castillo
 * \date   Sep 12, 2023
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

#ifndef CARDANO_CBOR_MAJOR_TYPE_H
#define CARDANO_CBOR_MAJOR_TYPE_H

/* DECLARATIONS **************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief Represents CBOR Major Types as defined in RFC 7049 section 2.1.
 *
 * These major types are used to identify the type of data in a CBOR data item.
 */
typedef enum
{
  /**
   * \brief An unsigned integer.
   *
   * Range: 0 to 2^64-1 inclusive. The value of the encoded item is the argument itself.
   */
  CBOR_MAJOR_TYPE_UNSIGNED_INTEGER = 0,

  /**
   * \brief A negative integer.
   *
   * Range: -2^64 to -1 inclusive. The value of the item is -1 minus the argument.
   */
  CBOR_MAJOR_TYPE_NEGATIVE_INTEGER = 1,

  /**
   * \brief A byte string.
   *
   * The number of bytes in the string is equal to the argument.
   */
  CBOR_MAJOR_TYPE_BYTE_STRING = 2,

  /**
   * \brief A text string encoded as UTF-8.
   *
   * Refer to Section 2 and RFC 3629. The number of bytes in the string is equal to the argument.
   */
  CBOR_MAJOR_TYPE_UTF8_STRING = 3,

  /**
   * \brief An array of data items.
   *
   * Also known as lists, sequences, or tuples. A "CBOR sequence" is slightly different (see RFC 8742).
   * The argument specifies the number of data items in the array.
   */
  CBOR_MAJOR_TYPE_ARRAY = 4,

  /**
   * \brief A map of pairs of data items.
   *
   * Also known as tables, dictionaries, hashes, or objects (in JSON).
   */
  CBOR_MAJOR_TYPE_MAP = 5,

  /**
   * \brief A tagged data item ("tag").
   *
   * Tag number ranges from 0 to 2^64-1 inclusive. The enclosed data item (tag content) follows the head.
   */
  CBOR_MAJOR_TYPE_TAG = 6,

  /**
   * \brief Simple values, floating-point numbers, and the "break" stop code.
   */
  CBOR_MAJOR_TYPE_SIMPLE = 7,

  /**
   * \brief Undefined major type.
   */
  CBOR_MAJOR_TYPE_UNDEFINED = 0xFFFFFFFF
} cbor_major_type_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // CARDANO_CBOR_MAJOR_TYPE_H