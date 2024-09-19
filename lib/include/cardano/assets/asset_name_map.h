/**
 * \file asset_name_map.h
 *
 * \author angel.castillo
 * \date   Sep 05, 2024
 *
 * Copyright 2024 Biglup Labs
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

#ifndef CARDANO_ASSET_NAME_MAP_H
#define CARDANO_ASSET_NAME_MAP_H

/* INCLUDES ******************************************************************/

#include <cardano/assets/asset_name.h>
#include <cardano/assets/asset_name_list.h>
#include <cardano/cbor/cbor_reader.h>
#include <cardano/cbor/cbor_writer.h>
#include <cardano/error.h>
#include <cardano/export.h>

/* DECLARATIONS **************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief Represents a map of asset name to coin amount.
 */
typedef struct cardano_asset_name_map_t cardano_asset_name_map_t;

/**
 * \brief Creates and initializes a new instance of a asset name map.
 *
 * This function allocates and initializes a new instance of \ref cardano_asset_name_map_t,
 * representing a map structure. It returns an error code to indicate the success or failure of the operation.
 *
 * \param[out] asset_name_map A pointer to a pointer to a \ref cardano_asset_name_map_t object. Upon successful
 *                        initialization, this will point to a newly created \ref cardano_asset_name_map_t
 *                        object. This object represents a "strong reference" to the asset_name_map,
 *                        fully initialized and ready for use. The caller is responsible for managing
 *                        the lifecycle of this object. Specifically, once the asset_name_map is no longer
 *                        needed, the caller must release it by calling \ref cardano_asset_name_map_unref.
 *
 * \return \ref CARDANO_SUCCESS if the asset_name_map was successfully created, or an appropriate error code
 *         indicating the failure reason.
 *
 * Usage Example:
 * \code{.c}
 * cardano_asset_name_map_t* asset_name_map = NULL;
 *
 * // Attempt to create a new asset_name_map
 * cardano_error_t result = cardano_asset_name_map_new(&asset_name_map);
 *
 * if (result == CARDANO_SUCCESS)
 * {
 *   // Use the asset_name_map
 *
 *   // Once done, ensure to clean up and release the asset_name_map
 *   cardano_asset_name_map_unref(&asset_name_map);
 * }
 * \endcode
 */
CARDANO_NODISCARD
CARDANO_EXPORT cardano_error_t
cardano_asset_name_map_new(cardano_asset_name_map_t** asset_name_map);

/**
 * \brief Creates a asset name map from a CBOR reader.
 *
 * This function parses CBOR data using a provided \ref cardano_cbor_reader_t and constructs a \ref cardano_asset_name_map_t object.
 * It assumes that the CBOR reader is set up correctly and that the CBOR data corresponds to the structure expected for a asset_name_map.
 *
 * \param[in] reader A pointer to an initialized \ref cardano_cbor_reader_t that is ready to read the CBOR-encoded asset_name_map data.
 * \param[out] asset_name_map A pointer to a pointer of \ref cardano_asset_name_map_t that will be set to the address
 *                        of the newly created asset_name_map object upon successful decoding.
 *
 * \return A \ref cardano_error_t value indicating the outcome of the operation. Returns \ref CARDANO_SUCCESS
 *         if the asset_name_map was successfully created, or an appropriate error code if an error occurred.
 *
 * \note If the function fails, the last error can be retrieved by calling \ref cardano_cbor_reader_get_last_error with the reader.
 *       The caller is responsible for freeing the created \ref cardano_asset_name_map_t object by calling
 *       \ref cardano_asset_name_map_unref when it is no longer needed.
 *
 * Usage Example:
 * \code{.c}
 * cardano_cbor_reader_t* reader = cardano_cbor_reader_new(cbor_data, data_size);
 * cardano_asset_name_map_t* asset_name_map = NULL;
 *
 * cardano_error_t result = cardano_asset_name_map_from_cbor(reader, &asset_name_map);
 *
 * if (result == CARDANO_SUCCESS)
 * {
 *   // Use the asset_name_map
 *
 *   // Once done, ensure to clean up and release the asset_name_map
 *   cardano_asset_name_map_unref(&asset_name_map);
 * }
 * else
 * {
 *   const char* error = cardano_cbor_reader_get_last_error(reader);
 *   printf("Failed to decode asset_name_map: %s\n", error);
 * }
 *
 * cardano_cbor_reader_unref(&reader); // Cleanup the CBOR reader
 * \endcode
 */
