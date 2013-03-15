/*
 *  account
 *
 * Copyright (c) 2012 - 2013 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Wonyoung Lee <wy1115.lee@samsung.com>, Sungchan Kim <sungchan81.kim@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __ACCOUNT_H__
#define __ACCOUNT_H__

#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include <account-types.h>
#include <account-error.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ACCOUNT_API
#define ACCOUNT_API __attribute__ ((visibility("default")))
#endif

/**
 * @file        account.h
 * @brief       This file contains the Account API for account management
 */

/**
 * @addtogroup CAPI_ACCOUNT_MANAGER_MODULE
 * @{
 */

/**
 * @brief	Called once for each account from database
 *
 * @param[in]	account	The account handle
 * @param[in]	user_data The user data passed from the foreach function
 *
 * @return @c true to continue with the next iteration of the loop or @c false to break out of the loop.
 *
 * @pre account_foreach_account_from_db(), account_query_account_by_account_id(), account_query_account_by_user_name() or account_query_account_by_package_name() invoke this callback.
 *
 * @see account_foreach_account_from_db()
 * @see account_query_account_by_account_id()
 * @see account_query_account_by_user_name()
 * @see account_query_account_by_package_name()
 */
typedef bool (*account_cb)(account_h account, void *user_data);


/**
 * @brief	Called once for each capability of account from database
 *
 * @param[in]	capability_type The capability type
 * @param[in]	capability_state The capability state
 * @param[in]	user_data The user data passed from the foreach function
 *
 * @return @c true to continue with the next iteration of the loop or @c false to break out of the loop.
 *
 * @pre account_query_capability_by_account_id() invokes this callback.
 *
 * @see account_query_capability_by_account_id()
 */
typedef bool (*capability_cb)(const char* capability_type, account_capability_state_e capability_state, void *user_data);

/**
 * @brief	Called once for each custom data of account from database
 *
 * @param[in]	key The user custom key
 * @param[in]	value The user custom value of the specific key
 * @param[in]	user_data The user data passed
 *
 * @return @c true to continue with the next iteration of the loop or @c false to break out of the loop.
 *
 * @see account_query_capability_by_account_id()
 */
typedef bool (*account_custom_cb)(char* key, char* value, void *user_data);


typedef bool (*account_type_cb)(account_type_h account_type, void *user_data);
typedef bool (*account_label_cb)(char* app_id, char* label, char* locale, void *user_data);
typedef bool (*provider_feature_cb)(char* app_id, char* key, void* user_data);
typedef bool (*account_event_cb)(const char* event_type, int account_id, void* user_data);


/**
 * @brief	Connects to the account database.
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_disconnect()
 */
ACCOUNT_API int account_connect(void);


/**
 * @brief	Disconnects from account database.
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_connect()
 */
ACCOUNT_API int account_disconnect(void);


/**
 * @brief	Creates a handle to the account.
 *
 * @remarks	@a account must be released with account_destroy() by you. \n
 *  The created handle is not added to the account database until account_insert_to_db() is called.
 *
 * @param[in]	account	The account handle
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of Memory
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_destroy()
 */
ACCOUNT_API int account_create(account_h *account);


/**
 * @brief	Destroys the account handle and releases all its resources.
 *
 * @param[in]	account	The account handle
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE	Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_create()
 */
ACCOUNT_API int account_destroy(account_h account);


/**
 * @brief	Inserts the account details to the account database.
 *
 * @param[in]	account	The account handle
 * @param[out]	account_db_id The account ID to be assigned to a account
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED  Database operation failed
 * @retval	#ACCOUNT_ERROR_DUPLICATED  Same user name exist in your application
 * @retval	#ACCOUNT_ERROR_NOT_ALLOW_MULTIPLE  when you try to add an account inspite of multiple account false
 * @retval	#ACCOUNT_ERROR_NOT_REGISTERED_PROVIDER  when you try to add an account though you didn't register account type in manifest
 *
 * @pre	This function requires an open connection to account service by account_connect().
 *
 * @see account_connect()
 * @see account_delete_from_db_by_id()
 * @see account_delete_from_db_by_user_name()
 * @see account_delete_from_db_by_package_name()
 * @see account_update_to_db_by_id()
 * @see account_update_to_db_by_user_name()
 */
ACCOUNT_API int account_insert_to_db(account_h account, int *account_db_id);


/**
 * @brief	Deletes the account from the account database by accound DB ID.
 *
 * @param[in]	account_db_id The account ID to delete
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED  Database operation failed
 *
 * @pre	This function requires an open connection to account service by account_connect()
 *
 * @see account_connect()
 * @see account_insert_to_db()
 * @see account_delete_from_db_by_user_name()
 * @see account_delete_from_db_by_package_name()
 * @see account_update_to_db_by_id()
 * @see account_update_to_db_by_user_name()
 */
ACCOUNT_API int account_delete_from_db_by_id(int account_db_id);


/**
 * @brief	Deletes the account from the account database by user name.
 *
 * @param[in]	user_name The user name of account to delete
 * @param[in]	package_name The package name of account to delete
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED  Database operation failed
 *
 * @pre	This function requires an open connection to account service by account_connect()
 *
 * @see account_connect()
 * @see account_insert_to_db()
 * @see account_delete_from_db_by_id()
 * @see account_delete_from_db_by_package_name()
 * @see account_update_to_db_by_id()
 * @see account_update_to_db_by_user_name()
 */
