/**
 * \file register_drep_cert.c
 *
 * \author angel.castillo
 * \date   Jul 31, 2024
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

/* INCLUDES ******************************************************************/

#include <cardano/certs/cert_type.h>
#include <cardano/certs/register_drep_cert.h>
#include <cardano/common/anchor.h>
#include <cardano/common/credential.h>
#include <cardano/error.h>
#include <cardano/object.h>

#include "../allocators.h"
#include "../cbor/cbor_validation.h"

#include <assert.h>
#include <string.h>

/* CONSTANTS *****************************************************************/

static const int64_t EMBEDDED_GROUP_SIZE = 4;

/* STRUCTURES ****************************************************************/

/**
 * \brief In Voltaire, existing stake credentials will be able to delegate their stake to DReps for voting
 * purposes, in addition to the current delegation to stake pools for block production.
 * DRep delegation will mimic the existing stake delegation mechanisms (via on-chain certificates).
 *
 * This certificate register a stake key as a DRep.
 */
typedef struct cardano_register_drep_cert_t
{
    cardano_object_t      base;
    cardano_credential_t* credential;
    uint64_t              deposit;
    cardano_anchor_t*     anchor;
} cardano_register_drep_cert_t;

/* STATIC FUNCTIONS **********************************************************/

/**
 * \brief Deallocates a register_drep_cert object.
 *
 * This function is responsible for properly deallocating a register_drep_cert object (`cardano_register_drep_cert_t`)
 * and its associated resources.
 *
 * \param object A void pointer to the register_drep_cert object to be deallocated. The function casts this
 *               pointer to the appropriate type (`cardano_register_drep_cert_t*`).
 *
 * \note It is assumed that this function is called only when the reference count of the register_drep_cert
 *       object reaches zero, as part of the reference counting mechanism implemented for managing the
 *       lifecycle of these objects.
 */
static void
cardano_register_drep_cert_deallocate(void* object)
{
  assert(object != NULL);

  cardano_register_drep_cert_t* data = (cardano_register_drep_cert_t*)object;

  cardano_credential_unref(&data->credential);
  cardano_anchor_unref(&data->anchor);

  _cardano_free(data);
}

/* DEFINITIONS ****************************************************************/

cardano_error_t
cardano_register_drep_cert_new(
  cardano_credential_t*          drep_credential,
  uint64_t                       deposit,
  cardano_anchor_t*              anchor,
  cardano_register_drep_cert_t** register_drep_cert)
{
  if (drep_credential == NULL)
  {
    return CARDANO_POINTER_IS_NULL;
  }

  if (register_drep_cert == NULL)
  {
    return CARDANO_POINTER_IS_NULL;
  }

  cardano_register_drep_cert_t* data = _cardano_malloc(sizeof(cardano_register_drep_cert_t));

  if (data == NULL)
  {
    return CARDANO_MEMORY_ALLOCATION_FAILED;
  }

  data->base.ref_count     = 1;
  data->base.last_error[0] = '\0';
  data->base.deallocator   = cardano_register_drep_cert_deallocate;

  cardano_credential_ref(drep_credential);
  data->credential = drep_credential;

  data->deposit = deposit;

  if (anchor != NULL)
  {
    cardano_anchor_ref(anchor);
  }

  data->anchor = anchor;

  *register_drep_cert = data;

  return CARDANO_SUCCESS;
}

