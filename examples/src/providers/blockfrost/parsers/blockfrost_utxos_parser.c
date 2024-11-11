/**
 * \file blockfrost_utxos_parser.c
 *
 * \author angel.castillo
 * \date   Sep 28, 2024
 *
 * Copyright 2024 Biglup Labs
 *
 * Licensed under the Apache License, Version 2.0 (the "License")
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

#include "blockfrost_parsers.h"
#include "utils.h"

#include <json-c/json.h>
#include <stdlib.h>
#include <string.h>

/* STATIC FUNCTIONS **********************************************************/

/**
 * \brief Parses an address from a JSON object and returns a \ref cardano_address_t object.
 *
 * This static function parses an address from the provided JSON object and converts it into a \ref cardano_address_t object.
 *
 * \param[in] provider A pointer to an initialized \ref cardano_provider_impl_t object that provides the necessary context for parsing.
 *                     This parameter must not be NULL.
 * \param[in] address_obj A pointer to a \ref json_object representing the address in JSON format.
 *                        The object must contain the necessary fields to parse the address.
 * \param[out] address On successful parsing, this will point to a newly created \ref cardano_address_t object
 *                     representing the parsed address. The caller is responsible for managing the lifecycle of this object
 *                     and must call \ref cardano_address_unref when it is no longer needed.
 *
 * \return \ref cardano_error_t indicating the outcome of the operation. Returns \ref CARDANO_SUCCESS if the address
 *         was successfully parsed, or an appropriate error code if an error occurred (e.g., if the JSON object format is invalid).
 */
static cardano_error_t
parse_address(
  cardano_provider_impl_t* provider,
  struct json_object*      address_obj,
  cardano_address_t**      address)
{
  const char*  address_data = json_object_get_string(address_obj);
  const size_t address_len  = json_object_get_string_len(address_obj);

  cardano_error_t result = cardano_address_from_string(address_data, address_len, address);

  if (result != CARDANO_SUCCESS)
  {
    cardano_utils_set_error_message(provider, "Failed to parse address from JSON response");
  }

  return result;
}

/**
 * \brief Parses a transaction hash from a JSON object and returns a \ref cardano_blake2b_hash_t object.
 *
 * This static function parses a transaction hash (TxHash) from the given JSON object and converts it into a \ref cardano_blake2b_hash_t object.
 *
 * \param[in] provider A pointer to an initialized \ref cardano_provider_impl_t object that provides the necessary context for parsing.
 *                     This parameter must not be NULL.
 * \param[in] tx_hash_obj A pointer to a \ref json_object representing the transaction hash in JSON format.
 *                        The object must contain the transaction hash as expected by the parser.
 * \param[out] tx_id On successful parsing, this will point to a newly created \ref cardano_blake2b_hash_t object
 *                   representing the parsed transaction ID. The caller is responsible for managing the lifecycle of this object
 *                   and must call \ref cardano_blake2b_hash_unref when it is no longer needed.
 *
 * \return \ref cardano_error_t indicating the outcome of the operation. Returns \ref CARDANO_SUCCESS if the transaction hash
 *         was successfully parsed, or an appropriate error code if an error occurred (e.g., if the JSON object format is invalid).
 */
static cardano_error_t
parse_tx_hash(
  cardano_provider_impl_t* provider,
  struct json_object*      tx_hash_obj,
  cardano_blake2b_hash_t** tx_id)
{
  const char*  tx_hash     = json_object_get_string(tx_hash_obj);
  const size_t tx_hash_len = json_object_get_string_len(tx_hash_obj);

  cardano_error_t result = cardano_blake2b_hash_from_hex(tx_hash, tx_hash_len, tx_id);

  if (result != CARDANO_SUCCESS)
  {
    cardano_utils_set_error_message(provider, "Failed to parse tx_hash from JSON response");
  }

  return result;
}

