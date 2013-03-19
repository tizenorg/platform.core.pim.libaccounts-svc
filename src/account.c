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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <account.h>
#include <glib.h>
#include <db-util.h>
#include <pthread.h>
#include <assert.h>
#include <account-private.h>
#include <vconf.h>
#include <app.h>

#define ACCOUNT_VERIFY
#ifdef ACCOUNT_VERIFY
#include <pkgmgr-info.h>
#include <aul.h>
#include <unistd.h>

#define EAS_CMDLINE "/usr/bin/eas-engine"
#define EMAIL_SERVICE_CMDLINE "/usr/bin/email-service"
#define ACTIVESYNC_APPID "activesync-ui"

#ifdef TARGET_ARM
#define EMAIL_APPID "email-setting-efl"
#else
#define EMAIL_APPID "vxqbrefica.Email"
#endif

#endif

static sqlite3* g_hAccountDB = NULL;
static int		g_refCntDB = 0;
pthread_mutex_t account_mutex = PTHREAD_MUTEX_INITIALIZER;

static char *_account_get_text(const char *text_data);
static int _account_gslist_free(GSList* list);
static int _account_glist_free(GList* list);
static char *_account_query_table_column_text(account_stmt pStmt, int pos);
static int _account_insert_custom(account_s *account, int account_id);
static int _account_update_custom(account_s *account, int account_id);
static int _account_query_custom_by_account_id(account_custom_cb cb_func, int account_id, void *user_data );
static int _account_type_update_provider_feature(account_type_s *account_type, const char* app_id);

#ifdef ACCOUNT_VERIFY
int _account_get_current_appid_cb(const pkgmgrinfo_appinfo_h handle, void *user_data)
{
	int ret = -1;
	char* appid = NULL;
	char* item = NULL;
	GSList** appid_list = (GSList**)user_data;

	ret = pkgmgrinfo_appinfo_get_appid(handle, &appid);
	item = _account_get_text(appid);
	*appid_list = g_slist_append(*appid_list, item);

	return 0;
}

static inline int __read_proc(const char *path, char *buf, int size)
{
	int fd = 0, ret = 0;

	if (buf == NULL || path == NULL) {
		ACCOUNT_ERROR("path and buffer is mandatory\n");
		return -1;
	}

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		ACCOUNT_ERROR("fd open error(%d)\n", fd);
		return -1;
	}

	ret = read(fd, buf, size - 1);
	if (ret <= 0) {
		ACCOUNT_ERROR("fd read error(%d)\n", fd);
		close(fd);
		return -1;
	} else
		buf[ret] = 0;

	close(fd);

	return ret;
}

char *_account_get_proc_cmdline_bypid(int pid)
{
	char buf[128];
	int ret = 0;

	ACCOUNT_SNPRINTF(buf, sizeof(buf), "/proc/%d/cmdline", pid);
	ret = __read_proc(buf, buf, sizeof(buf));
	if (ret <= 0) {
		ACCOUNT_INFO("No proc directory (%d)\n", pid);
		return NULL;
	}

	return strdup(buf);
}


static char* _account_get_current_appid()
{
	int pid=0, ret=-1;
	char appid[128]={0,};
	pkgmgrinfo_appinfo_h ahandle=NULL;
	char* appid_ret = NULL;

	pid = getpid();
	ACCOUNT_INFO("pid (%d)\n", pid, ret);
	ret = aul_app_get_appid_bypid(pid, appid, sizeof(appid));
	ACCOUNT_INFO("appid (%s) ret(%d)!!!!!!\n", appid, ret);

	/* SLP platform core exception */
	if(strlen(appid) == 0){
		char* cmdline = NULL;
		cmdline = _account_get_proc_cmdline_bypid(pid);
		ACCOUNT_INFO("cmdline (%s)!!!!!!\n", cmdline);
		if(!strcmp(cmdline, EAS_CMDLINE)) {
			appid_ret = _account_get_text(ACTIVESYNC_APPID);
			return appid_ret;
		} else if (!strcmp(cmdline, EMAIL_SERVICE_CMDLINE)) {
			appid_ret = _account_get_text(EMAIL_APPID);
			return appid_ret;
		} else {
			ACCOUNT_ERROR("No app id\n");
		return NULL;
	}
	}

	appid_ret = _account_get_text(appid);

	pkgmgrinfo_appinfo_destroy_appinfo(ahandle);
	return appid_ret;
}

static bool _account_verify_permission(const char* appid)
{
	bool b_ret = FALSE;
	int ret=-1;
	pkgmgrinfo_appinfo_h ahandle=NULL;
	pkgmgrinfo_pkginfo_h phandle=NULL;
	char* package_id;
	GSList* appid_list = NULL;
	GSList* iter = NULL;

	char* current_appid = _account_get_current_appid();

	if(current_appid == NULL) {
		// assuming current process is slp core daemon
		ACCOUNT_DEBUG("current app id is null\n");
		return FALSE;
	}

		/* Get app id family which is stored in account database */
		ret = pkgmgrinfo_appinfo_get_appinfo(appid, &ahandle);
		ACCOUNT_INFO("ahandle (%p), ret(%x)\n", ahandle, ret);
		ret = pkgmgrinfo_appinfo_get_pkgid(ahandle, &package_id);
		ACCOUNT_INFO("package_id (%s), ret(%x)\n", package_id, ret);
		ret = pkgmgrinfo_pkginfo_get_pkginfo(package_id, &phandle);
		ACCOUNT_INFO("phandle (%p), ret(%x)\n", package_id, ret);

		ret = pkgmgrinfo_appinfo_get_list(phandle, PMINFO_ALL_APP, _account_get_current_appid_cb, (void *)&appid_list); //==> pkg핸들에 속한 ui-appid 모두 찾음
		ACCOUNT_INFO("ret(%x)\n", ret);

		/* Compare current app id with the stored app id family */
		for(iter=appid_list;iter!=NULL;iter=g_slist_next(iter)){
			char* tmp = (char*)iter->data;
			if(tmp && !strcmp(tmp, current_appid)) {
			ACCOUNT_INFO("permission verified appid(%s), current appid(%s)\n", tmp, current_appid);
				b_ret = TRUE;
			}
		ACCOUNT_DEBUG("owner group app id(%s), current appid(%s)\n", tmp, current_appid);
			_ACCOUNT_FREE(tmp);
		}

		g_slist_free(appid_list);
		pkgmgrinfo_pkginfo_destroy_pkginfo(phandle);
		pkgmgrinfo_appinfo_destroy_appinfo(ahandle);

	_ACCOUNT_FREE(current_appid);

	return b_ret;

}
#endif

static const char *_account_db_err_msg()
{
	assert(NULL != g_hAccountDB);
	return sqlite3_errmsg(g_hAccountDB);
}

static void _account_insert_delete_update_notification_send(char *noti_name)
{
	if (!noti_name) {
		ACCOUNT_ERROR("Noti Name is NULL!!!!!!\n");
		return;
	}

	ACCOUNT_DEBUG("Sending notification with value %s\n", noti_name);

	if (vconf_set_str(VCONFKEY_ACCOUNT_MSG_STR, noti_name) == 0) {
		ACCOUNT_VERBOSE("Vconf MSG Str set SUCCESS !!!!!!\n");;
	} else {
		ACCOUNT_ERROR("Vconf MSG Str set FAILED !!!!!!\n");;
	}
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
		ACCOUNT_ERROR("sqlite3_step() failed(%d, %s).", rc, _account_db_err_msg());
		sqlite3_finalize(pStmt);
		return ACCOUNT_ERROR_DB_FAILED;
	}

	ncount = sqlite3_column_int(pStmt, 0);

	ACCOUNT_VERBOSE("count : %d, End", ncount);
	sqlite3_finalize(pStmt);

	return ncount;
}

static int _account_execute_query(char *query)
{
	int rc = -1;
	char* pszErrorMsg = NULL;

	assert(NULL != query);
	assert(NULL != g_hAccountDB);

	ACCOUNT_INFO("query : %s", query);

	rc = sqlite3_exec(g_hAccountDB, query, NULL, NULL, &pszErrorMsg);
	if (SQLITE_OK != rc) {
		ACCOUNT_FATAL("sqlite3_exec(%s) failed(%s).", query, pszErrorMsg);
		sqlite3_free(pszErrorMsg);
	}

	return rc;
}

static int _account_begin_transaction(void)
{
	int ret = -1;

	ret = _account_execute_query("BEGIN IMMEDIATE TRANSACTION");

	if (ret != SQLITE_OK) {
		ACCOUNT_FATAL("_account_svc_begin_transaction fail :: %d", ret);
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
		ACCOUNT_FATAL("_account_svc_end_transaction fail :: %d", ret);
		return ACCOUNT_ERROR_DB_FAILED;
	}

   return ACCOUNT_ERROR_NONE;
}

static bool _account_check_add_more_account(const char* app_id)
{
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0;

	ACCOUNT_RETURN_VAL((app_id != 0), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("APP ID IS NULL"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_DEBUG("app id (%s)\n", app_id);

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) FROM %s WHERE AppId = '%s' and MultipleAccountSupport = 1", ACCOUNT_TYPE_TABLE, app_id);
	rc = _account_get_record_count(query);

	ACCOUNT_DEBUG("app id (%s) supports multiple account. rc(%d)\n", app_id, rc);

	/* multiple account support case */
	if(rc > 0) {
		ACCOUNT_DEBUG("app id (%s) supports multiple account. rc(%d)\n", app_id, rc);
		return TRUE;
	}

	/* multiple account not support case */
	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);
	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) FROM %s WHERE package_name = '%s'", ACCOUNT_TABLE, app_id);
	rc = _account_get_record_count(query);

	if(rc <= 0) {
		ACCOUNT_DEBUG("app id (%s) supports single account. and there is no account of the app id\n", app_id);
		return TRUE;
	}

	return FALSE;
}