ACCOUNT_API int account_delete_from_db_by_user_name(char *user_name, char *package_name);


/**
 * @brief	Deletes the account from the account database by package name.
 *
 * @param[in]	package_name The package name of account(s) to delete
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED  Database operation failed
 *
 * @pre	This function requires an open connection to account service by account_connect()
 *
 * @see account_connect()
 * @see account_insert_to_db()
 * @see account_delete_from_db_by_id()
 * @see account_delete_from_db_by_user_name()
 * @see account_update_to_db_by_id()
 * @see account_update_to_db_by_user_name()
 */
ACCOUNT_API int account_delete_from_db_by_package_name(char *package_name);


/**
 * @brief	Updates the account details to the account database.
 *
 * @param[in]	account The account handle
 * @param[in]	account_id The account ID to update
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED  Database operation failed
 * @retval	#ACCOUNT_ERROR_RECORD_NOT_FOUND  The account to update not exist
 *
 * @pre	This function requires an open connection to account service by account_connect()
 *
 * @see account_connect()
 * @see account_insert_to_db()
 * @see account_delete_from_db_by_id()
 * @see account_delete_from_db_by_user_name()
 * @see account_delete_from_db_by_package_name()
 * @see account_update_to_db_by_user_name()
 */
ACCOUNT_API int account_update_to_db_by_id(account_h account, int account_id);


/**
 * @brief	Updates the account details to the account database.
 *
 * @param[in]	account The account handle
 * @param[in]	user_name The user name of account to update
 * @param[in]	package_name The package name for the user name
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED  Database operation failed
 * @retval	#ACCOUNT_ERROR_RECORD_NOT_FOUND  The account to update not exist
 *
 * @pre	This function requires an open connection to account service by account_connect()
 *
 * @see account_connect()
 * @see account_insert_to_db()
 * @see account_delete_from_db_by_id()
 * @see account_delete_from_db_by_user_name()
 * @see account_delete_from_db_by_package_name()
 * @see account_update_to_db_by_id()
 *
 */
ACCOUNT_API int account_update_to_db_by_user_name(account_h account, const char *user_name, const char *package_name);

/**
 * @brief	Gets the account id of account.
 *
 *
 * @param[in]	account The account handle
 * @param[out]	account_id The account id of account
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 */
ACCOUNT_API int account_get_account_id(account_h account, int *account_id);


/**
 * @brief	Gets the user name of account.
 *
 * @remarks	@a user_name must be released with free() by you.
 *
 * @param[in]	account The account handle
 * @param[out]	user_name The user name of account
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see account_set_user_name()
 */
ACCOUNT_API int account_get_user_name(account_h account, char **user_name);


/**
 * @brief	Sets the user name of account.
 *
 * @param[in]	account The account handle
 * @param[in]	user_name The string to set as user name
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_get_user_name()
 */
ACCOUNT_API int account_set_user_name(account_h account, const char *user_name);


/**
 * @brief	Gets the display name of account.
 *
 * @remarks	@a display_name must be released with free() by you.
 *
 * @param[in]	account The account handle
 * @param[out]	display_name The display name of account
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see account_get_display_name()
 */
ACCOUNT_API int account_get_display_name(account_h account, char **display_name);


/**
 * @brief	Sets the display name of account.
 *
 * @param[in]	account The account handle
 * @param[in]	display_name The text string to set as the display name
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_get_display_name()
 */
ACCOUNT_API int account_set_display_name(account_h account, const char *display_name);

/**
 * @brief	Gets the capability detail of account.
 *
 * @param[in]	account The account handle
 * @param[in]	capability_type Capability type to get the capability value
 * @param[out] capability_value Corresponding capability value(on/off) of the specified capability_type
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_RECORD_NOT_FOUND There is no given capability_type in the account
 *
 * @see account_set_capability()
 */
ACCOUNT_API int account_get_capability(account_h account, const char* capability_type, account_capability_state_e* capability_value);

/**
 * @brief	Gets all the capabilities of account.
 *
 * @param[in]	account The account handle
 * @param[in]	callback The callback function
 * @param[in]	user_data The user data to be passed to the callback function
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_set_capability()
 */
ACCOUNT_API int account_get_capability_all(account_h account, capability_cb cb_func, void *user_data);

/**
 * @brief	Sets the capability.
 *
 * @param[in]	account The account handle
 * @param[in]	capability_type The capability type
 * @param[in]	capability_state The capability state
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_get_capability()
 */
ACCOUNT_API int account_set_capability(account_h account, const char* capability_type, account_capability_state_e capability_state);


/**
 * @brief	Gets the icon path.
 *
 * @remarks	@a icon_path must be released with free() by you.
 *
 * @param[in]	account The account handle
 * @param[out]	icon_path The icon path
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see account_set_icon_path()
 */
ACCOUNT_API int account_get_icon_path(account_h account, char **icon_path);


/**
 * @brief	Sets the icon path.
 *
 * @param[in]	account The account handle
 * @param[in]	icon_path The text string to set as a icon path
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_get_icon_path()
 */
