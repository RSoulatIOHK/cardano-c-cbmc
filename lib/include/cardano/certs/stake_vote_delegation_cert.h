/**
 * \file stake_vote_delegation_cert.h
 *
 * \author angel.castillo
 * \date   Jul 23, 2024
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

#ifndef STAKE_VOTE_DELEGATION_CERT_H
#define STAKE_VOTE_DELEGATION_CERT_H

/* INCLUDES ******************************************************************/

#include <cardano/cbor/cbor_reader.h>
#include <cardano/cbor/cbor_writer.h>
#include <cardano/common/credential.h>
#include <cardano/common/drep.h>
#include <cardano/crypto/blake2b_hash.h>
#include <cardano/error.h>
#include <cardano/export.h>

/* DECLARATIONS **************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief This certificate is used when an individual wants to delegate their voting
 * rights to any other DRep and simultaneously wants to delegate their stake to a
 * specific stake pool.
 */
typedef struct cardano_stake_vote_delegation_cert_t cardano_stake_vote_delegation_cert_t;

/**
 * \brief Certificates are a means to encode various essential operations related to stake
 * delegation and stake pool management. Certificates are embedded in transactions and
 * included in blocks. They're a vital aspect of Cardano's proof-of-stake mechanism,
 * ensuring that stakeholders can participate in the protocol and its governance.
 */
typedef struct cardano_certificate_t cardano_certificate_t;

/**
 * \brief Creates a new stake vote delegation certificate.
 *
 * This function allocates and initializes a new \ref cardano_stake_vote_delegation_cert_t object. It represents a certificate
 * where a stakeholder delegates their voting rights along with their staking activity to a pool and a drep.
 *
 * \param[in] credential A pointer to an initialized \ref cardano_credential_t object representing the stakeholder's credential.
 * \param[in] pool_key_hash A pointer to an initialized \ref cardano_blake2b_hash_t object representing the pool's key hash to which the stake is delegated.
 * \param[in] drep A pointer to an initialized \ref cardano_drep_t object representing the drep to which the voting rights are delegated.
 * \param[out] stake_vote_delegation On successful initialization, this will point to a newly created \ref cardano_stake_vote_delegation_cert_t object.
 *                                            The caller is responsible for managing the lifecycle of this object.
 *
 * \return \ref cardano_error_t indicating the outcome of the operation. Returns \ref CARDANO_SUCCESS if the certificate was
 *         successfully created, or an appropriate error code indicating the failure reason.
 *
 * Usage Example:
 * \code{.c}
 * cardano_credential_t* credential = ...; // Assume credential is already initialized
 * cardano_blake2b_hash_t* pool_key_hash = ...; // Assume pool_key_hash is initialized
 * cardano_drep_t* drep = ...; // Assume drep is initialized
 * cardano_stake_vote_delegation_cert_t* certificate = NULL;
 *
 * cardano_error_t result = cardano_stake_vote_delegation_cert_new(credential, pool_key_hash, drep, &certificate);
 * if (result == CARDANO_SUCCESS)
 * {
 *   // The certificate can now be used in voting and staking operations
 *   // Remember to free the certificate when done
 *   cardano_stake_vote_delegation_cert_unref(&certificate);
 * }
 * else
 * {
 *   printf("Failed to create stake vote delegation certificate: %s\n", cardano_error_to_string(result));
 * }
 *
 * // Clean up resources
 * cardano_credential_unref(&credential);
 * cardano_blake2b_hash_unref(&pool_key_hash);
 * cardano_drep_unref(&drep);
 * \endcode
 *
 * \note This function increases the reference count of all input objects. The caller is responsible for releasing their references when they are no longer needed.
 */
CARDANO_NODISCARD
CARDANO_EXPORT cardano_error_t
cardano_stake_vote_delegation_cert_new(
  cardano_credential_t*                  credential,
  cardano_blake2b_hash_t*                pool_key_hash,
  cardano_drep_t*                        drep,
  cardano_stake_vote_delegation_cert_t** stake_vote_delegation);

