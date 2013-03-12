/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <tet_api.h>
#include <account.h>
#include <account-types.h>
#include <account-error.h>

#define TEST_PACKAGE_NAME		"com.samsung.facebook"

static void startup(void);
static void cleanup(void);

void (*tet_startup)(void) = startup;
void (*tet_cleanup)(void) = cleanup;

static void utc_account_connect_positive(void);
static void utc_account_connect_negative(void);
static void utc_account_create_positive(void);
static void utc_account_create_negative(void);
static void utc_account_destroy_positive(void);
static void utc_account_destroy_negative(void);
static void utc_account_insert_to_db_positive(void);
static void utc_account_insert_to_db_negative(void);
static void utc_account_delete_from_db_by_id_positive(void);
static void utc_account_delete_from_db_by_id_negative(void);
static void utc_account_delete_from_db_by_user_name_positive(void);
static void utc_account_delete_from_db_by_user_name_negative(void);
static void utc_account_delete_from_db_by_package_name_positive(void);
static void utc_account_delete_from_db_by_package_name_negative(void);
static void utc_account_update_to_db_by_id_positive(void);
static void utc_account_update_to_db_by_id_negative(void);
static void utc_account_update_to_db_by_user_name_positive(void);
static void utc_account_update_to_db_by_user_name_negative(void);
static void utc_account_update_sync_status_by_id_positive(void);
static void utc_account_update_sync_status_by_id_negative(void);
static void utc_account_get_account_id_positive(void);
static void utc_account_get_account_id_negative(void);
static void utc_account_get_user_name_positive(void);
static void utc_account_get_user_name_negative(void);
static void utc_account_set_user_name_positive(void);
static void utc_account_set_user_name_negative(void);
static void utc_account_get_display_name_positive(void);
static void utc_account_get_display_name_negative(void);
static void utc_account_set_display_name_positive(void);
static void utc_account_set_display_name_negative(void);
static void utc_account_get_capability_positive(void);
static void utc_account_get_capability_negative(void);
static void utc_account_set_capability_positive(void);
static void utc_account_set_capability_negative(void);
static void utc_account_get_icon_path_positive(void);
static void utc_account_get_icon_path_negative(void);
static void utc_account_set_icon_path_positive(void);
static void utc_account_set_icon_path_negative(void);
static void utc_account_get_domain_name_positive(void);
static void utc_account_get_domain_name_negative(void);
static void utc_account_set_domain_name_positive(void);
static void utc_account_set_domain_name_negative(void);
static void utc_account_get_email_address_positive(void);
static void utc_account_get_email_address_negative(void);
static void utc_account_set_email_address_positive(void);
static void utc_account_set_email_address_negative(void);
static void utc_account_get_package_name_positive(void);
static void utc_account_get_package_name_negative(void);
static void utc_account_set_package_name_positive(void);
static void utc_account_set_package_name_negative(void);
static void utc_account_get_access_token_positive(void);
static void utc_account_get_access_token_negative(void);
static void utc_account_set_access_token_positive(void);
static void utc_account_set_access_token_negative(void);
static void utc_account_get_user_text_positive(void);
static void utc_account_get_user_text_negative(void);
static void utc_account_set_user_text_positive(void);
static void utc_account_set_user_text_negative(void);
static void utc_account_get_user_int_positive(void);
static void utc_account_get_user_int_negative(void);
static void utc_account_set_user_int_positive(void);
static void utc_account_set_user_int_negative(void);
static void utc_account_get_auth_type_positive(void);
static void utc_account_get_auth_type_negative(void);
static void utc_account_set_auth_type_positive(void);
static void utc_account_set_auth_type_negative(void);
static void utc_account_get_secret_positive(void);
static void utc_account_get_secret_negative(void);
static void utc_account_set_secret_positive(void);
static void utc_account_set_secret_negative(void);
static void utc_account_get_sync_support_positive(void);
static void utc_account_get_sync_support_negative(void);
static void utc_account_set_sync_support_positive(void);
static void utc_account_set_sync_support_negative(void);
static void utc_account_get_source_positive(void);
static void utc_account_get_source_negative(void);
static void utc_account_set_source_positive(void);
static void utc_account_set_source_negative(void);
static void utc_account_foreach_account_from_db_positive(void);
static void utc_account_foreach_account_from_db_negative(void);
static void utc_account_query_account_by_account_id_positive(void);
static void utc_account_query_account_by_account_id_negative(void);
static void utc_account_query_account_by_user_name_positive(void);
static void utc_account_query_account_by_user_name_negative(void);
static void utc_account_query_account_by_package_name_positive(void);
static void utc_account_query_account_by_package_name_negative(void);
static void utc_account_query_account_by_capability_positive(void);
static void utc_account_query_account_by_capability_negative(void);
static void utc_account_query_capability_by_account_id_positive(void);
static void utc_account_query_capability_by_account_id_negative(void);
static void utc_account_get_total_count_from_db_positive(void);
static void utc_account_get_total_count_from_db_negative(void);