ACCOUNT_API int account_set_icon_path(account_h account, const char *icon_path);


/**
 * @brief	Gets the domain name.
 *
 * @remarks	@a domain_name must be released with free() by you.
 *
 * @param[in]	account The account handle
 * @param[out]	domain_name The domain name
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see account_set_domain_name()
 */
ACCOUNT_API int account_get_domain_name(account_h account, char **domain_name);


/**
 * @brief	Sets the domain name.
 *
 * @param[in]	account The account handle
 * @param[in]	domain_name The text string to set as a domain name
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_get_domain_name()
 */
ACCOUNT_API int account_set_domain_name(account_h account, const char *domain_name);


/**
 * @brief	Gets the email address.
 *
 * @remarks	@a email_address must be released with free() by you.
 *
 * @param[in]	account The account handle
 * @param[out]	email_address The email address
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see account_set_email_address()
 */
ACCOUNT_API int account_get_email_address(account_h account, char **email_address);


/**
 * @brief	Sets the email address.
 *
 * @param[in]	account The account handle
 * @param[in]	email_address The text string to set as a email address
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_get_email_addres()
 */
ACCOUNT_API int account_set_email_address(account_h account, const char *email_address);


/**
 * @brief	Gets the package name.
 *
 * @remarks	@a package_name must be released with free() by you.
 *
 * @param[in]	account The account handle
 * @param[out]	package_name The package name
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see account_set_package_name()
 */
ACCOUNT_API int account_get_package_name(account_h account, char **package_name);


/**
 * @brief	Sets the package name.
 *
 * @param[in]	account The account handle
 * @param[in]	package_name The text string to set as a package name
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_get_email_addres()
 */
ACCOUNT_API int account_set_package_name(account_h account, const char *package_name);


/**
 * @brief	Gets the access token.
 *
 * @remarks	@a access_token must be released with free() by you.
 *
 * @param[in]	account The account handle
 * @param[out]	access_token The access token
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see account_set_access_token()
 */
ACCOUNT_API int account_get_access_token(account_h account, char **access_token);


/**
 * @brief	Set the access token.
 *
 * @param[in]	account The account handle
 * @param[in]	access_token The text string to set as a access token
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_get_access_token()
 */
ACCOUNT_API int account_set_access_token(account_h account, const char *access_token);


/**
 * @brief	Gets the user text.
 *
 * @remarks	@a user_text must be released with free() by you.
 *
 * @param[in]	account The account handle
 * @param[in]	user_text_index The index of the user text (range: 0 ~ 4)
 * @param[out]	user_text The user text
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see account_set_user_text()
 */
ACCOUNT_API int account_get_user_text(account_h account, int user_text_index, char **user_text);


/**
 * @brief	Sets the user text.
 *
 * @param[in]	account The account handle
 * @param[in]	user_text_index The index of the user text (must be in range from 0 to 4)
 * @param[in]	user_text The text string to set as a user txt
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_get_user_text()
 */
ACCOUNT_API int account_set_user_text(account_h account, int user_text_index, const char *user_text);


/**
 * @brief	Gets the user integer.
 *
 * @param[in]	account The account handle
 * @param[in]	user_int_index The index of the user integer (must be in range from 0 to 4)
 * @param[out]	user_integer The user interger
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_set_user_int()
 */
ACCOUNT_API int account_get_user_int(account_h account, int user_int_index, int *user_integer);


/**
 * @brief	Set the user integer.
 *
 * @param[in]	account The account handle
 * @param[in]	user_int_index The index of the user integer (must be in range from 0 to 4)
 * @param[in]	user_integer The integer to set as user integer
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_get_user_int()
 */
ACCOUNT_API int account_set_user_int(account_h account, int user_int_index, int user_integer);


/**
 * @brief	Gets the auth type.
 *
 * @param[in]	account The account handle
 * @param[out]	auth_type The auth type
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_set_auth_type()
 */
ACCOUNT_API int account_get_auth_type(account_h account, account_auth_type_e *auth_type);


/**
 * @brief	Sets the auth type.
 *
 * @param[in]	account The account handle
 * @param[in]	auth_type Integer to be set as auth type
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_get_auth_type()
 */
ACCOUNT_API int account_set_auth_type(account_h account, const account_auth_type_e auth_type);


/**
 * @brief	Gets the secret.
 *
 * @param[in]	account The account handle
 * @param[out]	secret The secret
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_set_secret()
 */
ACCOUNT_API int account_get_secret(account_h account, account_secrecy_state_e *secret);


/**
 * @brief	Sets the secret.
 *
 * @param[in]	account The account handle
 * @param[in]	secret Secrecy to be set
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_get_secret()
 */
ACCOUNT_API int account_set_secret(account_h account, const account_secrecy_state_e secret);

/**
 * @brief	Gets the sync support.
 *
 * @param[in]	account The account handle
 * @param[out]	sync_support The sync support
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_set_sync_support()
 */
ACCOUNT_API int account_get_sync_support(account_h account, account_sync_state_e *sync_support);


/**
 * @brief	Sets the sync support.
 *
 * @param[in]	account The account handle
 * @param[in]	sync_support sync state to be set
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_get_sync_support()
 */
ACCOUNT_API int account_set_sync_support(account_h account, const account_sync_state_e sync_support);