/**
 * \brief Creates a \ref cardano_stake_vote_delegation_cert_t from a CBOR reader.
 *
 * This function parses CBOR data using a provided \ref cardano_cbor_reader_t and constructs a \ref cardano_stake_vote_delegation_cert_t object.
 * It assumes that the CBOR reader is set up correctly and that the CBOR data corresponds to the structure expected for a stake_registration.
 *
 * \param[in] reader A pointer to an initialized \ref cardano_cbor_reader_t that is ready to read the CBOR-encoded data.
 * \param[out] stake_registration A pointer to a pointer of \ref cardano_stake_vote_delegation_cert_t that will be set to the address
 *                        of the newly created stake_registration object upon successful decoding.
 *
 * \return A \ref cardano_error_t value indicating the outcome of the operation. Returns \ref CARDANO_SUCCESS
 *         if the object was successfully created, or an appropriate error code if an error occurred.
 *
 * \note If the function fails, the last error can be retrieved by calling \ref cardano_cbor_reader_get_last_error with the reader.
 *       The caller is responsible for freeing the created \ref cardano_stake_vote_delegation_cert_t object by calling
 *       \ref cardano_stake_vote_delegation_cert_unref when it is no longer needed.
 *
 * Usage Example:
 * \code{.c}
 * cardano_cbor_reader_t* reader = cardano_cbor_reader_new(cbor_data, data_size);
 * cardano_stake_vote_delegation_cert_t* stake_registration = NULL;
 *
 * cardano_error_t result = cardano_stake_vote_delegation_cert_from_cbor(reader, &stake_registration);
 *
 * if (result == CARDANO_SUCCESS)
 * {
 *   // Use the stake_registration
 *
 *   // Once done, ensure to clean up and release the stake_registration
 *   cardano_stake_vote_delegation_cert_unref(&stake_registration);
 * }
 * else
 * {
 *   const char* error = cardano_cbor_reader_get_last_error(reader);
 *   printf("Failed to decode stake_registration: %s\n", error);
 * }
 *
 * cardano_cbor_reader_unref(&reader); // Cleanup the CBOR reader
 * \endcode
 */
CARDANO_NODISCARD
CARDANO_EXPORT cardano_error_t
cardano_stake_vote_delegation_cert_from_cbor(cardano_cbor_reader_t* reader, cardano_stake_vote_delegation_cert_t** stake_registration);

/**
 * \brief Serializes the certificate into CBOR format using a CBOR writer.
 *
 * This function serializes the given \ref cardano_stake_vote_delegation_cert_t object using a \ref cardano_cbor_writer_t.
 *
 * \param[in] stake_registration A constant pointer to the \ref cardano_stake_vote_delegation_cert_t object that is to be serialized.
 * \param[out] writer A pointer to a \ref cardano_cbor_writer_t where the CBOR serialized data will be written.
 *                    The writer must already be initialized and ready to accept the data.
 *
 * \return Returns \ref CARDANO_SUCCESS if the serialization is successful. If the \p stake_registration or \p writer
 *         is NULL, returns \ref CARDANO_POINTER_IS_NULL.
 *
 * Usage Example:
 * \code{.c}
 * cardano_stake_vote_delegation_cert_t* stake_registration = ...;
 * cardano_cbor_writer_t* writer = cardano_cbor_writer_new();
 *
 * if (writer)
 * {
 *   cardano_error_t result = cardano_stake_vote_delegation_cert_to_cbor(stake_registration, writer);
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
 * cardano_stake_vote_delegation_cert_unref(&stake_registration);
 * \endcode
 */
CARDANO_NODISCARD
CARDANO_EXPORT cardano_error_t cardano_stake_vote_delegation_cert_to_cbor(
  const cardano_stake_vote_delegation_cert_t* stake_registration,
  cardano_cbor_writer_t*                      writer);