CARDANO_NODISCARD
CARDANO_EXPORT cardano_error_t
cardano_asset_name_map_from_cbor(cardano_cbor_reader_t* reader, cardano_asset_name_map_t** asset_name_map);

/**
 * \brief Serializes a asset name map into CBOR format using a CBOR writer.
 *
 * This function serializes the given \ref cardano_asset_name_map_t object using a \ref cardano_cbor_writer_t.
 *
 * \param[in] asset_name_map A constant pointer to the \ref cardano_asset_name_map_t object that is to be serialized.
 * \param[out] writer A pointer to a \ref cardano_cbor_writer_t where the CBOR serialized data will be written.
 *                    The writer must already be initialized and ready to accept the data.
 *
 * \return Returns \ref CARDANO_SUCCESS if the serialization is successful. If the \p asset_name_map or \p writer
 *         is NULL, returns \ref CARDANO_POINTER_IS_NULL.
 *
 * Usage Example:
 * \code{.c}
 * cardano_asset_name_map_t* asset_name_map = ...;
 * cardano_cbor_writer_t* writer = cardano_cbor_writer_new();
 *
 * if (writer)
 * {
 *   cardano_error_t result = cardano_asset_name_map_to_cbor(asset_name_map, writer);
 *
 *   if (result == CARDANO_SUCCESS)
 *   {
 *     // Use the writer's buffer containing the serialized data
 *   }
 *   else
 *   {
 *     const char* error_message = cardano_cbor_writer_get_last_error(writer);
 *     printf("Serialization failed: %s\n", error_message);
 *   }
 *
 *   cardano_cbor_writer_unref(&writer);
 * }
 *
 * cardano_asset_name_map_unref(&asset_name_map);
 * \endcode
 */
CARDANO_NODISCARD
CARDANO_EXPORT cardano_error_t cardano_asset_name_map_to_cbor(
  const cardano_asset_name_map_t* asset_name_map,
  cardano_cbor_writer_t*          writer);

/**
 * \brief Retrieves the length of the asset_name_map.
 *
 * This function returns the number of key-value pairs contained in the specified asset_name_map.
 *
 * \param[in] asset_name_map A constant pointer to the \ref cardano_asset_name_map_t object for which
 *                       the length is to be retrieved.
 *
 * \return The number of key-value pairs in the asset_name_map. Returns 0 if the asset_name_map is NULL.
 *
 * Usage Example:
 * \code{.c}
 * cardano_asset_name_map_t* asset_name_map = cardano_asset_name_map_new();
 *
 * // Populate the asset_name_map with key-value pairs
 *
 * size_t length = cardano_asset_name_map_get_length(asset_name_map);
 * printf("The length of the asset_name_map is: %zu\n", length);
 *
 * cardano_asset_name_map_unref(&asset_name_map);
 * \endcode
 */
CARDANO_NODISCARD
CARDANO_EXPORT size_t cardano_asset_name_map_get_length(const cardano_asset_name_map_t* asset_name_map);

/**
 * \brief Retrieves the value associated with a given key in the asset name map.
 *
 * This function retrieves the value associated with the specified key in the provided asset name map.
 * It returns the value through the output parameter `element`. If the key is not found in the asset name map,
 * the output parameter `element` will be set to NULL.
 *
 * \param[in] asset_name_map A constant pointer to the \ref cardano_asset_name_map_t object from which
 *                       the value is to be retrieved.
 * \param[in] key The key whose associated value is to be retrieved from the asset_name_map.
 * \param[out] element A pointer to a variable where the retrieved value will be stored. If the key
 *                     is found in the asset name map, this variable will be set to the associated value.
 *                     If the key is not found, this variable will be set to NULL.
 *
 * \return \ref CARDANO_SUCCESS if the value associated with the key was successfully retrieved, or
 *         an appropriate error code if the input parameters are invalid or any other error occurs.
 *
 * Usage Example:
 * \code{.c}
 * cardano_asset_name_map_t* asset_name_map = cardano_asset_name_map_new();
 *
 * // Populate the asset_name_map with key-value pairs
 *
 * cardano_asset_name_t* key = ...; // Create a asset_name object representing the key
 * int64_t value = 0;
 * cardano_error_t result = cardano_asset_name_map_get(asset_name_map, key, &value);
 *
 * if (result == CARDANO_SUCCESS && value != NULL)
 * {
 *   // Use the retrieved value
 *   // ...
 * }
 * else
 * {
 *   // Handle error or key not found
 * }
 *
 * cardano_asset_name_unref(&key); // Clean up the key resource
 * cardano_asset_name_map_unref(&asset_name_map);
 * \endcode
 */