/**
 * @brief	Gets the source.
 *
 * @remarks	@a user_text must be released with free() by you.
 *
 * @param[in]	account The account handle
 * @param[out]	source The source
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see account_set_source()
 */
ACCOUNT_API int account_get_source(account_h account, char **source);


/**
 * @brief	Sets the source.
 *
 * @param[in]	account The account handle
 * @param[in]	source The text string to set as a source
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_get_source()
 */
ACCOUNT_API int account_set_source(account_h account, const char *source);

/**
 * @brief	Sets the capability.
 *
 * @param[in]	account The account handle
 * @param[in]	key The user custom key for specific value
 * @param[in]	value The user custom value about the given key
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_get_custom()
 */
ACCOUNT_API int account_set_custom(account_h account, const char* key, const char* value);

/**
 * @brief	Gets the user specific custom text of account key.
 *
 * @param[in]	account The account handle
 * @param[in]	callback The callback function to retreive custom text. The callback function gives key and value
 * @param[out] value text of the given key
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_RECORD_NOT_FOUND There is no given capability_type in the account
 *
 * @see account_set_custom()
 */
ACCOUNT_API int account_get_custom(account_h account, const char* key, char** value);


/**
 * @brief	Gets all the user custom texts of account.
 *
 * @param[in]	account The account handle
 * @param[in]	callback The callback function to retreive all custom text. The callback function gives key and value
 * @param[in]	user_data The user data to be passed to the callback function
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_set_custom()
 */
ACCOUNT_API int account_get_custom_all(account_h account, account_custom_cb cb_func, void* user_data);

/**
 * @brief	Retrieves all accounts details by invoking the given callback function iteratively.
 *
 * @param[in]	callback The callback function to invoke
 * @param[in]	user_data The user data to be passed to the callback function
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED Database operation failed
 *
 * @pre	This function requires an open connection to account service by account_connect().
 * @post	This function invokes account_cb().
 *
 * @see account_connect()
 * @see account_query_account_by_account_id()
 * @see account_query_account_by_user_name()
 * @see account_query_account_by_package_name()
 * @see account_query_account_by_capability()
 */
ACCOUNT_API int account_foreach_account_from_db(account_cb callback, void *user_data);


/**
 * @brief	Retrieves all accounts with the account database ID.
 *
 * @param[in]	account_db_id The account database ID to search
 * @param[out] account account handle. Must be allocated by account_create and free after using by account_destroy.
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED Database operation failed
 *
 * @pre	This function requires an open connection to account service by account_connect().
 *
 * @see account_connect()
 * @see account_query_account_by_account_id()
 * @see account_query_account_by_user_name()
 * @see account_query_account_by_package_name()
 * @see account_query_account_by_capability()
 */
ACCOUNT_API int account_query_account_by_account_id(int account_db_id, account_h *account);

/**
 * @brief	Retrieves all accounts with the user name.
 *
 * @param[in]	callback The callback function to invoke
 * @param[in]	user_name The user name to search
 * @param[in]	user_data The user data to be passed to the callback function
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED Database operation failed
 *
 * @pre	This function requires an open connection to account service by account_connect().
 * @post	This function invokes account_cb().
 *
 * @see account_connect()
 * @see account_foreach_account_from_db()
 * @see account_query_account_by_account_id()
 * @see account_query_account_by_package_name()
 * @see account_query_account_by_capability()
 *
 */
ACCOUNT_API int account_query_account_by_user_name(account_cb callback, const char* user_name, void* user_data);

/**
 * @brief	Retrieves all accounts with the package name.
 *
 * @param[in]	callback The callback function to invoke
 * @param[in]	package_name The package name to search
 * @param[in]	user_data The user data to be passed to the callback function
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED Database operation failed
 *
 * @pre	This function requires an open connection to account service by account_connect().
 * @post	This function invokes account_cb().
 *
 * @see account_connect()
 * @see account_foreach_account_from_db()
 * @see account_query_account_by_account_id()
 * @see account_query_account_by_user_name()
 * @see account_query_account_by_capability()
 */
ACCOUNT_API int account_query_account_by_package_name(account_cb callback, const char *package_name, void *user_data);

/**
 * @brief	Retrieves all accounts with the capability type and capability value.
 *
 * @param[in]	callback The callback function to invoke
 * @param[in]	capability_type The capablity type to search
 * @param[in]	capability_value The capablity value to search
 * @param[in]	user_data The user data to be passed to the callback function
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED Database operation failed
 *
 * @pre This function requires an open connection to account service by account_connect().
 * @post	This function invokes account_cb().
 *
 * @see account_connect()
 * @see account_foreach_account_from_db()
 * @see account_query_account_by_account_id()
 * @see account_query_account_by_user_name()
 * @see account_query_account_by_package_name()
 */
ACCOUNT_API int account_query_account_by_capability(account_cb callback, const char* capability_type, account_capability_state_e capability_value, void *user_data);

/**
 * @brief	Retrieves all accounts with the capability type.
 *
 * @param[in]	callback The callback function to invoke
 * @param[in]	capability_type The capablity type to search
 * @param[in]	user_data The user data to be passed to the callback function
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED Database operation failed
 *
 * @pre This function requires an open connection to account service by account_connect().
 * @post	This function invokes account_cb().
 *
 * @see account_connect()
 * @see account_foreach_account_from_db()
 * @see account_query_account_by_account_id()
 * @see account_query_account_by_user_name()
 * @see account_query_account_by_package_name()
 */