/**
 * \brief Parses an amount from a JSON array and returns a \ref cardano_value_t object.
 *
 * This static function parses a Cardano amount, which is encoded in a JSON array, and converts it into a \ref cardano_value_t object.
 *
 * \param[in] provider A pointer to an initialized \ref cardano_provider_impl_t object that provides the necessary context for parsing.
 *                     This parameter must not be NULL.
 * \param[in] amount_array A pointer to a \ref json_object representing the amount in a JSON array format. The array contains
 *                         the amount in lovelaces (ADA) and, optionally, other assets.
 * \param[out] value On successful parsing, this will point to a newly created \ref cardano_value_t object representing
 *                   the parsed amount, including ADA and multi-assets. The caller is responsible for managing the lifecycle
 *                   of this object and must call \ref cardano_value_unref when it is no longer needed.
 *
 * \return \ref cardano_error_t indicating the outcome of the operation. Returns \ref CARDANO_SUCCESS if the amount was
 *         successfully parsed, or an appropriate error code if an error occurred (e.g., if the JSON array format is invalid).
 */
static cardano_error_t
parse_amount(cardano_provider_impl_t* provider, struct json_object* amount_array, cardano_value_t** value)
{
  size_t                  amount_len   = json_object_array_length(amount_array);
  cardano_asset_id_map_t* asset_id_map = NULL;

  cardano_error_t result = cardano_asset_id_map_new(&asset_id_map);
  if (result != CARDANO_SUCCESS)
  {
    cardano_utils_set_error_message(provider, "Failed to allocate memory for asset_id_map");

    return result;
  }

  for (size_t j = 0U; j < amount_len; ++j)
  {
    struct json_object* amount_obj = json_object_array_get_idx(amount_array, j);
    struct json_object* unit_obj;
    struct json_object* quantity_obj;

    uint64_t            quantity = 0;
    cardano_asset_id_t* asset_id = NULL;

    if (json_object_object_get_ex(amount_obj, "unit", &unit_obj))
    {
      const char*  unit     = json_object_get_string(unit_obj);
      const size_t unit_len = json_object_get_string_len(unit_obj);

      if (strcmp(unit, "lovelace") == 0)
      {
        result = cardano_asset_id_new_lovelace(&asset_id);
      }
      else
      {
        result = cardano_asset_id_from_hex(unit, unit_len, &asset_id);
      }

      if (result != CARDANO_SUCCESS)
      {
        cardano_utils_set_error_message(provider, "Failed to parse asset_id from JSON response");
        cardano_asset_id_map_unref(&asset_id_map);

        return result;
      }
    }

    if (json_object_object_get_ex(amount_obj, "quantity", &quantity_obj))
    {
      const char*  quantity_str = json_object_get_string(quantity_obj);
      const size_t quantity_len = json_object_get_string_len(quantity_obj);

      cardano_bigint_t* bigint = NULL;
      result                   = cardano_bigint_from_string(quantity_str, quantity_len, 10, &bigint);

      if (result != CARDANO_SUCCESS)
      {
        cardano_utils_set_error_message(provider, "Failed to parse quantity from JSON response");
        cardano_asset_id_map_unref(&asset_id_map);

        return result;
      }

      quantity = cardano_bigint_to_unsigned_int(bigint);
      cardano_bigint_unref(&bigint);
    }

    result = cardano_asset_id_map_insert(asset_id_map, asset_id, (int64_t)quantity);
    cardano_asset_id_unref(&asset_id);

    if (result != CARDANO_SUCCESS)
    {
      cardano_utils_set_error_message(provider, "Failed to insert asset_id into asset_id_map");
      cardano_asset_id_map_unref(&asset_id_map);

      return result;
    }
  }

  result = cardano_value_from_asset_map(asset_id_map, value);
  cardano_asset_id_map_unref(&asset_id_map);

  if (result != CARDANO_SUCCESS)
  {
    cardano_utils_set_error_message(provider, "Failed to create value from asset_id_map");
  }

  return result;
}