cardano_error_t
cardano_register_drep_cert_from_cbor(cardano_cbor_reader_t* reader, cardano_register_drep_cert_t** register_drep_cert)
{
  if (reader == NULL)
  {
    return CARDANO_POINTER_IS_NULL;
  }

  if (register_drep_cert == NULL)
  {
    return CARDANO_POINTER_IS_NULL;
  }

  static const char* validator_name = "register_drep_cert";

  cardano_error_t expect_array_result = cardano_cbor_validate_array_of_n_elements(validator_name, reader, EMBEDDED_GROUP_SIZE);

  if (expect_array_result != CARDANO_SUCCESS)
  {
    return expect_array_result;
  }

  uint64_t              type             = 0U;
  const cardano_error_t read_uint_result = cardano_cbor_validate_enum_value(
    validator_name,
    "type",
    reader,
    CARDANO_CERT_TYPE_DREP_REGISTRATION,
    (enum_to_string_callback_t)((void*)&cardano_cert_type_to_string),
    &type);

  if (read_uint_result != CARDANO_SUCCESS)
  {
    return read_uint_result;
  }

  cardano_credential_t* credential = NULL;

  cardano_error_t read_credential_result = cardano_credential_from_cbor(reader, &credential);

  if (read_credential_result != CARDANO_SUCCESS)
  {
    return read_credential_result;
  }

  uint64_t deposit = 0U;

  cardano_error_t read_deposit_result = cardano_cbor_reader_read_uint(reader, &deposit);

  if (read_deposit_result != CARDANO_SUCCESS)
  {
    cardano_credential_unref(&credential);

    return read_deposit_result;
  }

  cardano_anchor_t* anchor = NULL;

  cardano_cbor_reader_state_t state;

  cardano_error_t read_state = cardano_cbor_reader_peek_state(reader, &state);

  if (read_state != CARDANO_SUCCESS)
  {
    // LCOV_EXCL_START
    cardano_credential_unref(&credential);

    return read_state;
    // LCOV_EXCL_STOP
  }

  if (state == CARDANO_CBOR_READER_STATE_NULL)
  {
    cardano_error_t read_null = cardano_cbor_reader_read_null(reader);
    CARDANO_UNUSED(read_null);
  }
  else
  {
    cardano_error_t read_anchor_result = cardano_anchor_from_cbor(reader, &anchor);

    if (read_anchor_result != CARDANO_SUCCESS)
    {
      cardano_credential_unref(&credential);

      return read_anchor_result;
    }
  }

  cardano_error_t new_result = cardano_register_drep_cert_new(credential, deposit, anchor, register_drep_cert);

  cardano_credential_unref(&credential);
  cardano_anchor_unref(&anchor);

  if (new_result != CARDANO_SUCCESS)
  {
    return new_result; // LCOV_EXCL_LINE
  }

  return cardano_cbor_validate_end_array(validator_name, reader);
}

cardano_error_t
cardano_register_drep_cert_to_cbor(
  const cardano_register_drep_cert_t* register_drep_cert,
  cardano_cbor_writer_t*              writer)
{
  if (register_drep_cert == NULL)
  {
    return CARDANO_POINTER_IS_NULL;
  }

  if (writer == NULL)
  {
    return CARDANO_POINTER_IS_NULL;
  }

  cardano_error_t write_array_result = cardano_cbor_writer_write_start_array(writer, EMBEDDED_GROUP_SIZE);

  if (write_array_result != CARDANO_SUCCESS)
  {
    return write_array_result; // LCOV_EXCL_LINE
  }

  cardano_error_t write_type_result = cardano_cbor_writer_write_unsigned_int(writer, CARDANO_CERT_TYPE_DREP_REGISTRATION);

  if (write_type_result != CARDANO_SUCCESS)
  {
    return write_type_result; // LCOV_EXCL_LINE
  }

  cardano_error_t write_credential_result = cardano_credential_to_cbor(register_drep_cert->credential, writer);

  if (write_credential_result != CARDANO_SUCCESS)
  {
    return write_credential_result; // LCOV_EXCL_LINE
  }

  cardano_error_t write_deposit_result = cardano_cbor_writer_write_unsigned_int(writer, register_drep_cert->deposit);

  if (write_deposit_result != CARDANO_SUCCESS)
  {
    return write_deposit_result; // LCOV_EXCL_LINE
  }

  if (register_drep_cert->anchor != NULL)
  {
    cardano_error_t write_anchor_result = cardano_anchor_to_cbor(register_drep_cert->anchor, writer);

    if (write_anchor_result != CARDANO_SUCCESS)
    {
      return write_anchor_result; // LCOV_EXCL_LINE
    }
  }
  else
  {
    cardano_error_t write_null_result = cardano_cbor_writer_write_null(writer);

    if (write_null_result != CARDANO_SUCCESS)
    {
      return write_null_result; // LCOV_EXCL_LINE
    }
  }

  return CARDANO_SUCCESS;
}