struct tet_testlist tet_testlist[] = {
	{utc_account_connect_positive, 1},
	//{utc_account_connect_negative, 1},
	{utc_account_create_positive, 1},
	{utc_account_create_negative, 1},
	{utc_account_destroy_positive, 1},
	//{utc_account_destroy_negative, 1},
	{utc_account_insert_to_db_positive, 1},
	{utc_account_insert_to_db_negative, 1},
	{utc_account_update_to_db_by_id_positive, 1},
	{utc_account_update_to_db_by_id_negative, 1},
	{utc_account_update_to_db_by_user_name_positive, 1},
	{utc_account_update_to_db_by_user_name_negative, 1},
	{utc_account_update_sync_status_by_id_positive, 1},
	{utc_account_update_sync_status_by_id_negative, 1},
	{utc_account_get_account_id_positive, 1},
	{utc_account_get_account_id_negative, 1},
	{utc_account_get_user_name_positive, 1},
	{utc_account_get_user_name_negative, 1},
	{utc_account_set_user_name_positive, 1},
	{utc_account_set_user_name_negative, 1},
	{utc_account_get_display_name_positive, 1},
	{utc_account_get_display_name_negative, 1},
	{utc_account_set_display_name_positive, 1},
	{utc_account_set_display_name_negative, 1},
	{utc_account_get_capability_positive, 1},
	{utc_account_get_capability_negative, 1},
	{utc_account_set_capability_positive, 1},
	{utc_account_set_capability_negative, 1},
	{utc_account_get_icon_path_positive, 1},
	{utc_account_get_icon_path_negative, 1},
	{utc_account_set_icon_path_positive, 1},
	{utc_account_set_icon_path_negative, 1},
	{utc_account_get_domain_name_positive, 1},
	{utc_account_get_domain_name_negative, 1},
	{utc_account_set_domain_name_positive, 1},
	{utc_account_set_domain_name_negative, 1},
	{utc_account_get_email_address_positive, 1},
	{utc_account_get_email_address_negative, 1},
	{utc_account_set_email_address_positive, 1},
	{utc_account_set_email_address_negative, 1},
	{utc_account_get_package_name_positive, 1},
	{utc_account_get_package_name_negative, 1},
	{utc_account_set_package_name_positive, 1},
	{utc_account_set_package_name_negative, 1},
	{utc_account_get_access_token_positive, 1},
	{utc_account_get_access_token_negative, 1},
	{utc_account_set_access_token_positive, 1},
	{utc_account_set_access_token_negative, 1},
	{utc_account_get_user_text_positive, 1},
	{utc_account_get_user_text_negative, 1},
	{utc_account_set_user_text_positive, 1},
	{utc_account_set_user_text_negative, 1},
	{utc_account_get_user_int_negative, 1},
	{utc_account_get_user_int_negative, 1},
	{utc_account_set_user_int_negative, 1},
	{utc_account_set_user_int_negative, 1},
	{utc_account_get_auth_type_positive, 1},
	{utc_account_get_auth_type_negative, 1},
	{utc_account_set_auth_type_positive, 1},
	{utc_account_set_auth_type_negative, 1},
	{utc_account_get_secret_positive, 1},
	{utc_account_get_secret_negative, 1},
	{utc_account_set_secret_positive, 1},
	{utc_account_set_secret_negative, 1},
	{utc_account_get_sync_support_positive, 1},
	{utc_account_get_sync_support_negative, 1},
	{utc_account_set_sync_support_positive, 1},
	{utc_account_set_sync_support_negative, 1},
	{utc_account_get_source_positive, 1},
	{utc_account_get_source_negative, 1},
	{utc_account_set_source_positive, 1},
	{utc_account_set_source_negative, 1},
	{utc_account_foreach_account_from_db_positive, 1},
	{utc_account_foreach_account_from_db_negative, 1},
	{utc_account_query_account_by_account_id_positive, 1},
	{utc_account_query_account_by_account_id_negative, 1},
	{utc_account_query_account_by_user_name_positive, 1},
	{utc_account_query_account_by_user_name_negative, 1},
	{utc_account_query_account_by_package_name_positive, 1},
	{utc_account_query_account_by_package_name_negative, 1},
	{utc_account_query_account_by_capability_positive, 1},
	{utc_account_query_account_by_capability_negative, 1},
	{utc_account_query_capability_by_account_id_positive, 1},
	{utc_account_query_capability_by_account_id_negative, 1},
	{utc_account_get_total_count_from_db_positive, 1},
	{utc_account_get_total_count_from_db_negative, 1},
	{utc_account_delete_from_db_by_id_positive, 1},
	{utc_account_delete_from_db_by_id_negative, 1},
	{utc_account_delete_from_db_by_user_name_positive, 1},
	{utc_account_delete_from_db_by_user_name_negative, 1},
	{utc_account_delete_from_db_by_package_name_positive, 1},
	{utc_account_delete_from_db_by_package_name_negative, 1},
	/* TODO : Add more test case */
	{NULL, 0},
};