ACCOUNT_API int account_query_account_by_capability_type(account_cb cb_func, const char* capability_type, void* user_data);

/**
 * @brief	Retrieves all capabilities with the account database ID.
 *
 * @param[in]	callback The callback function to invoke
 * @param[in]	account_db_id The account database ID to search
 * @param[in]	user_data The user data to be passed to the callback function
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED Database operation failed
 *
 * @pre	This function requires an open connection to account service by account_connect().
 * @post	This function invokes capability_cb().
 *
 * @see account_connect()
 * @see account_get_capability()
 * @see account_set_capability()
 */
ACCOUNT_API int account_query_capability_by_account_id(capability_cb callback, int account_db_id, void *user_data);


/**
 * @brief	Retrieves number of account in the account database.
 *
 * @param[out] out parameter for number of all accounts
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_DB_FAILED Database operation failed
 *
 * @pre	This function requires an open connection to account service by account_connect().
 *
 * @see account_connect()
 */
ACCOUNT_API int account_get_total_count_from_db(int *count);

/**
 * @brief	changes sync status of account by account id
 *
 * @param[in]	account_db_id The account id which sync status needs to be change
 * @param[in]	sync_status New sync status
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED Database operation failed
 *
 * @pre	This function requires an open connection to account service by account_connect().
 *
 * @see account_connect()
 */
ACCOUNT_API int account_update_sync_status_by_id(int account_db_id, const account_sync_state_e sync_status);

/**
 * @brief	Called once for each service provider of account
 *
 * @param[in]	package_name package name of the service
 * @param[in]	icon_path icon path of the service
 * @param[in]	multiple if true supports multiple account else only one account support
 * @param[in]	user_data The user data passed from the account_query_service_list_by_capability function
 *
 * @return @c true to continue with the next iteration of the loop or @c false to break out of the loop.
 *
 * @pre account_query_service_list_by_capability() invokes this callback.
 *
 * @see account_query_service_list_by_capability()
 */
typedef bool (*service_list_cb)(char *package_name, char *icon_path, bool multiple, void *user_data);

/**
 * @brief	Retrieves all services which supports specified capability.
 *
 * @param[in]	callback The callback function to invoke
 * @param[in]	capability_type The capablity type to search
 * @param[in]	user_data The user data to be passed to the callback function
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_XML_PARSE_FAILED XML Parsing failed
 * @retval	#ACCOUNT_ERROR_XML_FILE_NOT_FOUND XML file not found
 *
 * @post	This function invokes service_list_cb().
 *
 */
ACCOUNT_API int account_query_service_list_by_capability(service_list_cb callback, const char* capability_type, void *user_data);


/**
 * @brief	Get whether application supports desired capability or not.
 *
 * @param[in]	package_name	The application's package name
 * @param[in]	capability_filter	which capability to check for
 * @param[out] supported		whether desired capability is supported, Returns true otherwise false
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_XML_PARSE_FAILED  XML Parsing failed
 * @retval	#ACCOUNT_ERROR_XML_FILE_NOT_FOUND XML file not found
 *
 */
ACCOUNT_API int account_get_application_capability_support(char *package_name, char *capability_filter, bool *supported);


/**
 * @brief	Get whether application supports multiple accounts or not.
 *
 * @param[in]	package_name	The application's package name
 * @param[out] supported		if multiple account is supported returns true otherwise false
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_XML_PARSE_FAILED  XML Parsing failed
 * @retval	#ACCOUNT_ERROR_XML_FILE_NOT_FOUND XML file not found
 *
 */
ACCOUNT_API int account_get_application_multiple_account_support(char * package_name, bool * supported);

/**
 * @brief	Get application's icon path.
 *
 * @remarks	@a icon_path must be released with free() by you.
 *
 * @param[in]	package_name	The application's package name
 * @param[out] icon_path		if path is availabe it will have icon_path, otherwise NULL
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_XML_PARSE_FAILED  XML Parsing failed
 * @retval	#ACCOUNT_ERROR_XML_FILE_NOT_FOUND XML file not found
 *
 */
ACCOUNT_API int account_get_application_icon_path(const char *package_name, char **icon_path);


/* Account type API */


/**
 * @brief	Creates a handle to the account.
 *
 * @remarks	@a account type handle must be released with account_type_destroy() by you. \n
 *  The created handle is not added to the account database until account_type_insert_to_db() is called.
 *
 * @param[in]	account_type	The account type handle
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of Memory
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_type_destroy()
 */
ACCOUNT_API int account_type_create(account_type_h *account_type);

/**
 * @brief	Destroys the account type handle and releases all its resources.
 * When you get account_type_h through account_type_create, the handle should be released by account_destroy to avoid memory leak
 *
 * @param[in]	account_type	The account type handle
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE	Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_type_create()
 */
ACCOUNT_API int account_type_destroy(account_type_h account_type);