CARDANO_NODISCARD
CARDANO_EXPORT cardano_error_t cardano_asset_name_map_get(cardano_asset_name_map_t* asset_name_map, cardano_asset_name_t* key, int64_t* element);

/**
 * \brief Inserts a key-value pair into the asset name map.
 *
 * This function inserts the specified key-value pair into the provided asset name map.
 *
 * \param[in] asset_name_map A constant pointer to the \ref cardano_asset_name_map_t object where
 *                       the key-value pair is to be inserted.
 * \param[in] key The key to be inserted into the asset name map. The caller is responsible for managing
 *                the lifecycle of the key object.
 * \param[in] value The value to be associated with the key and inserted into the asset_name_map.
 *                  The caller is responsible for managing the lifecycle of the value object.
 *
 * \return \ref CARDANO_SUCCESS if the key-value pair was successfully inserted, or an appropriate
 *         error code if the input parameters are invalid or any other error occurs.
 *
 * \remark If the key already exists in the asset name map, the value associated with the key will be updated.
 *
 * Usage Example:
 * \code{.c}
 * cardano_asset_name_map_t* asset_name_map = cardano_asset_name_map_new();
 *
 * // Create key and value objects
 * cardano_asset_name_t* key = ...;
 * int64_t value = 0;
 *
 * // Insert the key-value pair into the asset_name_map
 * cardano_error_t result = cardano_asset_name_map_insert(asset_name_map, key, value);
 *
 * if (result != CARDANO_SUCCESS)
 * {
 *   // Handle insertion failure
 * }
 *
 * // Clean up key and value objects
 * cardano_asset_name_unref(&key);
 *
 * cardano_asset_name_map_unref(&asset_name_map);
 * \endcode
 */
CARDANO_NODISCARD
CARDANO_EXPORT cardano_error_t cardano_asset_name_map_insert(cardano_asset_name_map_t* asset_name_map, cardano_asset_name_t* key, int64_t value);

/**
 * \brief Retrieves the keys from the asset name map.
 *
 * This function retrieves all the keys from the provided asset name map and returns them as a list.
 * The caller is responsible for managing the lifecycle of the returned list by calling
 * \ref cardano_asset_name_list_t when it is no longer needed.
 *
 * \param[in] asset_name_map A constant pointer to the \ref cardano_asset_name_map_t object from which
 *                       the keys are to be retrieved.
 * \param[out] keys A pointer to a variable where the retrieved keys will be stored as a list.
 *                  If successful, this variable will be set to point to the list of keys.
 *                  The caller is responsible for managing the lifecycle of this list.
 *                  It must be released by calling \ref cardano_asset_name_list_t when no longer needed.
 *
 * \return \ref CARDANO_SUCCESS if the keys were successfully retrieved, or an appropriate
 *         error code if the input parameters are invalid or any other error occurs.
 *
 * Usage Example:
 * \code{.c}
 * cardano_asset_name_map_t* asset_name_map = cardano_asset_name_map_new();
 *
 * // Populate the asset_name_map with key-value pairs
 *
 * cardano_asset_name_list_t* keys = NULL;
 * cardano_error_t result = cardano_asset_name_map_get_keys(asset_name_map, &keys);
 *
 * if (result == CARDANO_SUCCESS)
 * {
 *   // Use the list of keys
 *   // Keys must also be freed if retrieved from the list
 *
 *   // Once done, ensure to clean up and release the keys list
 *   cardano_asset_name_list_unref(&keys);
 * }
 * else
 * {
 *   // Handle error
 * }
 *
 * cardano_asset_name_map_unref(&asset_name_map);
 * \endcode
 */
CARDANO_NODISCARD
CARDANO_EXPORT cardano_error_t cardano_asset_name_map_get_keys(cardano_asset_name_map_t* asset_name_map, cardano_asset_name_list_t** keys);