static void startup(void)
{

}


static void cleanup(void)
{
	/* end of TC */
}

static void _account_free_text(char *text)
{
	if (text) {
		free(text);
		text = NULL;
	}
}

// START example for libaccounts-svc API test case

static void utc_account_connect_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_connect();

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

}

static void utc_account_connect_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	int ret = ACCOUNT_ERROR_NONE;

	ret = rename("/opt/dbspace/.account.db", "/opt/dbspace/.account-tmp.db");

	ret = rename("/opt/dbspace/.account.db-journal", "/opt/dbspace/.account-tmp.db-journal");

	ret = account_connect();

	if ( ret == ACCOUNT_ERROR_DB_NOT_OPENED ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = rename("/opt/dbspace/.account-tmp.db", "/opt/dbspace/.account.db");
	ret = rename("/opt/dbspace/.account-tmp.db-journal", "/opt/dbspace/.account.db-journal");

	ret = account_disconnect();
	if ( ret != ACCOUNT_ERROR_NONE) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_destroy_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_connect();
	if ( ret != ACCOUNT_ERROR_NONE) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

}

static void utc_account_destroy_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	int ret = ACCOUNT_ERROR_NONE;

	ret = rename("/opt/dbspace/.account.db", "/opt/dbspace/.account-tmp.db");

	ret = rename("/opt/dbspace/.account.db-journal", "/opt/dbspace/.account-tmp.db-journal");

	ret = account_disconnect();

	if ( ret == ACCOUNT_ERROR_DB_NOT_OPENED ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = rename("/opt/dbspace/.account-tmp.db", "/opt/dbspace/.account.db");
	ret = rename("/opt/dbspace/.account-tmp.db-journal", "/opt/dbspace/.account.db-journal");
}