/**
 * @brief	Sets the app id. It should be filled with your application id. e.g) com.tizen.testapp
 *
 * @remarks	@ app id is mandatory field and is not allowed duplicated app id in account type database
 *
 * @param[in]	account_type The account type handle. It should be assigned by account_type_create()
 * @param[in]	app_id Application id of yours.
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see account_type_get_app_id()
 */
ACCOUNT_API int account_type_set_app_id(account_type_h account_type, const char *app_id);

/**
 * @brief	Sets the service provider id. It should be filled with your service provider id.
 *
 * @remarks	@ service provider id is mandatory field.
 *
 * @param[in]	account_type The account type handle. It should be assigned by account_type_create()
 * @param[in]	service_provider_id Service provider id of yours.
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see account_type_get_service_provider_id()
 */
ACCOUNT_API int account_type_set_service_provider_id(account_type_h account_type, const char *service_provider_id);

/**
 * @brief	Sets icon path. It represents your service provider or application.
 *
 * @remarks	@ icon_path is not mandatory field. But when it is set, you can display this icon in add account screen.
 *
 * @param[in]	account_type The account type handle. It should be assigned by account_type_create()
 * @param[in]	icon_path of your application.
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see account_type_get_icon_path()
 */
ACCOUNT_API int account_type_set_icon_path(account_type_h account_type, const char *icon_path);

/**
 * @brief	Sets small icon path. It also represents your service provider or application.
 *
 * @remarks	@ small icon_path is not mandatory field.
 *
 * @param[in]	account_type The account type handle. It should be assigned by account_type_create()
 * @param[in]	small_icon_path the scaled icon of your application.
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see account_type_get_small_icon_path()
 */
ACCOUNT_API int account_type_set_small_icon_path(account_type_h account_type, const char *small_icon_path);

/**
 * @brief	Sets multiple account. It represents whether your application support multiple accounts or not.
 *
 * @remarks	@ The default value of multiple account support is FALSE.
 *
 * @param[in]	account_type The account type handle. It should be assigned by account_type_create()
 * @param[in]	multiple_account_support TRUE means that your application can make two or more accounts. FALSE means that your application can make only one account.
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see account_type_get_multiple_account_support()
 */
ACCOUNT_API int account_type_set_multiple_account_support(account_type_h account_type, const bool multiple_account_support);

/**
 * @brief	Sets label and locale. Label represents the name of account type.
 *
 * @param[in]	account_type The account type handle. It should be assigned by account_type_create()
 * @param[in]	label The name of account depend on the specified locale
 * @param[in]	locale Locale of the corresponding label
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see account_type_get_label()
 */
ACCOUNT_API int account_type_set_label(account_type_h account_type, const char* label, const char* locale);

/**
 * @brief	Sets the provider feature.
 *
 * @param[in]	account_type The account type handle
 * @param[in]	provider_feature This will be capability key of account
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_set_capability()
 */
ACCOUNT_API int account_type_set_provider_feature(account_type_h account_type, const char* provider_feature);

/**
 * @brief	Retrieves provider feature information with your applicaiton id.
 *
 * @param[in]	cb_func Callback function will carry provider feature name of app id.
 * @param[in]	app_id Your application ID to search
 * @param[in] user_data User data if you have your private data to carry into callback function, you can use.
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED Database operation failed
 *
 * @pre	This function requires an open connection to account service by account_connect().
 *
 * @see account_connect()
 * @see account_type_set_provider_feature()
 * @see account_disconnect()
 */
ACCOUNT_API int account_type_query_provider_feature_by_app_id(provider_feature_cb cb_func, const char* app_id, void *user_data );

/**
 * @brief	Retrieves label information with your applicaiton id.
 *
 * @param[in]	cb_func Callback function will carry provider feature name of app id.
 * @param[in]	app_id Your application ID to search
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	TRUE means the application supports the given capability
 * @retval	FALSE means the application does not support the given capability
 *
 * @pre	This function requires an open connection to account service by account_connect().
 *
 * @see account_connect()
 * @see account_type_set_provider_feature()
 * @see account_disconnect()
 */
ACCOUNT_API bool account_type_query_supported_feature(const char* app_id, const char* capability);

/**
 * @brief	Gets the application id of account type.
 *
 * @remarks	@a app_id must be released with free() by you.
 *
 * @param[in]	account_type The account type handle. It should be given by account_type_query_* functions or account_type_foreach_account_type_from_db
 * @param[out] app_id application id of account type item
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see account_type_foreach_account_type_from_db()
 * @see account_type_query_by_app_id()
 * @see account_type_set_app_id()
 */
ACCOUNT_API int account_type_get_app_id(account_type_h account_type, char **app_id);

/**
 * @brief	Gets the service provider id of account type.
 *
 * @remarks	@a service_provider_id must be released with free() by you.
 *
 * @param[in]	account_type The account type handle. It should be given by account_type_query_* functions or account_type_foreach_account_type_from_db
 * @param[out] service_provider_id service provider text id of account type item
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see account_type_foreach_account_type_from_db()
 * @see account_type_query_by_app_id()
 * @see account_type_set_service_provider_id()
 */
ACCOUNT_API int account_type_get_service_provider_id(account_type_h account_type, char **service_provider_id);