/**
 * \brief Retrieves the asset name at a specific index from the asset name map.
 *
 * This function retrieves the asset name at the specified index from the asset name map.
 *
 * \param[in] asset_name_map Pointer to the asset name map object.
 * \param[in] index The index of the asset name to retrieve.
 * \param[out] asset_name On successful retrieval, this will point to the asset name
 *                        at the specified index. The caller is responsible for managing the lifecycle
 *                        of this object. Specifically, once the asset name is no longer needed,
 *                        the caller must release it by calling \ref cardano_asset_name_unref.
 *
 * \return \ref cardano_error_t indicating the outcome of the operation. Returns \ref CARDANO_SUCCESS
 *         if the asset name was successfully retrieved, or an appropriate error code
 *         indicating the failure reason.
 *
 * Usage Example:
 * \code{.c}
 * cardano_asset_name_map_t* asset_name_map = NULL;
 * cardano_asset_name_t* asset_name = NULL;
 * size_t index = 0; // Index of the key hash to retrieve
 *
 * // Assume asset_name_map is initialized properly
 *
 * cardano_error_t result = cardano_asset_name_map_get_key_at(asset_name_map, index, &asset_name);
 *
 * if (result == CARDANO_SUCCESS)
 * {
 *   // Use the asset_name
 *
 *   // Once done, ensure to clean up and release the asset_name
 *   cardano_asset_name_unref(&asset_name);
 * }
 * else
 * {
 *   // Handle the error
 * }
 * \endcode
 */
CARDANO_NODISCARD
CARDANO_EXPORT cardano_error_t cardano_asset_name_map_get_key_at(
  const cardano_asset_name_map_t* asset_name_map,
  size_t                          index,
  cardano_asset_name_t**          asset_name);

/**
 * \brief Retrieves the withdrawal amount at a specific index from the asset name map.
 *
 * This function retrieves the withdrawal amount at the specified index from the asset name map.
 *
 * \param[in] asset_name_map Pointer to the asset name map object.
 * \param[in] index The index of the withdrawal amount to retrieve.
 * \param[out] amount On successful retrieval, this will point to the withdrawal amount
 *                    at the specified index.
 *
 * \return \ref cardano_error_t indicating the outcome of the operation. Returns \ref CARDANO_SUCCESS
 *         if the withdrawal amount was successfully retrieved, or an appropriate error code
 *         indicating the failure reason.
 *
 * Usage Example:
 * \code{.c}
 * cardano_asset_name_map_t* asset_name_map = NULL;
 * int64_t amount = 0;
 * size_t index = 0; // Index of the withdrawal amount to retrieve
 *
 * // Assume asset_name_map is initialized properly
 *
 * cardano_error_t result = cardano_asset_name_map_get_value_at(asset_name_map, index, &amount);
 *
 * if (result == CARDANO_SUCCESS)
 * {
 *   // Use the amount
 * }
 * else
 * {
 *   // Handle the error
 * }
 * \endcode
 */
CARDANO_NODISCARD
CARDANO_EXPORT cardano_error_t cardano_asset_name_map_get_value_at(
  const cardano_asset_name_map_t* asset_name_map,
  size_t                          index,
  int64_t*                        amount);

/**
 * \brief Retrieves the asset name and withdrawal amount at the specified index.
 *
 * This function retrieves the asset name and withdrawal amount from the proposed withdrawal amounts
 * at the specified index.
 *
 * \param[in]  asset_name_map    Pointer to the proposed withdrawal amounts object.
 * \param[in]  index             The index at which to retrieve the key-value pair.
 * \param[out] asset_name On successful retrieval, this will point to the asset name at the specified index.
 *                            The caller is responsible for managing the lifecycle of this object and should release it using
 *                            \ref cardano_asset_name_unref when it is no longer needed.
 * \param[out] amount On successful retrieval, this will point to the withdrawal amount at the specified index.
 *
 * \return \ref cardano_error_t indicating the outcome of the operation. Returns \ref CARDANO_SUCCESS
 *         if the key-value pair was successfully retrieved, or an appropriate error code indicating the failure reason.
 *
 * Usage Example:
 * \code{.c}
 * cardano_asset_name_map_t* asset_name_map = NULL;
 * // Assume asset_name_map is initialized properly
 *
 * size_t index = 0;
 * cardano_asset_name_t* asset_name = NULL;
 * int64_t amount = 0;
 *
 * cardano_error_t result = cardano_asset_name_map_get_key_value_at(asset_name_map, index, &asset_name, &amount);
 *
 * if (result == CARDANO_SUCCESS)
 * {
 *   if (asset_name != NULL && amount != NULL)
 *   {
 *     // Use the asset name and withdrawal amount
 *   }
 *   else
 *   {
 *     printf("Key-value pair not set at index %zu.\n", index);
 *   }
 * }
 * else
 * {
 *   // Handle error
 *   printf("Failed to get key-value pair at index %zu.\n", index);
 * }
 *
 * // Clean up
 * cardano_asset_name_map_unref(&asset_name_map);
 * cardano_asset_name_unref(&asset_name);
 * \endcode
 */