/**
 * \brief Parses a data hash from a JSON object and returns a \ref cardano_blake2b_hash_t object.
 *
 * This static function parses a data hash from the given JSON object and converts it into a \ref cardano_blake2b_hash_t object.
 *
 * \param[in] provider A pointer to an initialized \ref cardano_provider_impl_t object that provides the necessary context for parsing.
 *                     This parameter must not be NULL.
 * \param[in] data_hash_obj A pointer to a \ref json_object representing the data hash in JSON format. The object must contain
 *                          the necessary fields to parse the hash.
 * \param[out] data_hash On successful parsing, this will point to a newly created \ref cardano_blake2b_hash_t object
 *                       representing the parsed data hash. The caller is responsible for managing the lifecycle of this object
 *                       and must call \ref cardano_blake2b_hash_unref when it is no longer needed.
 *
 * \return \ref cardano_error_t indicating the outcome of the operation. Returns \ref CARDANO_SUCCESS if the data hash
 *         was successfully parsed, or an appropriate error code if an error occurred (e.g., if the JSON object format is invalid).
 */
static cardano_error_t
parse_data_hash(
  cardano_provider_impl_t* provider,
  struct json_object*      data_hash_obj,
  cardano_blake2b_hash_t** data_hash)
{
  const char*  data_hash_str = json_object_get_string(data_hash_obj);
  const size_t data_hash_len = json_object_get_string_len(data_hash_obj);

  if ((data_hash_str != NULL) && (data_hash_len > 0U))
  {
    cardano_error_t result = cardano_blake2b_hash_from_hex(data_hash_str, data_hash_len, data_hash);

    if (result != CARDANO_SUCCESS)
    {
      cardano_utils_set_error_message(provider, "Failed to parse data_hash from JSON response");

      return result;
    }
  }

  return CARDANO_SUCCESS;
}

/**
 * \brief Parses an inline datum from a JSON object and returns a \ref cardano_plutus_data_t object.
 *
 * This static function parses an inline datum, which is encoded in a JSON object, and converts it into a \ref cardano_plutus_data_t object.
 *
 * \param[in] provider A pointer to an initialized \ref cardano_provider_impl_t object that provides the necessary context for parsing.
 *                     This parameter must not be NULL.
 * \param[in] inline_datum_obj A pointer to a \ref json_object representing the inline datum in JSON format.
 *                             This object must contain the necessary datum fields required for parsing.
 * \param[out] plutus_data On successful parsing, this will point to a newly created \ref cardano_plutus_data_t object
 *                         representing the parsed Plutus data. The caller is responsible for managing the lifecycle of this object
 *                         and must call \ref cardano_plutus_data_unref when it is no longer needed.
 *
 * \return \ref cardano_error_t indicating the outcome of the operation. Returns \ref CARDANO_SUCCESS if the inline datum
 *         was successfully parsed, or an appropriate error code if an error occurred (e.g., if the JSON object format is invalid).
 */
static cardano_error_t
parse_inline_datum(
  cardano_provider_impl_t* provider,
  struct json_object*      inline_datum_obj,
  cardano_plutus_data_t**  plutus_data)
{
  const char*  inline_datum     = json_object_get_string(inline_datum_obj);
  const size_t inline_datum_len = json_object_get_string_len(inline_datum_obj);

  if ((inline_datum != NULL) && (inline_datum_len > 0U))
  {
    cardano_cbor_reader_t* reader = cardano_cbor_reader_from_hex(inline_datum, inline_datum_len);

    if (!reader)
    {
      cardano_utils_set_error_message(provider, "Failed to create CBOR reader for inline_datum");

      return CARDANO_ERROR_INVALID_JSON;
    }

    cardano_error_t result = cardano_plutus_data_from_cbor(reader, plutus_data);
    cardano_cbor_reader_unref(&reader);

    if (result != CARDANO_SUCCESS)
    {
      cardano_utils_set_error_message(provider, "Failed to parse inline_datum from JSON response");

      return result;
    }
  }

  return CARDANO_SUCCESS;
}