/**
 * \brief Retrieves the credential from a stake vote delegation certificate.
 *
 * This function extracts the credential from the specified \ref cardano_stake_vote_delegation_cert_t object. The credential
 * identifies the stakeholder whose voting rights are being delegated.
 *
 * \param[in] certificate A constant pointer to an initialized \ref cardano_stake_vote_delegation_cert_t object from which the credential will be retrieved.
 *
 * \return A pointer to the \ref cardano_credential_t object containing the credential. If the certificate is NULL or
 *         if the credential is not set, this function returns NULL. This returned object is a new reference and must be
 *         unreferenced by the caller when it is no longer needed.
 *
 * Usage Example:
 * \code{.c}
 * const cardano_stake_vote_delegation_cert_t* certificate = ...; // Assume certificate is already initialized
 * cardano_credential_t* credential = cardano_stake_vote_delegation_cert_get_credential(certificate);
 *
 * if (credential != NULL)
 * {
 *   // Process the credential
 *   cardano_credential_unref(&credential); // When done, release the credential
 * }
 * else
 * {
 *   printf("Failed to retrieve credential.\n");
 * }
 * \endcode
 *
 * \note The caller is responsible for managing the lifecycle of the returned credential, including releasing it
 *       when it is no longer needed.
 */
CARDANO_NODISCARD
CARDANO_EXPORT cardano_credential_t* cardano_stake_vote_delegation_cert_get_credential(cardano_stake_vote_delegation_cert_t* certificate);

/**
 * \brief Sets the credential for a stake vote delegation certificate.
 *
 * This function assigns a new credential to a given \ref cardano_stake_vote_delegation_cert_t object.
 * The credential specifies the stakeholder whose voting rights are being delegated.
 *
 * \param[in] certificate A pointer to an initialized \ref cardano_stake_vote_delegation_cert_t object to which the credential will be set.
 * \param[in] credential A pointer to an initialized \ref cardano_credential_t object representing the new credential. This function
 *                       increments the reference count of the credential, so it remains managed by the caller.
 *
 * \return \ref cardano_error_t indicating the outcome of the operation. Returns \ref CARDANO_SUCCESS
 *         if the credential was successfully set, or an appropriate error code indicating the failure reason,
 *         such as \ref CARDANO_POINTER_IS_NULL if any of the input pointers are NULL.
 *
 * Usage Example:
 * \code{.c}
 * cardano_stake_vote_delegation_cert_t* certificate = ...; // Assume certificate is already initialized
 * cardano_credential_t* credential = ...; // Assume credential is already initialized
 *
 * cardano_error_t result = cardano_stake_vote_delegation_cert_set_credential(certificate, credential);
 * if (result == CARDANO_SUCCESS)
 * {
 *   // The credential is now set for the certificate
 * }
 * else
 * {
 *   printf("Failed to set the credential.\n");
 * }
 *
 * // Both certificate and credential should be unreferenced when no longer needed
 * cardano_credential_unref(&credential);
 * cardano_stake_vote_delegation_cert_unref(&certificate);
 * \endcode
 *
 * \note This function takes a reference to the provided credential object. The caller must ensure
 *       that the credential remains valid for as long as it is associated with the certificate and
 *       is responsible for releasing its own reference when it is no longer needed.
 */
CARDANO_NODISCARD
CARDANO_EXPORT cardano_error_t
cardano_stake_vote_delegation_cert_set_credential(cardano_stake_vote_delegation_cert_t* certificate, cardano_credential_t* credential);

/**
 * \brief Retrieves the pool key hash from a stake vote delegation certificate.
 *
 * This function extracts the pool key hash from the specified \ref cardano_stake_vote_delegation_cert_t object.
 * The pool key hash identifies the stake pool to which the voting rights are delegated.
 *
 * \param[in] certificate A constant pointer to an initialized \ref cardano_stake_vote_delegation_cert_t object.
 *
 * \return A pointer to the \ref cardano_blake2b_hash_t object containing the pool key hash. If the certificate is NULL or
 *         if the pool key hash is not set, this function returns NULL. The returned object is a new reference and must be
 *         unreferenced by the caller when it is no longer needed.
 *
 * Usage Example:
 * \code{.c}
 * const cardano_stake_vote_delegation_cert_t* certificate = ...; // Assume certificate is already initialized
 * const cardano_blake2b_hash_t* pool_key_hash = cardano_stake_vote_delegation_cert_get_pool_key_hash(certificate);
 *
 * if (pool_key_hash != NULL)
 * {
 *   // Process the pool key hash
 *   // Display or use the hash as needed
 *   cardano_blake2b_hash_unref(&pool_key_hash);
 * }
 * \endcode
 *
 * \note This function returns a new reference to the pool key hash. It is the responsibility of the caller to manage
 *       this reference and ensure that it is unreferenced when no longer needed.
 */
