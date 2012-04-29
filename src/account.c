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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <account.h>
#include <glib.h>
#include <db-util.h>
#include <pthread.h>
#include <assert.h>
#include <account-private.h>
#include <dlog.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "ACCOUNT"

#define ACCOUNT_DLOG_USE
#ifdef ACCOUNT_DLOG_USE
#define ACCOUNT_DEBUG(fmt, arg...) \
		LOGD(": " fmt "\n", ##arg);
#else
#define ACCOUNT_DEBUG(fmt, arg...) \
		printf(": " fmt "\n", ##arg);
#endif

#define ACCOUNT_RETURN_VAL(eval, expr, ret_val, X)\
	if (!(eval)) \
{\
	expr; \
	if (1)\
	{ACCOUNT_DEBUG X;}\
	return ret_val;\
} else {;}

#define ACCOUNT_SNPRINTF(dest,size,format,arg...)	\
	do { \
			snprintf(dest,size-1,format,##arg); \
	}while(0)
	/*	If the same pointer is passed to free twice, 	known as a double free. To avoid this, set pointers to
NULL after passing 	them to free: free(NULL) is safe (it does nothing).
	 */

#define ACCOUNT_MEMSET(dest,value,size)	\
	do { \
			memset(dest,value,size); \
	}while(0)

#define ACCOUNT_CATCH_ERROR(eval, expr, error_val, X) \
	if (!(eval)) \
{\
	expr; \
	error_code = (error_val);\
	if (1)\
	{ACCOUNT_DEBUG X;}\
	goto CATCH;\
} else {;}


static sqlite3* g_hAccountDB = NULL;
static int		g_refCntDB = 0;
pthread_mutex_t account_mutex = PTHREAD_MUTEX_INITIALIZER;

static int _account_gslist_free(GSList* list);
static int _account_glist_free(GList* list);

static const char *_account_db_err_msg()
{
	assert(NULL != g_hAccountDB);
	return sqlite3_errmsg(g_hAccountDB);
}

static int _account_get_record_count(char* query)
{
	int rc = -1;
	int ncount = 0;
	account_stmt pStmt = NULL;

	assert(NULL != query);
	assert(NULL != g_hAccountDB);
	rc = sqlite3_prepare_v2(g_hAccountDB, query, strlen(query), &pStmt, NULL);

	rc = sqlite3_step(pStmt);
	if (SQLITE_ROW != rc) {
		ACCOUNT_DEBUG("sqlite3_step() failed(%d, %s).", rc, _account_db_err_msg());
		sqlite3_finalize(pStmt);
		return ACCOUNT_ERROR_DB_FAILED;
	}

	ncount = sqlite3_column_int(pStmt, 0);

	ACCOUNT_DEBUG("count : %d, End", ncount);
	sqlite3_finalize(pStmt);

	return ncount;
}

static int _account_execute_query(char *query)
{
	int rc = -1;
	char* pszErrorMsg = NULL;

	assert(NULL != query);
	assert(NULL != g_hAccountDB);

	ACCOUNT_DEBUG("query : %s", query);

	rc = sqlite3_exec(g_hAccountDB, query, NULL, NULL, &pszErrorMsg);
	if (SQLITE_OK != rc) {
		ACCOUNT_DEBUG("sqlite3_exec(%s) failed(%s).", query, pszErrorMsg);
		sqlite3_free(pszErrorMsg);
	}

	return rc;
}

static int _account_begin_transaction(void)
{
	int ret = -1;

	ret = _account_execute_query("BEGIN IMMEDIATE TRANSACTION");

	if (ret != SQLITE_OK) {
		ACCOUNT_DEBUG("_account_svc_begin_transaction fail :: %d", ret);
		return ACCOUNT_ERROR_DB_FAILED;
	}

   return ACCOUNT_ERROR_NONE;
}

static int _account_end_transaction(bool is_success)
{
	int ret = -1;

	if (is_success == true) {
		ret = _account_execute_query("COMMIT TRANSACTION");
	} else {
		ret = _account_execute_query("ROLLBACK TRANSACTION");
	}

	if (ret != SQLITE_OK) {
		ACCOUNT_DEBUG("_account_svc_end_transaction fail :: %d", ret);
		return ACCOUNT_ERROR_DB_FAILED;
	}

   return ACCOUNT_ERROR_NONE;
}

static int _account_create_all_tables(void)
{
	int rc = -1;
	int error_code = ACCOUNT_ERROR_NONE;
	char	query[ACCOUNT_SQL_LEN_MAX] = {0, };


	ACCOUNT_MEMSET(query, 0, sizeof(query));

	ACCOUNT_DEBUG("_account_create_all_tables begin");

	/*Create the account table*/
	ACCOUNT_SNPRINTF(query, sizeof(query), "select count(*) from sqlite_master where name in ('%s')", ACCOUNT_TABLE);
	rc = _account_get_record_count(query);
	ACCOUNT_DEBUG("rc = %d \n", rc);

	if (rc <= 0) {
		ACCOUNT_MEMSET(query, 0, sizeof(query));
		ACCOUNT_SNPRINTF(query, sizeof(query),  ACCOUNT_SCHEMA, ACCOUNT_TABLE);
		rc = _account_execute_query(query);
		ACCOUNT_RETURN_VAL((SQLITE_OK == rc), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_execute_query(%s) failed(%d, %s).\n", query, rc, _account_db_err_msg()));
	}

	ACCOUNT_MEMSET(query, 0, sizeof(query));
	/*Create capability table*/
	ACCOUNT_SNPRINTF(query, sizeof(query), "select count(*) from sqlite_master where name in ('%s')", CAPABILITY_TABLE);
	rc = _account_get_record_count(query);
	ACCOUNT_DEBUG("rc = %d \n", rc);

	if (rc <= 0) {
		ACCOUNT_MEMSET(query, 0, sizeof(query));
		ACCOUNT_SNPRINTF(query, sizeof(query),  CAPABILITY_SCHEMA, CAPABILITY_TABLE);
		rc = _account_execute_query(query);

		ACCOUNT_RETURN_VAL((SQLITE_OK == rc), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_execute_query(%s) failed(%d, %s).\n", query, rc, _account_db_err_msg()));
	}

	return error_code;
}


static bool _account_check_is_all_table_exists()
{
	int 	rc = 0;
	char	query[ACCOUNT_SQL_LEN_MAX] = {0,};
	ACCOUNT_MEMSET(query, 0, sizeof(query));

	ACCOUNT_DEBUG("_account_check_is_all_table_exists");

	ACCOUNT_SNPRINTF(query, sizeof(query), "select count(*) from sqlite_master where name in ('%s', '%s')",
			ACCOUNT_TABLE, CAPABILITY_TABLE);
	rc = _account_get_record_count(query);
	ACCOUNT_DEBUG("rc = %d \n", rc);

	if (rc != ACCOUNT_TABLE_TOTAL_COUNT) {
		ACCOUNT_DEBUG("Table count is not matched rc=%d\n", rc);
		return FALSE;
	}

	ACCOUNT_DEBUG("END of _account_check_is_all_table_exists\n");

	return TRUE;
}

static int _account_db_open(void)
{
	int 	rc = 0;
	bool is_success = false;
	char query[ACCOUNT_SQL_LEN_MAX] = {0, };

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));

	if (!g_hAccountDB) {
		rc = db_util_open(ACCOUNT_DB_NAME, &g_hAccountDB, DB_UTIL_REGISTER_HOOK_METHOD);
		ACCOUNT_RETURN_VAL((rc == SQLITE_OK), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected. rc : %d", rc));

		g_refCntDB++;
		is_success = true;
		ACCOUNT_DEBUG("_account_db_open: The database connected. refcnt=%d ", g_refCntDB);
	} else {
		g_refCntDB++;
		is_success = true;
		ACCOUNT_DEBUG("The database already connected. refcnt=%d ", g_refCntDB);
	}

	return ACCOUNT_ERROR_NONE;
}

static int _account_db_close(void)
{
	int 	rc = 0;

	if (g_hAccountDB) {
		if (g_refCntDB > 0) {
			g_refCntDB--;
			ACCOUNT_DEBUG("_account_svc_db_close: The database disconnected. refcnt=%d ", g_refCntDB);
		}

		if (g_refCntDB == 0) {
			rc = db_util_close(g_hAccountDB);
			ACCOUNT_RETURN_VAL((rc == SQLITE_OK), {}, ACCOUNT_ERROR_DB_FAILED, ("The database isn't connected. rc : %d", rc));

			g_hAccountDB = NULL;
			ACCOUNT_DEBUG( "_account_svc_db_close: The database disconnected really. ");
		}
	} else {
		ACCOUNT_DEBUG( "_account_svc_db_close: No handle(). refcnt=%d ", g_refCntDB);
	}

	return ACCOUNT_ERROR_NONE;
}

static int _account_connect(void)
{
	int error_code = ACCOUNT_ERROR_NONE;

	pthread_mutex_lock(&account_mutex);

	ACCOUNT_DEBUG("db path = %s\n", ACCOUNT_DB_NAME);

	error_code = _account_db_open();
	if (ACCOUNT_ERROR_NONE != error_code) {
		ACCOUNT_DEBUG("The database isn't connected.\n");
		pthread_mutex_unlock(&account_mutex);
		return ACCOUNT_ERROR_DB_NOT_OPENED;
	}

	if (FALSE == _account_check_is_all_table_exists())
		error_code = _account_create_all_tables();

	pthread_mutex_unlock(&account_mutex);
	return ACCOUNT_ERROR_NONE;
}

int account_connect (void)
{
	return _account_connect();
}

static int _account_disconnect(void)
{
	int error_code = ACCOUNT_ERROR_NONE;

	pthread_mutex_lock(&account_mutex);
	ACCOUNT_DEBUG("db path = %s have been closed!!!\n", ACCOUNT_DB_NAME);

	error_code = _account_db_close();
	pthread_mutex_unlock(&account_mutex);

	return error_code;
}

int account_disconnect (void)
{
	return _account_disconnect();
}

static int _account_free_capability_items(account_capability_s *data)
{
	_ACCOUNT_FREE(data->package_name);
	_ACCOUNT_FREE(data->user_name);

	return ACCOUNT_ERROR_NONE;
}

static int _account_free_account_items(account_s *data)
{
	_ACCOUNT_FREE(data->user_name);
	_ACCOUNT_FREE(data->email_address);
	_ACCOUNT_FREE(data->display_name);
	_ACCOUNT_FREE(data->icon_path);
	_ACCOUNT_FREE(data->source);
	_ACCOUNT_FREE(data->package_name);
	_ACCOUNT_FREE(data->domain_name);

	int i;
	for(i=0;i<USER_TXT_CNT;i++)
		_ACCOUNT_FREE(data->user_data_txt[i]);

	_account_gslist_free(data->capablity_list);
	_account_glist_free(data->account_list);

	return ACCOUNT_ERROR_NONE;
}

static int _account_gslist_free(GSList* list)
{
	ACCOUNT_RETURN_VAL((list != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("GSlist is NULL"));

	GSList* iter;

	for (iter = list; iter != NULL; iter = g_slist_next(iter)) {
		account_capability_s *cap_data = (account_capability_s*)iter->data;
		_account_free_capability_items(cap_data);
	}

	g_slist_free(list);
	list = NULL;

	return ACCOUNT_ERROR_NONE;
}

static int _account_glist_free(GList* list)
{
	ACCOUNT_RETURN_VAL((list != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Glist is NULL"));

	GList* iter;

	for (iter = list; iter != NULL; iter = g_list_next(iter)) {
		account_s *account_record = (account_s*)iter->data;
		_account_free_account_items(account_record);
	}

	g_list_free(list);
	list = NULL;

	return ACCOUNT_ERROR_NONE;
}

static gboolean _account_check_duplicated(account_s *data)
{
	char query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int count = 0;

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));

	ACCOUNT_SNPRINTF(query, sizeof(query), "select count(*) from %s where user_name='%s' and domain_name='%s'"
			, ACCOUNT_TABLE, data->user_name, data->domain_name);

	count = _account_get_record_count(query);
	if (count > 0) {
		ACCOUNT_DEBUG("_account_check_duplicated : duplicated %d account(s) exist!, user_name=%s, domain_name=%s\n",
			count, data->user_name, data->domain_name );
		return TRUE;
	}

	return FALSE;
}

static int _account_get_next_sequence(char *pszName)
{
	int 			rc = 0;
	account_stmt	pStmt = NULL;
	int 			max_seq = 0;
	char 			szQuery[ACCOUNT_SQL_LEN_MAX] = {0,};

	ACCOUNT_DEBUG( "[Enter] pszName:%s\n", pszName);

	ACCOUNT_MEMSET(szQuery, 0x00, sizeof(szQuery));
	ACCOUNT_SNPRINTF(szQuery, sizeof(szQuery),  "SELECT max(seq) FROM %s where name = '%s' ", ACCOUNT_SQLITE_SEQ, pszName);
	rc = sqlite3_prepare_v2(g_hAccountDB, szQuery, strlen(szQuery), &pStmt, NULL);

	rc = sqlite3_step(pStmt);
	max_seq = sqlite3_column_int(pStmt, 0);
	max_seq++;

	ACCOUNT_DEBUG( "sqlite3_column_int, rc=%d, max_seq=%d\n", rc, max_seq);

	/*Finalize Statement*/
	rc = sqlite3_finalize(pStmt);
	pStmt = NULL;

	return max_seq;
}

static account_stmt _account_prepare_query(char *query)
{
	int 			rc = -1;
	account_stmt 	pStmt = NULL;

	ACCOUNT_RETURN_VAL((query != NULL), {}, NULL, ("query is NULL"));

	ACCOUNT_DEBUG( "prepare query : %s", query);

	rc = sqlite3_prepare_v2(g_hAccountDB, query, strlen(query), &pStmt, NULL);
	ACCOUNT_RETURN_VAL((SQLITE_OK == rc), {}, NULL, ("sqlite3_prepare_v2(%s) failed(%s).", query, _account_db_err_msg()));

	return pStmt;
}

static int _account_query_bind_int(account_stmt pStmt, int pos, int num)
{
	assert(NULL != pStmt);
	assert(pos > -1);
	return sqlite3_bind_int(pStmt, pos, num);
}

static int _account_query_bind_text(account_stmt pStmt, int pos, const char *str)
{
	assert(NULL != pStmt);

	if(str)
		return sqlite3_bind_text(pStmt, pos, (const char*)str, strlen(str), SQLITE_STATIC);
	else
		return sqlite3_bind_null(pStmt, pos);
}

static int _account_convert_account_to_sql(account_s *account, account_stmt hstmt, char *sql_value)
{
	ACCOUNT_DEBUG( "_account_convert_account_to_sql");
	int count = 1;

	/*Caution : Keep insert query orders.*/

	/* 1. user name*/
	_account_query_bind_text(hstmt, count++, (char*)account->user_name);

	/* 2. email address*/
	_account_query_bind_text(hstmt, count++, (char*)account->email_address);

	/* 3. display name*/
	_account_query_bind_text(hstmt, count++, (char*)account->display_name);

	/* 4. icon path*/
	_account_query_bind_text(hstmt, count++, (char*)account->icon_path);

	/* 5. source*/
	_account_query_bind_text(hstmt, count++, (char*)account->source);

	/* 6. package name*/
	_account_query_bind_text(hstmt, count++, (char*)account->package_name);

	/* 7. access token*/
	_account_query_bind_text(hstmt, count++, (char*)account->access_token);

	/* 8. domain name*/
	_account_query_bind_text(hstmt, count++, (char*)account->domain_name);

	/* 9. auth type*/
	_account_query_bind_int(hstmt, count++, account->auth_type);

	/* 10. secret */
	_account_query_bind_int(hstmt, count++, account->secret);

	int i;

	/* 11. user text*/
	for(i=0; i< USER_TXT_CNT; i++)
		_account_query_bind_text(hstmt, count++, (char*)account->user_data_txt[i]);

	/* 12. user integer	*/
	for(i=0; i< USER_INT_CNT; i++)
		_account_query_bind_int(hstmt, count++, account->user_data_int[i]);

	return count;
}


static void _account_query_finalize(account_stmt pStmt)
{
	int rc = -1;

	if (!pStmt) {
		ACCOUNT_DEBUG( "pStmt is NULL");
		return;
	}

	rc = sqlite3_finalize(pStmt);
	if (rc != SQLITE_OK) {
		ACCOUNT_DEBUG( "sqlite3_finalize fail, rc : %d, db_error : %s\n", rc, _account_db_err_msg());
	}

	ACCOUNT_DEBUG( "sqlite3_finalize finish");
}


static int _account_query_step(account_stmt pStmt)
{
	assert(NULL != pStmt);
	return sqlite3_step(pStmt);
}


static int _account_execute_insert_query(account_s *account)
{
	int				rc = 0;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int				error_code = ACCOUNT_ERROR_NONE;
	account_stmt 	hstmt = NULL;

	if (!account->user_name && !account->display_name && !account->email_address) {
		ACCOUNT_DEBUG("Mandetory fields is NULL. At least one field is required among username, display name, email address\n");
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "INSERT INTO %s( user_name, email_address , display_name , icon_path , source , package_name , "
			"access_token , domain_name , auth_type , secret , txt_custom0, txt_custom1, txt_custom2, txt_custom3, txt_custom4, "
			"int_custom0, int_custom1, int_custom2, int_custom3, int_custom4, txt_custom0 ) values "
			"(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ? , ?)",	ACCOUNT_TABLE);

	hstmt = _account_prepare_query(query);
	ACCOUNT_RETURN_VAL((hstmt != NULL), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_prepare_query() failed(%s).\n", _account_db_err_msg()));

	_account_convert_account_to_sql(account, hstmt, query);

	rc = _account_query_step(hstmt);
	if (rc != SQLITE_DONE) {
		ACCOUNT_DEBUG( "account_db_query_step() failed(%d, %s)", rc, _account_db_err_msg());
		error_code = ACCOUNT_ERROR_DB_FAILED;
	}

	_account_query_finalize(hstmt);
	hstmt = NULL;

	return error_code;
}

static int _account_insert_capability(account_s *account, int account_id)
{
	int 			rc, count = 1;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	account_stmt 	hstmt = NULL;

	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));

	if (g_slist_length( account->capablity_list)==0) {
		ACCOUNT_DEBUG( "_account_insert_capability, no capability\n");
		return ACCOUNT_ERROR_NONE;
	}

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) from %s where _id=%d", ACCOUNT_TABLE, account_id);

	rc = _account_get_record_count(query);

	if (rc <= 0) {
		ACCOUNT_DEBUG( "_account_insert_capability : related account item is not existed rc=%d , %s", rc, _account_db_err_msg());
		return ACCOUNT_ERROR_RECORD_NOT_FOUND;
	}

	/* insert query*/

	GSList *iter;

	for (iter = account->capablity_list; iter != NULL; iter = g_slist_next(iter)) {
		int rc, ret;
		count = 1;
		ACCOUNT_MEMSET(query, 0x00, sizeof(query));
		ACCOUNT_SNPRINTF(query, sizeof(query), "INSERT INTO %s(key, value, package_name, user_name, account_id) VALUES "
				"(?, ?, ?, ?, ?) ", CAPABILITY_TABLE);

		hstmt = _account_prepare_query(query);

		ACCOUNT_RETURN_VAL((hstmt != NULL), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_prepare_query() failed(%s).\n", _account_db_err_msg()));

		account_capability_s* cap_data = NULL;
		cap_data = (account_capability_s*)iter->data;
		ACCOUNT_DEBUG( "@@@@@@@@@@@@@cap_data->type = %d, cap_data->value = %d \n @@@@@@@@@@", cap_data->type, cap_data->value);

		ret = _account_query_bind_int(hstmt, count++, cap_data->type);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_int(hstmt, count++, cap_data->value);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_text(hstmt, count++, (char*)account->package_name);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_text(hstmt, count++, (char*)account->user_name);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_int(hstmt, count++, (int)account_id);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Integer binding fail"));

		rc = _account_query_step(hstmt);

		if (rc != SQLITE_DONE) {
			ACCOUNT_DEBUG( "_account_query_step() failed(%d, %s)", rc, _account_db_err_msg());
			break;
		}

		_account_query_finalize(hstmt);
		hstmt = NULL;

	}

	ACCOUNT_DEBUG( "_account_insert_capability() DONE\n");

	return ACCOUNT_ERROR_NONE;
}


static int _account_update_capability(account_s *account, int account_id)
{
	int 			rc, count = 1;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	account_stmt 	hstmt = NULL;

	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));

	if (g_slist_length( account->capablity_list)==0) {
		ACCOUNT_DEBUG( "_account_insert_capability, no capability\n");
		return ACCOUNT_ERROR_NONE;
	}

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) from %s where _id=%d", ACCOUNT_TABLE, account_id);

	rc = _account_get_record_count(query);

	if (rc <= 0) {
		ACCOUNT_DEBUG( "_account_insert_capability : related account item is not existed rc=%d , %s", rc, _account_db_err_msg());
		return ACCOUNT_ERROR_RECORD_NOT_FOUND;
	}

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));

	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE account_id=? ", CAPABILITY_TABLE);
	hstmt = _account_prepare_query(query);
	count = 1;
	_account_query_bind_int(hstmt, count++, (int)account_id);
	_account_query_step(hstmt);
	_account_query_finalize(hstmt);
	hstmt = NULL;

	GSList *iter;

	for (iter = account->capablity_list; iter != NULL; iter = g_slist_next(iter)) {
		int rc, ret;
		count = 1;
		ACCOUNT_MEMSET(query, 0x00, sizeof(query));
		ACCOUNT_SNPRINTF(query, sizeof(query), "INSERT INTO %s(key, value, package_name, user_name, account_id) VALUES "
				"(?, ?, ?, ?, ?) ", CAPABILITY_TABLE);

		hstmt = _account_prepare_query(query);

		ACCOUNT_RETURN_VAL((hstmt != NULL), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_prepare_query() failed(%s).\n", _account_db_err_msg()));

		account_capability_s* cap_data = NULL;
		cap_data = (account_capability_s*)iter->data;

		ret = _account_query_bind_int(hstmt, count++, cap_data->type);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_int(hstmt, count++, cap_data->value);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_text(hstmt, count++, (char*)account->package_name);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_text(hstmt, count++, (char*)account->user_name);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_int(hstmt, count++, (int)account_id);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Integer binding fail"));

		rc = _account_query_step(hstmt);

		if (rc != SQLITE_DONE) {
			ACCOUNT_DEBUG( "_account_query_step() failed(%d, %s)", rc, _account_db_err_msg());
			break;
		}

		_account_query_finalize(hstmt);
		hstmt = NULL;

	}

	ACCOUNT_DEBUG( "_account_insert_capability() DONE\n");

	return ACCOUNT_ERROR_NONE;
}