static void utc_account_create_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_connect();
	if ( ret != ACCOUNT_ERROR_NONE) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);
	if ( ret != ACCOUNT_ERROR_NONE) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();
	if ( ret != ACCOUNT_ERROR_NONE) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_create_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_connect();
	if ( ret != ACCOUNT_ERROR_NONE) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();
	if ( ret != ACCOUNT_ERROR_NONE) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_insert_to_db_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_set_user_name(account, "tarun.kr");

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_set_package_name(account, TEST_PACKAGE_NAME);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_insert_to_db_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(NULL, &account_id);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_display_name_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_set_display_name(account, "tarun.kr");

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_display_name_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
	ret = account_set_display_name(account, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_user_name_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_set_user_name(account, "tarun.kr");

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_user_name_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
	ret = account_set_user_name(account, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_icon_path_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_set_icon_path(account, "icon-path");

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_icon_path_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
	ret = account_set_icon_path(account, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_domain_name_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_set_domain_name(account, "domain-name");

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_domain_name_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
	ret = account_set_domain_name(account, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_email_address_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_set_email_address(account, "email-address");

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_email_address_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
	ret = account_set_email_address(account, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_source_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_set_source(account, "source");

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_source_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
	ret = account_set_source(account, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_package_name_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_set_package_name(account, "package_name");

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_package_name_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
	ret = account_set_package_name(account, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_access_token_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_set_access_token(account, "access_token");

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_access_token_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
	ret = account_set_access_token(account, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_auth_type_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_set_auth_type(account, ACCOUNT_AUTH_TYPE_XAUTH);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_auth_type_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
	ret = account_set_auth_type(account, -999);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_secret_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_set_secret(account, ACCOUNT_SECRECY_INVISIBLE);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_secret_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
	ret = account_set_secret(account, -999);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_sync_support_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_set_sync_support(account, ACCOUNT_SYNC_STATUS_IDLE);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_sync_support_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
	ret = account_set_sync_support(account, -999);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_user_text_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
	int i;
	for ( i = 0; i < 5; i++)
		ret = account_set_user_text(account, i, "user-text");

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_user_text_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
	int i;

	for (i = 0; i < 5; i++)
		ret = account_set_user_text(account, i, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_user_int_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
	int i;
	for (i = 0; i < 5; i++)
		ret = account_set_user_int(account, i, 999);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_user_int_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
	int i;
	for (i = 0; i < 5; i++)
		ret = account_set_user_int(NULL, i, 999);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_capability_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_set_capability(account, ACCOUNT_CAPABILITY_CALENDAR, ACCOUNT_CAPABILITY_ENABLED);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_set_capability_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_set_capability(account, -100, -5);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_display_name_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	char * display_name;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_display_name(account, &display_name);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	_account_free_text(display_name);

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_display_name_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}


	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_display_name(account, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_account_id_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_account_id(account, &account_id);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_account_id_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_account_id(account, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_user_name_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	char *user_name;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_user_name(account, &user_name);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	_account_free_text(user_name);

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_user_name_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_user_name(account, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_icon_path_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	char *icon_path;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_icon_path(account, &icon_path);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	_account_free_text(icon_path);

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_icon_path_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_icon_path(account, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_domain_name_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	char *domain_name;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_domain_name(account, &domain_name);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	_account_free_text(domain_name);

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_domain_name_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_domain_name(account, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_email_address_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	char *email_address;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_email_address(account, &email_address);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	_account_free_text(email_address);

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_email_address_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_email_address(account, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_source_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	char *source;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_source(account, &source);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	_account_free_text(source);

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_source_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_source(account, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_package_name_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	char *package_name;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_package_name(account, &package_name);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	_account_free_text(package_name);

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_package_name_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_package_name(account, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_access_token_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	char *access_token;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_access_token(account, &access_token);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	_account_free_text(access_token);

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_access_token_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_access_token(account, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_auth_type_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int auth_type;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_auth_type(account, &auth_type);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_auth_type_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_auth_type(account, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_secret_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	account_secrecy_state_e secret;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_secret(account, &secret);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_sync_support_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	account_sync_state_e sync_support;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_sync_support(account, &sync_support);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_secret_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_secret(account, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_sync_support_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_sync_support(account, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_user_text_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	char *user_text;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
	int i;
	for (i = 0; i < 5; i++) {
		ret = account_get_user_text(account, i, &user_text);
		_account_free_text(user_text);
	}

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_user_text_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
	int i;
	for (i = 0; i < 5; i++)
		ret = account_get_user_text(account, i, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_user_int_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int user_int;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
	int i;
	for (i = 0; i < 5; i++)
		ret = account_get_user_int(account, i, &user_int);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_insert_to_db(account, &account_id);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_user_int_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
	int i;
	for (i = 0; i < 5; i++)
		ret = account_get_user_int(NULL, i, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}
bool capability_call_back(account_capability_type_e capability_type, account_capability_state_e capability_state, void *user_data)
{

}

static void utc_account_get_capability_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_capability(account, capability_call_back, NULL);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_capability_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_capability(account, NULL, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

bool account_call_back(account_h account, void *user_data)
{

}

static void utc_account_foreach_account_from_db_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_foreach_account_from_db(account_call_back, NULL);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_foreach_account_from_db_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_foreach_account_from_db(NULL, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_query_account_by_account_id_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(1, &account);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_query_account_by_account_id_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_account_id(-999, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_query_account_by_user_name_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_user_name(account_call_back, "tarun.kr", NULL);

	if ( ret == ACCOUNT_ERROR_NONE || ret == ACCOUNT_ERROR_RECORD_NOT_FOUND) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_query_account_by_user_name_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_user_name(account_call_back, NULL, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_query_account_by_package_name_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_package_name(account_call_back, TEST_PACKAGE_NAME, NULL);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_query_account_by_package_name_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_package_name(account_call_back, NULL, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_query_account_by_capability_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_capability(account_call_back, ACCOUNT_CAPABILITY_PHOTO, ACCOUNT_CAPABILITY_ENABLED, NULL);

	if ( ret == ACCOUNT_ERROR_NONE || ret == ACCOUNT_ERROR_RECORD_NOT_FOUND) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_query_account_by_capability_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_account_by_capability(account_call_back, -999, -999, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_query_capability_by_account_id_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_capability_by_account_id(capability_call_back, 1, NULL);

	if ( ret == ACCOUNT_ERROR_NONE || ret == ACCOUNT_ERROR_RECORD_NOT_FOUND) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_query_capability_by_account_id_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_query_capability_by_account_id(capability_call_back, -1, NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_total_count_from_db_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int *count;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_total_count_from_db(&count);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_get_total_count_from_db_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_get_total_count_from_db(NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_update_sync_status_by_id_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_update_sync_status_by_id(1, ACCOUNT_SYNC_STATUS_IDLE);

	if ( ret == ACCOUNT_ERROR_NONE || ret == ACCOUNT_ERROR_RECORD_NOT_FOUND) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_update_sync_status_by_id_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_update_sync_status_by_id(-1, ACCOUNT_SYNC_STATUS_IDLE);

	if ( ret == ACCOUNT_ERROR_RECORD_NOT_FOUND || ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

}

static void utc_account_delete_from_db_by_id_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_delete_from_db_by_id(1);

	if ( ret == ACCOUNT_ERROR_NONE || ret == ACCOUNT_ERROR_RECORD_NOT_FOUND) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_delete_from_db_by_id_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_delete_from_db_by_id(-1);

	if ( ret == ACCOUNT_ERROR_RECORD_NOT_FOUND || ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

}

static void utc_account_delete_from_db_by_user_name_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_delete_from_db_by_user_name("tarun.kr", TEST_PACKAGE_NAME);

	if ( ret == ACCOUNT_ERROR_NONE || ret == ACCOUNT_ERROR_RECORD_NOT_FOUND) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_delete_from_db_by_user_name_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_delete_from_db_by_user_name(NULL, TEST_PACKAGE_NAME);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_delete_from_db_by_package_name_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_delete_from_db_by_package_name(TEST_PACKAGE_NAME);

	if ( ret == ACCOUNT_ERROR_NONE || ret == ACCOUNT_ERROR_RECORD_NOT_FOUND) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_delete_from_db_by_package_name_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_delete_from_db_by_package_name(NULL);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_update_to_db_by_id_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_set_user_name(account, "update-tarun.kr");

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_set_package_name(account, TEST_PACKAGE_NAME);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_update_to_db_by_id(account, 1);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_update_to_db_by_id_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_update_to_db_by_id(account, -1);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_update_to_db_by_user_name_positive(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_set_user_name(account, "update-tarun.kr");

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_set_package_name(account, TEST_PACKAGE_NAME);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_update_to_db_by_user_name(account, "tarun.kr", TEST_PACKAGE_NAME);

	if ( ret == ACCOUNT_ERROR_NONE ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

static void utc_account_update_to_db_by_user_name_negative(void)
{
	const char *API_NAME = __FUNCTION__;
	account_h account;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_connect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_create(&account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_update_to_db_by_user_name(account, NULL, TEST_PACKAGE_NAME);

	if ( ret == ACCOUNT_ERROR_INVALID_PARAMETER ) {
		dts_pass(API_NAME, "passed");
	} else {
		dts_fail(API_NAME, "failed");
	}

	ret = account_destroy(account);

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}

	ret = account_disconnect();

	if ( ret != ACCOUNT_ERROR_NONE ) {
		dts_fail(API_NAME, "failed");
	}
}

// END of example for libaccounts-svc API unit test

