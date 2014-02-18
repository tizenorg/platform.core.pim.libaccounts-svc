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

#ifndef __ACCOUNT_PRIVATE_TYPES_H__
#define __ACCOUNT_PRIVATE_TYPES_H__


/*#include <glib.h>*/
#ifdef __cplusplus
extern "C"
{
#endif

#include <glib.h>
#include <dlog.h>
#include <tzplatform_config.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "ACCOUNT"

#define ACCOUNT_VERBOSE(fmt, arg...) \
		LOGV(": " fmt "\n", ##arg);
#define ACCOUNT_DEBUG(fmt, arg...) \
		LOGD(": " fmt "\n", ##arg);
#define ACCOUNT_INFO(fmt, arg...) \
		LOGI(": " fmt "\n", ##arg);
#define ACCOUNT_WARNING(fmt, arg...) \
		LOGW(": " fmt "\n", ##arg);
#define ACCOUNT_ERROR(fmt, arg...) \
		LOGW(": " fmt "\n", ##arg);
#define ACCOUNT_FATAL(fmt, arg...) \
		LOGF(": " fmt "\n", ##arg);

#define ACCOUNT_RETURN_VAL(eval, expr, ret_val, X)\
	if (!(eval)) \
{\
	expr; \
	if (1)\
	{ACCOUNT_ERROR X;}\
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
	{ACCOUNT_ERROR X;}\
	goto CATCH;\
} else {;}

#define _ACCOUNT_FREE(ptr)	\
		if (ptr != NULL) {	\
			free(ptr);	\
			ptr = NULL; \
		}	\


#define _ACCOUNT_GFREE(ptr)	\
				if (ptr != NULL) {	\
					g_free(ptr);	\
					ptr = NULL; \
				}	\

#define ACCOUNT_DB_NAME tzplatform_mkpath(TZ_SYS_DB, ".account.db")
#define ACCOUNT_TABLE "account"
#define CAPABILITY_TABLE "capability"
#define ACCOUNT_CUSTOM_TABLE "account_custom"
#define ACCOUNT_TYPE_TABLE "account_type"
#define LABEL_TABLE "label"
#define PROVIDER_FEATURE_TABLE "provider_feature"
#define ACCOUNT_SQLITE_SEQ "sqlite_sequence"
#define ACCOUNT_SQL_LEN_MAX 	1024
#define ACCOUNT_TABLE_TOTAL_COUNT	6

typedef struct _account_s
{
	int			id;
	char*		user_name;
	char*		email_address;
	char*		display_name;
	char*		icon_path;
	char*		source;
	char*		package_name;
	char*		access_token;
	char*		domain_name;		/*< domain name [Ex: google, facebook, twitter, samsung, ...] */
	int			auth_type;
	int 		secret;
	int 		sync_support;
	int			user_data_int[USER_INT_CNT];
	char*		user_data_txt[USER_TXT_CNT];
	GSList*		capablity_list;
	GList*		account_list;
	GSList*		custom_list;
}account_s;

typedef struct _capability_s
{
	int id;
	char* type;
	int value;
	char* package_name;
	char* user_name;
	int account_id;
}account_capability_s;


typedef struct _account_custom_s
{
	int 	account_id;
	char* 	app_id;
	char*	key;
	char*	value;
}account_custom_s;

typedef struct _account_type_s
{
	int 	id;
	char* 	app_id;
	char* 	service_provider_id;
	char* 	icon_path;
	char* 	small_icon_path;
	bool 	multiple_account_support;
	GSList*	label_list;
	GList*	account_type_list;
	GSList*	provider_feature_list;
}account_type_s;


typedef struct _label_s
{
	char* app_id;
	char* label;
	char* locale;
}label_s;

typedef struct _provider_feature_s
{
	char* key;
	char* app_id;
}provider_feature_s;

typedef struct _account_subscribe_s
{
	account_event_cb account_subscription_callback;
	void* user_data;
}account_subscribe_s;

/**
 * @brief   Enumarations for account handle fields.
 */

typedef enum {
	ACCOUNT_FIELD_NONE = -1,
	ACCOUNT_FIELD_ID,
	ACCOUNT_FIELD_USER_NAME,
	ACCOUNT_FIELD_EMAIL_ADDRESS,
	ACCOUNT_FIELD_DISPLAY_NAME,
	ACCOUNT_FIELD_ICON_PATH,
	ACCOUNT_FIELD_SOURCE,
	ACCOUNT_FIELD_PACKAGE_NAME,
	ACCOUNT_FIELD_ACCESS_TOKEN,
	ACCOUNT_FIELD_DOMAIN_NAME,
	ACCOUNT_FIELD_AUTH_TYPE,
	ACCOUNT_FIELD_SECRET,
	ACCOUNT_FIELD_SYNC_SUPPORT,
	ACCOUNT_FIELD_USER_TEXT_0,
	ACCOUNT_FIELD_USER_TEXT_1,
	ACCOUNT_FIELD_USER_TEXT_2,
	ACCOUNT_FIELD_USER_TEXT_3,
	ACCOUNT_FIELD_USER_TEXT_4,
	ACCOUNT_FIELD_USER_INT_0,
	ACCOUNT_FIELD_USER_INT_1,
	ACCOUNT_FIELD_USER_INT_2,
	ACCOUNT_FIELD_USER_INT_3,
	ACCOUNT_FIELD_USER_INT_4,
	ACCOUNT_DB_FIELD_END,
} ACCOUNT_DB_IDX;

/**
 * @brief	Enumarations for capability fields.
 */

typedef enum {
	CAPABILITY_FIELD_NONE = -1,
	CAPABILITY_FIELD_ID,
	CAPABILITY_FIELD_KEY,
	CAPABILITY_FIELD_VALUE,
	CAPABILITY_FIELD_PACKAGE_NAME,
	CAPABILITY_FIELD_USER_NAME,
	CAPABILITY_FIELD_ACCOUNT_ID,
	CAPABILITY_FIELD_END,
}CAPABILITY_DB_IDX;


typedef enum {
	ACCOUNT_CUSTOM_FIELD_NONE = -1,
	ACCOUNT_CUSTOM_FIELD_ACCOUNT_ID,
	ACCOUNT_CUSTOM_FIELD_APP_ID,
	ACCOUNT_CUSTOM_FIELD_KEY,
	ACCOUNT_CUSTOM_FIELD_VALUE,
	ACCOUNT_CUSTOM_FIELD_END,
}ACCOUNT_CUSTOM_DB_IDX;

typedef enum {
	ACCOUNT_TYPE_FIELD_NONE = -1,
	ACCOUNT_TYPE_FIELD_ID,
	ACCOUNT_TYPE_FIELD_APP_ID,
	ACCOUNT_TYPE_FIELD_SERVICE_PROVIDER_ID,
	ACCOUNT_TYPE_FIELD_ICON_PATH,
	ACCOUNT_TYPE_FIELD_SMALL_ICON_PATH,
	ACCOUNT_TYPE_FIELD_MULTIPLE_ACCOUNT_SUPPORT,
	ACCOUNT_TYPE_FIELD_END,
}ACCOUNT_TYPE_DB_IDX;

typedef enum {
	LABEL_FIELD_NONE = -1,
	LABEL_FIELD_APP_ID,
	LABEL_FIELD_LABEL,
	LABEL_FIELD_LOCALE,
	LABEL_FIELD_END,
}LABEL_DB_IDX;

typedef enum {
	PROVIDER_FEATURE_FIELD_NONE = -1,
	PROVIDER_FEATURE_FIELD_APP_ID,
	PROVIDER_FEATURE_FIELD_KEY,
	PROVIDER_FEATURE_FIELD_END,
}PROVIDER_FEATURE_DB_IDX;

typedef sqlite3_stmt* account_stmt;

#define ACCOUNT_SCHEMA	"create table %s \n"\
	"(\n"\
"_id INTEGER PRIMARY KEY AUTOINCREMENT, "\
"user_name TEXT, "\
"email_address TEXT, "\
"display_name TEXT, "\
"icon_path TEXT, "\
"source TEXT, "\
"package_name TEXT, "\
"access_token TEXT, "\
"domain_name TEXT, "\
"auth_type INTEGER, "\
"secret INTEGER, "\
"sync_support INTEGER, "\
"txt_custom0 TEXT, "\
"txt_custom1 TEXT, "\
"txt_custom2 TEXT, "\
"txt_custom3 TEXT, "\
"txt_custom4 TEXT, "\
"int_custom0 INTEGER, "\
"int_custom1 INTEGER, "\
"int_custom2 INTEGER, "\
"int_custom3 INTEGER, "\
"int_custom4 INTEGER "\
");"

#define CAPABILITY_SCHEMA  "create table %s \n"\
	"(\n"\
"_id INTEGER PRIMARY KEY AUTOINCREMENT, "\
"key TEXT, "\
"value INTEGER, "\
"package_name TEXT, "\
"user_name TEXT, "\
"account_id INTEGER "\
");"

#define ACCOUNT_CUSTOM_SCHEMA  "create table %s \n"\
	"(\n"\
"AccountId INTEGER, "\
"AppId TEXT, "\
"Key TEXT, "\
"Value TEXT "\
");"

#define ACCOUNT_TYPE_SCHEMA "create table %s \n"\
	"(\n"\
"_id INTEGER PRIMARY KEY AUTOINCREMENT, "\
"AppId TEXT, "\
"ServiceProviderId TEXT, "\
"IconPath TEXT, "\
"SmallIconPath TEXT, "\
"MultipleAccountSupport INTEGER "\
");"

#define LABEL_SCHEMA "create table %s \n"\
	"(\n"\
"AppId TEXT, "\
"Label TEXT, "\
"Locale TEXT"\
");"

#define PROVIDER_FEATURE_SCHEMA "create table %s \n"\
	"(\n"\
"app_id TEXT, "\
"key TEXT "\
");"

typedef struct GSList 		account_iterator_s;

#ifdef __cplusplus
}
#endif

#endif /* __SLP_ACCOUNT_PRIVATE_TYPES_H__*/