static int _account_update_capability_by_user_name(account_s *account, char *user_name, char *package_name )
{
	int 			rc, count = 1;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	account_stmt 	hstmt = NULL;

	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));

	if (g_slist_length( account->capablity_list)==0) {
		ACCOUNT_DEBUG( "_account_insert_capability, no capability\n");
		return ACCOUNT_ERROR_NONE;
	}

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) from %s where package_name=%s and user_name=%s", ACCOUNT_TABLE, package_name, user_name);

	rc = _account_get_record_count(query);

	if (rc <= 0) {
		ACCOUNT_DEBUG( "_account_insert_capability : related account item is not existed rc=%d , %s ", rc, _account_db_err_msg());
		return ACCOUNT_ERROR_RECORD_NOT_FOUND;
	}

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));

	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE package_name=? and user_name=? ", CAPABILITY_TABLE);
	hstmt = _account_prepare_query(query);
	count = 1;
	_account_query_bind_text(hstmt, count++, (char*)account->package_name);
	_account_query_bind_text(hstmt, count++, (char*)account->user_name);
	_account_query_step(hstmt);
	_account_query_finalize(hstmt);
	hstmt = NULL;

	GSList* iter;

	for (iter = account->capablity_list; iter != NULL; iter = g_slist_next(iter)) {
		int rc, ret;
		count = 1;
		ACCOUNT_MEMSET(query, 0x00, sizeof(query));
		ACCOUNT_SNPRINTF(query, sizeof(query), "INSERT INTO %s(key, value, package_name, user_name, account_id) VALUES "
				"(?, ?, ?, ?, ?) ", CAPABILITY_TABLE);

		hstmt = _account_prepare_query(query);

		ACCOUNT_RETURN_VAL((hstmt != NULL), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_prepare_query() failed(%s).\n", _account_db_err_msg()));

		account_capability_s* cap_data = NULL;
		cap_data = (account_capability_s*)iter->data;

		ret = _account_query_bind_int(hstmt, count++, cap_data->type);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_int(hstmt, count++, cap_data->value);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_text(hstmt, count++, (char*)account->package_name);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_text(hstmt, count++, (char*)account->user_name);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		/*ret = _account_query_bind_int(hstmt, count++, (int)account_id);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Integer binding fail"));*/

		rc = _account_query_step(hstmt);

		if (rc != SQLITE_DONE) {
			ACCOUNT_DEBUG( "_account_query_step() failed(%d, %s)", rc, _account_db_err_msg());
			break;
		}

		_account_query_finalize(hstmt);
		hstmt = NULL;

	}

	ACCOUNT_DEBUG( "_account_insert_capability() DONE\n");

	return ACCOUNT_ERROR_NONE;
}