CARDANO_NODISCARD
CARDANO_EXPORT cardano_error_t cardano_asset_name_map_get_key_value_at(
  const cardano_asset_name_map_t* asset_name_map,
  size_t                          index,
  cardano_asset_name_t**          asset_name,
  int64_t*                        amount);

/**
 * \brief Combines two asset name maps by adding the quantities of assets with the same asset names.
 *
 * This function adds the asset quantities from two \ref cardano_asset_name_map_t objects. It combines the quantities
 * of assets that have the same asset names. If an asset name exists in both maps, their quantities are summed.
 * If an asset name exists in only one of the maps, its quantity is copied to the result.
 *
 * \param[in] lhs A constant pointer to the first \ref cardano_asset_name_map_t object.
 * \param[in] rhs A constant pointer to the second \ref cardano_asset_name_map_t object.
 * \param[out] result On successful execution, this will point to a newly created \ref cardano_asset_name_map_t object
 *             that represents the combined result of both input asset name maps.
 *             The result is dynamically allocated and must be freed by the caller using \ref cardano_asset_name_map_unref
 *             when it is no longer needed.
 *
 * \return \ref cardano_error_t indicating the outcome of the operation. Returns \ref CARDANO_SUCCESS if the assets were
 *         successfully combined, or an appropriate error code indicating the failure reason,
 *         such as \ref CARDANO_POINTER_IS_NULL if any of the input pointers are NULL.
 *
 * Usage Example:
 * \code{.c}
 * const cardano_asset_name_map_t* lhs = ...; // Assume lhs is already initialized with some assets
 * const cardano_asset_name_map_t* rhs = ...; // Assume rhs is also initialized with some assets
 * cardano_asset_name_map_t* result = NULL;
 *
 * cardano_error_t add_result = cardano_asset_name_map_add(lhs, rhs, &result);
 * if (add_result == CARDANO_SUCCESS)
 * {
 *   // The assets have been successfully combined
 *   // Use the result as needed
 *
 *   // When done, release the result
 *   cardano_asset_name_map_unref(&result);
 * }
 * else
 * {
 *   printf("Failed to combine assets: %s\n", cardano_error_to_string(add_result));
 * }
 *
 * // Cleanup resources
 * cardano_asset_name_map_unref(&lhs);
 * cardano_asset_name_map_unref(&rhs);
 * \endcode
 */
CARDANO_NODISCARD
CARDANO_EXPORT cardano_error_t cardano_asset_name_map_add(
  const cardano_asset_name_map_t* lhs,
  const cardano_asset_name_map_t* rhs,
  cardano_asset_name_map_t**      result);