/**
 * @brief	Gets icon path of account type.
 *
 * @remarks	@a icon_path must be released with free() by you.
 *
 * @param[in]	account_type The account type handle. It should be given by account_type_query_* functions or account_type_foreach_account_type_from_db
 * @param[out] icon_path It represents the icon of account type item
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see account_type_foreach_account_type_from_db()
 * @see account_type_query_by_app_id()
 * @see account_type_set_icon_path()
 */
ACCOUNT_API int account_type_get_icon_path(account_type_h account_type, char **icon_path);

/**
 * @brief	Gets small icon path of account type.
 *
 * @remarks	@a small_icon_path must be released with free() by you.
 *
 * @param[in]	account_type The account type handle. It should be given by account_type_query_* functions or account_type_foreach_account_type_from_db
 * @param[out] small_icon_path It represents the small icon of account type item
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see account_type_foreach_account_type_from_db()
 * @see account_type_query_by_app_id()
 * @see account_type_set_small_icon_path()
 */
ACCOUNT_API int account_type_get_small_icon_path(account_type_h account_type, char **small_icon_path);

/**
 * @brief	Gets small icon path of account type.
 *
 * @param[in]	account_type The account type handle. It should be given by account_type_query_* functions or account_type_foreach_account_type_from_db
 * @param[out] multiple_account_support It represents whether the retreived account type supports over two accounts or not. TRUE or FALSE
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see account_type_foreach_account_type_from_db()
 * @see account_type_query_by_app_id()
 * @see account_type_set_small_icon_path()
 */
ACCOUNT_API int account_type_get_multiple_account_support(account_type_h account_type, int *multiple_account_support);

/**
 * @brief	Retrieves provider feature information with the given account_type_h.
 *
 * @param[in]	account_type The account type handle. It should be given by account_type_query_* functions or account_type_foreach_account_type_from_db
 * @param[in]	cb_func Callback function will carry provider feature name of app id.
 * @param[in] user_data User data if you have your private data to carry into callback function, you can use.
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED Database operation failed
 *
 * @pre	This function requires an open connection to account service by account_connect().
 *
 * @see account_connect()
 * @see account_type_set_provider_feature()
 * @see account_disconnect()
 */
ACCOUNT_API int account_type_get_provider_feature_all(account_type_h account_type, provider_feature_cb cb_func, void* user_data);

/**
 * @brief	Gets the label information detail of account type.
 *
 * @param[in]	account_type The account type handle. It should be given by account_type_query_* functions or account_type_foreach_account_type_from_db
 * @param[in]	callback The callback function carrying label information
 * @param[in]	user_data The user data to be passed to the callback function
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_type_foreach_account_type_from_db()
 * @see account_type_query_by_app_id()
 * @see account_type_set_label()
 */
ACCOUNT_API int account_type_get_label(account_type_h account_type, account_label_cb cb_func, void *user_data);

/**
 * @brief	Inserts the account type details to the database.
 *
 * @param[in]	account_type	The account handle which is created by account_type_create(). And the madatory fields must be filled with account_type_set_* functions
 * @param[out]	account_type_id The account type ID to be assigned after inserting account type handle.
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED  Database operation failed
 *
 * @pre	This function requires an open connection to account service by account_connect().
 * @post	account_disconnect() is recommended after insertion
 *
 * @see account_connect()
 * @see account_type_set_app_id()
 * @see account_type_set_service_provider_id()
 * @see account_type_set_icon_path()
 * @see account_type_set_small_icon_path()
 * @see account_type_set_multiple_account_support()
 * @see account_type_set_label()
 * @see account_type_delete_by_app_id()
 * @see account_type_update_to_db_by_app_id()
 * @see account_disconnect()
 */
ACCOUNT_API int account_type_insert_to_db(account_type_h account_type, int* account_type_id);

/**
 * @brief	Updates the account details to the account database.
 *
 * @param[in]	account_type The account handle which is created by account_type_create(). And the madatory fields must be filled with account_type_set_* functions
 * @param[in]	app_id Application id of account type
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED  Database operation failed
 *
 * @pre	This function requires an open connection to account service by account_connect()
 * @post	account_disconnect() is recommended after update
 *
 * @see account_connect()
 * @see account_type_insert_to_db()
 * @see account_type_set_app_id()
 * @see account_type_set_service_provider_id()
 * @see account_type_set_icon_path()
 * @see account_type_set_small_icon_path()
 * @see account_type_set_multiple_account_support()
 * @see account_type_set_label()
 * @see account_type_delete_by_app_id()
 * @see account_disconnect()
 */
ACCOUNT_API int account_type_update_to_db_by_app_id(const account_type_h account_type, const char* app_id);

/**
 * @brief	Deletes the account type from the account database by application id.
 *
 * @param[in]	app_id Application id of account type to be deleted
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED  Database operation failed
 *
 * @pre	This function requires an open connection to account service by account_connect()
 *
 * @see account_connect()
 * @see account_type_insert_to_db()
 * @see account_type_update_to_db_by_app_id()
 * @see account_disconnect()
 */
ACCOUNT_API int account_type_delete_by_app_id(const char* app_id);