/**
 * \brief Parses a reference script from a JSON object and returns a \ref cardano_script_t object.
 *
 * This static function parses a reference script from the given JSON object, which typically includes a script hash.
 *
 * \param[in] provider A pointer to an initialized \ref cardano_provider_impl_t object that interacts with the blockchain API.
 *                     This parameter must not be NULL.
 * \param[in] script_hash_obj A pointer to a \ref json_object representing the script hash in JSON format. This object must contain
 *                            the script hash data required to parse the reference script.
 * \param[out] reference_script On successful parsing, this will point to the newly created \ref cardano_script_t object
 *                              representing the reference script. The caller is responsible for managing the lifecycle of the script
 *                              and must call \ref cardano_script_unref when it is no longer needed.
 *
 * \return \ref cardano_error_t indicating the outcome of the operation. Returns \ref CARDANO_SUCCESS if the reference script
 *         was successfully parsed, or an appropriate error code if an error occurred (e.g., if the JSON object format is invalid).
 */
static cardano_error_t
parse_reference_script(
  cardano_provider_impl_t* provider,
  struct json_object*      script_hash_obj,
  cardano_script_t**       reference_script)
{
  const char*  reference_script_hash_hex     = json_object_get_string(script_hash_obj);
  const size_t reference_script_hash_hex_len = json_object_get_string_len(script_hash_obj);

  if ((reference_script_hash_hex != NULL) && (reference_script_hash_hex_len > 0U))
  {
    cardano_error_t result = cardano_blockfrost_get_script(provider, reference_script_hash_hex, reference_script_hash_hex_len, reference_script);

    if (result != CARDANO_SUCCESS)
    {
      cardano_utils_set_error_message(provider, "Failed to retrieve reference script from JSON response");

      return result;
    }
  }

  return CARDANO_SUCCESS;
}

/* IMPLEMENTATION *************************************************************/