CARDANO_NODISCARD
CARDANO_EXPORT cardano_blake2b_hash_t* cardano_stake_vote_delegation_cert_get_pool_key_hash(cardano_stake_vote_delegation_cert_t* certificate);

/**
 * \brief Sets the pool key hash for a stake vote delegation certificate.
 *
 * This function updates the pool key hash in the specified \ref cardano_stake_vote_delegation_cert_t object.
 * The pool key hash identifies the stake pool to which the voting rights are to be delegated.
 *
 * \param[in] certificate A pointer to an initialized \ref cardano_stake_vote_delegation_cert_t object to which the pool key hash will be set.
 * \param[in] hash A pointer to an initialized \ref cardano_blake2b_hash_t object representing the pool key hash.
 *                 The function increases the reference count of the hash object; it is the responsibility of the caller
 *                 to manage the lifecycle of their own reference to the hash.
 *
 * \return \ref cardano_error_t indicating the outcome of the operation. Returns \ref CARDANO_SUCCESS
 *         if the pool key hash was successfully set, or an appropriate error code indicating the failure reason,
 *         such as \ref CARDANO_POINTER_IS_NULL if any of the input pointers are NULL.
 *
 * Usage Example:
 * \code{.c}
 * cardano_stake_vote_delegation_cert_t* certificate = ...; // Assume certificate is already initialized
 * cardano_blake2b_hash_t* pool_key_hash = ...; // Assume pool_key_hash is already initialized
 *
 * cardano_error_t result = cardano_stake_vote_delegation_cert_set_pool_key_hash(certificate, pool_key_hash);
 * if (result == CARDANO_SUCCESS)
 * {
 *   printf("Pool key hash set successfully.\n");
 * }
 * else
 * {
 *   printf("Failed to set the pool key hash.\n");
 * }
 *
 * // Remember to unreference the hash if it's no longer used elsewhere in your application
 * cardano_blake2b_hash_unref(&pool_key_hash);
 * \endcode
 *
 * \note This function increments the reference count of the hash object. It is important for the caller
 *       to manage the reference correctly to avoid memory leaks.
 */
CARDANO_NODISCARD
CARDANO_EXPORT cardano_error_t
cardano_stake_vote_delegation_cert_set_pool_key_hash(cardano_stake_vote_delegation_cert_t* certificate, cardano_blake2b_hash_t* hash);

/**
 * \brief Retrieves the DREP from a stake vote delegation certificate.
 *
 * This function extracts the DREP associated with the specified \ref cardano_stake_vote_delegation_cert_t object.
 * The DREP object represents the delegated representative to which voting rights are assigned.
 *
 * \param[in] certificate A constant pointer to an initialized \ref cardano_stake_vote_delegation_cert_t object.
 *
 * \return A pointer to the \ref cardano_drep_t object containing the DREP information. If the certificate is NULL or
 *         if the DREP is not set, this function returns NULL. The returned DREP object is a new reference and must be
 *         managed by the caller, who is responsible for releasing it using \ref cardano_drep_unref when it is no longer needed.
 *
 * Usage Example:
 * \code{.c}
 * const cardano_stake_vote_delegation_cert_t* certificate = ...; // Assume certificate is already initialized
 * cardano_drep_t* drep = cardano_stake_vote_delegation_cert_get_drep(certificate);
 *
 * if (drep != NULL)
 * {
 *   // Process the DREP information
 *   cardano_drep_unref(&drep);
 * }
 * \endcode
 */
CARDANO_NODISCARD
CARDANO_EXPORT cardano_drep_t* cardano_stake_vote_delegation_cert_get_drep(cardano_stake_vote_delegation_cert_t* certificate);