static int _account_query_table_column_int(account_stmt pStmt, int pos)
{
	assert(NULL != pStmt);
	assert(pos > -1);
	return sqlite3_column_int(pStmt, pos);
}

static char *_account_query_table_column_text(account_stmt pStmt, int pos)
{
	assert(NULL != pStmt);
	assert(pos > -1);
	return (char *)sqlite3_column_text(pStmt, pos);
}

static void _account_db_data_to_text(char *textbuf, char **output)
{
	if (textbuf && strlen(textbuf)>0) {
		if (*output) {
			free(*output);
			*output = NULL;
		}
		*output = strdup(textbuf);
	} else {
		ACCOUNT_DEBUG("_account_db_data_to_text : no text");
	}
}

static void _account_convert_column_to_account(account_stmt hstmt, account_s *account_record)
{
	char   	*textbuf = NULL;

	account_record->id = _account_query_table_column_int(hstmt, ACCOUNT_FIELD_ID);

	textbuf = _account_query_table_column_text(hstmt, ACCOUNT_FIELD_USER_NAME);
	_account_db_data_to_text(textbuf, &(account_record->user_name));

	textbuf = _account_query_table_column_text(hstmt, ACCOUNT_FIELD_EMAIL_ADDRESS);
	_account_db_data_to_text(textbuf, &(account_record->email_address));

	textbuf = _account_query_table_column_text(hstmt, ACCOUNT_FIELD_DISPLAY_NAME);
	_account_db_data_to_text(textbuf, &(account_record->display_name));

	textbuf = _account_query_table_column_text(hstmt, ACCOUNT_FIELD_ICON_PATH);
	_account_db_data_to_text(textbuf, &(account_record->icon_path));

	textbuf = _account_query_table_column_text(hstmt, ACCOUNT_FIELD_SOURCE);
	_account_db_data_to_text(textbuf, &(account_record->source));

	textbuf = _account_query_table_column_text(hstmt, ACCOUNT_FIELD_PACKAGE_NAME);
	_account_db_data_to_text(textbuf, &(account_record->package_name));

	textbuf = _account_query_table_column_text(hstmt, ACCOUNT_FIELD_ACCESS_TOKEN);
	_account_db_data_to_text(textbuf, &(account_record->access_token));

	textbuf = _account_query_table_column_text(hstmt, ACCOUNT_FIELD_DOMAIN_NAME);
	_account_db_data_to_text(textbuf, &(account_record->domain_name));

	account_record->auth_type = _account_query_table_column_int(hstmt, ACCOUNT_FIELD_AUTH_TYPE);

	account_record->secret = _account_query_table_column_int(hstmt, ACCOUNT_FIELD_SECRET);

	textbuf = _account_query_table_column_text(hstmt, ACCOUNT_FIELD_USER_TEXT_0);
	_account_db_data_to_text(textbuf, &(account_record->user_data_txt[0]));

	textbuf = _account_query_table_column_text(hstmt, ACCOUNT_FIELD_USER_TEXT_1);
	_account_db_data_to_text(textbuf, &(account_record->user_data_txt[1]));

	textbuf = _account_query_table_column_text(hstmt, ACCOUNT_FIELD_USER_TEXT_2);
	_account_db_data_to_text(textbuf, &(account_record->user_data_txt[2]));

	textbuf = _account_query_table_column_text(hstmt, ACCOUNT_FIELD_USER_TEXT_3);
	_account_db_data_to_text(textbuf, &(account_record->user_data_txt[3]));

	textbuf = _account_query_table_column_text(hstmt, ACCOUNT_FIELD_USER_TEXT_4);
	_account_db_data_to_text(textbuf, &(account_record->user_data_txt[4]));

	account_record->user_data_int[0] = _account_query_table_column_int(hstmt, ACCOUNT_FIELD_USER_INT_0);
	account_record->user_data_int[1] = _account_query_table_column_int(hstmt, ACCOUNT_FIELD_USER_INT_1);
	account_record->user_data_int[2] = _account_query_table_column_int(hstmt, ACCOUNT_FIELD_USER_INT_2);
	account_record->user_data_int[3] = _account_query_table_column_int(hstmt, ACCOUNT_FIELD_USER_INT_3);
	account_record->user_data_int[4] = _account_query_table_column_int(hstmt, ACCOUNT_FIELD_USER_INT_4);

	ACCOUNT_DEBUG("END _account_convert_column_to_account");
}