cardano_error_t
cardano_blockfrost_parse_unspent_outputs(
  cardano_provider_impl_t* provider,
  const char*              json,
  size_t                   size,
  cardano_utxo_list_t**    utxo_list)
{
  struct json_tokener* tok         = json_tokener_new();
  struct json_object*  parsed_json = json_tokener_parse_ex(tok, json, (int32_t)size);

  if (parsed_json == NULL)
  {
    cardano_utils_set_error_message(provider, "Failed to parse JSON response");
    json_tokener_free(tok);
    return CARDANO_ERROR_INVALID_JSON;
  }

  cardano_error_t result = cardano_utxo_list_new(utxo_list);

  if (result != CARDANO_SUCCESS)
  {
    cardano_utils_set_error_message(provider, "Failed to allocate memory for UTXO list");
    json_object_put(parsed_json);
    json_tokener_free(tok);
    return result;
  }

  size_t array_len = json_object_array_length(parsed_json);

  uint64_t                      tx_index;
  cardano_blake2b_hash_t*       tx_id            = NULL;
  cardano_address_t*            address          = NULL;
  cardano_value_t*              value            = NULL;
  cardano_blake2b_hash_t*       plutus_data_hash = NULL;
  cardano_plutus_data_t*        plutus_data      = NULL;
  cardano_script_t*             reference_script = NULL;
  cardano_transaction_input_t*  input            = NULL;
  cardano_transaction_output_t* output           = NULL;
  cardano_utxo_t*               utxo             = NULL;

  for (size_t i = 0U; i < array_len; ++i)
  {
    // These are always freed at the end of the loop iteration,
    // so we can safely set them to NULL here.
    tx_id            = NULL;
    tx_index         = 0;
    address          = NULL;
    value            = NULL;
    plutus_data_hash = NULL;
    plutus_data      = NULL;
    reference_script = NULL;
    input            = NULL;
    output           = NULL;
    utxo             = NULL;

    struct json_object* tx_output = json_object_array_get_idx(parsed_json, i);

    struct json_object* address_obj = NULL;

    if (json_object_object_get_ex(tx_output, "address", &address_obj))
    {
      result = parse_address(provider, address_obj, &address);

      if (result != CARDANO_SUCCESS)
      {
        goto cleanup;
      }
    }

    struct json_object* tx_hash_obj = NULL;

    if (json_object_object_get_ex(tx_output, "tx_hash", &tx_hash_obj))
    {
      result = parse_tx_hash(provider, tx_hash_obj, &tx_id);

      if (result != CARDANO_SUCCESS)
      {
        goto cleanup;
      }
    }

    struct json_object* output_index_obj = NULL;

    if (json_object_object_get_ex(tx_output, "output_index", &output_index_obj))
    {
      tx_index = json_object_get_int64(output_index_obj);
    }

    struct json_object* amount_array = NULL;

    if (json_object_object_get_ex(tx_output, "amount", &amount_array))
    {
      result = parse_amount(provider, amount_array, &value);

      if (result != CARDANO_SUCCESS)
      {
        goto cleanup;
      }
    }

    struct json_object* data_hash_obj = NULL;

    if (json_object_object_get_ex(tx_output, "data_hash", &data_hash_obj))
    {
      result = parse_data_hash(provider, data_hash_obj, &plutus_data_hash);

      if (result != CARDANO_SUCCESS)
      {
        goto cleanup;
      }
    }

    struct json_object* inline_datum_obj = NULL;

    if (json_object_object_get_ex(tx_output, "inline_datum", &inline_datum_obj))
    {
      result = parse_inline_datum(provider, inline_datum_obj, &plutus_data);

      if (result != CARDANO_SUCCESS)
      {
        goto cleanup;
      }
    }

    struct json_object* reference_script_hash_obj = NULL;

    if (json_object_object_get_ex(tx_output, "reference_script_hash", &reference_script_hash_obj))
    {
      result = parse_reference_script(provider, reference_script_hash_obj, &reference_script);

      if (result != CARDANO_SUCCESS)
      {
        goto cleanup;
      }
    }

    result = cardano_transaction_input_new(tx_id, tx_index, &input);

    if (result != CARDANO_SUCCESS)
    {
      goto cleanup;
    }

    result = cardano_transaction_output_new(address, 0, &output);

    if (result != CARDANO_SUCCESS)
    {
      goto cleanup;
    }

    result = cardano_transaction_output_set_value(output, value);

    if (result != CARDANO_SUCCESS)
    {
      goto cleanup;
    }

    result = cardano_transaction_output_set_script_ref(output, reference_script);

    if (result != CARDANO_SUCCESS)
    {
      goto cleanup;
    }

    if (plutus_data_hash != NULL)
    {
      cardano_datum_t* datum = NULL;
      result                 = cardano_datum_new_data_hash(plutus_data_hash, &datum);

      if (result != CARDANO_SUCCESS)
      {
        goto cleanup;
      }

      result = cardano_transaction_output_set_datum(output, datum);
      cardano_datum_unref(&datum);

      if (result != CARDANO_SUCCESS)
      {
        goto cleanup;
      }
    }

    if (plutus_data != NULL)
    {
      cardano_datum_t* datum = NULL;
      result                 = cardano_datum_new_inline_data(plutus_data, &datum);

      if (result != CARDANO_SUCCESS)
      {
        goto cleanup;
      }

      result = cardano_transaction_output_set_datum(output, datum);
      cardano_datum_unref(&datum);

      if (result != CARDANO_SUCCESS)
      {
        goto cleanup;
      }
    }

    result = cardano_utxo_new(input, output, &utxo);

    if (result != CARDANO_SUCCESS)
    {
      goto cleanup;
    }

    result = cardano_utxo_list_add(*utxo_list, utxo);

    if (result != CARDANO_SUCCESS)
    {
      goto cleanup;
    }

    // Clean up allocated resources for this loop iteration
    cardano_transaction_input_unref(&input);
    cardano_transaction_output_unref(&output);
    cardano_utxo_unref(&utxo);
    cardano_blake2b_hash_unref(&tx_id);
    cardano_address_unref(&address);
    cardano_value_unref(&value);
    cardano_blake2b_hash_unref(&plutus_data_hash);
    cardano_plutus_data_unref(&plutus_data);
    cardano_script_unref(&reference_script);
  }

cleanup:
  if (result != CARDANO_SUCCESS)
  {
    cardano_utxo_list_unref(utxo_list);
    cardano_transaction_input_unref(&input);
    cardano_transaction_output_unref(&output);
    cardano_utxo_unref(&utxo);
    cardano_blake2b_hash_unref(&tx_id);
    cardano_address_unref(&address);
    cardano_value_unref(&value);
    cardano_blake2b_hash_unref(&plutus_data_hash);
    cardano_plutus_data_unref(&plutus_data);
    cardano_script_unref(&reference_script);
  }

  json_object_put(parsed_json);
  json_tokener_free(tok);

  return result;
}