/**
 * \brief Sets the DREP for a stake vote delegation certificate.
 *
 * This function assigns a new DREP to the specified \ref cardano_stake_vote_delegation_cert_t object.
 * The DREP object represents the delegated representative to which voting rights are being assigned.
 *
 * \param[in,out] certificate A pointer to an initialized \ref cardano_stake_vote_delegation_cert_t object where the DREP will be set.
 * \param[in] drep A pointer to an initialized \ref cardano_drep_t object to be assigned to the certificate.
 *
 * \return \ref cardano_error_t indicating the outcome of the operation. Returns \ref CARDANO_SUCCESS if the DREP was successfully set,
 *         or an appropriate error code indicating the failure reason, such as \ref CARDANO_POINTER_IS_NULL if any of the input pointers are NULL.
 *
 * \note This function increments the reference count of the \p drep object, so it remains managed by both the caller and the certificate.
 *       It is the caller's responsibility to unreference their own copy of \p drep when it is no longer needed.
 *
 * Usage Example:
 * \code{.c}
 * cardano_stake_vote_delegation_cert_t* certificate = ...; // Assume certificate is already initialized
 * cardano_drep_t* drep = ...; // Assume drep is already initialized
 *
 * cardano_error_t result = cardano_stake_vote_delegation_cert_set_drep(certificate, drep);
 * if (result == CARDANO_SUCCESS)
 * {
 *   // The drep is now set for the certificate
 * }
 * else
 * {
 *   printf("Failed to set the drep.\n");
 * }
 *
 * cardano_drep_unref(&drep); // Cleanup the drep reference owned by the caller
 * \endcode
 */
CARDANO_NODISCARD
CARDANO_EXPORT cardano_error_t
cardano_stake_vote_delegation_cert_set_drep(cardano_stake_vote_delegation_cert_t* certificate, cardano_drep_t* drep);

/**
 * \brief Decrements the reference count of a cardano_stake_vote_delegation_cert_t object.
 *
 * This function is responsible for managing the lifecycle of a \ref cardano_stake_vote_delegation_cert_t object
 * by decreasing its reference count. When the reference count reaches zero, the stake_registration is
 * finalized; its associated resources are released, and its memory is deallocated.
 *
 * \param[in,out] stake_registration A pointer to the pointer of the stake_registration object. This double
 *                            indirection allows the function to set the caller's pointer to
 *                            NULL, avoiding dangling pointer issues after the object has been
 *                            freed.
 *
 * Usage Example:
 * \code{.c}
 * cardano_stake_vote_delegation_cert_t* stake_registration = cardano_stake_vote_delegation_cert_new(major, minor);
 *
 * // Perform operations with the stake_registration...
 *
 * cardano_stake_vote_delegation_cert_unref(&stake_registration);
 * // At this point, stake_registration is NULL and cannot be used.
 * \endcode
 *
 * \note After calling \ref cardano_stake_vote_delegation_cert_unref, the pointer to the \ref cardano_stake_vote_delegation_cert_t object
 *       will be set to NULL to prevent its reuse.
 */
CARDANO_EXPORT void cardano_stake_vote_delegation_cert_unref(cardano_stake_vote_delegation_cert_t** stake_registration);

/**
 * \brief Increases the reference count of the cardano_stake_vote_delegation_cert_t object.
 *
 * This function is used to manually increment the reference count of an cardano_stake_vote_delegation_cert_t
 * object, indicating that another part of the code has taken ownership of it. This
 * ensures the object remains allocated and valid until all owners have released their
 * reference by calling \ref cardano_stake_vote_delegation_cert_unref.
 *
 * \param stake_registration A pointer to the cardano_stake_vote_delegation_cert_t object whose reference count is to be incremented.
 *
 * Usage Example:
 * \code{.c}
 * // Assuming stake_registration is a previously created stake_registration object
 *
 * cardano_stake_vote_delegation_cert_ref(stake_registration);
 *
 * // Now stake_registration can be safely used elsewhere without worrying about premature deallocation
 * \endcode
 *
 * \note Always ensure that for every call to \ref cardano_stake_vote_delegation_cert_ref there is a corresponding
 * call to \ref cardano_stake_vote_delegation_cert_unref to prevent memory leaks.
 */