/**
 * \brief Subtracts the quantities of assets under each asset name from one asset name map to another.
 *
 * This function subtracts the assets from the second \ref cardano_asset_name_map_t object (rhs) from the first
 * \ref cardano_asset_name_map_t object (lhs). It subtracts the quantities of assets under each asset name present
 * in both maps. If an asset name exists only in the rhs, the value is copied as negative, effectively subtracting
 * it from the lhs. If an asset name exists only in the lhs, the value is copied as is to the result.
 *
 * \param[in] lhs A constant pointer to the first \ref cardano_asset_name_map_t object from which assets will be subtracted.
 * \param[in] rhs A constant pointer to the second \ref cardano_asset_name_map_t object that provides the quantities to subtract.
 * \param[out] result On successful execution, this will point to a newly created \ref cardano_asset_name_map_t object
 *             that represents the result of the subtraction.
 *             The result is dynamically allocated and must be freed by the caller using \ref cardano_asset_name_map_unref
 *             when it is no longer needed.
 *
 * \return \ref cardano_error_t indicating the outcome of the operation. Returns \ref CARDANO_SUCCESS if the assets were
 *         successfully subtracted, or an appropriate error code indicating the failure reason,
 *         such as \ref CARDANO_POINTER_IS_NULL if any of the input pointers are NULL.
 *
 * Usage Example:
 * \code{.c}
 * const cardano_asset_name_map_t* lhs = ...; // Assume lhs is already initialized with some assets
 * const cardano_asset_name_map_t* rhs = ...; // Assume rhs is also initialized with some assets to subtract
 * cardano_asset_name_map_t* result = NULL;
 *
 * cardano_error_t subtract_result = cardano_asset_name_map_subtract(lhs, rhs, &result);
 * if (subtract_result == CARDANO_SUCCESS)
 * {
 *   // The assets have been successfully subtracted
 *   // Use the result
 *   cardano_asset_name_map_unref(&result);
 * }
 * else
 * {
 *   printf("Failed to subtract assets: %s\n", cardano_error_to_string(subtract_result));
 * }
 *
 * // Cleanup resources
 * cardano_asset_name_map_unref(&lhs);
 * cardano_asset_name_map_unref(&rhs);
 * \endcode
 */
CARDANO_NODISCARD
CARDANO_EXPORT cardano_error_t cardano_asset_name_map_subtract(
  const cardano_asset_name_map_t* lhs,
  const cardano_asset_name_map_t* rhs,
  cardano_asset_name_map_t**      result);

/**
 * \brief Compares two asset name map objects for equality.
 *
 * This function compares two asset_name_map objects to determine if they are equal.
 *
 * \param[in] lhs Pointer to the first asset name map object.
 * \param[in] rhs Pointer to the second asset name map object.
 *
 * \return \c true if the asset name map objects are equal, \c false otherwise.
 *
 * Usage Example:
 * \code{.c}
 * cardano_asset_name_map_t* asset_name_map1 = NULL;
 * cardano_asset_name_map_t* asset_name_map2 = NULL;
 *
 * // Assume asset_name_map1 and asset_name_map2 are initialized properly
 *
 * bool are_equal = cardano_asset_name_map_equals(asset_name_map1, asset_name_map2);
 *
 * if (are_equal)
 * {
 *   printf("The asset_name_map objects are equal.\n");
 * }
 * else
 * {
 *   printf("The asset_name_map objects are not equal.\n");
 * }
 *
 * // Clean up
 * cardano_asset_name_map_unref(&asset_name_map1);
 * cardano_asset_name_map_unref(&asset_name_map2);
 * \endcode
 */
CARDANO_NODISCARD
CARDANO_EXPORT bool
cardano_asset_name_map_equals(const cardano_asset_name_map_t* lhs, const cardano_asset_name_map_t* rhs);

/**
 * \brief Decrements the reference count of a asset_name_map object.
 *
 * This function is responsible for managing the lifecycle of a \ref cardano_asset_name_map_t object
 * by decreasing its reference count. When the reference count reaches zero, the asset_name_map is
 * finalized; its associated resources are released, and its memory is deallocated.
 *
 * \param[in,out] asset_name_map A pointer to the pointer of the asset_name_map object. This double
 *                            indirection allows the function to set the caller's pointer to
 *                            NULL, avoiding dangling pointer issues after the object has been
 *                            freed.
 *
 * Usage Example:
 * \code{.c}
 * cardano_asset_name_map_t* asset_name_map = cardano_asset_name_map_new();
 *
 * // Perform operations with the asset_name_map...
 *
 * cardano_asset_name_map_unref(&asset_name_map);
 * // At this point, asset_name_map is NULL and cannot be used.
 * \endcode
 *
 * \note After calling \ref cardano_asset_name_map_unref, the pointer to the \ref cardano_asset_name_map_t object
 *       will be set to NULL to prevent its reuse.
 */
CARDANO_EXPORT void cardano_asset_name_map_unref(cardano_asset_name_map_t** asset_name_map);