/**
 * @brief	Retrieves label information with your applicaiton id.
 *
 * @param[in]	cb_func Callback function will carry label_h for label information. label_h contains label info as parameter.
 * @param[in]	app_id Your application ID to search
 * @param[in] user_data User data if you have your private data to carry into callback function, you can use.
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED Database operation failed
 *
 * @pre	This function requires an open connection to account service by account_connect().
 *
 * @see account_connect()
 * @see account_type_query_by_app_id()
 * @see account_type_foreach_account_type_from_db()
 * @see account_disconnect()
 */
ACCOUNT_API int account_type_query_label_by_app_id(account_label_cb cb_func, const char* app_id, void *user_data );

/**
 * @brief	Retrieves account type information with your applicaiton id.
 *
 * @param[in]	app_id Your application ID to search
 * @param[out] account_type	The account handle which is created by account_type_create().
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED Database operation failed
 * @retval	#ACCOUNT_ERROR_RECORD_NOT_FOUND query data is not exist
 *
 * @pre	This function requires an open connection to account service by account_connect().
 *
 * @see account_connect()
 * @see account_type_create()
 * @see account_type_get_app_id()
 * @see account_type_get_service_provider_id()
 * @see account_type_get_icon_path()
 * @see account_type_get_small_icon_path()
 * @see account_type_get_multiple_account_support()
 * @see account_type_get_label()
 * @see account_type_destroy()
 */
ACCOUNT_API int account_type_query_by_app_id(const char* app_id, account_type_h *account_type);

/**
 * @brief	Retrieves all account type information with your applicaiton id.
 *
 * @param[in]	callback Retrieving account type information. You can get account information through account_type_get_* with the carried account_type_handle
 * @param[in] user_data User data. It will be carried through your callback function.
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED Database operation failed
 *
 * @pre	This function requires an open connection to account service by account_connect().
 *
 * @see account_connect()
 * @see account_type_create()
 * @see account_type_get_app_id()
 * @see account_type_get_service_provider_id()
 * @see account_type_get_icon_path()
 * @see account_type_get_small_icon_path()
 * @see account_type_get_multiple_account_support()
 * @see account_type_get_label()
 * @see account_type_destroy()
 */
ACCOUNT_API int account_type_foreach_account_type_from_db(account_type_cb callback, void *user_data);

/**
 * @brief	Retrieves all account type information with your applicaiton id.
 *
 * @param[in]	app_id Your application id
 * @param[in] locale Locale value to search label. e.g) en_US
 * @param[out] label label text corresponding app_id and locale. It must be free.
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED Database operation failed
 *
 * @pre	This function requires an open connection to account service by account_connect().
 *
 * @see account_connect()
 */
ACCOUNT_API int account_type_query_label_by_locale(const char* app_id, const char* locale, char** label);

/**
 * @brief	Retrieves account type information with provider feature name.
 *
 * @param[in] cb_func callback function to retrieve account type information
 * @param[in] key provider feature value to search account type . e.g) ACCOUNT_SUPPORTS_CAPABILITY_CONTACT or	"http://tizen.org/account/capability/contact"
 * @param[out] account_type_h handle corresponding provider feature name.
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_DB_FAILED Database operation failed
 *
 * @pre	This function requires an open connection to account service by account_connect().
 *
 * @see account_connect()
 */
ACCOUNT_API int account_type_query_by_provider_feature(account_type_cb cb_func, const char* key, void* user_data);

/**
 * @brief	Check account type db whether the given app_id exists.
 *
 * @param[in] app_id application id to check
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_RECORD_NOT_FOUND record not found
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER invalid app id
 *
 * @pre	This function requires an open connection to account service by account_connect().
 *
 * @see account_type_query_by_app_id()
 */
ACCOUNT_API int account_type_query_app_id_exist(const char* app_id);



/* End of account type api  */

/**
 * @brief	Creates a handle for account event subscription.
 *
 * @remarks	@a account_subscribe handle must be released with account_unsubscribe_notification() by you. \n
 *
 * @param[in]	account_subscribe	The account subscription handle
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_OUT_OF_MEMORY Out of Memory
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_unsubscribe_notification()
 * @see account_subscribe_notification()
 */
ACCOUNT_API int account_subscribe_create(account_subscribe_h* account_subscribe);

/**
 * @brief	Start to subscribe account event through the given callback function
 *
 * @param[in]	account_subscribe	The account subscription handle
 * @param[in]	cb_func	When an account is removed from account database. It will be called with event message and account id.
 * @param[in]	user_data user_data will be delivered to cb_func
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_EVENT_SUBSCRIPTION_FAIL Subscription fail
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_unsubscribe_notification()
 * @see account_subscribe_notification()
 */
ACCOUNT_API int account_subscribe_notification(account_subscribe_h account_subscribe, account_event_cb cb_func, void* user_data);

/**
 * @brief	Destroys the account subscribe handle and releases all its resources.
 *
 * @remarks	@account_unsubscribe_notification must be called when you don't need to subscribe account event \n
 *
 * @param[in]	account_subscribe	The account subscription handle
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE	Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval	#ACCOUNT_ERROR_EVENT_SUBSCRIPTION_FAIL Unsubscription fail
 *
 * @see account_create()
 */
ACCOUNT_API int account_unsubscribe_notification(account_subscribe_h account_subscribe);

/**
* @}
*/


#ifdef __cplusplus
}
#endif

#endif /* __ACCOUNT_H__ */
