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

#ifndef __ACCOUNT_PRIVATE_TYPES_H__
#define __ACCOUNT_PRIVATE_TYPES_H__


/*#include <glib.h>*/
#ifdef __cplusplus
extern "C"
{
#endif

#include <glib.h>

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

#define ACCOUNT_DB_NAME "/opt/dbspace/.account.db"
#define ACCOUNT_TABLE "account"
#define CAPABILITY_TABLE "capability"
#define ACCOUNT_SQLITE_SEQ "sqlite_sequence"
#define ACCOUNT_SQL_LEN_MAX 	1024
#define ACCOUNT_TABLE_TOTAL_COUNT	2

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
	/*int			service_type;*/ /* ACCOUNT_CATEGORY*/
	int			auth_type;
	int 		secret;
	int			user_data_int[USER_INT_CNT];
	char*		user_data_txt[USER_TXT_CNT];
	GSList*		capablity_list;
	GList*		account_list;
}account_s;

typedef struct _capability_s
{
	int id;
	int type;
	int value;
	char* package_name;
	char* user_name;
	int account_id;
}account_capability_s;

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
"key INTEGER, "\
"value INTEGER, "\
"package_name TEXT, "\
"user_name TEXT, "\
"account_id INTEGER "\
");"

typedef struct GSList 		account_iterator_s;

#ifdef __cplusplus
}
#endif

#endif /* __SLP_ACCOUNT_PRIVATE_TYPES_H__*/