cardano_error_t
cardano_blockfrost_parse_tx_unspent_outputs(
  cardano_provider_impl_t* provider,
  const char*              json,
  const size_t             size,
  const char*              tx_hash,
  const size_t             tx_hash_len,
  cardano_utxo_list_t**    utxo_list)
{
  struct json_tokener* tok         = json_tokener_new();
  struct json_object*  parsed_json = json_tokener_parse_ex(tok, json, (int32_t)size);

  if (parsed_json == NULL)
  {
    cardano_utils_set_error_message(provider, "Failed to parse JSON response");
    json_tokener_free(tok);
    return CARDANO_ERROR_INVALID_JSON;
  }

  cardano_error_t result = cardano_utxo_list_new(utxo_list);

  if (result != CARDANO_SUCCESS)
  {
    cardano_utils_set_error_message(provider, "Failed to allocate memory for UTXO list");
    json_object_put(parsed_json);
    json_tokener_free(tok);
    return result;
  }

  size_t array_len = json_object_array_length(parsed_json);

  uint64_t                      tx_index;
  cardano_blake2b_hash_t*       tx_id            = NULL;
  cardano_address_t*            address          = NULL;
  cardano_value_t*              value            = NULL;
  cardano_blake2b_hash_t*       plutus_data_hash = NULL;
  cardano_plutus_data_t*        plutus_data      = NULL;
  cardano_script_t*             reference_script = NULL;
  cardano_transaction_input_t*  input            = NULL;
  cardano_transaction_output_t* output           = NULL;
  cardano_utxo_t*               utxo             = NULL;

  for (size_t i = 0U; i < array_len; ++i)
  {
    // These are always freed at the end of the loop iteration,
    // so we can safely set them to NULL here.
    tx_id            = NULL;
    tx_index         = 0;
    address          = NULL;
    value            = NULL;
    plutus_data_hash = NULL;
    plutus_data      = NULL;
    reference_script = NULL;
    input            = NULL;
    output           = NULL;
    utxo             = NULL;

    result = cardano_blake2b_hash_from_hex(tx_hash, tx_hash_len, &tx_id);

    if (result != CARDANO_SUCCESS)
    {
      cardano_utils_set_error_message(provider, "Failed to parse tx_hash from JSON response");
      goto cleanup;
    }

    struct json_object* tx_output = json_object_array_get_idx(parsed_json, i);

    struct json_object* address_obj = NULL;

    if (json_object_object_get_ex(tx_output, "address", &address_obj))
    {
      result = parse_address(provider, address_obj, &address);

      if (result != CARDANO_SUCCESS)
      {
        goto cleanup;
      }
    }

    struct json_object* output_index_obj = NULL;

    if (json_object_object_get_ex(tx_output, "output_index", &output_index_obj))
    {
      tx_index = json_object_get_int64(output_index_obj);
    }

    struct json_object* amount_array = NULL;

    if (json_object_object_get_ex(tx_output, "amount", &amount_array))
    {
      result = parse_amount(provider, amount_array, &value);

      if (result != CARDANO_SUCCESS)
      {
        goto cleanup;
      }
    }

    struct json_object* data_hash_obj = NULL;

    if (json_object_object_get_ex(tx_output, "data_hash", &data_hash_obj))
    {
      result = parse_data_hash(provider, data_hash_obj, &plutus_data_hash);

      if (result != CARDANO_SUCCESS)
      {
        goto cleanup;
      }
    }

    struct json_object* inline_datum_obj = NULL;

    if (json_object_object_get_ex(tx_output, "inline_datum", &inline_datum_obj))
    {
      result = parse_inline_datum(provider, inline_datum_obj, &plutus_data);

      if (result != CARDANO_SUCCESS)
      {
        goto cleanup;
      }
    }

    struct json_object* reference_script_hash_obj = NULL;

    if (json_object_object_get_ex(tx_output, "reference_script_hash", &reference_script_hash_obj))
    {
      result = parse_reference_script(provider, reference_script_hash_obj, &reference_script);

      if (result != CARDANO_SUCCESS)
      {
        goto cleanup;
      }
    }

    result = cardano_transaction_input_new(tx_id, tx_index, &input);

    if (result != CARDANO_SUCCESS)
    {
      goto cleanup;
    }

    result = cardano_transaction_output_new(address, 0, &output);

    if (result != CARDANO_SUCCESS)
    {
      goto cleanup;
    }

    result = cardano_transaction_output_set_value(output, value);

    if (result != CARDANO_SUCCESS)
    {
      goto cleanup;
    }

    result = cardano_transaction_output_set_script_ref(output, reference_script);

    if (result != CARDANO_SUCCESS)
    {
      goto cleanup;
    }

    if (plutus_data_hash != NULL)
    {
      cardano_datum_t* datum = NULL;
      result                 = cardano_datum_new_data_hash(plutus_data_hash, &datum);

      if (result != CARDANO_SUCCESS)
      {
        goto cleanup;
      }

      result = cardano_transaction_output_set_datum(output, datum);
      cardano_datum_unref(&datum);

      if (result != CARDANO_SUCCESS)
      {
        goto cleanup;
      }
    }

    if (plutus_data != NULL)
    {
      cardano_datum_t* datum = NULL;
      result                 = cardano_datum_new_inline_data(plutus_data, &datum);

      if (result != CARDANO_SUCCESS)
      {
        goto cleanup;
      }

      result = cardano_transaction_output_set_datum(output, datum);
      cardano_datum_unref(&datum);

      if (result != CARDANO_SUCCESS)
      {
        goto cleanup;
      }
    }

    result = cardano_utxo_new(input, output, &utxo);

    if (result != CARDANO_SUCCESS)
    {
      goto cleanup;
    }

    result = cardano_utxo_list_add(*utxo_list, utxo);

    if (result != CARDANO_SUCCESS)
    {
      goto cleanup;
    }

    // Clean up allocated resources for this loop iteration
    cardano_transaction_input_unref(&input);
    cardano_transaction_output_unref(&output);
    cardano_utxo_unref(&utxo);
    cardano_blake2b_hash_unref(&tx_id);
    cardano_address_unref(&address);
    cardano_value_unref(&value);
    cardano_blake2b_hash_unref(&plutus_data_hash);
    cardano_plutus_data_unref(&plutus_data);
    cardano_script_unref(&reference_script);
  }

cleanup:
  if (result != CARDANO_SUCCESS)
  {
    cardano_utxo_list_unref(utxo_list);
    cardano_transaction_input_unref(&input);
    cardano_transaction_output_unref(&output);
    cardano_utxo_unref(&utxo);
    cardano_blake2b_hash_unref(&tx_id);
    cardano_address_unref(&address);
    cardano_value_unref(&value);
    cardano_blake2b_hash_unref(&plutus_data_hash);
    cardano_plutus_data_unref(&plutus_data);
    cardano_script_unref(&reference_script);
  }

  json_object_put(parsed_json);
  json_tokener_free(tok);

  return result;
}