static void _account_convert_column_to_capability(account_stmt hstmt, account_capability_s *capability_record)
{
	char *textbuf = NULL;

	capability_record->id = _account_query_table_column_int(hstmt, ACCOUNT_FIELD_ID);

	capability_record->type = _account_query_table_column_int(hstmt, CAPABILITY_FIELD_KEY);

	capability_record->value = _account_query_table_column_int(hstmt, CAPABILITY_FIELD_VALUE);

	textbuf = _account_query_table_column_text(hstmt, CAPABILITY_FIELD_PACKAGE_NAME);
	_account_db_data_to_text(textbuf, &(capability_record->package_name));

	textbuf = _account_query_table_column_text(hstmt, CAPABILITY_FIELD_USER_NAME);
	_account_db_data_to_text(textbuf, &(capability_record->user_name));

	capability_record->account_id = _account_query_table_column_int(hstmt, CAPABILITY_FIELD_ACCOUNT_ID);

	ACCOUNT_DEBUG("END _account_convert_column_to_capability");
}

bool _account_get_capability_text_cb(account_capability_type_e capability_type, account_capability_state_e capability_value, void *user_data)
{
	account_s *data = (account_s*)user_data;

	account_capability_s *cap_data = (account_capability_s*)malloc(sizeof(account_capability_s));

	if (cap_data == NULL)
		return FALSE;
	ACCOUNT_MEMSET(cap_data, 0, sizeof(account_capability_s));

	cap_data->type = capability_type;
	cap_data->value = capability_value;

	data->capablity_list = g_slist_append(data->capablity_list, (gpointer)cap_data);

	ACCOUNT_DEBUG("_account_get_capability_text_cb :: %d\n", capability_type);

	return TRUE;
}

static char *_account_get_text(char *text_data)
{
	char *text_value = NULL;

	if (text_data != NULL) {
		text_value = strdup(text_data);
		ACCOUNT_DEBUG("text_value = %s", text_value);
	}
	return text_value;
}

static int _account_update_account_by_user_name(account_s *account, char *user_name, char *package_name)
{
	int				rc = 0, binding_count =0;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int				error_code = ACCOUNT_ERROR_NONE;
	account_stmt 	hstmt = NULL;

	ACCOUNT_RETURN_VAL((account->user_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("user_name is NULL.\n"));
	ACCOUNT_RETURN_VAL((account->email_address != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("email_address is NULL.\n"));

	if (!account->user_name && !account->display_name && !account->email_address) {
		ACCOUNT_DEBUG("One field should be set among user name, display name, email address\n");
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "UPDATE %s SET user_name=?, email_address =?, display_name =?, "
			"icon_path =?, source =?, package_name =? , access_token =?, domain_name =?, auth_type =?, secret =?,"
			"txt_custom0=?, txt_custom1=?, txt_custom2=?, txt_custom3=?, txt_custom4=?, "
			"int_custom0=?, int_custom1=?, int_custom2=?, int_custom3=?, int_custom4=? WHERE user_name=? and package_name=? ", ACCOUNT_TABLE);

	hstmt = _account_prepare_query(query);
	ACCOUNT_RETURN_VAL((hstmt != NULL), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_svc_query_prepare() failed(%s).\n", _account_db_err_msg()));

	binding_count = _account_convert_account_to_sql(account, hstmt, query);

	_account_query_bind_text(hstmt, binding_count++, user_name);
	_account_query_bind_text(hstmt, binding_count++, package_name);
	ACCOUNT_DEBUG("_account_query_step : user_name = %s, package_name=%s\n", user_name, package_name);
	rc = _account_query_step(hstmt);
	ACCOUNT_DEBUG("_account_query_step return rc=%d\n", rc);
	if (rc != SQLITE_DONE) {
		ACCOUNT_DEBUG( "account_db_query_step() failed(%d, %s)", rc, _account_db_err_msg());
	}

	_account_query_finalize(hstmt);
	hstmt = NULL;

	/*update capability*/
	_account_update_capability_by_user_name(account, user_name, package_name);

	return error_code;
}

int account_insert_to_db(account_h account, int *account_id)
{
	int ret = ACCOUNT_ERROR_NONE;
	int		error_code = ACCOUNT_ERROR_NONE;

	if(!g_hAccountDB) {
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected. rc : %d", ret));
	}

	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));

	account_s *data = (account_s*)account;

	pthread_mutex_lock(&account_mutex);

	if (_account_check_duplicated(data)) {
		error_code = ACCOUNT_ERROR_DUPLICATED;
	} else {
		*account_id = _account_get_next_sequence(ACCOUNT_TABLE);

		error_code = _account_execute_insert_query(data);

		if (error_code != ACCOUNT_ERROR_NONE)
			*account_id = -1;
	}

	ACCOUNT_DEBUG( "_account_execute_insert_query, insert error_code : %d", error_code);

	_account_insert_capability(data, *account_id);

	pthread_mutex_unlock(&account_mutex);

	return ACCOUNT_ERROR_NONE;

}

int account_create(account_h *account)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)malloc(sizeof(account_s));

	if (data == NULL)
		return ACCOUNT_ERROR_OUT_OF_MEMORY;
	ACCOUNT_MEMSET(data, 0, sizeof(account_s));

	ACCOUNT_DEBUG("create handle=%p\n", *account);

	*account = (account_h)data;

	return ACCOUNT_ERROR_NONE;
}