CARDANO_EXPORT void cardano_stake_vote_delegation_cert_ref(cardano_stake_vote_delegation_cert_t* stake_registration);

/**
 * \brief Retrieves the current reference count of the cardano_stake_vote_delegation_cert_t object.
 *
 * This function returns the number of active references to an cardano_stake_vote_delegation_cert_t object. It's useful
 * for debugging purposes or managing the lifecycle of the object in complex scenarios.
 *
 * \warning This function does not account for transitive references. A transitive reference
 * occurs when an object holds a reference to another object, rather than directly to the
 * cardano_stake_vote_delegation_cert_t. As such, the reported count may not fully represent the total number
 * of conceptual references in cases where such transitive relationships exist.
 *
 * \param stake_registration A pointer to the cardano_stake_vote_delegation_cert_t object whose reference count is queried.
 *                    The object must not be NULL.
 *
 * \return The number of active references to the specified cardano_stake_vote_delegation_cert_t object. If the object
 * is properly managed (i.e., every \ref cardano_stake_vote_delegation_cert_ref call is matched with a
 * \ref cardano_stake_vote_delegation_cert_unref call), this count should reach zero right before the object
 * is deallocated.
 *
 * Usage Example:
 * \code{.c}
 * // Assuming stake_registration is a previously created stake_registration object
 *
 * size_t ref_count = cardano_stake_vote_delegation_cert_refcount(stake_registration);
 *
 * printf("Reference count: %zu\n", ref_count);
 * \endcode
 */
CARDANO_NODISCARD
CARDANO_EXPORT size_t cardano_stake_vote_delegation_cert_refcount(const cardano_stake_vote_delegation_cert_t* stake_registration);

/**
 * \brief Sets the last error message for a given cardano_stake_vote_delegation_cert_t object.
 *
 * Records an error message in the stake_registration's last_error buffer, overwriting any existing message.
 * This is useful for storing descriptive error information that can be later retrieved. The message
 * is truncated if it exceeds the buffer's capacity.
 *
 * \param[in] stake_registration A pointer to the \ref cardano_stake_vote_delegation_cert_t instance whose last error message is
 *                       to be set. If \c NULL, the function does nothing.
 * \param[in] message A null-terminated string containing the error message. If \c NULL, the stake_registration's
 *                    last_error is set to an empty string, indicating no error.
 *
 * \note The error message is limited to 1023 characters, including the null terminator, due to the
 * fixed size of the last_error buffer.
 */
CARDANO_EXPORT void cardano_stake_vote_delegation_cert_set_last_error(
  cardano_stake_vote_delegation_cert_t* stake_registration,
  const char*                           message);

/**
 * \brief Retrieves the last error message recorded for a specific stake_registration.
 *
 * This function returns a pointer to the null-terminated string containing
 * the last error message set by \ref cardano_stake_vote_delegation_cert_set_last_error for the given
 * stake_registration. If no error message has been set, or if the last_error buffer was
 * explicitly cleared, an empty string is returned, indicating no error.
 *
 * \param[in] stake_registration A pointer to the \ref cardano_stake_vote_delegation_cert_t instance whose last error
 *                   message is to be retrieved. If the stake_registration is NULL, the function
 *                   returns a generic error message indicating the null stake_registration.
 *
 * \return A pointer to a null-terminated string containing the last error
 *         message for the specified stake_registration. If the stake_registration is NULL, "Object is NULL."
 *         is returned to indicate the error.
 *
 * \note The returned string points to internal storage within the object and
 *       must not be modified by the caller. The string remains valid until the
 *       next call to \ref cardano_stake_vote_delegation_cert_set_last_error for the same stake_registration, or until
 *       the stake_registration is deallocated.
 */
CARDANO_NODISCARD
CARDANO_EXPORT const char* cardano_stake_vote_delegation_cert_get_last_error(
  const cardano_stake_vote_delegation_cert_t* stake_registration);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // STAKE_VOTE_DELEGATION_CERT_H