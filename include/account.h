/*
 * libaccounts-svc
 *
 * Copyright (c) 2010 - 2012 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Tarun Kumar <tarun.kr@samsung.com>, Sukumar Moharana <msukumar@samsung.com>, Wonyoung Lee <wy1115.lee@samsung.com>
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

/**
 * @file        account.h
 * @brief       This file contains the Account API for account management
 */

/**
 * @addtogroup CAPI_SOCIAL_ACCOUNT_MODULE
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
typedef bool (*capability_cb)(account_capability_type_e capability_type, account_capability_state_e capability_state, void *user_data);


/**
 * @brief	Connects to the account database.
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_disconnect()
 */
int account_connect(void);


/**
 * @brief	Disconnects from account database.
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval	#ACCOUNT_ERROR_NONE Successful
 * @retval	#ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_connect()
 */
int account_disconnect(void);


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
int account_create(account_h *account);


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
int account_destroy(account_h account);


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
int account_insert_to_db(account_h account, int *account_db_id);


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
int account_delete_from_db_by_id(int account_db_id);


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
int account_delete_from_db_by_user_name(char *user_name, char *package_name);


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
int account_delete_from_db_by_package_name(char *package_name);


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
int account_update_to_db_by_id(account_h account, int account_id);


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
int account_update_to_db_by_user_name(account_h account, const char *user_name, const char *package_name);

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
int account_get_account_id(account_h account, int *account_id);


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
int account_get_user_name(account_h account, char **user_name);


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
int account_set_user_name(account_h account, const char *user_name);


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
int account_get_display_name(account_h account, char **display_name);


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
int account_set_display_name(account_h account, const char *display_name);


/**
 * @brief	Gets the capability detail of account.
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
int account_get_capability(account_h account, capability_cb cb_func, void *user_data);


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
int account_set_capability(account_h account, account_capability_type_e capability_type, account_capability_state_e capability_state);


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
int account_get_icon_path(account_h account, char **icon_path);


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
int account_set_icon_path(account_h account, const char *icon_path);


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
int account_get_domain_name(account_h account, char **domain_name);


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
int account_set_domain_name(account_h account, const char *domain_name);


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
int account_get_email_address(account_h account, char **email_address);


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
int account_set_email_address(account_h account, const char *email_address);


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
int account_get_package_name(account_h account, char **package_name);


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
int account_set_package_name(account_h account, const char *package_name);


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
int account_get_access_token(account_h account, char **access_token);


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
int account_set_access_token(account_h account, const char *access_token);


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
int account_get_user_text(account_h account, int user_text_index, char **user_text);


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
int account_set_user_text(account_h account, int user_text_index, const char *user_text);


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
int account_get_user_int(account_h account, int user_int_index, int *user_integer);


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
int account_set_user_int(account_h account, int user_int_index, int user_integer);


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
int account_get_auth_type(account_h account, account_auth_type_e *auth_type);


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
int account_set_auth_type(account_h account, const account_auth_type_e auth_type);


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
int account_get_secret(account_h account, account_secrecy_state_e *secret);


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
int account_set_secret(account_h account, const account_secrecy_state_e secret);


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
int account_get_source(account_h account, char **source);


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
int account_set_source(account_h account, const char *source);


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
int account_foreach_account_from_db(account_cb callback, void *user_data);


/**
 * @brief	Retrieves all accounts with the account database ID.
 *
 * @param[in]	account_db_id The account database ID to search
 * @param[out] account account handle
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
int account_query_account_by_account_id(int account_db_id, account_h *account);

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
int account_query_account_by_user_name(account_cb callback, const char* user_name, void* user_data);

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
int account_query_account_by_package_name(account_cb callback, const char *package_name, void *user_data);

/**
 * @brief	Retrieves all accounts with the capability.
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
int account_query_account_by_capability(account_cb callback, account_capability_type_e capability_type, account_capability_state_e capability_value, void *user_data);

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
int account_query_capability_by_account_id(capability_cb callback, int account_db_id, void *user_data);


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
int account_get_total_count_from_db(int *count);


/**
* @}
*/


#ifdef __cplusplus
}
#endif

#endif /* __ACCOUNT_H__ */