cardano_credential_t*
cardano_register_drep_cert_get_credential(cardano_register_drep_cert_t* certificate)
{
  if (certificate == NULL)
  {
    return NULL;
  }

  cardano_credential_ref(certificate->credential);

  return certificate->credential;
}

cardano_error_t
cardano_register_drep_cert_set_credential(
  cardano_register_drep_cert_t* certificate,
  cardano_credential_t*         credential)
{
  if (certificate == NULL)
  {
    return CARDANO_POINTER_IS_NULL;
  }

  if (credential == NULL)
  {
    return CARDANO_POINTER_IS_NULL;
  }

  cardano_credential_ref(credential);
  cardano_credential_unref(&certificate->credential);
  certificate->credential = credential;

  return CARDANO_SUCCESS;
}

uint64_t
cardano_register_drep_cert_get_deposit(const cardano_register_drep_cert_t* certificate)
{
  if (certificate == NULL)
  {
    return 0U;
  }

  return certificate->deposit;
}

cardano_error_t
cardano_register_drep_cert_set_deposit(
  cardano_register_drep_cert_t* certificate,
  const uint64_t                deposit)
{
  if (certificate == NULL)
  {
    return CARDANO_POINTER_IS_NULL;
  }

  certificate->deposit = deposit;

  return CARDANO_SUCCESS;
}

cardano_anchor_t*
cardano_register_drep_cert_get_anchor(cardano_register_drep_cert_t* certificate)
{
  if (certificate == NULL)
  {
    return NULL;
  }

  cardano_anchor_ref(certificate->anchor);

  return certificate->anchor;
}

cardano_error_t
cardano_register_drep_cert_set_anchor(
  cardano_register_drep_cert_t* certificate,
  cardano_anchor_t*             anchor)
{
  if (certificate == NULL)
  {
    return CARDANO_POINTER_IS_NULL;
  }

  if (anchor == NULL)
  {
    return CARDANO_POINTER_IS_NULL;
  }

  cardano_anchor_ref(anchor);
  cardano_anchor_unref(&certificate->anchor);
  certificate->anchor = anchor;

  return CARDANO_SUCCESS;
}

void
cardano_register_drep_cert_unref(cardano_register_drep_cert_t** register_drep_cert)
{
  if ((register_drep_cert == NULL) || (*register_drep_cert == NULL))
  {
    return;
  }

  cardano_object_t* object = &(*register_drep_cert)->base;
  cardano_object_unref(&object);

  if (object == NULL)
  {
    *register_drep_cert = NULL;
    return;
  }
}

void
cardano_register_drep_cert_ref(cardano_register_drep_cert_t* register_drep_cert)
{
  if (register_drep_cert == NULL)
  {
    return;
  }

  cardano_object_ref(&register_drep_cert->base);
}

size_t
cardano_register_drep_cert_refcount(const cardano_register_drep_cert_t* register_drep_cert)
{
  if (register_drep_cert == NULL)
  {
    return 0;
  }

  return cardano_object_refcount(&register_drep_cert->base);
}

void
cardano_register_drep_cert_set_last_error(cardano_register_drep_cert_t* register_drep_cert, const char* message)
{
  cardano_object_set_last_error(&register_drep_cert->base, message);
}

const char*
cardano_register_drep_cert_get_last_error(const cardano_register_drep_cert_t* register_drep_cert)
{
  return cardano_object_get_last_error(&register_drep_cert->base);
}