/**
 * \brief Increases the reference count of the cardano_asset_name_map_t object.
 *
 * This function is used to manually increment the reference count of a asset_name_map
 * object, indicating that another part of the code has taken ownership of it. This
 * ensures the object remains allocated and valid until all owners have released their
 * reference by calling \ref cardano_asset_name_map_unref.
 *
 * \param asset_name_map A pointer to the asset_name_map object whose reference count is to be incremented.
 *
 * Usage Example:
 * \code{.c}
 * // Assuming asset_name_map is a previously created asset_name_map object
 *
 * cardano_asset_name_map_ref(asset_name_map);
 *
 * // Now asset_name_map can be safely used elsewhere without worrying about premature deallocation
 * \endcode
 *
 * \note Always ensure that for every call to \ref cardano_asset_name_map_ref there is a corresponding
 * call to \ref cardano_asset_name_map_unref to prevent memory leaks.
 */
CARDANO_EXPORT void cardano_asset_name_map_ref(cardano_asset_name_map_t* asset_name_map);

/**
 * \brief Retrieves the current reference count of the cardano_asset_name_map_t object.
 *
 * This function returns the number of active references to a asset_name_map object. It's useful
 * for debugging purposes or managing the lifecycle of the object in complex scenarios.
 *
 * \warning This function does not account for transitive references. A transitive reference
 * occurs when an object holds a reference to another object, rather than directly to the
 * cardano_asset_name_map_t. As such, the reported count may not fully represent the total number
 * of conceptual references in cases where such transitive relationships exist.
 *
 * \param asset_name_map A pointer to the asset_name_map object whose reference count is queried.
 *                    The object must not be NULL.
 *
 * \return The number of active references to the specified asset_name_map object. If the object
 * is properly managed (i.e., every \ref cardano_asset_name_map_ref call is matched with a
 * \ref cardano_asset_name_map_unref call), this count should reach zero right before the object
 * is deallocated.
 *
 * Usage Example:
 * \code{.c}
 * // Assuming asset_name_map is a previously created asset_name_map object
 *
 * size_t ref_count = cardano_asset_name_map_refcount(asset_name_map);
 *
 * printf("Reference count: %zu\n", ref_count);
 * \endcode
 */
CARDANO_NODISCARD
CARDANO_EXPORT size_t cardano_asset_name_map_refcount(const cardano_asset_name_map_t* asset_name_map);

/**
 * \brief Sets the last error message for a given asset_name_map object.
 *
 * Records an error message in the asset_name_map's last_error buffer, overwriting any existing message.
 * This is useful for storing descriptive error information that can be later retrieved. The message
 * is truncated if it exceeds the buffer's capacity.
 *
 * \param[in] asset_name_map A pointer to the \ref cardano_asset_name_map_t instance whose last error message is
 *                       to be set. If \c NULL, the function does nothing.
 * \param[in] message A null-terminated string containing the error message. If \c NULL, the asset_name_map's
 *                    last_error is set to an empty string, indicating no error.
 *
 * \note The error message is limited to 1023 characters, including the null terminator, due to the
 * fixed size of the last_error buffer.
 */
CARDANO_EXPORT void cardano_asset_name_map_set_last_error(cardano_asset_name_map_t* asset_name_map, const char* message);

/**
 * \brief Retrieves the last error message recorded for a specific asset_name_map.
 *
 * This function returns a pointer to the null-terminated string containing
 * the last error message set by \ref cardano_asset_name_map_set_last_error for the given
 * asset_name_map. If no error message has been set, or if the last_error buffer was
 * explicitly cleared, an empty string is returned, indicating no error.
 *
 * \param[in] asset_name_map A pointer to the \ref cardano_asset_name_map_t instance whose last error
 *                   message is to be retrieved. If the asset_name_map is NULL, the function
 *                   returns a generic error message indicating the null asset_name_map.
 *
 * \return A pointer to a null-terminated string containing the last error
 *         message for the specified asset_name_map. If the asset_name_map is NULL, "Object is NULL."
 *         is returned to indicate the error.
 *
 * \note The returned string points to internal storage within the object and
 *       must not be modified by the caller. The string remains valid until the
 *       next call to \ref cardano_asset_name_map_set_last_error for the same asset_name_map, or until
 *       the asset_name_map is deallocated.
 */
CARDANO_NODISCARD
CARDANO_EXPORT const char* cardano_asset_name_map_get_last_error(const cardano_asset_name_map_t* asset_name_map);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // CARDANO_ASSET_NAME_MAP_H