static int _account_create_all_tables(void)
{
	int rc = -1;
	int error_code = ACCOUNT_ERROR_NONE;
	char	query[ACCOUNT_SQL_LEN_MAX] = {0, };


	ACCOUNT_MEMSET(query, 0, sizeof(query));

	ACCOUNT_VERBOSE("_account_create_all_tables begin");

	/*Create the account table*/
	ACCOUNT_SNPRINTF(query, sizeof(query), "select count(*) from sqlite_master where name in ('%s')", ACCOUNT_TABLE);
	rc = _account_get_record_count(query);
	if (rc <= 0) {
		ACCOUNT_MEMSET(query, 0, sizeof(query));
		ACCOUNT_SNPRINTF(query, sizeof(query),  ACCOUNT_SCHEMA, ACCOUNT_TABLE);
		ACCOUNT_INFO("Create %s table", ACCOUNT_TABLE);
		rc = _account_execute_query(query);
		ACCOUNT_RETURN_VAL((SQLITE_OK == rc), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_execute_query(%s) failed(%d, %s).\n", query, rc, _account_db_err_msg()));
	}

	/*Create capability table*/
	ACCOUNT_MEMSET(query, 0, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "select count(*) from sqlite_master where name in ('%s')", CAPABILITY_TABLE);
	rc = _account_get_record_count(query);
	if (rc <= 0) {
		ACCOUNT_MEMSET(query, 0, sizeof(query));
		ACCOUNT_SNPRINTF(query, sizeof(query),  CAPABILITY_SCHEMA, CAPABILITY_TABLE);
		ACCOUNT_INFO("Create %s table", CAPABILITY_TABLE);
		rc = _account_execute_query(query);
		ACCOUNT_RETURN_VAL((SQLITE_OK == rc), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_execute_query(%s) failed(%d, %s).\n", query, rc, _account_db_err_msg()));
	}

	/* Create account custom table */
	ACCOUNT_MEMSET(query, 0, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "select count(*) from sqlite_master where name in ('%s')", ACCOUNT_CUSTOM_TABLE);
	rc = _account_get_record_count(query);
	if (rc <= 0) {
		ACCOUNT_MEMSET(query, 0, sizeof(query));
		ACCOUNT_SNPRINTF(query, sizeof(query),  ACCOUNT_CUSTOM_SCHEMA, ACCOUNT_CUSTOM_TABLE);
		ACCOUNT_INFO("Create %s table", ACCOUNT_CUSTOM_TABLE);
		rc = _account_execute_query(query);
		ACCOUNT_RETURN_VAL((SQLITE_OK == rc), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_execute_query(%s) failed(%d, %s).\n", query, rc, _account_db_err_msg()));
	}

	/* Create account type table */
	ACCOUNT_MEMSET(query, 0, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "select count(*) from sqlite_master where name in ('%s')", ACCOUNT_TYPE_TABLE);
	rc = _account_get_record_count(query);
	if (rc <= 0) {
		ACCOUNT_MEMSET(query, 0, sizeof(query));
		ACCOUNT_SNPRINTF(query, sizeof(query),  ACCOUNT_TYPE_SCHEMA, ACCOUNT_TYPE_TABLE);
		ACCOUNT_INFO("Create %s table", ACCOUNT_TYPE_TABLE);
		rc = _account_execute_query(query);
		ACCOUNT_RETURN_VAL((SQLITE_OK == rc), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_execute_query(%s) failed(%d, %s).\n", query, rc, _account_db_err_msg()));
	}

	/* Create label table */
	ACCOUNT_MEMSET(query, 0, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "select count(*) from sqlite_master where name in ('%s')", LABEL_TABLE);
	rc = _account_get_record_count(query);
	if (rc <= 0) {
		ACCOUNT_MEMSET(query, 0, sizeof(query));
		ACCOUNT_SNPRINTF(query, sizeof(query),  LABEL_SCHEMA, LABEL_TABLE);
		ACCOUNT_INFO("Create %s table", LABEL_TABLE);
		rc = _account_execute_query(query);
		ACCOUNT_RETURN_VAL((SQLITE_OK == rc), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_execute_query(%s) failed(%d, %s).\n", query, rc, _account_db_err_msg()));
	}

	/* Create account feature table */
	ACCOUNT_MEMSET(query, 0, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "select count(*) from sqlite_master where name in ('%s')", PROVIDER_FEATURE_TABLE);
	rc = _account_get_record_count(query);
	if (rc <= 0) {
		ACCOUNT_MEMSET(query, 0, sizeof(query));
		ACCOUNT_SNPRINTF(query, sizeof(query),  PROVIDER_FEATURE_SCHEMA, PROVIDER_FEATURE_TABLE);
		ACCOUNT_INFO("Create %s table", PROVIDER_FEATURE_TABLE);
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

	ACCOUNT_VERBOSE("_account_check_is_all_table_exists");

	ACCOUNT_SNPRINTF(query, sizeof(query), "select count(*) from sqlite_master where name in ('%s', '%s', '%s', '%s', '%s', '%s')",
			ACCOUNT_TABLE, CAPABILITY_TABLE, ACCOUNT_CUSTOM_TABLE, ACCOUNT_TYPE_TABLE, LABEL_TABLE, PROVIDER_FEATURE_TABLE);
	rc = _account_get_record_count(query);

	ACCOUNT_DEBUG("Table count = %d\n", rc);

	if (rc != ACCOUNT_TABLE_TOTAL_COUNT) {
		ACCOUNT_ERROR("Table count is not matched rc=%d\n", rc);
		return FALSE;
	}

	ACCOUNT_VERBOSE("END of _account_check_is_all_table_exists\n");

	return TRUE;
}

static int _account_db_open(void)
{
	int 	rc = 0;
	char query[ACCOUNT_SQL_LEN_MAX] = {0, };

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));

	if (!g_hAccountDB) {
		rc = db_util_open(ACCOUNT_DB_NAME, &g_hAccountDB, DB_UTIL_REGISTER_HOOK_METHOD);
		ACCOUNT_RETURN_VAL((rc == SQLITE_OK), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected. rc : %d", rc));

		g_refCntDB++;
		ACCOUNT_INFO("_account_db_open: The database connected. refcnt=%d ", g_refCntDB);
	} else {
		g_refCntDB++;
		ACCOUNT_INFO("The database already connected. refcnt=%d ", g_refCntDB);
	}

	return ACCOUNT_ERROR_NONE;
}

static int _account_db_close(void)
{
	int 	rc = 0;
	int ret = -1;

	if (g_hAccountDB) {
		if (g_refCntDB > 0) {
			g_refCntDB--;
			ACCOUNT_INFO("_account_svc_db_close: The database disconnected. refcnt=%d ", g_refCntDB);
		}
		if (g_refCntDB == 0) {
			rc = db_util_close(g_hAccountDB);
			ACCOUNT_RETURN_VAL((rc == SQLITE_OK), {}, ACCOUNT_ERROR_DB_FAILED, ("The database isn't connected. rc : %d", rc));
			g_hAccountDB = NULL;
			ACCOUNT_INFO( "_account_svc_db_close: The database disconnected really. ");
		}
		ret = ACCOUNT_ERROR_NONE;
	} else {
		ACCOUNT_ERROR( "_account_svc_db_close: No handle(). refcnt=%d ", g_refCntDB);
		ret = ACCOUNT_ERROR_DB_FAILED;
	}

	return ret;
}

static int _account_connect(void)
{
	int error_code = ACCOUNT_ERROR_NONE;

	pthread_mutex_lock(&account_mutex);

	ACCOUNT_VERBOSE("db path = %s\n", ACCOUNT_DB_NAME);

	error_code = _account_db_open();
	if (ACCOUNT_ERROR_NONE != error_code) {
		ACCOUNT_ERROR("The database isn't connected.\n");
		pthread_mutex_unlock(&account_mutex);
		return ACCOUNT_ERROR_DB_NOT_OPENED;
	}

	if (FALSE == _account_check_is_all_table_exists())
		error_code = _account_create_all_tables();

	pthread_mutex_unlock(&account_mutex);
	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_connect (void)
{
	return _account_connect();
}

static int _account_disconnect(void)
{
	int error_code = ACCOUNT_ERROR_NONE;

	pthread_mutex_lock(&account_mutex);
	ACCOUNT_INFO("db path = %s have been closed!!!\n", ACCOUNT_DB_NAME);

	error_code = _account_db_close();
	pthread_mutex_unlock(&account_mutex);

	return error_code;
}

ACCOUNT_API int account_disconnect (void)
{
	return _account_disconnect();
}

static int _account_free_capability_items(account_capability_s *data)
{
	_ACCOUNT_FREE(data->type);
	_ACCOUNT_FREE(data->package_name);
	_ACCOUNT_FREE(data->user_name);

	return ACCOUNT_ERROR_NONE;
}

static int _account_custom_item_free(account_custom_s *data)
{
	_ACCOUNT_FREE(data->app_id);
	_ACCOUNT_FREE(data->key);
	_ACCOUNT_FREE(data->value);

	return ACCOUNT_ERROR_NONE;
}

static int _account_custom_gslist_free(GSList* list)
{
	ACCOUNT_RETURN_VAL((list != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("GSlist is NULL"));

	GSList* iter;

	for (iter = list; iter != NULL; iter = g_slist_next(iter)) {
		account_custom_s *custom_data = (account_custom_s*)iter->data;
		_account_custom_item_free(custom_data);
		_ACCOUNT_FREE(custom_data);
	}

	g_slist_free(list);
	list = NULL;

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
	_ACCOUNT_FREE(data->access_token);

	int i;
	for(i=0;i<USER_TXT_CNT;i++)
		_ACCOUNT_FREE(data->user_data_txt[i]);

	_account_gslist_free(data->capablity_list);
	_account_glist_free(data->account_list);
	_account_custom_gslist_free(data->custom_list);

	return ACCOUNT_ERROR_NONE;
}

static int _account_gslist_free(GSList* list)
{
	ACCOUNT_RETURN_VAL((list != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("GSlist is NULL"));

	GSList* iter;

	for (iter = list; iter != NULL; iter = g_slist_next(iter)) {
		account_capability_s *cap_data = (account_capability_s*)iter->data;
		_account_free_capability_items(cap_data);
		_ACCOUNT_FREE(cap_data);
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
		_ACCOUNT_FREE(account_record);
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

	ACCOUNT_SNPRINTF(query, sizeof(query), "select count(*) from %s where user_name='%s' and package_name='%s'"
			, ACCOUNT_TABLE, data->user_name, data->package_name);

	count = _account_get_record_count(query);
	if (count > 0) {
		ACCOUNT_INFO("_account_check_duplicated : duplicated %d account(s) exist!, user_name=%s, domain_name=%s\n",
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

	ACCOUNT_VERBOSE( "[Enter] pszName:%s\n", pszName);

	ACCOUNT_MEMSET(szQuery, 0x00, sizeof(szQuery));
	ACCOUNT_SNPRINTF(szQuery, sizeof(szQuery),  "SELECT max(seq) FROM %s where name = '%s' ", ACCOUNT_SQLITE_SEQ, pszName);
	rc = sqlite3_prepare_v2(g_hAccountDB, szQuery, strlen(szQuery), &pStmt, NULL);

	rc = sqlite3_step(pStmt);
	max_seq = sqlite3_column_int(pStmt, 0);
	max_seq++;

	ACCOUNT_VERBOSE( "sqlite3_column_int, rc=%d, max_seq=%d\n", rc, max_seq);

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

	ACCOUNT_INFO( "prepare query : %s", query);

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
	ACCOUNT_VERBOSE( "_account_convert_account_to_sql");
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

	/* 11. sync_support */
	_account_query_bind_int(hstmt, count++, account->sync_support);

	int i;

	/* 12. user text*/
	for(i=0; i< USER_TXT_CNT; i++)
		_account_query_bind_text(hstmt, count++, (char*)account->user_data_txt[i]);

	/* 13. user integer	*/
	for(i=0; i< USER_INT_CNT; i++)
		_account_query_bind_int(hstmt, count++, account->user_data_int[i]);

	return count;
}

static void _account_query_finalize(account_stmt pStmt)
{
	int rc = -1;

	if (!pStmt) {
		ACCOUNT_FATAL( "pStmt is NULL");
		return;
	}

	rc = sqlite3_finalize(pStmt);
	if (rc != SQLITE_OK) {
		ACCOUNT_FATAL( "sqlite3_finalize fail, rc : %d, db_error : %s\n", rc, _account_db_err_msg());
	}

	ACCOUNT_VERBOSE( "sqlite3_finalize finish");
}

static int _account_query_step(account_stmt pStmt)
{
	assert(NULL != pStmt);
	return sqlite3_step(pStmt);
}



static int _do_account_owner_existance_check()
{
        /* TODO check owner*/
	return ACCOUNT_ERROR_NONE;
}

static int _account_execute_insert_query(account_s *account)
{
	int				rc = 0;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int				error_code = ACCOUNT_ERROR_NONE;
	account_stmt 	hstmt = NULL;

	/* check whether app id exist in account type db */

	if (!account->user_name && !account->display_name && !account->email_address) {
		ACCOUNT_ERROR("Mandetory fields is NULL. At least one field is required among username, display name, email address\n");
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	/* Check account own icon existence */
	if(!account->icon_path) {
		char* temptxt = NULL;
		account_type_h account_type = NULL;
		error_code = account_type_create(&account_type);
		error_code = account_type_query_by_app_id((const char*)account->package_name, &account_type);
		error_code = account_type_get_icon_path(account_type, &temptxt);
		if (error_code != ACCOUNT_ERROR_NONE) {
				ACCOUNT_ERROR("account_type_get_icon_path: Failed \n");
		} else if (temptxt && strlen(temptxt)) {
			ACCOUNT_DEBUG("icon path (%s) app_id(%s) \n", temptxt, account->package_name);
			account->icon_path = _account_get_text(temptxt);
		} else {
			ACCOUNT_DEBUG("account_get_icon_path: returned NULL \n");
		}
		_ACCOUNT_FREE(temptxt);
		error_code = account_type_destroy(account_type);
	}

	/* End of Check account own icon existence */

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "INSERT INTO %s( user_name, email_address , display_name , icon_path , source , package_name , "
			"access_token , domain_name , auth_type , secret , sync_support , txt_custom0, txt_custom1, txt_custom2, txt_custom3, txt_custom4, "
			"int_custom0, int_custom1, int_custom2, int_custom3, int_custom4, txt_custom0 ) values "
			"(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ? , ?, ?)",	ACCOUNT_TABLE);

	hstmt = _account_prepare_query(query);
	ACCOUNT_RETURN_VAL((hstmt != NULL), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_prepare_query() failed(%s).\n", _account_db_err_msg()));

	_account_convert_account_to_sql(account, hstmt, query);

	rc = _account_query_step(hstmt);
	if (rc != SQLITE_DONE) {
		ACCOUNT_FATAL( "account_db_query_step() failed(%d, %s)", rc, _account_db_err_msg());
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
		ACCOUNT_ERROR( "_account_insert_capability, no capability\n");
		return ACCOUNT_ERROR_NONE;
	}

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) from %s where _id=%d", ACCOUNT_TABLE, account_id);

	rc = _account_get_record_count(query);

	if (rc <= 0) {
		ACCOUNT_WARNING( "_account_insert_capability : related account item is not existed rc=%d , %s", rc, _account_db_err_msg());
		return ACCOUNT_ERROR_RECORD_NOT_FOUND;
	}

	/* insert query*/

	GSList *iter;

	for (iter = account->capablity_list; iter != NULL; iter = g_slist_next(iter)) {
		int rc, ret;
		count = 1;

		account_capability_s* cap_data = NULL;
		cap_data = (account_capability_s*)iter->data;

		ACCOUNT_VERBOSE("cap_data->type = %s, cap_data->value = %d \n", cap_data->type, cap_data->value);

		ACCOUNT_MEMSET(query, 0x00, sizeof(query));
		ACCOUNT_SNPRINTF(query, sizeof(query), "INSERT INTO %s(key, value, package_name, user_name, account_id) VALUES "
				"(?, ?, ?, ?, ?) ", CAPABILITY_TABLE);
		hstmt = _account_prepare_query(query);

		ACCOUNT_RETURN_VAL((hstmt != NULL), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_prepare_query() failed(%s).\n", _account_db_err_msg()));

		ret = _account_query_bind_text(hstmt, count++, cap_data->type);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_int(hstmt, count++, cap_data->value);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Integer binding fail"));
		ret = _account_query_bind_text(hstmt, count++, (char*)account->package_name);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_text(hstmt, count++, (char*)account->user_name);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_int(hstmt, count++, (int)account_id);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Integer binding fail"));

		rc = _account_query_step(hstmt);

		if (rc != SQLITE_DONE) {
			ACCOUNT_FATAL( "_account_query_step() failed(%d, %s)", rc, _account_db_err_msg());
			break;
		}

		_account_query_finalize(hstmt);
		hstmt = NULL;

	}

	ACCOUNT_VERBOSE( "_account_insert_capability() DONE\n");

	return ACCOUNT_ERROR_NONE;
}

static int _account_update_capability(account_s *account, int account_id)
{
	int 			rc, count = 1;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	account_stmt 	hstmt = NULL;

	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));

	if (g_slist_length( account->capablity_list)==0) {
		ACCOUNT_ERROR( "_account_insert_capability, no capability\n");
		return ACCOUNT_ERROR_NONE;
	}

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) from %s where _id=%d", ACCOUNT_TABLE, account_id);

	rc = _account_get_record_count(query);

	if (rc <= 0) {
		ACCOUNT_WARNING( "_account_insert_capability : related account item is not existed rc=%d , %s", rc, _account_db_err_msg());
		return ACCOUNT_ERROR_RECORD_NOT_FOUND;
	}

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));

	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE account_id=? ", CAPABILITY_TABLE);
	hstmt = _account_prepare_query(query);
	count = 1;
	_account_query_bind_int(hstmt, count++, (int)account_id);
	rc = _account_query_step(hstmt);

	if (rc != SQLITE_DONE) {
		ACCOUNT_FATAL( "_account_query_step() failed(%d, %s)", rc, _account_db_err_msg());
		return ACCOUNT_ERROR_DB_FAILED;
	}
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

		ret = _account_query_bind_text(hstmt, count++, cap_data->type);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_int(hstmt, count++, cap_data->value);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Integer binding fail"));
		ret = _account_query_bind_text(hstmt, count++, (char*)account->package_name);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_text(hstmt, count++, (char*)account->user_name);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_int(hstmt, count++, (int)account_id);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Integer binding fail"));

		rc = _account_query_step(hstmt);

		if (rc != SQLITE_DONE) {
			ACCOUNT_FATAL( "_account_query_step() failed(%d, %s)", rc, _account_db_err_msg());
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
		ACCOUNT_ERROR( "_account_insert_capability, no capability\n");
		return ACCOUNT_ERROR_NONE;
	}

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) from %s where package_name=%s and user_name=%s", ACCOUNT_TABLE, package_name, user_name);

	rc = _account_get_record_count(query);

	if (rc <= 0) {
		ACCOUNT_WARNING( "_account_insert_capability : related account item is not existed rc=%d , %s ", rc, _account_db_err_msg());
		return ACCOUNT_ERROR_RECORD_NOT_FOUND;
	}

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));

	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE package_name=? and user_name=? ", CAPABILITY_TABLE);
	hstmt = _account_prepare_query(query);
	count = 1;
	_account_query_bind_text(hstmt, count++, (char*)account->package_name);
	_account_query_bind_text(hstmt, count++, (char*)account->user_name);
	rc = _account_query_step(hstmt);
	if (rc != SQLITE_DONE) {
		ACCOUNT_FATAL( "_account_query_step() failed(%d, %s)", rc, _account_db_err_msg());
		return ACCOUNT_ERROR_DB_FAILED;
	}

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

		ret = _account_query_bind_text(hstmt, count++, cap_data->type);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_int(hstmt, count++, cap_data->value);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Integer binding fail"));
		ret = _account_query_bind_text(hstmt, count++, (char*)account->package_name);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_text(hstmt, count++, (char*)account->user_name);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_int(hstmt, count++, (int)account->id);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Integer binding fail"));

		rc = _account_query_step(hstmt);

		if (rc != SQLITE_DONE) {
			ACCOUNT_FATAL( "_account_query_step() failed(%d, %s)", rc, _account_db_err_msg());
			break;
		}

		_account_query_finalize(hstmt);
		hstmt = NULL;

	}

	ACCOUNT_VERBOSE( "_account_insert_capability() DONE\n");

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

static void _account_db_data_to_text(const char *textbuf, char **output)
{
	if (textbuf && strlen(textbuf)>0) {
		if (*output) {
			free(*output);
			*output = NULL;
		}
		*output = strdup(textbuf);
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

	account_record->sync_support = _account_query_table_column_int(hstmt, ACCOUNT_FIELD_SYNC_SUPPORT);

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

	ACCOUNT_VERBOSE("END _account_convert_column_to_account");
}

static void _account_convert_column_to_capability(account_stmt hstmt, account_capability_s *capability_record)
{
	char *textbuf = NULL;

	capability_record->id = _account_query_table_column_int(hstmt, CAPABILITY_FIELD_ID);

	textbuf = _account_query_table_column_text(hstmt, CAPABILITY_FIELD_KEY);
	_account_db_data_to_text(textbuf, &(capability_record->type));

	capability_record->value = _account_query_table_column_int(hstmt, CAPABILITY_FIELD_VALUE);

	textbuf = _account_query_table_column_text(hstmt, CAPABILITY_FIELD_PACKAGE_NAME);
	_account_db_data_to_text(textbuf, &(capability_record->package_name));

	textbuf = _account_query_table_column_text(hstmt, CAPABILITY_FIELD_USER_NAME);
	_account_db_data_to_text(textbuf, &(capability_record->user_name));

	capability_record->account_id = _account_query_table_column_int(hstmt, CAPABILITY_FIELD_ACCOUNT_ID);

	ACCOUNT_VERBOSE("END _account_convert_column_to_capability");
}

static void _account_convert_column_to_custom(account_stmt hstmt, account_custom_s *custom_record)
{
	char *textbuf = NULL;

	custom_record->account_id = _account_query_table_column_int(hstmt, ACCOUNT_CUSTOM_FIELD_ACCOUNT_ID);

	textbuf = _account_query_table_column_text(hstmt, ACCOUNT_CUSTOM_FIELD_APP_ID);
	_account_db_data_to_text(textbuf, &(custom_record->app_id));

	textbuf = _account_query_table_column_text(hstmt, ACCOUNT_CUSTOM_FIELD_KEY);
	_account_db_data_to_text(textbuf, &(custom_record->key));

	textbuf = _account_query_table_column_text(hstmt, ACCOUNT_CUSTOM_FIELD_VALUE);
	_account_db_data_to_text(textbuf, &(custom_record->value));

	ACCOUNT_VERBOSE("END _account_convert_column_to_custom");
}

bool _account_get_capability_text_cb(const char* capability_type, account_capability_state_e capability_value, void *user_data)
{
	account_s *data = (account_s*)user_data;

	account_capability_s *cap_data = (account_capability_s*)malloc(sizeof(account_capability_s));

	if (cap_data == NULL)
		return FALSE;
	ACCOUNT_MEMSET(cap_data, 0, sizeof(account_capability_s));

	cap_data->type = _account_get_text(capability_type);
	cap_data->value = capability_value;

	data->capablity_list = g_slist_append(data->capablity_list, (gpointer)cap_data);

	ACCOUNT_VERBOSE("_account_get_capability_text_cb :: %s\n", capability_type);

	return TRUE;
}


bool _account_get_custom_text_cb(char* key, char* value, void *user_data)
{
	account_s *data = (account_s*)user_data;

	account_custom_s *custom_data = (account_custom_s*)malloc(sizeof(account_custom_s));

	if (custom_data == NULL) {
		ACCOUNT_DEBUG("_account_get_custom_text_cb :: malloc fail\n");
		return FALSE;
	}
	ACCOUNT_MEMSET(custom_data, 0, sizeof(account_custom_s));

	custom_data->account_id = data->id;
	custom_data->app_id = _account_get_text(data->package_name);
	custom_data->key = _account_get_text(key);
	custom_data->value = _account_get_text(value);

	data->custom_list = g_slist_append(data->custom_list, (gpointer)custom_data);

	return TRUE;
}


static char *_account_get_text(const char *text_data)
{
	char *text_value = NULL;

	if (text_data != NULL) {
		text_value = strdup(text_data);
		ACCOUNT_VERBOSE("text_value = %s", text_value);
	}
	return text_value;
}

static int _account_compare_old_record_by_user_name(account_s *new_account, const char* user_name, const char* package_name)
{
	int				error_code = ACCOUNT_ERROR_NONE;
	account_stmt 	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int				rc = 0;
	account_s *old_account = NULL;

	ACCOUNT_RETURN_VAL((new_account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT IS NULL"));
	ACCOUNT_RETURN_VAL((user_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("USER NAME IS NULL"));
	ACCOUNT_RETURN_VAL((package_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("PACKAGE NAME IS NULL"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));

	old_account = (account_s*)calloc(1, sizeof(account_s));
	if(!old_account) {
		ACCOUNT_FATAL("Memory alloc fail\n");
		return ACCOUNT_ERROR_OUT_OF_MEMORY;
	}

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT * FROM %s WHERE user_name = '%s' and package_name='%s'", ACCOUNT_TABLE, user_name, package_name);
	hstmt = _account_prepare_query(query);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_ROW, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	while (rc == SQLITE_ROW) {
		_account_convert_column_to_account(hstmt, old_account);
		ACCOUNT_VERBOSE("get account info by id %p\n", old_account);
		rc = _account_query_step(hstmt);
	}

	_account_query_finalize(hstmt);
	hstmt = NULL;

	// get capability
	error_code = account_query_capability_by_account_id(_account_get_capability_text_cb, old_account->id, (void*)old_account);
	ACCOUNT_CATCH_ERROR((error_code == ACCOUNT_ERROR_NONE), {}, error_code, ("account_query_capability_by_account_id error"));

	// get custom text
	error_code = _account_query_custom_by_account_id(_account_get_custom_text_cb, old_account->id, (void*)old_account);
	ACCOUNT_CATCH_ERROR((error_code == ACCOUNT_ERROR_NONE), {}, error_code, ("_account_query_custom_by_account_id error"));

	// compare
	new_account->id = old_account->id;

	//user name
	if(!new_account->user_name) {
		if(old_account->user_name)
			new_account->user_name = _account_get_text(old_account->user_name);
	}

	// display name
	if(!new_account->display_name) {
		if(old_account->display_name)
			new_account->display_name = _account_get_text(old_account->display_name);
	}

	// email address
	if(!new_account->email_address) {
		if(old_account->email_address)
			new_account->email_address = _account_get_text(old_account->email_address);
	}

	// domain name
	if(!new_account->domain_name) {
		if(old_account->domain_name)
			new_account->domain_name = _account_get_text(old_account->domain_name);
	}

	// icon path
	if(!new_account->icon_path) {
		if(old_account->icon_path)
			new_account->icon_path = _account_get_text(old_account->icon_path);
	}

	// source
	if(!new_account->source) {
		if(old_account->source)
			new_account->source = _account_get_text(old_account->source);
	}

#ifndef ACCOUNT_VERIFY
	// package name
	if(!new_account->package_name) {
		if(old_account->package_name)
			new_account->package_name = _account_get_text(old_account->package_name);
	}
#endif

	// access token
	if(!new_account->access_token) {
		if(old_account->access_token)
			new_account->access_token = _account_get_text(old_account->access_token);
	}

	// user text
	int i;
	for(i=0;i<USER_TXT_CNT;i++) {
		if(!new_account->user_data_txt[i]) {
			if(old_account->user_data_txt[i])
				new_account->user_data_txt[i] = _account_get_text(old_account->user_data_txt[i]);
		}
	}

	// auth type
	if(new_account->auth_type == ACCOUNT_AUTH_TYPE_INVALID) {
		new_account->auth_type = old_account->auth_type;
	}

	//secret
	if(new_account->secret== ACCOUNT_SECRECY_INVALID) {
		new_account->secret = old_account->secret;
	}

	// sync support
	if(new_account->sync_support == ACCOUNT_SYNC_INVALID) {
		new_account->sync_support = old_account->sync_support;
	}

	// user int
	for(i=0;i<USER_INT_CNT;i++) {
		if(new_account->user_data_int[i] == 0) {
				new_account->user_data_int[i] = old_account->user_data_int[i];
		}
	}

	// capability

	// user custom table



	CATCH:
		if (hstmt != NULL) {
			_account_query_finalize(hstmt);
			hstmt = NULL;
		}

		if (old_account) {
			_account_free_account_items(old_account);
			_ACCOUNT_FREE(old_account);
		}

	return ACCOUNT_ERROR_NONE;
}



static int _account_update_account_by_user_name(account_s *account, char *user_name, char *package_name)
{
	int				rc = 0, binding_count = 0, count = 0;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int				error_code = ACCOUNT_ERROR_NONE;
	account_stmt 	hstmt = NULL;

	ACCOUNT_RETURN_VAL((user_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("user_name is NULL.\n"));
	ACCOUNT_RETURN_VAL((package_name!= NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("package_name is NULL.\n"));

	_account_compare_old_record_by_user_name(account, user_name, package_name);

	if (!account->user_name && !account->display_name && !account->email_address) {
		ACCOUNT_ERROR("One field should be set among user name, display name, email address\n");
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}


	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) FROM %s WHERE user_name='%s' and package_name='%s'"
			, ACCOUNT_TABLE, user_name, package_name);

	count = _account_get_record_count(query);
	if (count <= 0) {
		ACCOUNT_INFO("_account_update_account_by_user_name : The account not exist!, count = %d, user_name=%s, package_name=%s\n",
			count, user_name, package_name);
		return ACCOUNT_ERROR_RECORD_NOT_FOUND;
	}

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "UPDATE %s SET user_name=?, email_address =?, display_name =?, "
			"icon_path =?, source =?, package_name =? , access_token =?, domain_name =?, auth_type =?, secret =?, sync_support =?,"
			"txt_custom0=?, txt_custom1=?, txt_custom2=?, txt_custom3=?, txt_custom4=?, "
			"int_custom0=?, int_custom1=?, int_custom2=?, int_custom3=?, int_custom4=? WHERE user_name=? and package_name=? ", ACCOUNT_TABLE);

	hstmt = _account_prepare_query(query);
	ACCOUNT_RETURN_VAL((hstmt != NULL), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_svc_query_prepare() failed(%s).\n", _account_db_err_msg()));

	binding_count = _account_convert_account_to_sql(account, hstmt, query);

	_account_query_bind_text(hstmt, binding_count++, user_name);
	_account_query_bind_text(hstmt, binding_count++, package_name);
	rc = _account_query_step(hstmt);
	if (rc != SQLITE_DONE) {
		ACCOUNT_FATAL( "account_db_query_step() failed(%d, %s)", rc, _account_db_err_msg());
	}
	_account_query_finalize(hstmt);
	hstmt = NULL;

	/*update capability*/
	error_code = _account_update_capability_by_user_name(account, user_name, package_name);

	/* update custom */
	error_code = _account_update_custom(account, account->id);

	return error_code;
}

ACCOUNT_API int account_insert_to_db(account_h account, int *account_id)
{
	int		error_code = ACCOUNT_ERROR_NONE;
	int 	ret_transaction = 0;

	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((account_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT ID POINTER IS NULL"));

	account_s *data = (account_s*)account;

	pthread_mutex_lock(&account_mutex);

	/* transaction control required*/
	ret_transaction = _account_begin_transaction();

	if (ret_transaction != ACCOUNT_ERROR_NONE) {
		ACCOUNT_ERROR("account_delete:_account_begin_transaction fail %d\n", ret_transaction);
		pthread_mutex_unlock(&account_mutex);
		return ret_transaction;
	}

	if (_account_check_duplicated(data)) {
		ret_transaction = _account_end_transaction(FALSE);
		ACCOUNT_ERROR("Duplicated, rollback insert query(%x)!!!!\n", ret_transaction);
		*account_id = -1;
		pthread_mutex_unlock(&account_mutex);
		return ACCOUNT_ERROR_DUPLICATED;
	} else {
		*account_id = _account_get_next_sequence(ACCOUNT_TABLE);

#ifdef ACCOUNT_VERIFY
		char*			appid = NULL;
		appid = _account_get_current_appid();

		if(appid) {
			//replace appid to account->package_name
			_ACCOUNT_FREE(data->package_name);
			data->package_name = _account_get_text(appid);
			_ACCOUNT_FREE(appid);
		}else {
			// API caller cannot be recognized
			ACCOUNT_ERROR("APP ID not detectable!\n");
			return ACCOUNT_ERROR_NOT_REGISTERED_PROVIDER;
		}
#else
		if (!account->package_name) {
			ACCOUNT_ERROR("Package name is mandetory field, it can not be NULL!!!!\n");
			return ACCOUNT_ERROR_INVALID_PARAMETER;
		}
#endif

		if(account_type_query_app_id_exist(data->package_name) != ACCOUNT_ERROR_NONE) {
			ret_transaction = _account_end_transaction(FALSE);
			ACCOUNT_ERROR("App id is not registered in account type DB, transaction ret (%x)!!!!\n", ret_transaction);
			pthread_mutex_unlock(&account_mutex);
			return ACCOUNT_ERROR_NOT_REGISTERED_PROVIDER;
		}

		if(!_account_check_add_more_account(data->package_name)) {
			ret_transaction = _account_end_transaction(FALSE);
			ACCOUNT_ERROR("No more account cannot be added, transaction ret (%x)!!!!\n", ret_transaction);
			pthread_mutex_unlock(&account_mutex);
			return ACCOUNT_ERROR_NOT_ALLOW_MULTIPLE;
		}

		error_code = _account_execute_insert_query(data);

		if (error_code != ACCOUNT_ERROR_NONE) {
			ret_transaction = _account_end_transaction(FALSE);
			ACCOUNT_ERROR("INSERT account fail, rollback insert query(%x)!!!!\n", ret_transaction);
			*account_id = -1;
			pthread_mutex_unlock(&account_mutex);
			return error_code;
		}
	}

	ACCOUNT_VERBOSE( "_account_execute_insert_query, insert error_code : %d", error_code);

	error_code = _account_insert_capability(data, *account_id);
	if (error_code != ACCOUNT_ERROR_NONE) {
		ret_transaction = _account_end_transaction(FALSE);
		ACCOUNT_ERROR("INSERT capability fail, rollback insert capability query(%x)!!!!\n", ret_transaction);
		*account_id = -1;
		pthread_mutex_unlock(&account_mutex);
		return error_code;
	}

	error_code = _account_insert_custom(data, *account_id);
	if (error_code != ACCOUNT_ERROR_NONE) {
		ret_transaction = _account_end_transaction(FALSE);
		ACCOUNT_ERROR("INSERT custom fail, rollback insert capability query(%x)!!!!\n", ret_transaction);
		*account_id = -1;
		pthread_mutex_unlock(&account_mutex);
		return error_code;
	}

	pthread_mutex_unlock(&account_mutex);

	char buf[64]={0,};
	ACCOUNT_SNPRINTF(buf, sizeof(buf), "%s:%d", ACCOUNT_NOTI_NAME_INSERT, *account_id);
	_account_insert_delete_update_notification_send(buf);

	_account_end_transaction(TRUE);

	return ACCOUNT_ERROR_NONE;

}

ACCOUNT_API int account_create(account_h *account)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)malloc(sizeof(account_s));

	if (data == NULL) {
		ACCOUNT_FATAL("Memory Allocation Failed");
		return ACCOUNT_ERROR_OUT_OF_MEMORY;
	}
	ACCOUNT_MEMSET(data, 0, sizeof(account_s));

	ACCOUNT_VERBOSE("create handle=%p\n", *account);

	/*Setting account as visible by default*/
	data->secret = ACCOUNT_SECRECY_VISIBLE;

	/*Setting account as not supporting sync by default*/
	data->sync_support = ACCOUNT_SYNC_NOT_SUPPORT;

	*account = (account_h)data;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_user_name(account_h account, const char *user_name)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!user_name) {
		ACCOUNT_ERROR("(%s)-(%d) user_name is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->user_name);
	data->user_name = _account_get_text(user_name);

	ACCOUNT_VERBOSE("(%s)-(%d) %s, %s\n", __FUNCTION__, __LINE__ , data->user_name, user_name);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_display_name(account_h account, const char *display_name)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!display_name) {
		ACCOUNT_ERROR("(%s)-(%d) display_name is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->display_name);
	data->display_name = _account_get_text(display_name);

	ACCOUNT_VERBOSE("(%s)-(%d) %s, %s\n", __FUNCTION__, __LINE__ , data->display_name, display_name);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_email_address(account_h account, const char *email_address)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!email_address) {
		ACCOUNT_ERROR("(%s)-(%d) email_address is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->email_address);
	data->email_address = _account_get_text(email_address);

	ACCOUNT_VERBOSE("(%s)-(%d) %s, %s\n", __FUNCTION__, __LINE__ , data->email_address, email_address);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_icon_path(account_h account, const char *icon_path)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!icon_path) {
		ACCOUNT_ERROR("(%s)-(%d) icon_path is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->icon_path);
	data->icon_path = _account_get_text(icon_path);

	ACCOUNT_VERBOSE("(%s)-(%d) %s, %s\n", __FUNCTION__, __LINE__ , data->icon_path, icon_path);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_source(account_h account, const char *source)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!source) {
		ACCOUNT_ERROR("(%s)-(%d) source is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->source);
	data->source = _account_get_text(source);

	ACCOUNT_VERBOSE("(%s)-(%d) %s, %s\n", __FUNCTION__, __LINE__ , data->source, source);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_package_name(account_h account, const char *package_name)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!package_name) {
		ACCOUNT_ERROR("(%s)-(%d) package_name is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->package_name);
	data->package_name = _account_get_text(package_name);

	ACCOUNT_VERBOSE("(%s)-(%d) %s, %s\n", __FUNCTION__, __LINE__ , data->package_name, package_name);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_domain_name(account_h account, const char *domain_name)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!domain_name) {
		ACCOUNT_ERROR("(%s)-(%d) domain_name is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->domain_name);
	data->domain_name = _account_get_text(domain_name);

	ACCOUNT_VERBOSE("(%s)-(%d) %s, %s\n", __FUNCTION__, __LINE__ , data->domain_name, domain_name);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_access_token(account_h account, const char *access_token)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!access_token) {
		ACCOUNT_ERROR("(%s)-(%d) access_token is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->access_token);
	data->access_token = _account_get_text(access_token);

	ACCOUNT_VERBOSE("(%s)-(%d) %s, %s\n", __FUNCTION__, __LINE__ , data->access_token, access_token);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_user_text(account_h account, int index, const char *user_txt)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!user_txt) {
		ACCOUNT_ERROR("(%s)-(%d) user_txt is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	if (index >= USER_TXT_CNT || index < 0) {
		ACCOUNT_ERROR("(%s)-(%d) index rage should be between 0-4.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->user_data_txt[index]);
	data->user_data_txt[index] = _account_get_text(user_txt);

	ACCOUNT_VERBOSE("(%s)-(%d) %s, %s\n", __FUNCTION__, __LINE__ , data->user_data_txt[index], user_txt);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_custom(account_h account, const char* key, const char* value)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!key) {
		ACCOUNT_ERROR("(%s)-(%d) key is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!value) {
		ACCOUNT_ERROR("(%s)-(%d) value is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	GSList *iter;
	bool b_is_new = TRUE;

	for (iter = data->custom_list; iter != NULL; iter = g_slist_next(iter)) {

		account_custom_s* custom_data = NULL;
		custom_data = (account_custom_s*)iter->data;
		ACCOUNT_VERBOSE( "account_custom_s->key = %s, account_custom_s->value = %s \n", custom_data->key, custom_data->value);

		if(!strcmp(custom_data->key, key)) {
			_ACCOUNT_FREE(custom_data->value);
			custom_data->value = _account_get_text(value);
			b_is_new = FALSE;
		}
	}

	if(b_is_new) {
		account_custom_s* custom_data = (account_custom_s*)malloc(sizeof(account_custom_s));

		if (custom_data == NULL) {
			ACCOUNT_FATAL("(%s)-(%d) MALLOC FAIL\n", __FUNCTION__, __LINE__);
			return ACCOUNT_ERROR_OUT_OF_MEMORY;
		}
		ACCOUNT_MEMSET(custom_data, 0, sizeof(account_custom_s));
		custom_data->account_id = data->id;
		custom_data->app_id = _account_get_text(data->package_name);
		custom_data->key = _account_get_text(key);
		custom_data->value = _account_get_text(value);
		data->custom_list = g_slist_append(data->custom_list, (gpointer)custom_data);
	}

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_auth_type(account_h account, const account_auth_type_e auth_type)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("(%s)-(%d) account handle is NULL.\n",  __FUNCTION__, __LINE__));

	if ( (auth_type < 0) || (auth_type > ACCOUNT_AUTH_TYPE_CLIENT_LOGIN)) {
		ACCOUNT_ERROR("(%s)-(%d) auth_type is less than 1 or more than enum max.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	data->auth_type = (int)auth_type;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_secret(account_h account, const account_secrecy_state_e secret)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("(%s)-(%d) account handle is NULL.\n",	__FUNCTION__, __LINE__));

	if ( (secret < 0) || (secret > ACCOUNT_SECRECY_VISIBLE)) {
		ACCOUNT_ERROR("(%s)-(%d) auth_type is less than 1 or more than enum max.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	data->secret = (int)secret;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_sync_support(account_h account, const account_sync_state_e sync_support)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("(%s)-(%d) account handle is NULL.\n",	__FUNCTION__, __LINE__));

	if ( (sync_support < 0) || (sync_support > ACCOUNT_SUPPORTS_SYNC)) {
		ACCOUNT_ERROR("(%s)-(%d) sync_support is less than 1 or more than enum max.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	data->sync_support= (int)sync_support;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_user_int(account_h account, int index, const int user_int)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (index >= USER_INT_CNT ||index < 0) {
		ACCOUNT_ERROR("(%s)-(%d) index rage should be between 0-4.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	data->user_data_int[index] = user_int;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_capability(account_h account, const char* capability_type, account_capability_state_e capability_value)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("account handle is null"));
	ACCOUNT_RETURN_VAL((capability_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("capability_type is null"));

	if ((capability_value != ACCOUNT_CAPABILITY_DISABLED) && (capability_value != ACCOUNT_CAPABILITY_ENABLED)) {
		ACCOUNT_ERROR("(%s)-(%d) capability_value is not equal to 0 or 1.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	GSList *iter = NULL;
	bool b_is_new = TRUE;

	for(iter = data->capablity_list; iter != NULL; iter = g_slist_next(iter)) {
		account_capability_s *cap_data = NULL;
		cap_data = (account_capability_s*)iter->data;

		if(!strcmp(cap_data->type, capability_type)) {
			cap_data->value = capability_value;
			b_is_new = FALSE;
			break;
		}
	}

	if(b_is_new) {
		account_capability_s* cap_data = (account_capability_s*)malloc(sizeof(account_capability_s));

		if (cap_data == NULL)
			return ACCOUNT_ERROR_OUT_OF_MEMORY;
		ACCOUNT_MEMSET(cap_data, 0, sizeof(account_capability_s));

		cap_data->type = _account_get_text(capability_type);
		cap_data->value = capability_value;
		data->capablity_list = g_slist_append(data->capablity_list, (gpointer)cap_data);
	}

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_user_name(account_h account, char **user_name)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!user_name) {
		ACCOUNT_ERROR("(%s)-(%d) user name is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*user_name) = NULL;
	*user_name = _account_get_text(data->user_name);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_display_name(account_h account, char **display_name)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!display_name) {
		ACCOUNT_ERROR("(%s)-(%d) display name is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*display_name) = NULL;

	*display_name = _account_get_text(data->display_name);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_email_address(account_h account,char **email_address)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!email_address) {
		ACCOUNT_ERROR("(%s)-(%d) email address is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*email_address) = NULL;

	*email_address = _account_get_text(data->email_address);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int  account_get_icon_path(account_h account, char **icon_path)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!icon_path) {
		ACCOUNT_ERROR("(%s)-(%d) icon path is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*icon_path) = NULL;

	*icon_path = _account_get_text(data->icon_path);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_source(account_h account, char **source)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!source) {
		ACCOUNT_ERROR("(%s)-(%d) source is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*source) = NULL;

	*source = _account_get_text(data->source);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_package_name(account_h account, char **package_name)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!package_name) {
		ACCOUNT_ERROR("(%s)-(%d) package name is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*package_name) = NULL;

	*package_name = _account_get_text(data->package_name);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_domain_name(account_h account, char **domain_name)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!domain_name) {
		ACCOUNT_ERROR("(%s)-(%d) domain name is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*domain_name) = NULL;

	*domain_name = _account_get_text(data->domain_name);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_access_token(account_h account, char **access_token)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!access_token) {
		ACCOUNT_ERROR("(%s)-(%d) access token is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*access_token) = NULL;

	*access_token = _account_get_text(data->access_token);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_user_text(account_h account, int user_text_index, char **text)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!text) {
		ACCOUNT_ERROR("(%s)-(%d) text is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	ACCOUNT_RETURN_VAL((user_text_index >=0 && user_text_index < USER_TXT_CNT ), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("INVALID USER TEXT INDEX"));

	account_s *data = (account_s*)account;

	(*text) = NULL;

	*text = _account_get_text(data->user_data_txt[user_text_index]);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_auth_type(account_h account, account_auth_type_e *auth_type)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	if (!auth_type) {
		ACCOUNT_ERROR("(%s)-(%d) auth_type is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s* data = (account_s*)account;

	*auth_type = data->auth_type;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_secret(account_h account, account_secrecy_state_e *secret)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	if (!secret) {
		ACCOUNT_ERROR("(%s)-(%d) secret is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s* data = (account_s*)account;

	*secret = data->secret;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_sync_support(account_h account, account_sync_state_e *sync_support)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	if (!sync_support) {
		ACCOUNT_ERROR("(%s)-(%d) sync_support is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s* data = (account_s*)account;

	*sync_support = data->sync_support;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_account_id(account_h account, int *account_id)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	if (!account_id) {
		ACCOUNT_ERROR("(%s)-(%d) account_id is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	*account_id = data->id;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_user_int(account_h account, int user_int_index, int *integer)
{
	if (!account) {
		ACCOUNT_ERROR("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	ACCOUNT_RETURN_VAL((user_int_index >=0 && user_int_index < USER_TXT_CNT ), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("INVALID USER TEXT INDEX"));

	if (!integer) {
		ACCOUNT_ERROR("(%s)-(%d) integer is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	*integer = data->user_data_int[user_int_index];

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_capability(account_h account, const char* capability_type, account_capability_state_e* capability_value)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((capability_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("capability_type is NULL"));
	ACCOUNT_RETURN_VAL((capability_value != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("capability_value is NULL"));

	GSList *iter;
	account_s *data = (account_s*)account;

	for (iter = data->capablity_list; iter != NULL; iter = g_slist_next(iter)) {
		account_capability_s *cap_data = NULL;

		cap_data = (account_capability_s*)iter->data;

		ACCOUNT_VERBOSE("account_get_capability :: type = %d, value = %d", cap_data->type, cap_data->value);

		if(!strcmp(capability_type, cap_data->type)) {
			*capability_value = cap_data->value;
			return ACCOUNT_ERROR_NONE;
		}
	}

	return ACCOUNT_ERROR_RECORD_NOT_FOUND;
}

ACCOUNT_API int account_get_capability_all(account_h account, capability_cb cb_func, void *user_data)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((cb_func != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("NO CALLBACK FUNCTION"));

	GSList *iter;
	account_s *data = (account_s*)account;

	for (iter = data->capablity_list; iter != NULL; iter = g_slist_next(iter)) {
		account_capability_s *cap_data = NULL;

		cap_data = (account_capability_s*)iter->data;

		ACCOUNT_VERBOSE("account_get_capability :: type = %d, value = %d", cap_data->type, cap_data->value);

		cb_func(cap_data->type, cap_data->value, user_data);
	}

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_custom(account_h account, const char* key, char** value)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((key != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("NO KEY TO REQUEST"));
	ACCOUNT_RETURN_VAL((value != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("VALUE POINTER IS NULL"));

	GSList *iter;
	account_s *data = (account_s*)account;

	for (iter = data->custom_list; iter != NULL; iter = g_slist_next(iter)) {
		account_custom_s *custom_data = NULL;

		custom_data = (account_custom_s*)iter->data;

		ACCOUNT_VERBOSE("account_get_custom ::  key = %s, value = %s", custom_data->key, custom_data->value);

		if(!strcmp(key, custom_data->key)) {
			(*value) = NULL;
			*value = _account_get_text(custom_data->value);
			return ACCOUNT_ERROR_NONE;
		}
	}

	ACCOUNT_INFO("key is not found %s", key);

	return ACCOUNT_ERROR_RECORD_NOT_FOUND;
}

ACCOUNT_API int account_get_custom_all(account_h account, account_custom_cb cb_func, void* user_data)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((cb_func != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("NO CALLBACK FUNCTION"));

	GSList *iter;
	account_s *data = (account_s*)account;

	for (iter = data->custom_list; iter != NULL; iter = g_slist_next(iter)) {
		bool cb_ret = FALSE;
		account_custom_s *custom_data = NULL;

		custom_data = (account_custom_s*)iter->data;

		ACCOUNT_VERBOSE("account_get_custom ::  key = %s, value = %s", custom_data->key, custom_data->value);

		cb_ret = cb_func(custom_data->key, custom_data->value, user_data);
		if(!cb_ret) {
			ACCOUNT_INFO("account_get_custom_all callback func ret = %d", cb_ret);
			break;
	}
	}

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_query_capability_by_account_id(capability_cb cb_func, int account_id, void *user_data )
{
	int 			error_code = ACCOUNT_ERROR_NONE;
	account_stmt	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0;
	int ret = ACCOUNT_ERROR_NONE;

	ACCOUNT_RETURN_VAL((account_id > 0), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT INDEX IS LESS THAN 0"));
	ACCOUNT_RETURN_VAL((cb_func != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("NO CALLBACK FUNCTION"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));

	ret = _do_account_owner_existance_check();
	if (ret != ACCOUNT_ERROR_NONE) {
		ACCOUNT_ERROR("_do_account_owner_existance_check Failed !!!\n");
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
			ACCOUNT_FATAL("malloc Failed");
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

static int _account_compare_old_record(account_s *new_account, int account_id)
{
	int				error_code = ACCOUNT_ERROR_NONE;
	account_stmt 	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int				rc = 0;
	account_s *old_account = NULL;

	ACCOUNT_RETURN_VAL((account_id > 0), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT INDEX IS LESS THAN 0"));
	ACCOUNT_RETURN_VAL((new_account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT IS NULL"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));

	old_account = (account_s*)calloc(1, sizeof(account_s));

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT * FROM %s WHERE _id = %d", ACCOUNT_TABLE, account_id);
	hstmt = _account_prepare_query(query);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_ROW, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	while (rc == SQLITE_ROW) {
		_account_convert_column_to_account(hstmt, old_account);
		ACCOUNT_VERBOSE("get account info by id %p\n", old_account);
		rc = _account_query_step(hstmt);
	}

	_account_query_finalize(hstmt);
	hstmt = NULL;

	// get capability
	error_code = account_query_capability_by_account_id(_account_get_capability_text_cb, old_account->id, (void*)old_account);
	ACCOUNT_CATCH_ERROR((error_code == ACCOUNT_ERROR_NONE), {}, error_code, ("account_query_capability_by_account_id error"));

	// get custom text
	error_code = _account_query_custom_by_account_id(_account_get_custom_text_cb, old_account->id, (void*)old_account);
	ACCOUNT_CATCH_ERROR((error_code == ACCOUNT_ERROR_NONE), {}, error_code, ("_account_query_custom_by_account_id error"));

	// compare

	new_account->id = old_account->id;

	//user name
	if(!new_account->user_name) {
		if(old_account->user_name)
			new_account->user_name = _account_get_text(old_account->user_name);
	}

	// display name
	if(!new_account->display_name) {
		if(old_account->display_name)
			new_account->display_name = _account_get_text(old_account->display_name);
	}

	// email address
	if(!new_account->email_address) {
		if(old_account->email_address)
			new_account->email_address = _account_get_text(old_account->email_address);
	}

	// domain name
	if(!new_account->domain_name) {
		if(old_account->domain_name)
			new_account->domain_name = _account_get_text(old_account->domain_name);
	}

	// icon path
	if(!new_account->icon_path) {
		if(old_account->icon_path)
			new_account->icon_path = _account_get_text(old_account->icon_path);
	}

	// source
	if(!new_account->source) {
		if(old_account->source)
			new_account->source = _account_get_text(old_account->source);
	}

#ifndef ACCOUNT_VERIFY
	// package name
	if(!new_account->package_name) {
		if(old_account->package_name)
			new_account->package_name = _account_get_text(old_account->package_name);
	}
#endif

	// access token
	if(!new_account->access_token) {
		if(old_account->access_token)
			new_account->access_token = _account_get_text(old_account->access_token);
	}

	// user text
	int i;
	for(i=0;i<USER_TXT_CNT;i++) {
		if(!new_account->user_data_txt[i]) {
			if(old_account->user_data_txt[i])
				new_account->user_data_txt[i] = _account_get_text(old_account->user_data_txt[i]);
		}
	}

	// auth type
	if(new_account->auth_type == ACCOUNT_AUTH_TYPE_INVALID) {
		new_account->auth_type = old_account->auth_type;
	}

	//secret
	if(new_account->secret== ACCOUNT_SECRECY_INVALID) {
		new_account->secret = old_account->secret;
	}

	// sync support
	if(new_account->sync_support == ACCOUNT_SYNC_INVALID) {
		new_account->sync_support = old_account->sync_support;
	}

	// user int
	for(i=0;i<USER_INT_CNT;i++) {
		if(new_account->user_data_int[i] == 0) {
				new_account->user_data_int[i] = old_account->user_data_int[i];
		}
	}

	// capability

	// user custom table



	CATCH:
		if (hstmt != NULL) {
			_account_query_finalize(hstmt);
			hstmt = NULL;
		}

		if (old_account) {
			_account_free_account_items(old_account);
			_ACCOUNT_FREE(old_account);
		}

	return ACCOUNT_ERROR_NONE;
}

static int _account_update_account(account_s *account, int account_id)
{
	int				rc = 0, binding_count =0;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int				error_code = ACCOUNT_ERROR_NONE, count=0, ret_transaction = 0;
	account_stmt 	hstmt = NULL;

#ifdef ACCOUNT_VERIFY
	char* appid = NULL;

	_ACCOUNT_FREE(account->package_name);
	appid = _account_get_current_appid();
	account->package_name = _account_get_text(appid);
	_ACCOUNT_FREE(appid);

	/* Check permission of requested appid */
	if(!_account_verify_permission(account->package_name)) {
		ACCOUNT_ERROR("No permission to update\n");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}
#endif

	_account_compare_old_record(account, account_id);

	if (!account->package_name) {
		ACCOUNT_ERROR("Package name is mandetory field, it can not be NULL!!!!\n");
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}


	if(account_type_query_app_id_exist(account->package_name) != ACCOUNT_ERROR_NONE) {
		ACCOUNT_ERROR("App id is not registered in account type DB!!!!\n");
		return ACCOUNT_ERROR_NOT_REGISTERED_PROVIDER;
	}

	if (!account->user_name && !account->display_name && !account->email_address) {
		ACCOUNT_ERROR("One field should be set among user name, display name, email address\n");
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) FROM %s WHERE _id = %d ", ACCOUNT_TABLE, account_id);

	count = _account_get_record_count(query);
	if (count <= 0) {
		ACCOUNT_WARNING(" Account record not found, count = %d\n", count);
		return ACCOUNT_ERROR_RECORD_NOT_FOUND;
	}

	/* transaction control required*/
	ret_transaction = _account_begin_transaction();

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "UPDATE %s SET user_name=?, email_address =?, display_name =?, "
			"icon_path =?, source =?, package_name =? , access_token =?, domain_name =?, auth_type =?, secret =?, sync_support =?,"
			"txt_custom0=?, txt_custom1=?, txt_custom2=?, txt_custom3=?, txt_custom4=?, "
			"int_custom0=?, int_custom1=?, int_custom2=?, int_custom3=?, int_custom4=? WHERE _id=? ", ACCOUNT_TABLE);

	hstmt = _account_prepare_query(query);
	ACCOUNT_RETURN_VAL((hstmt != NULL), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_svc_query_prepare() failed(%s)(%x).\n", _account_db_err_msg(), _account_end_transaction(FALSE)));

	binding_count = _account_convert_account_to_sql(account, hstmt, query);
	_account_query_bind_int(hstmt, binding_count++, account_id);

	rc = _account_query_step(hstmt);
	if (rc != SQLITE_DONE) {
		ACCOUNT_DEBUG( "account_db_query_step() failed(%d, %s)", rc, _account_db_err_msg());
	}

	_account_query_finalize(hstmt);
	hstmt = NULL;

	/*update capability*/
	error_code = _account_update_capability(account, account_id);
	if(error_code != ACCOUNT_ERROR_NONE && error_code!= ACCOUNT_ERROR_RECORD_NOT_FOUND){
		ret_transaction = _account_end_transaction(FALSE);
		ACCOUNT_ERROR("update capability Failed, trying to roll back(%x) !!!\n", ret_transaction);
		return error_code;
	}

	/* update custom */
	error_code = _account_update_custom(account, account_id);
	if(error_code != ACCOUNT_ERROR_NONE && error_code!= ACCOUNT_ERROR_RECORD_NOT_FOUND){
		ret_transaction = _account_end_transaction(FALSE);
		ACCOUNT_ERROR("update capability Failed, trying to roll back(%x) !!!\n", ret_transaction);
		return error_code;
	}

	ret_transaction = _account_end_transaction(TRUE);
	ACCOUNT_DEBUG("update capability success, trying to commit(%x) !!!\n", ret_transaction);

	return error_code;
}

ACCOUNT_API int account_update_to_db_by_id(const account_h account, int account_id)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("DATA IS NULL"));
	ACCOUNT_RETURN_VAL((account_id > 0), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Account id is not valid"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));
	int	error_code = ACCOUNT_ERROR_NONE;
	account_s* data = (account_s*)account;

	pthread_mutex_lock(&account_mutex);

	error_code = _account_update_account(data, account_id);

	if(error_code != ACCOUNT_ERROR_NONE) {
		pthread_mutex_unlock(&account_mutex);
		return error_code;
	}

	pthread_mutex_unlock(&account_mutex);

	char buf[64]={0,};
	ACCOUNT_SNPRINTF(buf, sizeof(buf), "%s:%d", ACCOUNT_NOTI_NAME_UPDATE, account_id);
	_account_insert_delete_update_notification_send(buf);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_update_to_db_by_user_name(account_h account, const char *user_name, const char *package_name)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("DATA IS NULL"));
	ACCOUNT_RETURN_VAL((user_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("USER NAME IS NULL"));
	ACCOUNT_RETURN_VAL((package_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("PACKAGE NAME IS NULL"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));

	int	error_code = ACCOUNT_ERROR_NONE;
	account_s *data = (account_s*)account;

	pthread_mutex_lock(&account_mutex);

	error_code = _account_update_account_by_user_name(data, (char*)user_name, (char*)package_name);

	pthread_mutex_unlock(&account_mutex);

	char buf[64]={0,};
	ACCOUNT_SNPRINTF(buf, sizeof(buf), "%s:%d", ACCOUNT_NOTI_NAME_UPDATE, data->id);
	_account_insert_delete_update_notification_send(buf);

	return error_code;
}

ACCOUNT_API int account_foreach_account_from_db(account_cb callback, void *user_data)
{
	int 			error_code = ACCOUNT_ERROR_NONE;
	account_stmt	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0;
	GList			*account_list = NULL;
	int ret = ACCOUNT_ERROR_NONE;

	ACCOUNT_RETURN_VAL((callback != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT INFO IS NULL"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));

	ret = _do_account_owner_existance_check();
	if (ret != ACCOUNT_ERROR_NONE) {
		ACCOUNT_WARNING("_do_account_owner_existance_check Failed !!!\n");
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
			ACCOUNT_FATAL("malloc Failed");
			break;
		}

		ACCOUNT_MEMSET(account_record, 0x00, sizeof(account_s));
		_account_convert_column_to_account(hstmt, account_record);
		account_list = g_list_append(account_list, account_record);
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
		_account_query_custom_by_account_id(_account_get_custom_text_cb, account->id, (void*)account);
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

ACCOUNT_API int account_update_sync_status_by_id(int account_db_id, const account_sync_state_e sync_status)
{
	int				error_code = ACCOUNT_ERROR_NONE;
	account_stmt 	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int				rc = 0;
	int count =1;

	ACCOUNT_RETURN_VAL((account_db_id > 0), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT INDEX IS LESS THAN 0"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));
	if ( (sync_status < 0) || (sync_status > ACCOUNT_SYNC_STATUS_RUNNING)) {
		ACCOUNT_ERROR("(%s)-(%d) sync_status is less than 0 or more than enum max.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	pthread_mutex_lock(&account_mutex);

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) from %s where _id=%d", ACCOUNT_TABLE, account_db_id);

	rc = _account_get_record_count(query);

	if (rc <= 0) {
		ACCOUNT_ERROR( "account_update_sync_status_by_id : related account item is not existed rc=%d , %s", rc, _account_db_err_msg());
		pthread_mutex_unlock(&account_mutex);
		return ACCOUNT_ERROR_RECORD_NOT_FOUND;
	}

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_SNPRINTF(query, sizeof(query), "UPDATE %s SET sync_support=? WHERE _id = %d", ACCOUNT_TABLE, account_db_id);
	hstmt = _account_prepare_query(query);

	_account_query_bind_int(hstmt, count, (int)sync_status);

	rc = _account_query_step(hstmt);

	ACCOUNT_CATCH_ERROR(rc == SQLITE_DONE, {}, ACCOUNT_ERROR_DB_FAILED,
				("account_db_query_step() failed(%d, %s)", rc, _account_db_err_msg()));

	_account_query_finalize(hstmt);

	char buf[64]={0,};
	ACCOUNT_SNPRINTF(buf, sizeof(buf), "%s:%d", ACCOUNT_NOTI_NAME_SYNC_UPDATE, account_db_id);
	_account_insert_delete_update_notification_send(buf);

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

ACCOUNT_API int account_query_account_by_account_id(int account_db_id, account_h *account)
{
	int				error_code = ACCOUNT_ERROR_NONE;
	account_stmt 	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int				rc = 0;
	int ret = ACCOUNT_ERROR_NONE;

	ACCOUNT_RETURN_VAL((account_db_id > 0), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT INDEX IS LESS THAN 0"));
	ACCOUNT_RETURN_VAL((*account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT IS NULL"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));

	ret = _do_account_owner_existance_check();
	if (ret != ACCOUNT_ERROR_NONE) {
		ACCOUNT_WARNING("_do_account_owner_existance_check Failed !!!\n");
	}

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT * FROM %s WHERE _id = %d", ACCOUNT_TABLE, account_db_id);
	hstmt = _account_prepare_query(query);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_ROW, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	account_s *account_record = (account_s *)(*account);

	while (rc == SQLITE_ROW) {
		_account_convert_column_to_account(hstmt, account_record);
		ACCOUNT_VERBOSE("get account info by id %p\n", account_record);
		rc = _account_query_step(hstmt);
	}

	_account_query_finalize(hstmt);
	account_query_capability_by_account_id(_account_get_capability_text_cb, account_record->id, (void*)account_record);
	_account_query_custom_by_account_id(_account_get_custom_text_cb, account_record->id, (void*)account_record);

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

ACCOUNT_API int account_query_account_by_user_name(account_cb cb_func, const char *user_name, void *user_data)
{
	int				error_code = ACCOUNT_ERROR_NONE;
	account_stmt 	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int				rc = 0;

	ACCOUNT_RETURN_VAL((user_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("USER NAME IS NULL"));
	ACCOUNT_RETURN_VAL((cb_func != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("CALL BACK IS NULL"));


	int ret = ACCOUNT_ERROR_NONE;


	ret = _do_account_owner_existance_check();
	if (ret != ACCOUNT_ERROR_NONE) {
		ACCOUNT_WARNING("_do_account_owner_existance_check Failed !!!\n");
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
		ACCOUNT_FATAL("malloc Failed");
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
			ACCOUNT_FATAL("malloc Failed");
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
	ACCOUNT_VERBOSE("GLIST LEN %d\n", tmp);

	for (iter = account_head->account_list; iter != NULL; iter = g_list_next(iter)) {
		account_h account;
		account = (account_h)iter->data;

		account_s *testaccount = (account_s*)account;

		ACCOUNT_VERBOSE("id = %d", testaccount->id);
		ACCOUNT_VERBOSE("user_name = %s", testaccount->user_name);
		ACCOUNT_VERBOSE("email_address = %s", testaccount->email_address);
		ACCOUNT_VERBOSE("display_name = %s", testaccount->display_name);
		ACCOUNT_VERBOSE("icon_path = %s", testaccount->icon_path);
		ACCOUNT_VERBOSE("source = %s", testaccount->source);
		ACCOUNT_VERBOSE("package_name = %s", testaccount->package_name);
		ACCOUNT_VERBOSE("access_token = %s", testaccount->access_token);
		ACCOUNT_VERBOSE("domain_name = %s", testaccount->domain_name);
		ACCOUNT_VERBOSE("auth_type = %d", testaccount->auth_type);
		ACCOUNT_VERBOSE("secret = %d", testaccount->secret);
		ACCOUNT_VERBOSE("sync_support = %d", testaccount->sync_support);

		account_query_capability_by_account_id(_account_get_capability_text_cb, testaccount->id, (void*)testaccount);
		_account_query_custom_by_account_id(_account_get_custom_text_cb, testaccount->id, (void*)testaccount);

		ACCOUNT_VERBOSE("capability_list address = %p", testaccount->capablity_list);

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
		}
		_ACCOUNT_FREE(account_head);
	}

	pthread_mutex_unlock(&account_mutex);
	return error_code;
}

ACCOUNT_API int account_query_account_by_capability(account_cb cb_func, const char* capability_type, account_capability_state_e capability_value, void* user_data)
{
	int 			error_code = ACCOUNT_ERROR_NONE;
	account_stmt	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0;
	int ret = ACCOUNT_ERROR_NONE;

	ACCOUNT_RETURN_VAL((capability_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("capability_type IS NULL"));

	if ((capability_value  < 0) || (capability_value > ACCOUNT_CAPABILITY_ENABLED)) {
		ACCOUNT_ERROR("(%s)-(%d) capability_value is not equal to 0 or 1.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	ACCOUNT_RETURN_VAL((cb_func != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("CALL BACK IS NULL"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));

	ret = _do_account_owner_existance_check();
	if (ret != ACCOUNT_ERROR_NONE) {
		ACCOUNT_WARNING("_do_account_owner_existance_check Failed !!!\n");
	}

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT * FROM %s WHERE _id IN (SELECT account_id from %s WHERE key=? AND value=?)", ACCOUNT_TABLE, CAPABILITY_TABLE);

	hstmt = _account_prepare_query(query);

	int binding_count = 1;
	_account_query_bind_text(hstmt, binding_count++, (char*)capability_type);
	_account_query_bind_int(hstmt, binding_count++, (int)capability_value);

	rc = _account_query_step(hstmt);

	account_s* account_head = NULL;

	ACCOUNT_CATCH_ERROR(rc == SQLITE_ROW, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	int tmp = 0;

	account_head = (account_s*) malloc(sizeof(account_s));
	if (account_head == NULL) {
		ACCOUNT_FATAL("malloc Failed");
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
			ACCOUNT_FATAL("malloc Failed");
			break;
		}
		ACCOUNT_MEMSET(account_record, 0x00, sizeof(account_s));

		_account_convert_column_to_account(hstmt, account_record);

		account_head->account_list = g_list_append(account_head->account_list, account_record);

		rc = _account_query_step(hstmt);
		tmp++;
		ACCOUNT_VERBOSE("DETECTED COUNT %d\n", tmp);
	}

	_account_query_finalize(hstmt);
	hstmt = NULL;

	GList *iter;


	tmp = g_list_length(account_head->account_list);
	ACCOUNT_VERBOSE("GLIST LEN %d\n", tmp);

	for (iter = account_head->account_list; iter != NULL; iter = g_list_next(iter)) {
		account_h account = NULL;
		account = (account_h)iter->data;
		account_s* testaccount = (account_s*)account;

		account_query_capability_by_account_id(_account_get_capability_text_cb, testaccount->id, (void*)testaccount);
		_account_query_custom_by_account_id(_account_get_custom_text_cb, testaccount->id, (void*)testaccount);

		ACCOUNT_VERBOSE("capability_list address = %p", testaccount->capablity_list);

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
		}
		_ACCOUNT_FREE(account_head);
	}

	pthread_mutex_unlock(&account_mutex);
	return error_code;
}

ACCOUNT_API int account_query_account_by_capability_type(account_cb cb_func, const char* capability_type, void* user_data)
{
	int 			error_code = ACCOUNT_ERROR_NONE;
	account_stmt	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0;
	int ret = ACCOUNT_ERROR_NONE;

	ACCOUNT_RETURN_VAL((capability_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("capability_type IS NULL"));
	ACCOUNT_RETURN_VAL((cb_func != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("CALL BACK IS NULL"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));

	ret = _do_account_owner_existance_check();
	if (ret != ACCOUNT_ERROR_NONE) {
		ACCOUNT_WARNING("_do_account_owner_existance_check Failed !!!\n");
	}

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT * FROM %s WHERE _id IN (SELECT account_id from %s WHERE key=?)", ACCOUNT_TABLE, CAPABILITY_TABLE);

	hstmt = _account_prepare_query(query);

	int binding_count = 1;
	_account_query_bind_text(hstmt, binding_count++, (char*)capability_type);

	rc = _account_query_step(hstmt);

	account_s* account_head = NULL;

	ACCOUNT_CATCH_ERROR(rc == SQLITE_ROW, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	int tmp = 0;

	account_head = (account_s*) malloc(sizeof(account_s));
	if (account_head == NULL) {
		ACCOUNT_FATAL("malloc Failed");
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
			ACCOUNT_FATAL("malloc Failed");
			break;
		}
		ACCOUNT_MEMSET(account_record, 0x00, sizeof(account_s));

		_account_convert_column_to_account(hstmt, account_record);

		account_head->account_list = g_list_append(account_head->account_list, account_record);

		rc = _account_query_step(hstmt);
		tmp++;
		ACCOUNT_VERBOSE("DETECTED COUNT %d\n", tmp);
	}

	_account_query_finalize(hstmt);
	hstmt = NULL;

	GList *iter;


	tmp = g_list_length(account_head->account_list);
	ACCOUNT_VERBOSE("GLIST LEN %d\n", tmp);

	for (iter = account_head->account_list; iter != NULL; iter = g_list_next(iter)) {
		account_h account = NULL;
		account = (account_h)iter->data;
		account_s* testaccount = (account_s*)account;

		account_query_capability_by_account_id(_account_get_capability_text_cb, testaccount->id, (void*)testaccount);
		_account_query_custom_by_account_id(_account_get_custom_text_cb, testaccount->id, (void*)testaccount);

		ACCOUNT_VERBOSE("capability_list address = %p", testaccount->capablity_list);

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
		}
		_ACCOUNT_FREE(account_head);
	}

	pthread_mutex_unlock(&account_mutex);
	return error_code;
}

ACCOUNT_API int account_query_account_by_package_name(account_cb cb_func, const char* package_name, void* user_data)
{
	int 			error_code = ACCOUNT_ERROR_NONE;
	account_stmt	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0;
	int ret = ACCOUNT_ERROR_NONE;

	ACCOUNT_RETURN_VAL((package_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("PACKAGE NAME IS NULL"));
	ACCOUNT_RETURN_VAL((cb_func != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("CALL BACK IS NULL"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));

	ret = _do_account_owner_existance_check();
	if (ret != ACCOUNT_ERROR_NONE) {
		ACCOUNT_ERROR("_do_account_owner_existance_check Failed !!!\n");
	}

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT * FROM %s WHERE package_name=?", ACCOUNT_TABLE);

	hstmt = _account_prepare_query(query);

	int binding_count = 1;
	_account_query_bind_text(hstmt, binding_count++, package_name);

	rc = _account_query_step(hstmt);

	account_s* account_head = NULL;

	ACCOUNT_CATCH_ERROR(rc == SQLITE_ROW, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.(%s)\n", package_name));

	int tmp = 0;

	account_head = (account_s*) malloc(sizeof(account_s));
	if (account_head == NULL) {
		ACCOUNT_FATAL("malloc Failed");
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
			ACCOUNT_FATAL("malloc Failed");
			break;
		}
		ACCOUNT_MEMSET(account_record, 0x00, sizeof(account_s));

		_account_convert_column_to_account(hstmt, account_record);

		account_head->account_list = g_list_append(account_head->account_list, account_record);

		rc = _account_query_step(hstmt);
		tmp++;
		ACCOUNT_VERBOSE("DETECTED COUNT %d\n", tmp);
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

		ACCOUNT_VERBOSE("id = %d", testaccount->id);
		ACCOUNT_VERBOSE("user_name = %s", testaccount->user_name);
		ACCOUNT_VERBOSE("email_address = %s", testaccount->email_address);
		ACCOUNT_VERBOSE("display_name = %s", testaccount->display_name);
		ACCOUNT_VERBOSE("icon_path = %s", testaccount->icon_path);
		ACCOUNT_VERBOSE("source = %s", testaccount->source);
		ACCOUNT_VERBOSE("package_name = %s", testaccount->package_name);
		ACCOUNT_VERBOSE("access_token = %s", testaccount->access_token);
		ACCOUNT_VERBOSE("domain_name = %s", testaccount->domain_name);
		ACCOUNT_VERBOSE("auth_type = %d", testaccount->auth_type);
		ACCOUNT_VERBOSE("secret = %d", testaccount->secret);
		ACCOUNT_VERBOSE("sync_support = %d", testaccount->sync_support);

		account_query_capability_by_account_id(_account_get_capability_text_cb, testaccount->id, (void*)testaccount);
		_account_query_custom_by_account_id(_account_get_custom_text_cb, testaccount->id, (void*)testaccount);

		ACCOUNT_VERBOSE("capability_list address = %p", testaccount->capablity_list);

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
		}
		_ACCOUNT_FREE(account_head);
	}

	pthread_mutex_unlock(&account_mutex);
	return error_code;
}

ACCOUNT_API int account_delete(int account_id)
{
	int				error_code = ACCOUNT_ERROR_NONE;
	account_stmt 	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int				rc = 0;
	int				ret_transaction = 0;
	bool			is_success = FALSE;

	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));

#ifdef ACCOUNT_VERIFY
	int count = -1;
	account_h		account = NULL;
	/* Check requested ID to delete */
	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) FROM %s WHERE _id=%d", ACCOUNT_TABLE, account_id);

	count = _account_get_record_count(query);
	if (count <= 0) {
		ACCOUNT_ERROR("account id(%d) is not exist. count(%d)\n", account_id, count);
		return ACCOUNT_ERROR_RECORD_NOT_FOUND;
	}

	/* Check permission of requested appid */
	account_create(&account);
	account_query_account_by_account_id(account_id, &account);

	if(!_account_verify_permission(((account_s*)account)->package_name)) {
		ACCOUNT_ERROR("No permission to delete\n");
		account_destroy(account);
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	account_destroy(account);
#endif

	/* transaction control required*/
	ret_transaction = _account_begin_transaction();

	if (ret_transaction != ACCOUNT_ERROR_NONE) {
		ACCOUNT_ERROR("account_delete:_account_begin_transaction fail %d\n", ret_transaction);
		pthread_mutex_unlock(&account_mutex);
		return ret_transaction;
	}

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));
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
	hstmt = NULL;

	/* delete custom data */
	ACCOUNT_MEMSET(query, 0, sizeof(query));

	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE AccountId = %d", ACCOUNT_CUSTOM_TABLE, account_id);

	hstmt = _account_prepare_query(query);
	ACCOUNT_CATCH_ERROR(hstmt != NULL, {}, ACCOUNT_ERROR_DB_FAILED,
			("_account_svc_query_prepare(%s) failed(%s).\n", query, _account_db_err_msg()));

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_DONE, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found. id=%d, rc=%d\n", account_id, rc));

	_account_query_finalize(hstmt);

	is_success = TRUE;

CATCH:
	if (hstmt != NULL) {
		_account_query_finalize(hstmt);
		hstmt = NULL;
	}

	ret_transaction = _account_end_transaction(is_success);

	if (ret_transaction != ACCOUNT_ERROR_NONE) {
		ACCOUNT_ERROR("account_svc_delete:_account_svc_end_transaction fail %d, is_success=%d\n", ret_transaction, is_success);
	} else {
		if (is_success == true) {
			char buf[64]={0,};
			ACCOUNT_SNPRINTF(buf, sizeof(buf), "%s:%d", ACCOUNT_NOTI_NAME_DELETE, account_id);
			_account_insert_delete_update_notification_send(buf);
		}
	}

	pthread_mutex_unlock(&account_mutex);

	return error_code;

}

ACCOUNT_API int account_delete_from_db_by_id(int account_db_id)
{
	ACCOUNT_RETURN_VAL((account_db_id > 0), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT ID IS LESS THAN ZERO."));

	return account_delete(account_db_id);
}

static int _account_query_account_by_username_and_package(const char* username, const char* package_name, account_h *account)
{
	int				error_code = ACCOUNT_ERROR_NONE;
	account_stmt 	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int				rc = 0;
	int 			ret = ACCOUNT_ERROR_NONE;
	int				binding_count = 1;

	ACCOUNT_RETURN_VAL((username != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("username IS NULL"));
	ACCOUNT_RETURN_VAL((package_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("package_name IS NULL"));
	ACCOUNT_RETURN_VAL((*account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT IS NULL"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));

	ret = _do_account_owner_existance_check();
	if (ret != ACCOUNT_ERROR_NONE) {
		ACCOUNT_WARNING("_do_account_owner_existance_check Failed !!!\n");
	}

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT * FROM %s WHERE user_name = ? and package_name = ?", ACCOUNT_TABLE);
	hstmt = _account_prepare_query(query);

	_account_query_bind_text(hstmt, binding_count++, username);
	_account_query_bind_text(hstmt, binding_count++, package_name);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_ROW, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	account_s *account_record = (account_s *)(*account);

	while (rc == SQLITE_ROW) {
		_account_convert_column_to_account(hstmt, account_record);
		ACCOUNT_VERBOSE("get account info by id %p\n", account_record);
		rc = _account_query_step(hstmt);
	}

	_account_query_finalize(hstmt);
	account_query_capability_by_account_id(_account_get_capability_text_cb, account_record->id, (void*)account_record);
	_account_query_custom_by_account_id(_account_get_custom_text_cb, account_record->id, (void*)account_record);

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

ACCOUNT_API int account_delete_from_db_by_user_name(char *user_name, char *package_name)
{
	int 			error_code = ACCOUNT_ERROR_NONE;
	account_stmt	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0;
	int 			ret_transaction = 0;
	bool			is_success = FALSE;
	account_h		account = NULL;
	int 			binding_count = 1;
	int				account_id = -1;

	ACCOUNT_RETURN_VAL((user_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("user_name is null!"));
	ACCOUNT_RETURN_VAL((package_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("package_name is null!"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));

	rc = account_create(&account);
	rc = _account_query_account_by_username_and_package(user_name, package_name, &account);

	rc = account_get_account_id(account, &account_id);

	rc = account_destroy(account);

	/* transaction control required*/
	ret_transaction = _account_begin_transaction();

	if (ret_transaction != ACCOUNT_ERROR_NONE) {
		ACCOUNT_ERROR("account_delete:_account_begin_transaction fail %d\n", ret_transaction);
		pthread_mutex_unlock(&account_mutex);
		return ret_transaction;
	}

	/* delete custom data */
	ACCOUNT_MEMSET(query, 0, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE AccountId = ?", ACCOUNT_CUSTOM_TABLE);

	hstmt = _account_prepare_query(query);

	ACCOUNT_CATCH_ERROR(hstmt != NULL, {}, ACCOUNT_ERROR_DB_FAILED,
			("_account_svc_query_prepare(%s) failed(%s).\n", query, _account_db_err_msg()));

	_account_query_bind_int(hstmt, binding_count++, account_id);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_DONE, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	_account_query_finalize(hstmt);
	hstmt = NULL;

	/* delete capability */
	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE user_name = ? and package_name = ?", CAPABILITY_TABLE);

	hstmt = _account_prepare_query(query);

	ACCOUNT_CATCH_ERROR(hstmt != NULL, {}, ACCOUNT_ERROR_DB_FAILED,
			("_account_svc_query_prepare(%s) failed(%s).\n", query, _account_db_err_msg()));

	binding_count = 1;
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
		ACCOUNT_ERROR("account_svc_delete:_account_svc_end_transaction fail %d, is_success=%d\n", ret_transaction, is_success);
	} else {
		if (is_success == true) {
			char buf[64]={0,};
			ACCOUNT_SNPRINTF(buf, sizeof(buf), "%s:%d", ACCOUNT_NOTI_NAME_DELETE, account_id);
			_account_insert_delete_update_notification_send(buf);
		}
	}

	pthread_mutex_unlock(&account_mutex);

	return error_code;
}

ACCOUNT_API int account_delete_from_db_by_package_name(char *package_name)
{
	int 			error_code = ACCOUNT_ERROR_NONE;
	account_stmt	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0;
	int 			ret_transaction = 0;
	bool			is_success = FALSE;
	int 			binding_count = 1;

	ACCOUNT_RETURN_VAL((package_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("package_name is null!"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));

	/* transaction control required*/
	ret_transaction = _account_begin_transaction();

	if (ret_transaction != ACCOUNT_ERROR_NONE) {
		ACCOUNT_ERROR("account_delete:_account_begin_transaction fail %d\n", ret_transaction);
		pthread_mutex_unlock(&account_mutex);
		return ret_transaction;
	}

	/* delete custom table  */
	ACCOUNT_MEMSET(query, 0, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE AppId = ?", ACCOUNT_CUSTOM_TABLE);

	hstmt = _account_prepare_query(query);

	ACCOUNT_CATCH_ERROR(hstmt != NULL, {}, ACCOUNT_ERROR_DB_FAILED,
			("_account_svc_query_prepare(%s) failed(%s).\n", query, _account_db_err_msg()));

	binding_count = 1;
	_account_query_bind_text(hstmt, binding_count++, package_name);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_DONE, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	_account_query_finalize(hstmt);
	hstmt = NULL;

	/* delete capability table */
	ACCOUNT_MEMSET(query, 0, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE package_name = ?", CAPABILITY_TABLE);

	hstmt = _account_prepare_query(query);

	ACCOUNT_CATCH_ERROR(hstmt != NULL, {}, ACCOUNT_ERROR_DB_FAILED,
			("_account_svc_query_prepare(%s) failed(%s).\n", query, _account_db_err_msg()));

	binding_count = 1;
	_account_query_bind_text(hstmt, binding_count++, package_name);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_DONE, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	_account_query_finalize(hstmt);
	hstmt = NULL;

	/* delete account table */
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
		ACCOUNT_ERROR("account_svc_delete:_account_svc_end_transaction fail %d, is_success=%d\n", ret_transaction, is_success);
	} else {
		if (is_success == true) {
			char buf[64]={0,};
			ACCOUNT_SNPRINTF(buf, sizeof(buf), "%s:%d", ACCOUNT_NOTI_NAME_DELETE, -1);
			_account_insert_delete_update_notification_send(buf);
		}
	}

	pthread_mutex_unlock(&account_mutex);

	return error_code;
}

ACCOUNT_API int account_get_total_count_from_db(int *count)
{
	if (!count) {
		ACCOUNT_ERROR("(%s)-(%d) count is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	int ret = ACCOUNT_ERROR_NONE;
	char query[1024] = {0, };
	ACCOUNT_MEMSET(query, 0x00, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "select count(*) from %s", ACCOUNT_TABLE);

	ret = _do_account_owner_existance_check();
	if (ret != ACCOUNT_ERROR_NONE) {
		ACCOUNT_WARNING("_do_account_owner_existance_check Failed !!!\n");
	}

	*count = _account_get_record_count(query);
	int rc = -1;
	int ncount = 0;
	account_stmt pStmt = NULL;

	assert(NULL != g_hAccountDB);
	rc = sqlite3_prepare_v2(g_hAccountDB, query, strlen(query), &pStmt, NULL);

	rc = sqlite3_step(pStmt);
	if (SQLITE_ROW != rc) {
		ACCOUNT_ERROR("[ERROR] sqlite3_step() failed\n");
		sqlite3_finalize(pStmt);
		return ACCOUNT_ERROR_RECORD_NOT_FOUND;
	}

	ncount = sqlite3_column_int(pStmt, 0);

	*count = ncount;

	ACCOUNT_VERBOSE("Number of account : %d", ncount);
	sqlite3_finalize(pStmt);

	if (ncount < 0) {
		ACCOUNT_ERROR("[ERROR] Number of account : %d, End", ncount);
		return ACCOUNT_ERROR_DB_FAILED;
	}

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_destroy(account_h account)
{
	account_s *data = (account_s*)account;

	ACCOUNT_RETURN_VAL((data != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Account handle is null!"));

	ACCOUNT_INFO("destroy handle=%p\n", data);

	_account_free_account_items(data);
	_ACCOUNT_FREE(data);

	return ACCOUNT_ERROR_NONE;
}

static int _account_type_free_label_items(label_s *data)
{
	_ACCOUNT_FREE(data->app_id);
	_ACCOUNT_FREE(data->label);
	_ACCOUNT_FREE(data->locale);

	return ACCOUNT_ERROR_NONE;
}

static int _account_type_free_feature_items(provider_feature_s *data)
{
	_ACCOUNT_FREE(data->app_id);
	_ACCOUNT_FREE(data->key);

	return ACCOUNT_ERROR_NONE;
}

static int _account_type_gslist_free(GSList* list)
{
	ACCOUNT_RETURN_VAL((list != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("GSlist is NULL"));

	GSList* iter;

	for (iter = list; iter != NULL; iter = g_slist_next(iter)) {
		label_s *label_data = (label_s*)iter->data;
		_account_type_free_label_items(label_data);
		_ACCOUNT_FREE(label_data);
	}

	g_slist_free(list);
	list = NULL;

	return ACCOUNT_ERROR_NONE;
}

static int _account_type_item_free(account_type_s *data)
{
	_ACCOUNT_FREE(data->app_id);
	_ACCOUNT_FREE(data->service_provider_id);
	_ACCOUNT_FREE(data->icon_path);
	_ACCOUNT_FREE(data->small_icon_path);

	return ACCOUNT_ERROR_NONE;
}

static int _account_type_glist_free(GList* list)
{
	ACCOUNT_RETURN_VAL((list != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Glist is NULL"));

	GList* iter;

	for (iter = list; iter != NULL; iter = g_list_next(iter)) {
		account_type_s *account_type_record = (account_type_s*)iter->data;
		_account_type_item_free(account_type_record);
		_ACCOUNT_FREE(account_type_record);
	}

	g_list_free(list);
	list = NULL;

	return ACCOUNT_ERROR_NONE;
}

static int _account_type_free_account_type_items(account_type_s *data)
{
	_account_type_item_free(data);

	_account_type_gslist_free(data->label_list);
	_account_type_glist_free(data->account_type_list);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_create(account_type_h *account_type)
{
	if (!account_type) {
		ACCOUNT_ERROR("(%s)-(%d) account type handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_type_s *data = (account_type_s*)malloc(sizeof(account_type_s));

	if (data == NULL) {
		ACCOUNT_ERROR("Memory Allocation Failed");
		return ACCOUNT_ERROR_OUT_OF_MEMORY;
	}

	ACCOUNT_MEMSET(data, 0, sizeof(account_type_s));

	ACCOUNT_VERBOSE("create handle=%p\n", *account_type);

	*account_type = (account_type_h)data;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_destroy(account_type_h account_type)
{
	account_type_s *data = (account_type_s*)account_type;

	ACCOUNT_RETURN_VAL((data != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Account type handle is null!"));

	ACCOUNT_VERBOSE("destroy handle=%p\n", data);

	_account_type_free_account_type_items(data);
	_ACCOUNT_FREE(data);

	return ACCOUNT_ERROR_NONE;
}

//app_id mandatory field
ACCOUNT_API int account_type_set_app_id(account_type_h account_type, const char *app_id)
{
	if (!account_type) {
		ACCOUNT_ERROR("(%s)-(%d) account_type handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!app_id) {
		ACCOUNT_ERROR("(%s)-(%d) app_id is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_type_s *data = (account_type_s*)account_type;

	_ACCOUNT_FREE(data->app_id);
	data->app_id = _account_get_text(app_id);

	ACCOUNT_VERBOSE("(%s)-(%d) %s, %s\n", __FUNCTION__, __LINE__ , data->app_id, app_id);
	return ACCOUNT_ERROR_NONE;
}

//service_provider_id mandatory field
ACCOUNT_API int account_type_set_service_provider_id(account_type_h account_type, const char *service_provider_id)
{
	if (!account_type) {
		ACCOUNT_ERROR("(%s)-(%d) account_type handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!service_provider_id) {
		ACCOUNT_ERROR("(%s)-(%d) service_provider_id is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_type_s *data = (account_type_s*)account_type;

	_ACCOUNT_FREE(data->service_provider_id);
	data->service_provider_id = _account_get_text(service_provider_id);

	ACCOUNT_VERBOSE("(%s)-(%d) %s, %s\n", __FUNCTION__, __LINE__ , data->service_provider_id, service_provider_id);
	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_set_icon_path(account_type_h account_type, const char *icon_path)
{
	if (!account_type) {
		ACCOUNT_ERROR("(%s)-(%d) account_type handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!icon_path) {
		ACCOUNT_ERROR("(%s)-(%d) icon_path is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_type_s *data = (account_type_s*)account_type;

	_ACCOUNT_FREE(data->icon_path);
	data->icon_path = _account_get_text(icon_path);

	ACCOUNT_VERBOSE("(%s)-(%d) %s, %s\n", __FUNCTION__, __LINE__ , data->icon_path, icon_path);
	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_set_small_icon_path(account_type_h account_type, const char *small_icon_path)
{
	if (!account_type) {
		ACCOUNT_ERROR("(%s)-(%d) account_type handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!small_icon_path) {
		ACCOUNT_ERROR("(%s)-(%d) small_icon_path is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_type_s *data = (account_type_s*)account_type;

	_ACCOUNT_FREE(data->small_icon_path);
	data->small_icon_path = _account_get_text(small_icon_path);

	ACCOUNT_VERBOSE("(%s)-(%d) %s, %s\n", __FUNCTION__, __LINE__ , data->small_icon_path, small_icon_path);
	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_set_multiple_account_support(account_type_h account_type, const bool multiple_account_support)
{
	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("(%s)-(%d) account handle is NULL.\n",  __FUNCTION__, __LINE__));

	account_type_s *data = (account_type_s*)account_type;

	data->multiple_account_support = multiple_account_support;

	return ACCOUNT_ERROR_NONE;
}

// unset?
ACCOUNT_API int account_type_set_label(account_type_h account_type, const char* label, const char* locale)
{
	if (!account_type) {
		ACCOUNT_ERROR("(%s)-(%d) account_type handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if(!label || !locale) {
		ACCOUNT_ERROR("(%s)-(%d) label(%p) or locale(%p) is NULL.\n", __FUNCTION__, __LINE__, label, locale);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_type_s *data = (account_type_s*)account_type;
	label_s *label_data = (label_s*)malloc(sizeof(label_s));

	if (label_data == NULL) {
		ACCOUNT_FATAL("(%s)-(%d) Malloc fail. label_data is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_OUT_OF_MEMORY;
	}
	ACCOUNT_MEMSET(label_data, 0, sizeof(label_s));

	label_data->label = _account_get_text(label);
	label_data->locale = _account_get_text(locale);

	data->label_list = g_slist_append(data->label_list, (gpointer)label_data);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_get_app_id(account_type_h account_type, char **app_id)
{
	if (!account_type) {
		ACCOUNT_ERROR("(%s)-(%d) account type handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!app_id) {
		ACCOUNT_ERROR("(%s)-(%d) app id is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_type_s *data = (account_type_s*)account_type;

	(*app_id) = NULL;
	*app_id = _account_get_text(data->app_id);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_get_service_provider_id(account_type_h account_type, char **service_provider_id)
{
	if (!account_type) {
		ACCOUNT_ERROR("(%s)-(%d) account type handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!service_provider_id) {
		ACCOUNT_ERROR("(%s)-(%d) service provider id is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_type_s *data = (account_type_s*)account_type;

	(*service_provider_id) = NULL;
	*service_provider_id = _account_get_text(data->service_provider_id);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_get_icon_path(account_type_h account_type, char **icon_path)
{
	if (!account_type) {
		ACCOUNT_ERROR("(%s)-(%d) account type handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!icon_path) {
		ACCOUNT_ERROR("(%s)-(%d) icon path is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_type_s *data = (account_type_s*)account_type;

	(*icon_path) = NULL;
	*icon_path = _account_get_text(data->icon_path);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_get_small_icon_path(account_type_h account_type, char **small_icon_path)
{
	if (!account_type) {
		ACCOUNT_ERROR("(%s)-(%d) account type handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!small_icon_path) {
		ACCOUNT_ERROR("(%s)-(%d) icon path is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_type_s *data = (account_type_s*)account_type;

	(*small_icon_path) = NULL;
	*small_icon_path = _account_get_text(data->small_icon_path);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_get_multiple_account_support(account_type_h account_type, int *multiple_account_support)
{
	if (!account_type) {
		ACCOUNT_ERROR("(%s)-(%d) account_type handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	if (!multiple_account_support) {
		ACCOUNT_ERROR("(%s)-(%d) multiple_account_support is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_type_s *data = (account_type_s*)account_type;

	*multiple_account_support = data->multiple_account_support;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_get_label(account_type_h account_type, account_label_cb cb_func, void *user_data)
{
	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((cb_func != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("NO CALLBACK FUNCTION"));

	GSList *iter;
	account_type_s *data = (account_type_s*)account_type;

	for (iter = data->label_list; iter != NULL; iter = g_slist_next(iter)) {
		label_s *label_data = NULL;

		label_data = (label_s*)iter->data;

		ACCOUNT_VERBOSE("account_type_get_label :: app_id=%s, label=%s, locale=%s", label_data->app_id, label_data->label, label_data->locale);

		cb_func(label_data->app_id, label_data->label, label_data->locale, user_data);
	}

	return ACCOUNT_ERROR_NONE;
}

static gboolean _account_type_check_duplicated(account_type_s *data)
{
	char query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int count = 0;

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) FROM %s WHERE AppId='%s'"
			, ACCOUNT_TYPE_TABLE, data->app_id);

	count = _account_get_record_count(query);
	if (count > 0) {
		ACCOUNT_VERBOSE("_account_check_duplicated : duplicated %d account(s) exist!, user_name=%s, domain_name=%s\n",
			count, data->app_id, data->service_provider_id);
		return TRUE;
	}

	return FALSE;
}

static int _account_type_convert_account_to_sql(account_type_s *account_type, account_stmt hstmt, char *sql_value)
{
	int count = 1;

	/*Caution : Keep insert query orders.*/

	/* 1. app id*/
	_account_query_bind_text(hstmt, count++, (char*)account_type->app_id);

	/* 2. service provider id*/
	_account_query_bind_text(hstmt, count++, (char*)account_type->service_provider_id);

	/* 3. icon path*/
	_account_query_bind_text(hstmt, count++, (char*)account_type->icon_path);

	/* 4. small icon path*/
	_account_query_bind_text(hstmt, count++, (char*)account_type->small_icon_path);

	/* 5. multiple accont support*/
	_account_query_bind_int(hstmt, count++, account_type->multiple_account_support);

	return count;
}


static int _account_type_execute_insert_query(account_type_s *account_type)
{
	int				rc = 0;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int				error_code = ACCOUNT_ERROR_NONE;
	account_stmt 	hstmt = NULL;

	/* check mandatory field */
	// app id & service provider id
	if (!account_type->app_id) {
		ACCOUNT_ERROR("App id is mandetory field, it can not be NULL!!!!\n");
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "INSERT INTO %s( AppId, ServiceProviderId , IconPath , SmallIconPath , MultipleAccountSupport ) values "
			"(?, ?, ?, ?, ?)",	ACCOUNT_TYPE_TABLE);

	hstmt = _account_prepare_query(query);
	ACCOUNT_RETURN_VAL((hstmt != NULL), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_prepare_query() failed(%s).\n", _account_db_err_msg()));

	_account_type_convert_account_to_sql(account_type, hstmt, query);

	rc = _account_query_step(hstmt);
	if (rc != SQLITE_DONE) {
		ACCOUNT_FATAL( "account_db_query_step() failed(%d, %s)", rc, _account_db_err_msg());
		error_code = ACCOUNT_ERROR_DB_FAILED;
	}

	_account_query_finalize(hstmt);
	hstmt = NULL;

	return error_code;
}

static int _account_type_insert_label(account_type_s *account_type)
{
	int 			rc, count = 1;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	account_stmt 	hstmt = NULL;

	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));

	if (g_slist_length( account_type->label_list)==0) {
		ACCOUNT_ERROR( "_account_type_insert_label, no label\n");
		return ACCOUNT_ERROR_NONE;
	}

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) from %s where AppId = '%s'", ACCOUNT_TYPE_TABLE, account_type->app_id);

	rc = _account_get_record_count(query);

	if (rc <= 0) {
		ACCOUNT_ERROR( "_account_insert_label : related account item is not existed rc=%d , %s", rc, _account_db_err_msg());
		return ACCOUNT_ERROR_RECORD_NOT_FOUND;
	}

	/* insert query*/
	GSList *iter;

	for (iter = account_type->label_list; iter != NULL; iter = g_slist_next(iter)) {
		int rc, ret;
		count = 1;
		ACCOUNT_MEMSET(query, 0x00, sizeof(query));
		ACCOUNT_SNPRINTF(query, sizeof(query), "INSERT INTO %s(AppId, Label, Locale) VALUES "
				"(?, ?, ?) ", LABEL_TABLE);

		hstmt = _account_prepare_query(query);

		ACCOUNT_RETURN_VAL((hstmt != NULL), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_prepare_query() failed(%s).\n", _account_db_err_msg()));

		label_s* label_data = NULL;
		label_data = (label_s*)iter->data;
		ACCOUNT_VERBOSE( "label_data->appid = %s, label_data->label = %s, label_data->locale \n", label_data->app_id, label_data->label, label_data->locale);

		ret = _account_query_bind_text(hstmt, count++, account_type->app_id);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_text(hstmt, count++, label_data->label);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_text(hstmt, count++, (char*)label_data->locale);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));

		rc = _account_query_step(hstmt);

		if (rc != SQLITE_DONE) {
			ACCOUNT_FATAL( "_account_query_step() failed(%d, %s)", rc, _account_db_err_msg());
			break;
		}

		_account_query_finalize(hstmt);
		hstmt = NULL;

	}

	ACCOUNT_VERBOSE( "_account_type_insert_label() DONE\n");

	return ACCOUNT_ERROR_NONE;
}

static void _account_type_convert_column_to_provider_feature(account_stmt hstmt, provider_feature_s *feature_record)
{
	char *textbuf = NULL;

	textbuf = _account_query_table_column_text(hstmt, PROVIDER_FEATURE_FIELD_APP_ID);
	_account_db_data_to_text(textbuf, &(feature_record->app_id));

	textbuf = _account_query_table_column_text(hstmt, PROVIDER_FEATURE_FIELD_KEY);
	_account_db_data_to_text(textbuf, &(feature_record->key));

	ACCOUNT_VERBOSE("END _account_type_convert_column_to_provider_feature");
}

ACCOUNT_API int account_type_query_provider_feature_by_app_id(provider_feature_cb cb_func, const char* app_id, void *user_data )
{
	int 			error_code = ACCOUNT_ERROR_NONE;
	account_stmt	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0, binding_count = 1;

	ACCOUNT_RETURN_VAL((app_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("APP ID IS NULL"));
	ACCOUNT_RETURN_VAL((cb_func != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("NO CALLBACK FUNCTION"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));

	ACCOUNT_VERBOSE("app_id = %s", app_id);

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT * FROM %s WHERE app_id = ?", PROVIDER_FEATURE_TABLE);
	hstmt = _account_prepare_query(query);

	_account_query_bind_text(hstmt, binding_count++, app_id);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_ROW, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	provider_feature_s* feature_record = NULL;

	while (rc == SQLITE_ROW) {
		feature_record = (provider_feature_s*) malloc(sizeof(provider_feature_s));

		if (feature_record == NULL) {
			ACCOUNT_FATAL("malloc Failed");
			break;
		}

		ACCOUNT_MEMSET(feature_record, 0x00, sizeof(provider_feature_s));

		_account_type_convert_column_to_provider_feature(hstmt, feature_record);

		cb_func(feature_record->app_id, feature_record->key, user_data);

		_account_type_free_feature_items(feature_record);
		_ACCOUNT_FREE(feature_record);

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

ACCOUNT_API bool account_type_query_supported_feature(const char* app_id, const char* capability)
{
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			record_count = 0;

	ACCOUNT_RETURN_VAL((app_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("APP ID IS NULL"));
	ACCOUNT_RETURN_VAL((capability != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("NO CAPABILITY"));

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) FROM %s where app_id='%s' and key='%s'", PROVIDER_FEATURE_TABLE, app_id, capability);

	record_count = _account_get_record_count(query);

	if (record_count <= 0) {
		ACCOUNT_WARNING( "related capability type item is not existed rc=%d , %s", record_count, _account_db_err_msg());
		return FALSE;
	}

	return TRUE;

}


ACCOUNT_API int account_type_get_provider_feature_all(account_type_h account_type, provider_feature_cb cb_func, void* user_data)
{
	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((cb_func != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("NO CALLBACK FUNCTION"));

	GSList *iter;
	account_type_s *data = (account_type_s*)account_type;

	for (iter = data->provider_feature_list; iter != NULL; iter = g_slist_next(iter)) {
		provider_feature_s *feature_data = NULL;

		feature_data = (provider_feature_s*)iter->data;

		ACCOUNT_VERBOSE("appid = %s, key = %s", feature_data->key, feature_data->app_id);

		cb_func(feature_data->app_id, feature_data->key, user_data);
	}

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_set_provider_feature(account_type_h account_type, const char* provider_feature)
{
	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("account type handle is null"));
	ACCOUNT_RETURN_VAL((provider_feature != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("provider_feature is null"));

	account_type_s *data = (account_type_s*)account_type;

	GSList *iter = NULL;
	bool b_is_new = TRUE;

	for(iter = data->provider_feature_list; iter != NULL; iter = g_slist_next(iter)) {
		provider_feature_s *feature_data = NULL;
		feature_data = (provider_feature_s*)iter->data;

		if(!strcmp(feature_data->key, provider_feature)) {
			b_is_new = FALSE;
			break;
		}
	}

	if(b_is_new) {
		provider_feature_s* feature_data = (provider_feature_s*)malloc(sizeof(provider_feature_s));

		if (feature_data == NULL)
			return ACCOUNT_ERROR_OUT_OF_MEMORY;
		ACCOUNT_MEMSET(feature_data, 0, sizeof(provider_feature_s));

		feature_data->key = _account_get_text(provider_feature);
		data->provider_feature_list = g_slist_append(data->provider_feature_list, (gpointer)feature_data);
	}

	return ACCOUNT_ERROR_NONE;
}

static int _account_type_insert_provider_feature(account_type_s *account_type, const char* app_id)
{
	int 			rc, count = 1;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	account_stmt 	hstmt = NULL;

	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((app_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("APP ID IS NULL"));

	if (g_slist_length( account_type->provider_feature_list)==0) {
		ACCOUNT_ERROR( "no capability\n");
		return ACCOUNT_ERROR_NONE;
	}

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) from %s where AppId='%s'", ACCOUNT_TYPE_TABLE, app_id);

	rc = _account_get_record_count(query);

	if (rc <= 0) {
		ACCOUNT_WARNING( "related account type item is not existed rc=%d , %s", rc, _account_db_err_msg());
		return ACCOUNT_ERROR_RECORD_NOT_FOUND;
	}

	/* insert query*/

	GSList *iter;

	for (iter = account_type->provider_feature_list; iter != NULL; iter = g_slist_next(iter)) {
		int rc, ret;
		count = 1;
		ACCOUNT_MEMSET(query, 0x00, sizeof(query));
		ACCOUNT_SNPRINTF(query, sizeof(query), "INSERT INTO %s(app_id, key) VALUES "
				"(?, ?) ", PROVIDER_FEATURE_TABLE);

		hstmt = _account_prepare_query(query);

		ACCOUNT_RETURN_VAL((hstmt != NULL), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_prepare_query() failed(%s).\n", _account_db_err_msg()));

		provider_feature_s* feature_data = NULL;
		feature_data = (provider_feature_s*)iter->data;
		ACCOUNT_VERBOSE("key = %s, app_id = %s \n", feature_data->key, app_id);

		ret = _account_query_bind_text(hstmt, count++, app_id);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_text(hstmt, count++, feature_data->key);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Integer binding fail"));

		rc = _account_query_step(hstmt);

		if (rc != SQLITE_DONE) {
			ACCOUNT_FATAL( "_account_query_step() failed(%d, %s)", rc, _account_db_err_msg());
			break;
		}

		_account_query_finalize(hstmt);
		hstmt = NULL;

	}

	ACCOUNT_VERBOSE( "_account_type_insert_provider_feature() DONE\n");

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_insert_to_db(account_type_h account_type, int* account_type_id)
{
	int		error_code = ACCOUNT_ERROR_NONE, ret_transaction = 0;

	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));
	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT TYPE HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((account_type_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT TYPE ID POINTER IS NULL"));

	account_type_s *data = (account_type_s*)account_type;

	pthread_mutex_lock(&account_mutex);


	/* transaction control required*/
	ret_transaction = _account_begin_transaction();

	if (ret_transaction != ACCOUNT_ERROR_NONE) {
		ACCOUNT_ERROR("_account_begin_transaction fail %d\n", ret_transaction);
		pthread_mutex_unlock(&account_mutex);
		return ret_transaction;
	}

	if (_account_type_check_duplicated(data)) {
		ret_transaction = _account_end_transaction(FALSE);
		ACCOUNT_ERROR("Duplicated, rollback insert query(%x)!!!!\n", ret_transaction);
		*account_type_id = -1;
		pthread_mutex_unlock(&account_mutex);
		return ACCOUNT_ERROR_DUPLICATED;
	} else {
		*account_type_id = _account_get_next_sequence(ACCOUNT_TYPE_TABLE);

		error_code = _account_type_execute_insert_query(data);

		if (error_code != ACCOUNT_ERROR_NONE){
			error_code = ACCOUNT_ERROR_DUPLICATED;
			ret_transaction = _account_end_transaction(FALSE);
			ACCOUNT_ERROR("Insert fail, rollback insert query(%x)!!!!\n", ret_transaction);
			*account_type_id = -1;
			pthread_mutex_unlock(&account_mutex);
			return error_code;
		}
	}

	ACCOUNT_INFO( "_account_type_execute_insert_query, insert error_code : %d", error_code);

	error_code = _account_type_insert_provider_feature(data, data->app_id);
	if(error_code != ACCOUNT_ERROR_NONE) {
		ret_transaction = _account_end_transaction(FALSE);
		ACCOUNT_ERROR("Insert provider feature fail(%x), rollback insert query(%x)!!!!\n", error_code, ret_transaction);
		pthread_mutex_unlock(&account_mutex);
		return error_code;
	}
	error_code = _account_type_insert_label(data);
	if(error_code != ACCOUNT_ERROR_NONE) {
		ret_transaction = _account_end_transaction(FALSE);
		ACCOUNT_ERROR("Insert label fail(%x), rollback insert query(%x)!!!!\n", error_code, ret_transaction);
		pthread_mutex_unlock(&account_mutex);
		return error_code;
	}

	ret_transaction = _account_end_transaction(TRUE);
	ACCOUNT_DEBUG("Insert success(%x), commit insert query(%x)!!!!\n", error_code, ret_transaction);

	pthread_mutex_unlock(&account_mutex);

	return ACCOUNT_ERROR_NONE;
}

static int _account_type_update_provider_feature(account_type_s *account_type, const char* app_id)
{
	int 			rc, count = 1;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	account_stmt	hstmt = NULL;

	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));

	if (g_slist_length( account_type->provider_feature_list)==0) {
		ACCOUNT_ERROR( "no feature\n");
		return ACCOUNT_ERROR_NONE;
	}

	ACCOUNT_DEBUG( "app id", app_id);

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));

	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE app_id=? ", PROVIDER_FEATURE_TABLE);
	hstmt = _account_prepare_query(query);
	count = 1;
	_account_query_bind_text(hstmt, count++, app_id);
	rc = _account_query_step(hstmt);

	if (rc != SQLITE_DONE) {
		ACCOUNT_FATAL( "_account_query_step() failed(%d, %s)", rc, _account_db_err_msg());
		return ACCOUNT_ERROR_DB_FAILED;
	}
	_account_query_finalize(hstmt);
	hstmt = NULL;

	GSList *iter;

	for (iter = account_type->provider_feature_list; iter != NULL; iter = g_slist_next(iter)) {
		int rc, ret;
		count = 1;
		ACCOUNT_MEMSET(query, 0x00, sizeof(query));
		ACCOUNT_SNPRINTF(query, sizeof(query), "INSERT INTO %s(app_id, key) VALUES "
				"(?, ?) ", PROVIDER_FEATURE_TABLE);

		hstmt = _account_prepare_query(query);

		ACCOUNT_RETURN_VAL((hstmt != NULL), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_prepare_query() failed(%s).\n", _account_db_err_msg()));

		provider_feature_s* feature_data = NULL;
		feature_data = (provider_feature_s*)iter->data;

		ret = _account_query_bind_text(hstmt, count++, account_type->app_id);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_text(hstmt, count++, feature_data->key);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));

		rc = _account_query_step(hstmt);

		if (rc != SQLITE_DONE) {
			ACCOUNT_FATAL( "_account_query_step() failed(%d, %s)", rc, _account_db_err_msg());
			break;
		}
		_account_query_finalize(hstmt);
		hstmt = NULL;
	}

	ACCOUNT_VERBOSE( "_account_type_update_label() DONE\n");

	return ACCOUNT_ERROR_NONE;
}

static int _account_type_update_label(account_type_s *account_type, const char* app_id)
{
	int 			rc, count = 1;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	account_stmt 	hstmt = NULL;

	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));

	if (g_slist_length( account_type->label_list)==0) {
		ACCOUNT_ERROR( "_account_type_update_label, no label\n");
		return ACCOUNT_ERROR_NONE;
	}

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));

	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE AppId=? ", LABEL_TABLE);
	hstmt = _account_prepare_query(query);
	count = 1;
	_account_query_bind_text(hstmt, count++, app_id);
	rc = _account_query_step(hstmt);

	if (rc != SQLITE_DONE) {
		ACCOUNT_FATAL( "_account_query_step() failed(%d, %s)", rc, _account_db_err_msg());
		return ACCOUNT_ERROR_DB_FAILED;
	}
	_account_query_finalize(hstmt);
	hstmt = NULL;

	GSList *iter;

	for (iter = account_type->label_list; iter != NULL; iter = g_slist_next(iter)) {
		int rc, ret;
		count = 1;
		ACCOUNT_MEMSET(query, 0x00, sizeof(query));
		ACCOUNT_SNPRINTF(query, sizeof(query), "INSERT INTO %s(AppId, Label, Locale) VALUES "
				"(?, ?, ?) ", LABEL_TABLE);

		hstmt = _account_prepare_query(query);

		ACCOUNT_RETURN_VAL((hstmt != NULL), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_prepare_query() failed(%s).\n", _account_db_err_msg()));

		label_s* label_data = NULL;
		label_data = (label_s*)iter->data;

		ret = _account_query_bind_text(hstmt, count++, account_type->app_id);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_text(hstmt, count++, label_data->label);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_text(hstmt, count++, label_data->locale);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));

		rc = _account_query_step(hstmt);

		if (rc != SQLITE_DONE) {
			ACCOUNT_FATAL( "_account_query_step() failed(%d, %s)", rc, _account_db_err_msg());
			break;
		}
		_account_query_finalize(hstmt);
		hstmt = NULL;
	}

	ACCOUNT_VERBOSE( "_account_type_update_label() DONE\n");

	return ACCOUNT_ERROR_NONE;
}


static int _account_type_update_account(account_type_s *account_type, const char* app_id)
{
	int				rc = 0, binding_count =1;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int				error_code = ACCOUNT_ERROR_NONE;
	account_stmt 	hstmt = NULL;

	if (!account_type->app_id) {
		ACCOUNT_ERROR("app id is mandetory field, it can not be NULL!!!!\n");
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "UPDATE %s SET AppId=?, ServiceProviderId=?, IconPath=?, "
			"SmallIconPath=?, MultipleAccountSupport=? WHERE AppId=? ", ACCOUNT_TYPE_TABLE);

	hstmt = _account_prepare_query(query);
	ACCOUNT_RETURN_VAL((hstmt != NULL), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_svc_query_prepare() failed(%s).\n", _account_db_err_msg()));

	binding_count = _account_type_convert_account_to_sql(account_type, hstmt, query);
	_account_query_bind_text(hstmt, binding_count++, app_id);

	rc = _account_query_step(hstmt);
	if (rc != SQLITE_DONE) {
		ACCOUNT_FATAL( "account_db_query_step() failed(%d, %s)", rc, _account_db_err_msg());
	}

	_account_query_finalize(hstmt);
	hstmt = NULL;

	/*update label*/
	error_code = _account_type_update_label(account_type, app_id);
	/* update provider feature */
	error_code = _account_type_update_provider_feature(account_type, app_id);

	return error_code;
}

ACCOUNT_API int account_type_update_to_db_by_app_id(account_type_h account_type, const char* app_id)
{
	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("DATA IS NULL"));
	ACCOUNT_RETURN_VAL((app_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("APP ID IS NULL"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));

	int	error_code = ACCOUNT_ERROR_NONE;
	account_type_s* data = (account_type_s*)account_type;

	pthread_mutex_lock(&account_mutex);

	error_code = _account_type_update_account(data, app_id);

	pthread_mutex_unlock(&account_mutex);

	return error_code;
}

ACCOUNT_API int account_type_delete_by_app_id(const char* app_id)
{
	int 			error_code = ACCOUNT_ERROR_NONE;
	account_stmt	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0, count = -1;
	int 			ret_transaction = 0;
	int				binding_count = 1;
	bool			is_success = FALSE;

	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));
	ACCOUNT_RETURN_VAL((app_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("The database isn't connected."));

	/* Check requested ID to delete */
	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) FROM %s WHERE AppId = '%s'", ACCOUNT_TYPE_TABLE, app_id);

	count = _account_get_record_count(query);
	if (count <= 0) {
		ACCOUNT_ERROR("app id(%s) is not exist. count(%d)\n", app_id, count);
		return ACCOUNT_ERROR_RECORD_NOT_FOUND;
	}

	/* transaction control required*/
	ret_transaction = _account_begin_transaction();

	if (ret_transaction != ACCOUNT_ERROR_NONE) {
		ACCOUNT_FATAL("account_delete:_account_begin_transaction fail %d\n", ret_transaction);
		pthread_mutex_unlock(&account_mutex);
		return ret_transaction;
	}

	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE AppId = ?", LABEL_TABLE);

	hstmt = _account_prepare_query(query);
	ACCOUNT_CATCH_ERROR(hstmt != NULL, {}, ACCOUNT_ERROR_DB_FAILED,
			("_account_svc_query_prepare(%s) failed(%s).\n", query, _account_db_err_msg()));

	_account_query_bind_text(hstmt, binding_count++, app_id);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_DONE, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	_account_query_finalize(hstmt);
	hstmt = NULL;

	binding_count = 1;
	ACCOUNT_MEMSET(query, 0, sizeof(query));

	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE app_id = ? ", PROVIDER_FEATURE_TABLE);

	hstmt = _account_prepare_query(query);
	ACCOUNT_CATCH_ERROR(hstmt != NULL, {}, ACCOUNT_ERROR_DB_FAILED,
			("_account_svc_query_prepare(%s) failed(%s).\n", query, _account_db_err_msg()));

	_account_query_bind_text(hstmt, binding_count++, app_id);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_DONE, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found. AppId=%s, rc=%d\n", app_id, rc));

	_account_query_finalize(hstmt);
	is_success = TRUE;

	hstmt = NULL;

	binding_count = 1;
	ACCOUNT_MEMSET(query, 0, sizeof(query));

	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE AppId = ? ", ACCOUNT_TYPE_TABLE);

	hstmt = _account_prepare_query(query);
	ACCOUNT_CATCH_ERROR(hstmt != NULL, {}, ACCOUNT_ERROR_DB_FAILED,
			("_account_svc_query_prepare(%s) failed(%s).\n", query, _account_db_err_msg()));

	_account_query_bind_text(hstmt, binding_count++, app_id);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_DONE, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found. AppId=%s, rc=%d\n", app_id, rc));

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
		ACCOUNT_FATAL("account_svc_delete:_account_svc_end_transaction fail %d, is_success=%d\n", ret_transaction, is_success);
	}

	pthread_mutex_unlock(&account_mutex);

	return error_code;
}

static void _account_type_convert_column_to_account_type(account_stmt hstmt, account_type_s *account_type_record)
{
	char   	*textbuf = NULL;

	account_type_record->id = _account_query_table_column_int(hstmt, ACCOUNT_TYPE_FIELD_ID);

	textbuf = _account_query_table_column_text(hstmt, ACCOUNT_TYPE_FIELD_APP_ID);
	_account_db_data_to_text(textbuf, &(account_type_record->app_id));

	textbuf = _account_query_table_column_text(hstmt, ACCOUNT_TYPE_FIELD_SERVICE_PROVIDER_ID);
	_account_db_data_to_text(textbuf, &(account_type_record->service_provider_id));

	textbuf = _account_query_table_column_text(hstmt, ACCOUNT_TYPE_FIELD_ICON_PATH);
	_account_db_data_to_text(textbuf, &(account_type_record->icon_path));

	textbuf = _account_query_table_column_text(hstmt, ACCOUNT_TYPE_FIELD_SMALL_ICON_PATH);
	_account_db_data_to_text(textbuf, &(account_type_record->small_icon_path));

	account_type_record->multiple_account_support = _account_query_table_column_int(hstmt, ACCOUNT_TYPE_FIELD_MULTIPLE_ACCOUNT_SUPPORT);

	ACCOUNT_VERBOSE("END _account_type_convert_column_to_account_type");
}

static void _account_type_convert_column_to_label(account_stmt hstmt, label_s *label_record)
{
	char *textbuf = NULL;

	textbuf = _account_query_table_column_text(hstmt, LABEL_FIELD_APP_ID);
	_account_db_data_to_text(textbuf, &(label_record->app_id));

	textbuf = _account_query_table_column_text(hstmt, LABEL_FIELD_LABEL);
	_account_db_data_to_text(textbuf, &(label_record->label));

	textbuf = _account_query_table_column_text(hstmt, LABEL_FIELD_LOCALE);
	_account_db_data_to_text(textbuf, &(label_record->locale));

	ACCOUNT_VERBOSE("END _account_type_convert_column_to_label");
}

ACCOUNT_API int account_type_query_label_by_app_id(account_label_cb cb_func, const char* app_id, void *user_data )
{
	int 			error_code = ACCOUNT_ERROR_NONE;
	account_stmt	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0, binding_count = 1;

	ACCOUNT_RETURN_VAL((app_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("APP ID IS NULL"));
	ACCOUNT_RETURN_VAL((cb_func != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("NO CALLBACK FUNCTION"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT * FROM %s WHERE AppId = ?", LABEL_TABLE);
	hstmt = _account_prepare_query(query);

	_account_query_bind_text(hstmt, binding_count++, app_id);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_ROW, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	label_s* label_record = NULL;

	while (rc == SQLITE_ROW) {
		label_record = (label_s*) malloc(sizeof(label_s));

		if (label_record == NULL) {
			ACCOUNT_FATAL("malloc Failed");
			break;
		}

		ACCOUNT_MEMSET(label_record, 0x00, sizeof(label_s));

		_account_type_convert_column_to_label(hstmt, label_record);

		cb_func(label_record->app_id, label_record->label , label_record->locale, user_data);

		_account_type_free_label_items(label_record);
		_ACCOUNT_FREE(label_record);

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

int _account_type_label_get_app_id(label_h label, char **app_id)
{
	if (!label) {
		ACCOUNT_ERROR("(%s)-(%d) label handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!app_id) {
		ACCOUNT_ERROR("(%s)-(%d) app_id is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	label_s *data = (label_s*)label;

	(*app_id) = NULL;

	*app_id = _account_get_text(data->app_id);

	return ACCOUNT_ERROR_NONE;
}

bool _account_get_label_text_cb(char* app_id, char* label, char* locale, void *user_data)
{
	account_type_s *data = (account_type_s*)user_data;

	label_s *label_data = (label_s*)malloc(sizeof(label_s));

	if (label_data == NULL) {
		ACCOUNT_FATAL("_account_get_label_text_cb : MALLOC FAIL\n");
		return FALSE;
	}
	ACCOUNT_MEMSET(label_data, 0, sizeof(label_s));

	label_data->app_id = _account_get_text(app_id);
	label_data->label = _account_get_text(label);
	label_data->locale = _account_get_text(locale);

	data->label_list = g_slist_append(data->label_list, (gpointer)label_data);

	ACCOUNT_VERBOSE("_account_get_label_text_cb :: appid=%s, label=%s\n", label_data->app_id, label_data->label);

	return TRUE;
}

bool _account_get_provider_feature_cb(char* app_id, char* key, void* user_data)
{
	account_type_s *data = (account_type_s*)user_data;

	provider_feature_s *feature_data = (provider_feature_s*)malloc(sizeof(provider_feature_s));

	if (feature_data == NULL) {
		ACCOUNT_FATAL("_account_get_provider_feature_cb : MALLOC FAIL\n");
		return FALSE;
	}
	ACCOUNT_MEMSET(feature_data, 0, sizeof(provider_feature_s));

	feature_data->app_id = _account_get_text(app_id);
	feature_data->key = _account_get_text(key);

	data->provider_feature_list = g_slist_append(data->provider_feature_list, (gpointer)feature_data);

	ACCOUNT_VERBOSE("appid=%s, key=%s\n", feature_data->app_id, feature_data->key);

	return TRUE;
}

ACCOUNT_API int account_type_query_by_app_id(const char* app_id, account_type_h *account_type)
{
	int 			error_code = ACCOUNT_ERROR_NONE;
	account_stmt	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0, binding_count = 1;

	ACCOUNT_RETURN_VAL((app_id != 0), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("APP ID IS NULL"));
	ACCOUNT_RETURN_VAL((*account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT TYPE IS NULL"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_DEBUG("app id (%s)\n", app_id);
	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT * FROM %s WHERE AppId = ?", ACCOUNT_TYPE_TABLE);
	hstmt = _account_prepare_query(query);

	_account_query_bind_text(hstmt, binding_count++, app_id);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_ROW, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	account_type_s *account_type_record = (account_type_s *)(*account_type);

	while (rc == SQLITE_ROW) {
		_account_type_convert_column_to_account_type(hstmt, account_type_record);
		ACCOUNT_DEBUG("get account info by id %p\n", account_type_record);
		rc = _account_query_step(hstmt);
	}

	_account_query_finalize(hstmt);
	account_type_query_label_by_app_id(_account_get_label_text_cb, app_id, (void*)account_type_record);
	account_type_query_provider_feature_by_app_id(_account_get_provider_feature_cb, app_id,(void*)account_type_record);

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

ACCOUNT_API int account_type_query_app_id_exist(const char* app_id)
{
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0;

	ACCOUNT_RETURN_VAL((app_id != 0), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("APP ID IS NULL"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_DEBUG("app id (%s)\n", app_id);

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) FROM %s WHERE AppId = '%s'", ACCOUNT_TYPE_TABLE, app_id);
	rc = _account_get_record_count(query);

	if (rc <= 0) {
		ACCOUNT_DEBUG("(%s) not exist in account type db");
		return ACCOUNT_ERROR_RECORD_NOT_FOUND;
	}

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_query_by_provider_feature(account_type_cb cb_func, const char* key, void* user_data)
{
	int 			error_code = ACCOUNT_ERROR_NONE;
	account_stmt	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0;
	GList			*account_type_list = NULL;

	ACCOUNT_RETURN_VAL((key != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("capability_type IS NULL"));
	ACCOUNT_RETURN_VAL((cb_func != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("CALL BACK IS NULL"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT * FROM %s WHERE AppId IN (SELECT app_id from %s WHERE key=?)", ACCOUNT_TYPE_TABLE, PROVIDER_FEATURE_TABLE);

	hstmt = _account_prepare_query(query);

	int binding_count = 1;
	_account_query_bind_text(hstmt, binding_count++, (char*)key);

	rc = _account_query_step(hstmt);

	account_type_s *account_type_record = NULL;

	ACCOUNT_CATCH_ERROR(rc == SQLITE_ROW, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	int k=0;
	while(rc == SQLITE_ROW) {
		account_type_record = (account_type_s*) malloc(sizeof(account_type_s));

		if (account_type_record == NULL) {
			ACCOUNT_FATAL("malloc Failed");
			break;
		}

		ACCOUNT_MEMSET(account_type_record, 0x00, sizeof(account_type_s));
		_account_type_convert_column_to_account_type(hstmt, account_type_record);
		account_type_list = g_list_append(account_type_list, account_type_record);
		rc = _account_query_step(hstmt);
		k++;
	}

	_account_query_finalize(hstmt);
	hstmt = NULL;

	GList* iter;
	k = 0;
	for (iter = account_type_list; iter != NULL; iter = g_list_next(iter)) {
		account_type_s *account_type = NULL;
		account_type = (account_type_s*)iter->data;
		account_type_query_label_by_app_id(_account_get_label_text_cb,account_type->app_id,(void*)account_type);
		account_type_query_provider_feature_by_app_id(_account_get_provider_feature_cb, account_type->app_id,(void*)account_type);
		cb_func((account_type_h)account_type, user_data);
		k++;
	}

	error_code = ACCOUNT_ERROR_NONE;

CATCH:
	if (hstmt != NULL) {
		_account_query_finalize(hstmt);
		hstmt = NULL;
	}
	if (account_type_list) {
		_account_type_glist_free(account_type_list);
		account_type_list = NULL;
	}

	return error_code;
}


ACCOUNT_API int account_type_foreach_account_type_from_db(account_type_cb callback, void *user_data)
{
	int 			error_code = ACCOUNT_ERROR_NONE;
	account_stmt	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0;
	GList			*account_type_list = NULL;

	ACCOUNT_RETURN_VAL((callback != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT INFO IS NULL"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT * FROM %s ", ACCOUNT_TYPE_TABLE);
	hstmt = _account_prepare_query(query);

	rc = _account_query_step(hstmt);

	account_type_s *account_type_record = NULL;

	ACCOUNT_CATCH_ERROR(rc == SQLITE_ROW, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	int k=0;
	while(rc == SQLITE_ROW) {
		account_type_record = (account_type_s*) malloc(sizeof(account_type_s));

		if (account_type_record == NULL) {
			ACCOUNT_FATAL("malloc Failed");
			break;
		}

		ACCOUNT_MEMSET(account_type_record, 0x00, sizeof(account_type_s));
		_account_type_convert_column_to_account_type(hstmt, account_type_record);
		account_type_list = g_list_append(account_type_list, account_type_record);
		rc = _account_query_step(hstmt);
		k++;
	}

	_account_query_finalize(hstmt);
	hstmt = NULL;

	GList* iter;
	k = 0;
	for (iter = account_type_list; iter != NULL; iter = g_list_next(iter)) {
		account_type_s *account_type = NULL;
		account_type = (account_type_s*)iter->data;
		account_type_query_label_by_app_id(_account_get_label_text_cb,account_type->app_id,(void*)account_type);
		account_type_query_provider_feature_by_app_id(_account_get_provider_feature_cb, account_type->app_id,(void*)account_type);
		callback((account_type_h)account_type, user_data);
		k++;
	}

	error_code = ACCOUNT_ERROR_NONE;

CATCH:
	if (hstmt != NULL) {
		_account_query_finalize(hstmt);
		hstmt = NULL;
	}
	if (account_type_list) {
		_account_type_glist_free(account_type_list);
		account_type_list = NULL;
	}

	return error_code;
}

// output parameter label must be free
ACCOUNT_API int account_type_query_label_by_locale(const char* app_id, const char* locale, char** label)
{
	int 			error_code = ACCOUNT_ERROR_NONE;
	account_stmt	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0, binding_count = 1;

	ACCOUNT_RETURN_VAL((app_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("NO APP ID"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));
	ACCOUNT_RETURN_VAL((label != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("label char is null"));

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT * FROM %s WHERE AppId = ? AND Locale = '%s' ", LABEL_TABLE, locale);
	hstmt = _account_prepare_query(query);

	_account_query_bind_text(hstmt, binding_count++, app_id);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_ROW, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	label_s* label_record = NULL;

	while (rc == SQLITE_ROW) {
		label_record = (label_s*) malloc(sizeof(label_s));

		if (label_record == NULL) {
			ACCOUNT_FATAL("malloc Failed");
			break;
		}

		ACCOUNT_MEMSET(label_record, 0x00, sizeof(label_s));

		_account_type_convert_column_to_label(hstmt,label_record);

		*label = _account_get_text(label_record->label);

		_account_type_free_label_items(label_record);
		_ACCOUNT_FREE(label_record);

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

static int _account_insert_custom(account_s *account, int account_id)
{
	int 			rc, count = 1;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	account_stmt 	hstmt = NULL;

	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));

	if (g_slist_length( account->custom_list)==0) {
		ACCOUNT_ERROR( "_account_insert_custom, no custom data\n");
		return ACCOUNT_ERROR_NONE;
	}

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) from %s where _id=%d", ACCOUNT_TABLE, account_id);

	rc = _account_get_record_count(query);

	if (rc <= 0) {
		ACCOUNT_ERROR( "_account_insert_custom : related account item is not existed rc=%d , %s", rc, _account_db_err_msg());
		return ACCOUNT_ERROR_RECORD_NOT_FOUND;
	}

	/* insert query*/

	GSList *iter;

	for (iter = account->custom_list; iter != NULL; iter = g_slist_next(iter)) {
		int rc, ret;
		count = 1;
		ACCOUNT_MEMSET(query, 0x00, sizeof(query));
		ACCOUNT_SNPRINTF(query, sizeof(query), "INSERT INTO %s (AccountId, AppId, Key, Value) VALUES "
				"(?, ?, ?, ?) ", ACCOUNT_CUSTOM_TABLE);

		hstmt = _account_prepare_query(query);

		ACCOUNT_RETURN_VAL((hstmt != NULL), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_prepare_query() failed(%s).\n", _account_db_err_msg()));

		account_custom_s* custom_data = NULL;
		custom_data = (account_custom_s*)iter->data;
		ACCOUNT_VERBOSE( "account_custom_s->key = %s, account_custom_s->value = %s \n", custom_data->key, custom_data->value);

		ret = _account_query_bind_int(hstmt, count++, account_id);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Int binding fail"));
		ret = _account_query_bind_text(hstmt, count++, account->package_name);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_text(hstmt, count++, (char*)custom_data->key);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_text(hstmt, count++, (char*)custom_data->value);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));

		rc = _account_query_step(hstmt);

		if (rc != SQLITE_DONE) {
			ACCOUNT_FATAL( "_account_query_step() failed(%d, %s)", rc, _account_db_err_msg());
			break;
		}

		_account_query_finalize(hstmt);
		hstmt = NULL;

	}

	ACCOUNT_VERBOSE( "_account_insert_custom() DONE\n");

	return ACCOUNT_ERROR_NONE;
}

static int _account_update_custom(account_s *account, int account_id)
{
	int 			rc, count = 1;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	account_stmt 	hstmt = NULL;

	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));

	ACCOUNT_INFO( "account handle=%p, account_id=%d", account, account_id);

	if (g_slist_length( account->custom_list)==0) {
		ACCOUNT_ERROR( "_account_update_custom, no custom data\n");
		return ACCOUNT_ERROR_NONE;
	}

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) from %s where _id=%d", ACCOUNT_TABLE, account_id);

	rc = _account_get_record_count(query);

	if (rc <= 0) {
		ACCOUNT_ERROR( "_account_update_custom : related account item is not existed rc=%d , %s", rc, _account_db_err_msg());
		return ACCOUNT_ERROR_RECORD_NOT_FOUND;
	}

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));

	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE AccountId=? ", ACCOUNT_CUSTOM_TABLE);
	hstmt = _account_prepare_query(query);
	count = 1;
	_account_query_bind_int(hstmt, count++, (int)account_id);
	rc = _account_query_step(hstmt);

	if (rc != SQLITE_DONE) {
		ACCOUNT_FATAL( "_account_query_step() failed(%d, %s)", rc, _account_db_err_msg());
		return ACCOUNT_ERROR_DB_FAILED;
	}
	_account_query_finalize(hstmt);
	hstmt = NULL;

	GSList *iter;

	for (iter = account->custom_list; iter != NULL; iter = g_slist_next(iter)) {
		int rc, ret;
		count = 1;
		ACCOUNT_MEMSET(query, 0x00, sizeof(query));
		ACCOUNT_SNPRINTF(query, sizeof(query), "INSERT INTO %s(AccountId, AppId, Key, Value) VALUES "
				"(?, ?, ?, ?) ", ACCOUNT_CUSTOM_TABLE);

		hstmt = _account_prepare_query(query);

		ACCOUNT_RETURN_VAL((hstmt != NULL), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_prepare_query() failed(%s).\n", _account_db_err_msg()));

		account_custom_s* custom_data = NULL;
		custom_data = (account_custom_s*)iter->data;

		ret = _account_query_bind_int(hstmt, count++, (int)account_id);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Int binding fail"));
		ret = _account_query_bind_text(hstmt, count++, (char*)account->package_name);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_text(hstmt, count++, (char*)custom_data->key);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_text(hstmt, count++, (char*)custom_data->value);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));

		rc = _account_query_step(hstmt);

		if (rc != SQLITE_DONE) {
			ACCOUNT_FATAL( "_account_query_step() failed(%d, %s)", rc, _account_db_err_msg());
			break;
		}

		_account_query_finalize(hstmt);
		hstmt = NULL;

	}

	ACCOUNT_VERBOSE( "_account_update_custom() DONE\n");

	return ACCOUNT_ERROR_NONE;
}

static int _account_query_custom_by_account_id(account_custom_cb cb_func, int account_id, void *user_data )
{
	int 			error_code = ACCOUNT_ERROR_NONE;
	account_stmt	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0;

	ACCOUNT_RETURN_VAL((account_id > 0), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT INDEX IS LESS THAN 0"));
	ACCOUNT_RETURN_VAL((cb_func != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("NO CALLBACK FUNCTION"));
	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));

	ACCOUNT_MEMSET(query, 0x00, ACCOUNT_SQL_LEN_MAX);

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT * FROM %s WHERE AccountId = %d", ACCOUNT_CUSTOM_TABLE, account_id);
	hstmt = _account_prepare_query(query);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_ROW, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	account_custom_s* custom_record = NULL;

	while (rc == SQLITE_ROW) {
		custom_record = (account_custom_s*) malloc(sizeof(account_custom_s));

		if (custom_record == NULL) {
			ACCOUNT_FATAL("malloc Failed");
			break;
		}

		ACCOUNT_MEMSET(custom_record, 0x00, sizeof(account_custom_s));

		_account_convert_column_to_custom(hstmt, custom_record);

		cb_func(custom_record->key, custom_record->value, user_data);

		_account_custom_item_free(custom_record);
		_ACCOUNT_FREE(custom_record);

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

static void _account_subscribe_vconf_callback(keynode_t *key, void *user_data)
{
	account_subscribe_s* tmp = (account_subscribe_s*)user_data;
	char *msg = NULL, *vconf_key = NULL;
	char event_msg[256] ={0, };
	int account_id = -1;

	ACCOUNT_DEBUG("START\n");

	if(!key) {
		ACCOUNT_ERROR("No subscribtion msg !!!!!\n");
		return;
	}

	if(!tmp) {
		ACCOUNT_ERROR("user data required\n");
		return;
	}

	if(!memcmp(vconf_keynode_get_name(key), VCONFKEY_ACCOUNT_MSG_STR, strlen(VCONFKEY_ACCOUNT_MSG_STR)))
	{
		vconf_key = vconf_keynode_get_str(key);
		msg = strdup(vconf_key);

		char* event_type = NULL;
		char* id = NULL;
		char* ptr = NULL;

		event_type = strtok_r(msg, ":", &ptr);
		id = strtok_r(NULL, ":", &ptr);

		ACCOUNT_DEBUG("msg(%s), event_type(%s), id(%s)\n", msg, event_type, id);

		ACCOUNT_SNPRINTF(event_msg,sizeof(event_msg),"%s", event_type);

		account_id = atoi(id);

		if(tmp->account_subscription_callback)
			tmp->account_subscription_callback(event_msg, account_id, tmp->user_data);
	}

	_ACCOUNT_FREE(msg);

	ACCOUNT_DEBUG("END\n");
}

ACCOUNT_API int account_subscribe_create(account_subscribe_h* account_subscribe)
{
	if (!account_subscribe) {
		ACCOUNT_ERROR("(%s)-(%d) account is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_subscribe_s *data = (account_subscribe_s*)calloc(1,sizeof(account_subscribe_s));

	if(!data) {
		ACCOUNT_FATAL("OUT OF MEMORY\n");
		return ACCOUNT_ERROR_OUT_OF_MEMORY;
	}

	ACCOUNT_VERBOSE("create handle=%p\n", *data);

	*account_subscribe = (account_subscribe_h)data;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_subscribe_notification(account_subscribe_h account_subscribe, account_event_cb cb_func, void* user_data)
{
	ACCOUNT_DEBUG("START\n");

	ACCOUNT_RETURN_VAL((account_subscribe != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("(%s)-(%d) account subscribe handle is NULL.\n",	__FUNCTION__, __LINE__));

	account_subscribe_s* tmp =(account_subscribe_s*)account_subscribe;

	tmp->account_subscription_callback = cb_func;
	tmp->user_data = user_data;

	if (vconf_notify_key_changed(VCONFKEY_ACCOUNT_MSG_STR,
				(vconf_callback_fn)_account_subscribe_vconf_callback,
				(void*)tmp) != 0) {
		ACCOUNT_FATAL("Vconf Subscription Failed !!!!!\n");
		return ACCOUNT_ERROR_EVENT_SUBSCRIPTION_FAIL;
	}

	ACCOUNT_DEBUG("Vconf Subscription Success!!!!\n");
	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_unsubscribe_notification(account_subscribe_h account_subscribe)
{
	ACCOUNT_RETURN_VAL((account_subscribe != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("(%s)-(%d) account subscribe handle is NULL.\n",	__FUNCTION__, __LINE__));

	account_subscribe_s* tmp =(account_subscribe_s*)account_subscribe;

	_ACCOUNT_FREE(tmp);

	if (vconf_ignore_key_changed(VCONFKEY_ACCOUNT_MSG_STR,
	   (vconf_callback_fn)_account_subscribe_vconf_callback) != 0) {
		ACCOUNT_FATAL("Vconf Subscription Failed !!!!!\n");
		return ACCOUNT_ERROR_EVENT_SUBSCRIPTION_FAIL;
	}

	ACCOUNT_DEBUG("Vconf Unsubscription Success!!!!\n");
	return ACCOUNT_ERROR_NONE;
}