int account_set_user_name(account_h account, const char *user_name)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!user_name) {
		ACCOUNT_DEBUG("(%s)-(%d) user_name is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	ACCOUNT_DEBUG("account=%p\n", account);

	_ACCOUNT_FREE(data->user_name);
	data->user_name = strdup(user_name);

	ACCOUNT_DEBUG("(%s)-(%d) %s, %s\n", __FUNCTION__, __LINE__ , data->user_name, user_name);

	return ACCOUNT_ERROR_NONE;
}


int account_set_display_name(account_h account, const char *display_name)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!display_name) {
		ACCOUNT_DEBUG("(%s)-(%d) display_name is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->display_name);
	data->display_name = strdup(display_name);

	ACCOUNT_DEBUG("(%s)-(%d) %s, %s\n", __FUNCTION__, __LINE__ , data->display_name, display_name);

	return ACCOUNT_ERROR_NONE;
}

int account_set_email_address(account_h account, const char *email_address)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!email_address) {
		ACCOUNT_DEBUG("(%s)-(%d) email_address is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->email_address);
	data->email_address = strdup(email_address);

	ACCOUNT_DEBUG("(%s)-(%d) %s, %s\n", __FUNCTION__, __LINE__ , data->email_address, email_address);

	return ACCOUNT_ERROR_NONE;
}

int account_set_icon_path(account_h account, const char *icon_path)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!icon_path) {
		ACCOUNT_DEBUG("(%s)-(%d) icon_path is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->icon_path);
	data->icon_path = strdup(icon_path);

	ACCOUNT_DEBUG("(%s)-(%d) %s, %s\n", __FUNCTION__, __LINE__ , data->icon_path, icon_path);

	return ACCOUNT_ERROR_NONE;
}


int account_set_source(account_h account, const char *source)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!source) {
		ACCOUNT_DEBUG("(%s)-(%d) source is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->source);
	data->source = strdup(source);

	ACCOUNT_DEBUG("(%s)-(%d) %s, %s\n", __FUNCTION__, __LINE__ , data->source, source);

	return ACCOUNT_ERROR_NONE;
}

int account_set_package_name(account_h account, const char *package_name)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!package_name) {
		ACCOUNT_DEBUG("(%s)-(%d) package_name is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->package_name);
	data->package_name = strdup(package_name);

	ACCOUNT_DEBUG("(%s)-(%d) %s, %s\n", __FUNCTION__, __LINE__ , data->package_name, package_name);

	return ACCOUNT_ERROR_NONE;
}


int account_set_domain_name(account_h account, const char *domain_name)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!domain_name) {
		ACCOUNT_DEBUG("(%s)-(%d) domain_name is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->domain_name);
	data->domain_name = strdup(domain_name);

	ACCOUNT_DEBUG("(%s)-(%d) %s, %s\n", __FUNCTION__, __LINE__ , data->domain_name, domain_name);

	return ACCOUNT_ERROR_NONE;
}


int account_set_access_token(account_h account, const char *access_token)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!access_token) {
		ACCOUNT_DEBUG("(%s)-(%d) access_token is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->access_token);
	data->access_token = strdup(access_token);

	ACCOUNT_DEBUG("(%s)-(%d) %s, %s\n", __FUNCTION__, __LINE__ , data->access_token, access_token);

	return ACCOUNT_ERROR_NONE;
}

int account_set_user_text(account_h account, int index, const char *user_txt)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!user_txt) {
		ACCOUNT_DEBUG("(%s)-(%d) user_txt is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	if (index >= USER_TXT_CNT) {
		ACCOUNT_DEBUG("(%s)-(%d) index rage should be between 0-4.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->user_data_txt[index]);
	data->user_data_txt[index] = strdup(user_txt);

	ACCOUNT_DEBUG("(%s)-(%d) %s, %s\n", __FUNCTION__, __LINE__ , data->user_data_txt[index], user_txt);

	return ACCOUNT_ERROR_NONE;
}

int account_set_auth_type(account_h account, const account_auth_type_e auth_type)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("(%s)-(%d) account handle is NULL.\n",  __FUNCTION__, __LINE__));

	if ( (auth_type < 0) || (auth_type > ACCOUNT_AUTH_TYPE_CLIENT_LOGIN)) {
		ACCOUNT_DEBUG("(%s)-(%d) auth_type is less than 1 or more than enum max.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	data->auth_type = (int)auth_type;

	return ACCOUNT_ERROR_NONE;
}

int account_set_secret(account_h account, const account_secrecy_state_e secret)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("(%s)-(%d) account handle is NULL.\n",	__FUNCTION__, __LINE__));

	if ( (secret < 0) || (secret > ACCOUNT_SECRECY_VISIBLE)) {
		ACCOUNT_DEBUG("(%s)-(%d) auth_type is less than 1 or more than enum max.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	data->secret = (int)secret;

	return ACCOUNT_ERROR_NONE;
}

int account_set_user_int(account_h account, int index, const int user_int)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (index >= USER_INT_CNT) {
		ACCOUNT_DEBUG("(%s)-(%d) index rage should be between 0-4.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	data->user_data_int[index] = user_int;

	return ACCOUNT_ERROR_NONE;
}


int account_set_capability(account_h account, account_capability_type_e capability_type, account_capability_state_e capability_value)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if ( (capability_type < 0) || (capability_type > ACCOUNT_CAPABILITY_MOBILE_TRACKER)) {
		ACCOUNT_DEBUG("(%s)-(%d) capability_type is less than 1 or more than enum max.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if ((capability_value != ACCOUNT_CAPABILITY_DISABLED) && (capability_value != ACCOUNT_CAPABILITY_ENABLED)) {
		ACCOUNT_DEBUG("(%s)-(%d) capability_value is not equal to 0 or 1.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	account_capability_s* cap_data = (account_capability_s*)malloc(sizeof(account_capability_s));

	if (cap_data == NULL)
		return ACCOUNT_ERROR_OUT_OF_MEMORY;
	ACCOUNT_MEMSET(cap_data, 0, sizeof(account_capability_s));

	cap_data->type = capability_type;
	cap_data->value = capability_value;
	data->capablity_list = g_slist_append(data->capablity_list, (gpointer)cap_data);

	return ACCOUNT_ERROR_NONE;
}

int account_get_user_name(account_h account, char **user_name)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!user_name) {
		ACCOUNT_DEBUG("(%s)-(%d) user name is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*user_name) = NULL;
	*user_name = _account_get_text(data->user_name);

	return ACCOUNT_ERROR_NONE;
}

int account_get_display_name(account_h account, char **display_name)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!display_name) {
		ACCOUNT_DEBUG("(%s)-(%d) display name is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*display_name) = NULL;

	*display_name = _account_get_text(data->display_name);

	return ACCOUNT_ERROR_NONE;
}

int account_get_email_address(account_h account,char **email_address)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!email_address) {
		ACCOUNT_DEBUG("(%s)-(%d) email address is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*email_address) = NULL;

	*email_address = _account_get_text(data->email_address);

	return ACCOUNT_ERROR_NONE;
}

int  account_get_icon_path(account_h account, char **icon_path)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!icon_path) {
		ACCOUNT_DEBUG("(%s)-(%d) icon path is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*icon_path) = NULL;

	*icon_path = _account_get_text(data->icon_path);

	return ACCOUNT_ERROR_NONE;
}

int account_get_source(account_h account, char **source)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!source) {
		ACCOUNT_DEBUG("(%s)-(%d) source is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*source) = NULL;

	*source = _account_get_text(data->source);

	return ACCOUNT_ERROR_NONE;
}

int account_get_package_name(account_h account, char **package_name)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!package_name) {
		ACCOUNT_DEBUG("(%s)-(%d) package name is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*package_name) = NULL;

	*package_name = _account_get_text(data->package_name);

	return ACCOUNT_ERROR_NONE;
}

int account_get_domain_name(account_h account, char **domain_name)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!domain_name) {
		ACCOUNT_DEBUG("(%s)-(%d) domain name is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*domain_name) = NULL;

	*domain_name = _account_get_text(data->domain_name);

	return ACCOUNT_ERROR_NONE;
}

int account_get_access_token(account_h account, char **access_token)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!access_token) {
		ACCOUNT_DEBUG("(%s)-(%d) access token is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*access_token) = NULL;

	*access_token = _account_get_text(data->access_token);

	return ACCOUNT_ERROR_NONE;
}

int account_get_user_text(account_h account, int user_text_index, char **text)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!text) {
		ACCOUNT_DEBUG("(%s)-(%d) text is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	ACCOUNT_RETURN_VAL((user_text_index >=0 && user_text_index < USER_TXT_CNT ), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("INVALID USER TEXT INDEX"));

	account_s *data = (account_s*)account;

	(*text) = NULL;

	*text = _account_get_text(data->user_data_txt[user_text_index]);

	return ACCOUNT_ERROR_NONE;
}

int account_get_auth_type(account_h account, account_auth_type_e *auth_type)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	if (!auth_type) {
		ACCOUNT_DEBUG("(%s)-(%d) auth_type is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s* data = (account_s*)account;

	*auth_type = data->auth_type;

	return ACCOUNT_ERROR_NONE;
}

int account_get_secret(account_h account, account_secrecy_state_e *secret)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	if (!secret) {
		ACCOUNT_DEBUG("(%s)-(%d) secret is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s* data = (account_s*)account;

	*secret = data->secret;

	return ACCOUNT_ERROR_NONE;
}

int account_get_account_id(account_h account, int *account_id)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	if (!account_id) {
		ACCOUNT_DEBUG("(%s)-(%d) account_id is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	*account_id = data->id;

	return ACCOUNT_ERROR_NONE;
}

int account_get_user_int(account_h account, int user_int_index, int *integer)
{
	if (!account) {
		ACCOUNT_DEBUG("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	ACCOUNT_RETURN_VAL((user_int_index >=0 && user_int_index < USER_TXT_CNT ), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("INVALID USER TEXT INDEX"));

	if (!integer) {
		ACCOUNT_DEBUG("(%s)-(%d) integer is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	*integer = data->user_data_int[user_int_index];

	return ACCOUNT_ERROR_NONE;
}

int account_get_capability(account_h account, capability_cb cb_func, void *user_data)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((cb_func != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("NO CALLBACK FUNCTION"));

	GSList *iter;
	account_s *data = (account_s*)account;

	for (iter = data->capablity_list; iter != NULL; iter = g_slist_next(iter)) {
		account_capability_s *cap_data = NULL;

		cap_data = (account_capability_s*)iter->data;

		ACCOUNT_DEBUG("account_get_capability :: type = %d, value = %d", cap_data->type, cap_data->value);

		cb_func(cap_data->type, cap_data->value, user_data);
	}

	return ACCOUNT_ERROR_NONE;
}

int account_query_capability_by_account_id(capability_cb cb_func, int account_id, void *user_data )
{
	int 			error_code = ACCOUNT_ERROR_NONE;
	account_stmt	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0;

	ACCOUNT_RETURN_VAL((account_id > 0), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT INDEX IS LESS THAN 0"));

	int ret = ACCOUNT_ERROR_NONE;

	if (!g_hAccountDB) {
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected. rc : %d", ret));
	}

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT * FROM %s WHERE account_id = %d", CAPABILITY_TABLE, account_id);
	hstmt = _account_prepare_query(query);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_ROW, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	account_capability_s* capability_record = NULL;

	while (rc == SQLITE_ROW) {
		capability_record = (account_capability_s*) malloc(sizeof(account_capability_s));

		if (capability_record == NULL) {
			ACCOUNT_DEBUG("malloc Failed");
			break;
		}

		ACCOUNT_MEMSET(capability_record, 0x00, sizeof(account_capability_s));

		_account_convert_column_to_capability(hstmt, capability_record);

		cb_func(capability_record->type, capability_record->value, user_data);

		_account_free_capability_items(capability_record);
		_ACCOUNT_FREE(capability_record);

		rc = _account_query_step(hstmt);
	}

	_account_query_finalize(hstmt);
	hstmt = NULL;

	error_code = ACCOUNT_ERROR_NONE;

CATCH:
	if (hstmt != NULL) {
		_account_query_finalize(hstmt);
		hstmt = NULL;
	}

	pthread_mutex_unlock(&account_mutex);
	return error_code;
}

static int _account_update_account(account_s *account, int account_id)
{
	int				rc = 0, binding_count =0;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int				error_code = ACCOUNT_ERROR_NONE;
	account_stmt 	hstmt = NULL;

	ACCOUNT_RETURN_VAL((account->user_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("user_name is NULL.\n"));
	ACCOUNT_RETURN_VAL((account->email_address != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("email_address is NULL.\n"));

	if (!account->user_name && !account->display_name && !account->email_address) {
		ACCOUNT_DEBUG("One field should be set among user name, display name, email address\n");
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "UPDATE %s SET user_name=?, email_address =?, display_name =?, "
			"icon_path =?, source =?, package_name =? , access_token =?, domain_name =?, auth_type =?, secret =?,"
			"txt_custom0=?, txt_custom1=?, txt_custom2=?, txt_custom3=?, txt_custom4=?, "
			"int_custom0=?, int_custom1=?, int_custom2=?, int_custom3=?, int_custom4=? WHERE _id=? ", ACCOUNT_TABLE);

	hstmt = _account_prepare_query(query);
	ACCOUNT_RETURN_VAL((hstmt != NULL), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_svc_query_prepare() failed(%s).\n", _account_db_err_msg()));

	binding_count = _account_convert_account_to_sql(account, hstmt, query);
	_account_query_bind_int(hstmt, binding_count++, account_id);

	rc = _account_query_step(hstmt);
	ACCOUNT_DEBUG("_account_query_step return rc=%d\n", rc);
	if (rc != SQLITE_DONE) {
		ACCOUNT_DEBUG( "account_db_query_step() failed(%d, %s)", rc, _account_db_err_msg());
	}

	_account_query_finalize(hstmt);
	hstmt = NULL;

	/*update capability*/
	_account_update_capability(account, account_id);

	return error_code;
}

int account_update_to_db_by_id(const account_h account, int account_id)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("DATA IS NULL"));
	ACCOUNT_RETURN_VAL((account_id > 0), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Account id is not valid"));
	int	error_code = ACCOUNT_ERROR_NONE;
	account_s* data = (account_s*)account;

	int ret = ACCOUNT_ERROR_NONE;

	if (!g_hAccountDB) {
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected. rc : %d", ret));
	}

	pthread_mutex_lock(&account_mutex);

	error_code = _account_update_account(data, account_id);

	pthread_mutex_unlock(&account_mutex);

	return ACCOUNT_ERROR_NONE;
}

int account_update_to_db_by_user_name(account_h account, const char *user_name, const char *package_name)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("DATA IS NULL"));
	ACCOUNT_RETURN_VAL((user_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("USER NAME IS NULL"));
	ACCOUNT_RETURN_VAL((package_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("PACKAGE NAME IS NULL"));

	int	error_code = ACCOUNT_ERROR_NONE;
	account_s *data = (account_s*)account;

	int ret = ACCOUNT_ERROR_NONE;

	if (!g_hAccountDB) {
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected. rc : %d", ret));
	}
	pthread_mutex_lock(&account_mutex);

	error_code = _account_update_account_by_user_name(data, (char*)user_name, (char*)package_name);

	pthread_mutex_unlock(&account_mutex);

	return ACCOUNT_ERROR_NONE;
}

int account_foreach_account_from_db(account_cb callback, void *user_data)
{
	int 			error_code = ACCOUNT_ERROR_NONE;
	account_stmt	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0;
	GList			*account_list = NULL;

	ACCOUNT_RETURN_VAL((callback != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT INFO IS NULL"));

	int ret = ACCOUNT_ERROR_NONE;

	if (!g_hAccountDB) {
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected. rc : %d", ret));
	}

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT * FROM %s ", ACCOUNT_TABLE);
	hstmt = _account_prepare_query(query);

	rc = _account_query_step(hstmt);

	account_s *account_record = NULL;

	ACCOUNT_CATCH_ERROR(rc == SQLITE_ROW, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	int k=0;
	while(rc == SQLITE_ROW) {
		account_record = (account_s*) malloc(sizeof(account_s));

		if (account_record == NULL) {
			ACCOUNT_DEBUG("malloc Failed");
			break;
		}
		ACCOUNT_MEMSET(account_record, 0x00, sizeof(account_s));

		_account_convert_column_to_account(hstmt, account_record);

		account_list = g_list_append(account_list, account_record);

		ACCOUNT_DEBUG("##### URUSA account_record(%d) = %p\n", k, account_record);

		rc = _account_query_step(hstmt);
		k++;
	}

	_account_query_finalize(hstmt);
	hstmt = NULL;

	GList* iter;
	k = 0;
	for (iter = account_list; iter != NULL; iter = g_list_next(iter)) {
		account_s *account = NULL;
		account = (account_s*)iter->data;
		account_query_capability_by_account_id(_account_get_capability_text_cb, account->id, (void*)account);
		callback((account_h)account, user_data);
		k++;
	}

	error_code = ACCOUNT_ERROR_NONE;

CATCH:
	if (hstmt != NULL) {
		_account_query_finalize(hstmt);
		hstmt = NULL;
	}
	if (account_list) {
		_account_glist_free(account_list);
		account_list = NULL;
	}

	return error_code;
}


int account_query_account_by_account_id(int account_db_id, account_h *account)
{
	int				error_code = ACCOUNT_ERROR_NONE;
	account_stmt 	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int				rc = 0;

	ACCOUNT_RETURN_VAL((account_db_id > 0), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT INDEX IS LESS THAN 0"));

	int ret = ACCOUNT_ERROR_NONE;

	if (!g_hAccountDB) {
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected. rc : %d", ret));
	}

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT * FROM %s WHERE _id = %d", ACCOUNT_TABLE, account_db_id);
	hstmt = _account_prepare_query(query);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_ROW, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	account_s *account_record = (account_s *)(*account);

	while (rc == SQLITE_ROW) {
		_account_convert_column_to_account(hstmt, account_record);
		ACCOUNT_DEBUG("get account info by id %p\n", account_record);
		rc = _account_query_step(hstmt);
	}

	_account_query_finalize(hstmt);
	account_query_capability_by_account_id(_account_get_capability_text_cb, account_record->id, (void*)account_record);

	hstmt = NULL;
	error_code = ACCOUNT_ERROR_NONE;

CATCH:
	if (hstmt != NULL) {
		_account_query_finalize(hstmt);
		hstmt = NULL;
	}

	pthread_mutex_unlock(&account_mutex);
	return error_code;
}

int account_query_account_by_user_name(account_cb cb_func, const char *user_name, void *user_data)
{
	int				error_code = ACCOUNT_ERROR_NONE;
	account_stmt 	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int				rc = 0;

	ACCOUNT_RETURN_VAL((user_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("USER NAME IS NULL"));


	int ret = ACCOUNT_ERROR_NONE;

	if (!g_hAccountDB) {
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected. rc : %d", ret));
	}

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT * FROM %s WHERE user_name = ?", ACCOUNT_TABLE);

	hstmt = _account_prepare_query(query);

	int binding_count = 1;
	_account_query_bind_text(hstmt, binding_count++, user_name);

	rc = _account_query_step(hstmt);

	account_s *account_head = NULL;

	ACCOUNT_CATCH_ERROR(rc == SQLITE_ROW, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	int tmp = 0;

	account_head = (account_s*) malloc(sizeof(account_s));
	if (account_head == NULL) {
		ACCOUNT_DEBUG("malloc Failed");
		if (hstmt != NULL) {
			_account_query_finalize(hstmt);
			hstmt = NULL;
		}
		return ACCOUNT_ERROR_OUT_OF_MEMORY;
	}
	ACCOUNT_MEMSET(account_head, 0x00, sizeof(account_s));

	while (rc == SQLITE_ROW) {
		account_s* account_record = NULL;

		account_record = (account_s*) malloc(sizeof(account_s));

		if (account_record == NULL) {
			ACCOUNT_DEBUG("malloc Failed");
			break;
		}
		ACCOUNT_MEMSET(account_record, 0x00, sizeof(account_s));

		_account_convert_column_to_account(hstmt, account_record);

		account_head->account_list = g_list_append(account_head->account_list, account_record);

		rc = _account_query_step(hstmt);
		tmp++;
		ACCOUNT_DEBUG("DETECTED COUNT %d\n", tmp);
	}

	_account_query_finalize(hstmt);
	hstmt = NULL;

	GList *iter;


	tmp = g_list_length(account_head->account_list);
	ACCOUNT_DEBUG("GLIST LEN %d\n", tmp);

	for (iter = account_head->account_list; iter != NULL; iter = g_list_next(iter)) {
		account_h account;
		account = (account_h)iter->data;

		account_s *testaccount = (account_s*)account;

		ACCOUNT_DEBUG("id = %d", testaccount->id);
		ACCOUNT_DEBUG("user_name = %s", testaccount->user_name);
		ACCOUNT_DEBUG("email_address = %s", testaccount->email_address);
		ACCOUNT_DEBUG("display_name = %s", testaccount->display_name);
		ACCOUNT_DEBUG("icon_path = %s", testaccount->icon_path);
		ACCOUNT_DEBUG("source = %s", testaccount->source);
		ACCOUNT_DEBUG("package_name = %s", testaccount->package_name);
		ACCOUNT_DEBUG("access_token = %s", testaccount->access_token);
		ACCOUNT_DEBUG("domain_name = %s", testaccount->domain_name);
		ACCOUNT_DEBUG("auth_type = %d", testaccount->auth_type);
		ACCOUNT_DEBUG("secret = %d", testaccount->secret);
		ACCOUNT_DEBUG("user text = %s", testaccount->user_data_txt[0]);
		ACCOUNT_DEBUG("user text = %s", testaccount->user_data_txt[1]);
		ACCOUNT_DEBUG("user text = %s", testaccount->user_data_txt[2]);
		ACCOUNT_DEBUG("user text = %s", testaccount->user_data_txt[3]);
		ACCOUNT_DEBUG("user text = %s", testaccount->user_data_txt[4]);
		ACCOUNT_DEBUG("user int = %d", testaccount->user_data_int[0]);
		ACCOUNT_DEBUG("user int = %d", testaccount->user_data_int[1]);
		ACCOUNT_DEBUG("user int = %d", testaccount->user_data_int[2]);
		ACCOUNT_DEBUG("user int = %d", testaccount->user_data_int[3]);
		ACCOUNT_DEBUG("user int = %d", testaccount->user_data_int[4]);

		account_query_capability_by_account_id(_account_get_capability_text_cb, testaccount->id, (void*)testaccount);

		ACCOUNT_DEBUG("capability_list address = %p", testaccount->capablity_list);

		cb_func(account, user_data);

	}

	error_code = ACCOUNT_ERROR_NONE;

CATCH:
	if (hstmt != NULL) {
		_account_query_finalize(hstmt);
		hstmt = NULL;
	}
	if (account_head) {
		if (account_head->account_list) {
			_account_glist_free(account_head->account_list);
			account_head->account_list = NULL;
			_account_free_account_items(account_head);
			_ACCOUNT_FREE(account_head);
		}
	}

	pthread_mutex_unlock(&account_mutex);
	return error_code;
}

int account_query_account_by_capability(account_cb cb_func, account_capability_type_e capability_type, account_capability_state_e capability_value, void* user_data)
{
	int 			error_code = ACCOUNT_ERROR_NONE;
	account_stmt	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0;

	if ( (capability_type < 0) || (capability_type > ACCOUNT_CAPABILITY_MOBILE_TRACKER)) {
		ACCOUNT_DEBUG("(%s)-(%d) capability_type is less than 1 or more than enum max.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if ((capability_value  < 0) || (capability_value > ACCOUNT_CAPABILITY_ENABLED)) {
		ACCOUNT_DEBUG("(%s)-(%d) capability_value is not equal to 0 or 1.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	ACCOUNT_RETURN_VAL((cb_func != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("CALL BACK IS NULL"));

	int ret = ACCOUNT_ERROR_NONE;

	if (!g_hAccountDB) {
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected. rc : %d", ret));
	}

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT * FROM %s WHERE _id IN (SELECT account_id from %s WHERE key=? AND value=?)", ACCOUNT_TABLE, CAPABILITY_TABLE);

	hstmt = _account_prepare_query(query);

	int binding_count = 1;
	_account_query_bind_int(hstmt, binding_count++, (int)capability_type);
	_account_query_bind_int(hstmt, binding_count++, (int)capability_value);

	rc = _account_query_step(hstmt);

	account_s* account_head = NULL;

	ACCOUNT_CATCH_ERROR(rc == SQLITE_ROW, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	int tmp = 0;

	account_head = (account_s*) malloc(sizeof(account_s));
	if (account_head == NULL) {
		ACCOUNT_DEBUG("malloc Failed");
		if (hstmt != NULL) {
			_account_query_finalize(hstmt);
			hstmt = NULL;
		}
		return ACCOUNT_ERROR_OUT_OF_MEMORY;
	}
	ACCOUNT_MEMSET(account_head, 0x00, sizeof(account_s));

	while (rc == SQLITE_ROW) {
		account_s* account_record = NULL;

		account_record = (account_s*) malloc(sizeof(account_s));

		if (account_record == NULL) {
			ACCOUNT_DEBUG("malloc Failed");
			break;
		}
		ACCOUNT_MEMSET(account_record, 0x00, sizeof(account_s));

		_account_convert_column_to_account(hstmt, account_record);

		account_head->account_list = g_list_append(account_head->account_list, account_record);

		rc = _account_query_step(hstmt);
		tmp++;
		ACCOUNT_DEBUG("DETECTED COUNT %d\n", tmp);
	}

	_account_query_finalize(hstmt);
	hstmt = NULL;

	GList *iter;


	tmp = g_list_length(account_head->account_list);
	ACCOUNT_DEBUG("GLIST LEN %d\n", tmp);

	for (iter = account_head->account_list; iter != NULL; iter = g_list_next(iter)) {
		account_h account = NULL;
		account = (account_h)iter->data;


		account_s* testaccount = (account_s*)account;

		ACCOUNT_DEBUG("id = %d", testaccount->id);
		ACCOUNT_DEBUG("user_name = %s", testaccount->user_name);
		ACCOUNT_DEBUG("email_address = %s", testaccount->email_address);
		ACCOUNT_DEBUG("display_name = %s", testaccount->display_name);
		ACCOUNT_DEBUG("icon_path = %s", testaccount->icon_path);
		ACCOUNT_DEBUG("source = %s", testaccount->source);
		ACCOUNT_DEBUG("package_name = %s", testaccount->package_name);
		ACCOUNT_DEBUG("access_token = %s", testaccount->access_token);
		ACCOUNT_DEBUG("domain_name = %s", testaccount->domain_name);
		ACCOUNT_DEBUG("auth_type = %d", testaccount->auth_type);
		ACCOUNT_DEBUG("secret = %d", testaccount->secret);
		ACCOUNT_DEBUG("user text = %s", testaccount->user_data_txt[0]);
		ACCOUNT_DEBUG("user text = %s", testaccount->user_data_txt[1]);
		ACCOUNT_DEBUG("user text = %s", testaccount->user_data_txt[2]);
		ACCOUNT_DEBUG("user text = %s", testaccount->user_data_txt[3]);
		ACCOUNT_DEBUG("user text = %s", testaccount->user_data_txt[4]);
		ACCOUNT_DEBUG("user int = %d", testaccount->user_data_int[0]);
		ACCOUNT_DEBUG("user int = %d", testaccount->user_data_int[1]);
		ACCOUNT_DEBUG("user int = %d", testaccount->user_data_int[2]);
		ACCOUNT_DEBUG("user int = %d", testaccount->user_data_int[3]);
		ACCOUNT_DEBUG("user int = %d", testaccount->user_data_int[4]);

		account_query_capability_by_account_id(_account_get_capability_text_cb, testaccount->id, (void*)testaccount);

		ACCOUNT_DEBUG("capability_list address = %p", testaccount->capablity_list);

		cb_func(account, user_data);

	}


	error_code = ACCOUNT_ERROR_NONE;

CATCH:
	if (hstmt != NULL) {
		_account_query_finalize(hstmt);
		hstmt = NULL;
	}
	if (account_head) {
		if (account_head->account_list) {
			_account_glist_free(account_head->account_list);
			account_head->account_list = NULL;
			_account_free_account_items(account_head);
			_ACCOUNT_FREE(account_head);
		}
	}

	pthread_mutex_unlock(&account_mutex);
	return error_code;
}


int account_query_account_by_package_name(account_cb cb_func, const char* package_name, void* user_data)
{
	int 			error_code = ACCOUNT_ERROR_NONE;
	account_stmt	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0;

	ACCOUNT_RETURN_VAL((package_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("PACKAGE NAME IS NULL"));
	ACCOUNT_RETURN_VAL((cb_func != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("CALL BACK IS NULL"));

	int ret = ACCOUNT_ERROR_NONE;

	if (!g_hAccountDB) {
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected. rc : %d", ret));
	}

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT * FROM %s WHERE package_name=?", ACCOUNT_TABLE);

	hstmt = _account_prepare_query(query);

	int binding_count = 1;
	_account_query_bind_text(hstmt, binding_count++, package_name);

	rc = _account_query_step(hstmt);

	account_s* account_head = NULL;

	ACCOUNT_CATCH_ERROR(rc == SQLITE_ROW, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	int tmp = 0;

	account_head = (account_s*) malloc(sizeof(account_s));
	if (account_head == NULL) {
		ACCOUNT_DEBUG("malloc Failed");
		if (hstmt != NULL) {
			_account_query_finalize(hstmt);
			hstmt = NULL;
		}
		return ACCOUNT_ERROR_OUT_OF_MEMORY;
	}
	ACCOUNT_MEMSET(account_head, 0x00, sizeof(account_s));

	while (rc == SQLITE_ROW) {
		account_s* account_record = NULL;

		account_record = (account_s*) malloc(sizeof(account_s));

		if (account_record == NULL) {
			ACCOUNT_DEBUG("malloc Failed");
			break;
		}
		ACCOUNT_MEMSET(account_record, 0x00, sizeof(account_s));

		_account_convert_column_to_account(hstmt, account_record);

		account_head->account_list = g_list_append(account_head->account_list, account_record);

		rc = _account_query_step(hstmt);
		tmp++;
		ACCOUNT_DEBUG("DETECTED COUNT %d\n", tmp);
	}

	_account_query_finalize(hstmt);
	hstmt = NULL;

	GList *iter;


	tmp = g_list_length(account_head->account_list);
	ACCOUNT_DEBUG("GLIST LEN %d\n", tmp);

	for (iter = account_head->account_list; iter != NULL; iter = g_list_next(iter)) {
		account_h account = NULL;
		account = (account_h)iter->data;


		account_s* testaccount = (account_s*)account;

		ACCOUNT_DEBUG("id = %d", testaccount->id);
		ACCOUNT_DEBUG("user_name = %s", testaccount->user_name);
		ACCOUNT_DEBUG("email_address = %s", testaccount->email_address);
		ACCOUNT_DEBUG("display_name = %s", testaccount->display_name);
		ACCOUNT_DEBUG("icon_path = %s", testaccount->icon_path);
		ACCOUNT_DEBUG("source = %s", testaccount->source);
		ACCOUNT_DEBUG("package_name = %s", testaccount->package_name);
		ACCOUNT_DEBUG("access_token = %s", testaccount->access_token);
		ACCOUNT_DEBUG("domain_name = %s", testaccount->domain_name);
		ACCOUNT_DEBUG("auth_type = %d", testaccount->auth_type);
		ACCOUNT_DEBUG("secret = %d", testaccount->secret);
		ACCOUNT_DEBUG("user text = %s", testaccount->user_data_txt[0]);
		ACCOUNT_DEBUG("user text = %s", testaccount->user_data_txt[1]);
		ACCOUNT_DEBUG("user text = %s", testaccount->user_data_txt[2]);
		ACCOUNT_DEBUG("user text = %s", testaccount->user_data_txt[3]);
		ACCOUNT_DEBUG("user text = %s", testaccount->user_data_txt[4]);
		ACCOUNT_DEBUG("user int = %d", testaccount->user_data_int[0]);
		ACCOUNT_DEBUG("user int = %d", testaccount->user_data_int[1]);
		ACCOUNT_DEBUG("user int = %d", testaccount->user_data_int[2]);
		ACCOUNT_DEBUG("user int = %d", testaccount->user_data_int[3]);
		ACCOUNT_DEBUG("user int = %d", testaccount->user_data_int[4]);

		account_query_capability_by_account_id(_account_get_capability_text_cb, testaccount->id, (void*)testaccount);

		ACCOUNT_DEBUG("capability_list address = %p", testaccount->capablity_list);

		cb_func(account, user_data);

	}

	error_code = ACCOUNT_ERROR_NONE;

CATCH:
	if (hstmt != NULL) {
		_account_query_finalize(hstmt);
		hstmt = NULL;
	}
	if (account_head) {
		if (account_head->account_list) {
			_account_glist_free(account_head->account_list);
			account_head->account_list = NULL;
			_account_free_account_items(account_head);
			_ACCOUNT_FREE(account_head);
		}
	}

	pthread_mutex_unlock(&account_mutex);
	return error_code;
}

int account_delete(int account_id)
{
	int				error_code = ACCOUNT_ERROR_NONE;
	account_stmt 	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int				rc = 0;
	int				ret_transaction = 0, ret =0;
	bool			is_success = FALSE;

	if (!g_hAccountDB) {
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected. rc : %d", ret));
	}

	/* transaction control required*/
	ret_transaction = _account_begin_transaction();

	if (ret_transaction != ACCOUNT_ERROR_NONE) {
		ACCOUNT_DEBUG("account_delete:_account_begin_transaction fail %d\n", ret_transaction);
		pthread_mutex_unlock(&account_mutex);
		return ret_transaction;
	}

	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE account_id = %d", CAPABILITY_TABLE, account_id);

	hstmt = _account_prepare_query(query);
	ACCOUNT_CATCH_ERROR(hstmt != NULL, {}, ACCOUNT_ERROR_DB_FAILED,
			("_account_svc_query_prepare(%s) failed(%s).\n", query, _account_db_err_msg()));

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_DONE, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	_account_query_finalize(hstmt);
	hstmt = NULL;

	ACCOUNT_MEMSET(query, 0, sizeof(query));

	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE _id = %d", ACCOUNT_TABLE, account_id);

	hstmt = _account_prepare_query(query);
	ACCOUNT_CATCH_ERROR(hstmt != NULL, {}, ACCOUNT_ERROR_DB_FAILED,
			("_account_svc_query_prepare(%s) failed(%s).\n", query, _account_db_err_msg()));

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_DONE, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found. id=%d, rc=%d\n", account_id, rc));

	_account_query_finalize(hstmt);
	is_success = TRUE;

	hstmt = NULL;

CATCH:
	if (hstmt != NULL) {
		_account_query_finalize(hstmt);
		hstmt = NULL;
	}

	ret_transaction = _account_end_transaction(is_success);

	if (ret_transaction != ACCOUNT_ERROR_NONE) {
		ACCOUNT_DEBUG("account_svc_delete:_account_svc_end_transaction fail %d, is_success=%d\n", ret_transaction, is_success);
	}

	pthread_mutex_unlock(&account_mutex);

	return error_code;

}

int account_delete_from_db_by_id(int account_db_id)
{
	ACCOUNT_RETURN_VAL((account_db_id > 0), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT ID IS LESS THAN ZERO."));

	account_delete(account_db_id);
}

int account_delete_from_db_by_user_name(char *user_name, char *package_name)
{
	int 			error_code = ACCOUNT_ERROR_NONE;
	account_stmt	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0;
	int 			ret_transaction = 0, ret =0;
	bool			is_success = FALSE;

	ACCOUNT_RETURN_VAL((user_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("user_name is null!"));
	ACCOUNT_RETURN_VAL((package_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("package_name is null!"));

	if (!g_hAccountDB) {
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected. rc : %d", ret));
	}

	/* transaction control required*/
	ret_transaction = _account_begin_transaction();

	if (ret_transaction != ACCOUNT_ERROR_NONE) {
		ACCOUNT_DEBUG("account_delete:_account_begin_transaction fail %d\n", ret_transaction);
		pthread_mutex_unlock(&account_mutex);
		return ret_transaction;
	}

	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE user_name = ? and package_name = ?", CAPABILITY_TABLE);

	hstmt = _account_prepare_query(query);

	ACCOUNT_CATCH_ERROR(hstmt != NULL, {}, ACCOUNT_ERROR_DB_FAILED,
			("_account_svc_query_prepare(%s) failed(%s).\n", query, _account_db_err_msg()));

	int binding_count = 1;
	_account_query_bind_text(hstmt, binding_count++, user_name);
	_account_query_bind_text(hstmt, binding_count++, package_name);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_DONE, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	_account_query_finalize(hstmt);
	hstmt = NULL;

	ACCOUNT_MEMSET(query, 0, sizeof(query));

	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE user_name = ? and package_name = ?", ACCOUNT_TABLE);

	hstmt = _account_prepare_query(query);
	ACCOUNT_CATCH_ERROR(hstmt != NULL, {}, ACCOUNT_ERROR_DB_FAILED,
			("_account_svc_query_prepare(%s) failed(%s).\n", query, _account_db_err_msg()));

	binding_count = 1;
	_account_query_bind_text(hstmt, binding_count++, user_name);
	_account_query_bind_text(hstmt, binding_count++, package_name);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_DONE, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found. user_name=%s, package_name=%s, rc=%d\n", user_name, package_name, rc));

	_account_query_finalize(hstmt);
	is_success = TRUE;

	hstmt = NULL;

CATCH:
	if (hstmt != NULL) {
		_account_query_finalize(hstmt);
		hstmt = NULL;
	}

	ret_transaction = _account_end_transaction(is_success);

	if (ret_transaction != ACCOUNT_ERROR_NONE) {
		ACCOUNT_DEBUG("account_svc_delete:_account_svc_end_transaction fail %d, is_success=%d\n", ret_transaction, is_success);
	}

	pthread_mutex_unlock(&account_mutex);

	return error_code;
}

int account_delete_from_db_by_package_name(char *package_name)
{
	int 			error_code = ACCOUNT_ERROR_NONE;
	account_stmt	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0;
	int 			ret_transaction = 0, ret =0;
	bool			is_success = FALSE;

	ACCOUNT_RETURN_VAL((package_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("package_name is null!"));

	if (!g_hAccountDB) {
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected. rc : %d", ret));
	}

	/* transaction control required*/
	ret_transaction = _account_begin_transaction();

	if (ret_transaction != ACCOUNT_ERROR_NONE) {
		ACCOUNT_DEBUG("account_delete:_account_begin_transaction fail %d\n", ret_transaction);
		pthread_mutex_unlock(&account_mutex);
		return ret_transaction;
	}

	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE package_name = ?", CAPABILITY_TABLE);

	hstmt = _account_prepare_query(query);

	ACCOUNT_CATCH_ERROR(hstmt != NULL, {}, ACCOUNT_ERROR_DB_FAILED,
			("_account_svc_query_prepare(%s) failed(%s).\n", query, _account_db_err_msg()));

	int binding_count = 1;
	_account_query_bind_text(hstmt, binding_count++, package_name);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_DONE, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	_account_query_finalize(hstmt);
	hstmt = NULL;

	ACCOUNT_MEMSET(query, 0, sizeof(query));

	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE package_name = ?", ACCOUNT_TABLE);

	hstmt = _account_prepare_query(query);
	ACCOUNT_CATCH_ERROR(hstmt != NULL, {}, ACCOUNT_ERROR_DB_FAILED,
			("_account_svc_query_prepare(%s) failed(%s).\n", query, _account_db_err_msg()));

	binding_count = 1;
	_account_query_bind_text(hstmt, binding_count++, package_name);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_DONE, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found. package_name=%s, rc=%d\n", package_name, rc));

	_account_query_finalize(hstmt);
	is_success = TRUE;

	hstmt = NULL;

CATCH:
	if (hstmt != NULL) {
		_account_query_finalize(hstmt);
		hstmt = NULL;
	}

	ret_transaction = _account_end_transaction(is_success);

	if (ret_transaction != ACCOUNT_ERROR_NONE) {
		ACCOUNT_DEBUG("account_svc_delete:_account_svc_end_transaction fail %d, is_success=%d\n", ret_transaction, is_success);
	}

	pthread_mutex_unlock(&account_mutex);

	return error_code;
}

int account_get_total_count_from_db(int *count)
{
	if (!count) {
		ACCOUNT_DEBUG("(%s)-(%d) count is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	char query[1024] = {0, };
	ACCOUNT_MEMSET(query, 0x00, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "select count(*) from %s", ACCOUNT_TABLE);

	*count = _account_get_record_count(query);
	int rc = -1;
	int ncount = 0;
	account_stmt pStmt = NULL;

	assert(NULL != g_hAccountDB);
	rc = sqlite3_prepare_v2(g_hAccountDB, query, strlen(query), &pStmt, NULL);

	rc = sqlite3_step(pStmt);
	if (SQLITE_ROW != rc) {
		ACCOUNT_DEBUG("[ERROR] sqlite3_step() failed\n");
		sqlite3_finalize(pStmt);
		return ACCOUNT_ERROR_DB_FAILED;
	}

	ncount = sqlite3_column_int(pStmt, 0);

	*count = ncount;

	ACCOUNT_DEBUG("Number of account : %d", ncount);
	sqlite3_finalize(pStmt);

	if (ncount < 0) {
		ACCOUNT_DEBUG("[ERROR] Number of account : %d, End", ncount);
		return ACCOUNT_ERROR_DB_FAILED;
	}

	return ACCOUNT_ERROR_NONE;
}

int account_destroy(account_h account)
{
	account_s *data = (account_s*)account;

	ACCOUNT_RETURN_VAL((data != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Account handle is null!"));

	ACCOUNT_DEBUG("destroy handle=%p\n", data);

	_account_free_account_items(data);
	_ACCOUNT_FREE(data);

	return ACCOUNT_ERROR_NONE;
}


