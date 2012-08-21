/*
 *  utc-account
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Wonyoung Lee <wy1115.lee@samsung.com>, Tarun Kumar <tarun.kr@samsung.com>
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
#include "account.h"
#include <string.h>
#include <dbus/dbus.h>

int account_id = 0;

static void _account_free_text(char *text)
{
	if (text) {
		free(text);
		text = NULL;
	}
}

bool _capability_get_callback(account_capability_type_e capability_type, account_capability_state_e capability_value, void* user_data)
{
	int test_type = capability_type;
	int test_value = capability_value;

	printf("_capability_get_callback test_type = : %d\n", test_type);
	printf("_capability_get_callback test_value = : %d\n", test_value);
	return TRUE;
}

int _account_get_account_info(account_h handle)
{
	printf("_account_get_callback\n");

	char *test_text = NULL;

	account_get_user_name(handle, &test_text);
	printf("_account_get_callback : %s\n", test_text);

	_account_free_text(test_text);

	account_get_display_name(handle, &test_text);
	printf("_account_get_callback : %s\n", test_text);

	_account_free_text(test_text);

	account_get_email_address(handle, &test_text);
	printf("_account_get_callback : %s\n", test_text);

	_account_free_text(test_text);

	account_get_icon_path(handle, &test_text);
	printf("_account_get_callback : %s\n", test_text);

	_account_free_text(test_text);

	account_get_source(handle, &test_text);
	printf("_account_get_callback : %s\n", test_text);

	_account_free_text(test_text);

	account_get_package_name(handle, &test_text);
	printf("_account_get_callback : %s\n", test_text);

	_account_free_text(test_text);

	account_get_domain_name(handle, &test_text);
	printf("_account_get_callback : %s\n", test_text);

	_account_free_text(test_text);

	account_get_access_token(handle, &test_text);
	printf("_account_get_callback : %s\n", test_text);


	int i;

	for(i=0;i<USER_TXT_CNT;i++)
	{
		_account_free_text(test_text);

		account_get_user_text(handle, i, &test_text);
		printf("_account_get_callback : %s\n", test_text);
	}

	int test_int  = -1;

	account_auth_type_e auth_type;
	account_get_auth_type(handle, &auth_type);
	printf("account_get_auth_type : %d\n", auth_type);

	account_secrecy_state_e secret;
	account_get_secret(handle, &secret);
	printf("account_get_secret : %d\n", secret);

	account_sync_state_e sync_support;
	account_get_sync_support(handle, &sync_support);
	printf("account_get_sync_support : %d\n", sync_support);

	for(i=0;i<USER_INT_CNT;i++)
	{
		test_int  = -1;
		account_get_user_int(handle, i, &test_int);
		printf("_account_get_callback : %d\n", test_int);
	}

	account_get_capability(handle, _capability_get_callback, NULL);

	return 0;
}


bool _account_get_callback(account_h handle, void* user_data)
{
	printf("_account_get_callback\n");

	char *test_text = NULL;

	account_get_user_name(handle, &test_text);
	printf("_account_get_callback : %s\n", test_text);

	_account_free_text(test_text);

	account_get_display_name(handle, &test_text);
	printf("_account_get_callback : %s\n", test_text);

	_account_free_text(test_text);

	account_get_email_address(handle, &test_text);
	printf("_account_get_callback : %s\n", test_text);

	_account_free_text(test_text);

	account_get_icon_path(handle, &test_text);
	printf("_account_get_callback : %s\n", test_text);

	_account_free_text(test_text);

	account_get_source(handle, &test_text);
	printf("_account_get_callback : %s\n", test_text);

	_account_free_text(test_text);

	account_get_package_name(handle, &test_text);
	printf("_account_get_callback : %s\n", test_text);

	_account_free_text(test_text);

	account_get_domain_name(handle, &test_text);
	printf("_account_get_callback : %s\n", test_text);

	_account_free_text(test_text);

	account_get_access_token(handle, &test_text);
	printf("_account_get_callback : %s\n", test_text);

	int i;

	for(i=0;i<USER_TXT_CNT;i++)
	{
		_account_free_text(test_text);

		account_get_user_text(handle, i, &test_text);
		printf("_account_get_callback : %s\n", test_text);
	}

	int test_int  = -1;

	account_auth_type_e auth_type;
	account_get_auth_type(handle, &auth_type);
	printf("account_get_auth_type : %d\n", auth_type);

	account_secrecy_state_e secret;
	account_get_secret(handle, &secret);
	printf("account_get_secret : %d\n", secret);

	account_sync_state_e sync_support;
	account_get_sync_support(handle, &sync_support);
	printf("account_get_sync_support : %d\n", sync_support);

	for(i=0;i<USER_INT_CNT;i++)
	{
		test_int  = -1;
		account_get_user_int(handle, i, &test_int);
		printf("_account_get_callback : %d\n", test_int);
	}

	account_get_capability(handle, _capability_get_callback, NULL);

	return TRUE;
}

static int _account_insert_test_new(void)
{
	int ret = -1;

	account_h account;
	int temp_int = -1;

	ret = account_connect();

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	char temp[128];
	memset(temp, 0x00, sizeof(temp));

	ret = account_create(&account);
	printf("account_set_user_name: Enter User Name to be set \n");
	scanf("%s", temp);

	ret = account_set_user_name(account, temp);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	memset(temp, 0x00, sizeof(temp));
	printf("account_set_display_name: Enter display Name to be set \n");
	scanf("%s", temp);

	ret = account_set_display_name(account, temp);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	memset(temp, 0x00, sizeof(temp));
	printf("account_set_domain_name: Enter domain Name to be set \n");
	scanf("%s", temp);

	ret = account_set_domain_name(account, temp);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	memset(temp, 0x00, sizeof(temp));
	printf("account_set_email_address: Enter email Name to be set \n");
	scanf("%s", temp);
	ret = account_set_email_address(account, temp);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	memset(temp, 0x00, sizeof(temp));
	printf("account_set_package_name: Enter package Name to be set \n");
	scanf("%s", temp);

	ret = account_set_package_name(account, temp);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	memset(temp, 0x00, sizeof(temp));
	printf("account_set_icon_path: Enter icon path to be set \n");
	scanf("%s", temp);

	ret = account_set_icon_path(account, temp);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	memset(temp, 0x00, sizeof(temp));

	printf("account_set_access_token: Enter access token to be set \n");
	scanf("%s", temp);

	ret = account_set_access_token(account, temp);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	memset(temp, 0x00, sizeof(temp));
	printf("account_set_auth_type: Enter auth type to be set \n");
	scanf("%d", &temp_int);

	ret = account_set_auth_type( account, temp_int );
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	temp_int = -1;
	printf("account_set_secret: Enter secret to be set \n");
	scanf("%d", &temp_int);

	ret = account_set_secret( account, temp_int );
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	temp_int = -1;
	printf("account_set_sync_support: Enter sync support state \n");
	scanf("%d", &temp_int);

	ret = account_set_sync_support( account, temp_int );
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	memset(temp, 0x00, sizeof(temp));
	printf("account_set_source: Enter source to be set \n");
	scanf("%s", temp);
	ret = account_set_source(account, temp);

	int i;

	for(i=0;i<USER_TXT_CNT;i++)
	{
		memset(temp, 0x00, sizeof(temp));
		printf("account_set_user_text [%d]: Enter user text to be set \n", i);
		scanf("%s", temp);
		ret = account_set_user_text(account, i, temp);
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}

	}

	for(i=0;i<USER_INT_CNT;i++)
	{
		temp_int = -1;
		printf("account_set_user_int [%d]: Enter user int to be set \n", i);
		scanf("%d", &temp_int);
		ret = account_set_user_int(account, i, temp_int);
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}
	}

	int cap_key = -1;
	int cap_value = -1;
	int want_to_enter_more_cap = 1;
	while (want_to_enter_more_cap){
		printf("account_set_capability: Enter capability key to set \n");
		scanf("%d", &cap_key);
		printf("account_set_capability: Enter capability value to set \n");
		scanf("%d", &cap_value);
		ret = account_set_capability(account, cap_key, cap_value);
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
		}
		printf("account_set_capability: Want to set more capabilitiy key value then enter 1 else 0 \n");
		scanf("%d", &temp_int);
		if (temp_int == 1)
			want_to_enter_more_cap = 1;
		else
			want_to_enter_more_cap = 0;
	}
	int account_id = -1;
	ret = account_insert_to_db(account, &account_id);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
		return -1;
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_disconnect();

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	return account_id;
}

static int _account_insert_test(void)
{
	int ret = -1;

	account_h account;

	printf("account_connect returns %d\n", ret);

	ret = account_create(&account);

	printf("account = %p\n", account);

	ret = account_set_user_name(account, "wy1115.lee");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, "Wonyoung Lee");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_domain_name(account, "Samsung electronics");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_email_address(account, "urusa77@gmail.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_package_name(account, "com.samsung.account");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_icon_path(account, "icon-path");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_access_token(account, "ACCESSTOKEN");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_auth_type( account, ACCOUNT_AUTH_TYPE_OAUTH );
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_secret( account, ACCOUNT_SECRECY_INVISIBLE);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_source(account, "URUSA inc.");

	int i;

	for(i=0;i<USER_TXT_CNT;i++)
	{
		ret = account_set_user_text(account, i, "user txt");
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}

	}

	for(i=0;i<USER_INT_CNT;i++)
	{
		ret = account_set_user_int(account, i, 888);
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}
	}

	ret = account_set_capability(account, ACCOUNT_CAPABILITY_CONTACT, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_CALENDAR, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_PHOTO, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_VIDEO, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_EMAIL, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_STATUS_POST, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_VOIP, ACCOUNT_CAPABILITY_ENABLED);
	//ret = account_set_capability(account, ACCOUNT_CAPABILITY_IM, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_SAMSUNG_APPS, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_MOBILE_TRACKER, ACCOUNT_CAPABILITY_ENABLED);

	int account_id = -1;
	ret = account_insert_to_db(account, &account_id);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
		return -1;
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account=NULL;

	printf("account_connect returns %d\n", ret);

	ret = account_create(&account);

	account_query_account_by_account_id(account_id, &account);
	_account_get_account_info(account);

	account_query_capability_by_account_id(_capability_get_callback, account_id, NULL);

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}


	account=NULL;

	printf("account_connect returns %d\n", ret);

	ret = account_create(&account);

	account_query_account_by_user_name(_account_get_callback, "usernamed-wy1115", NULL);

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	return account_id;
}

static int _account_update_test_new_by_user_name(char *user_name, char *package_name)
{
	int ret = -1;

	account_h account;
	int temp_int = -1;

	char temp[128];
	memset(temp, 0x00, sizeof(temp));
	ret = account_connect();

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_create(&account);
	printf("account_set_user_name: Enter User Name to be set \n");
	scanf("%s", temp);

	ret = account_set_user_name(account, temp);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	memset(temp, 0x00, sizeof(temp));

	printf("account_set_display_name: Enter display Name to be set \n");
	scanf("%s", temp);

	ret = account_set_display_name(account, temp);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	memset(temp, 0x00, sizeof(temp));

	printf("account_set_domain_name: Enter domain Name to be set \n");
	scanf("%s", temp);

	ret = account_set_domain_name(account, temp);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	memset(temp, 0x00, sizeof(temp));

	printf("account_set_email_address: Enter email Name to be set \n");
	scanf("%s", temp);
	ret = account_set_email_address(account, temp);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	memset(temp, 0x00, sizeof(temp));

	printf("account_set_package_name: Enter package Name to be set \n");
	scanf("%s", temp);

	ret = account_set_package_name(account, temp);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	memset(temp, 0x00, sizeof(temp));

	printf("account_set_icon_path: Enter icon path to be set \n");
	scanf("%s", temp);

	ret = account_set_icon_path(account, temp);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	memset(temp, 0x00, sizeof(temp));

	printf("account_set_access_token: Enter access token to be set \n");
	scanf("%s", temp);

	ret = account_set_access_token(account, temp);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	memset(temp, 0x00, sizeof(temp));

	printf("account_set_auth_type: Enter auth type to be set \n");
	scanf("%d", &temp_int);

	ret = account_set_auth_type( account, temp_int );
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	temp_int = -1;
	printf("account_set_secret: Enter secret to be set \n");
	scanf("%d", &temp_int);

	ret = account_set_secret( account, temp_int );
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	temp_int = -1;
	printf("account_set_sync_support: Enter sync support state \n");
	scanf("%d", &temp_int);

	ret = account_set_sync_support( account, temp_int );
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	memset(temp, 0x00, sizeof(temp));

	printf("account_set_source: Enter source to be set \n");
	scanf("%s", temp);
	ret = account_set_source(account, temp);

	int i;

	for(i=0;i<USER_TXT_CNT;i++)
	{
		memset(temp, 0x00, sizeof(temp));

		printf("account_set_user_text [%d]: Enter user text to be set \n", i);
		scanf("%s", temp);
		ret = account_set_user_text(account, i, temp);
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}

	}

	for(i=0;i<USER_INT_CNT;i++)
	{
		temp_int = -1;
		printf("account_set_user_int [%d]: Enter user int to be set \n", i);
		scanf("%d", &temp_int);
		ret = account_set_user_int(account, i, temp_int);
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}
	}

	int cap_key = -1;
	int cap_value = -1;
	int want_to_enter_more_cap = 1;
	while (want_to_enter_more_cap){
		printf("account_set_capability: Enter capability key to set \n");
		scanf("%d", &cap_key);
		printf("account_set_capability: Enter capability value to set \n");
		scanf("%d", &cap_value);
		ret = account_set_capability(account, cap_key, cap_value);
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
		}
		printf("account_set_capability: Want to set more capabilitiy key value then enter 1 else 0 \n");
		scanf("%d", &temp_int);
		if (temp_int == 1)
			want_to_enter_more_cap = 1;
		else
			want_to_enter_more_cap = 0;
	}


	ret = account_update_to_db_by_user_name(account, user_name, package_name);


	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	return ret;

}


static int _account_update_test_new(int account_id)
{

	int ret = -1;

	account_h account;
	int temp_int = -1;

	char temp[128];
	memset(temp, 0x00, sizeof(temp));
	ret = account_connect();

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}


	ret = account_create(&account);
	printf("account_set_user_name: Enter User Name to be set \n");
	scanf("%s", temp);

	ret = account_set_user_name(account, temp);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	memset(temp, 0x00, sizeof(temp));

	printf("account_set_display_name: Enter display Name to be set \n");
	scanf("%s", temp);

	ret = account_set_display_name(account, temp);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	memset(temp, 0x00, sizeof(temp));
	printf("account_set_domain_name: Enter domain Name to be set \n");
	scanf("%s", temp);

	ret = account_set_domain_name(account, temp);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	memset(temp, 0x00, sizeof(temp));
	printf("account_set_email_address: Enter email Name to be set \n");
	scanf("%s", temp);
	ret = account_set_email_address(account, temp);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	memset(temp, 0x00, sizeof(temp));

	printf("account_set_package_name: Enter package Name to be set \n");
	scanf("%s", temp);

	ret = account_set_package_name(account, temp);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	memset(temp, 0x00, sizeof(temp));

	printf("account_set_icon_path: Enter icon path to be set \n");
	scanf("%s", temp);

	ret = account_set_icon_path(account, temp);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	memset(temp, 0x00, sizeof(temp));

	printf("account_set_access_token: Enter access token to be set \n");
	scanf("%s", temp);

	ret = account_set_access_token(account, temp);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	memset(temp, 0x00, sizeof(temp));

	printf("account_set_auth_type: Enter auth type to be set \n");
	scanf("%d", &temp_int);

	ret = account_set_auth_type( account, temp_int );
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	temp_int = -1;
	printf("account_set_secret: Enter secret to be set \n");
	scanf("%d", &temp_int);

	ret = account_set_secret( account, temp_int );
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	temp_int = -1;
	printf("account_set_sync_support: Enter sync support state \n");
	scanf("%d", &temp_int);

	ret = account_set_sync_support( account, temp_int );
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	memset(temp, 0x00, sizeof(temp));

	printf("account_set_source: Enter source to be set \n");
	scanf("%s", temp);
	ret = account_set_source(account, temp);

	int i;

	for(i=0;i<USER_TXT_CNT;i++)
	{
		memset(temp, 0x00, sizeof(temp));

		printf("account_set_user_text [%d]: Enter user text to be set \n", i);
		scanf("%s", temp);
		ret = account_set_user_text(account, i, temp);
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}

	}

	for(i=0;i<USER_INT_CNT;i++)
	{
		temp_int = -1;
		printf("account_set_user_int [%d]: Enter user int to be set \n", i);
		scanf("%d", &temp_int);
		ret = account_set_user_int(account, i, temp_int);
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}
	}

	int cap_key = -1;
	int cap_value = -1;
	int want_to_enter_more_cap = 1;
	while (want_to_enter_more_cap){
		printf("account_set_capability: Enter capability key to set \n");
		scanf("%d", &cap_key);
		printf("account_set_capability: Enter capability value to set \n");
		scanf("%d", &cap_value);
		ret = account_set_capability(account, cap_key, cap_value);
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
		}
		printf("account_set_capability: Want to set more capabilitiy key value then enter 1 else 0 \n");
		scanf("%d", &temp_int);
		if (temp_int == 1)
			want_to_enter_more_cap = 1;
		else
			want_to_enter_more_cap = 0;
	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_disconnect();

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	return ret;
}

static int _account_update_test(int account_id)
{
	int ret = - 1;

	account_h account;

	ret = account_create(&account);

	ret = account_set_user_name(account, "updated-wy1115");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, "updated-Wonyoung Lee");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_domain_name(account, "updated-Samsung Facebook");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_email_address(account, "updated-urusa77@gmail.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_package_name(account, "updated-com.samsung.account");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_icon_path(account, "updated-icon-path");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_access_token(account, "updated-ACCESSTOKEN");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_auth_type( account, ACCOUNT_AUTH_TYPE_OAUTH );
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_secret( account, ACCOUNT_SECRECY_INVISIBLE);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_source(account, "updated-URUSA inc.");

	int i;

	for(i=0;i<USER_TXT_CNT;i++)
	{
		ret = account_set_user_text(account, i, "updated-user txt");
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}

	}

	for(i=0;i<USER_INT_CNT;i++)
	{
		ret = account_set_user_int(account, i, 111);
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}
	}


	ret = account_set_capability(account, ACCOUNT_CAPABILITY_PHOTO, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_VIDEO, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_STATUS_POST, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_SAMSUNG_APPS, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_MOBILE_TRACKER, ACCOUNT_CAPABILITY_ENABLED);

	ret = account_update_to_db_by_id(account, account_id);


	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	return ret;

}


static int _account_update_test_by_username()
{
	int ret = - 1;

	account_h account;

	ret = account_create(&account);

	ret = account_set_user_name(account, "usernamed-wy1115");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, "usernamed-Wonyoung Lee");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_domain_name(account, "usernamed-Samsung Facebook");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_email_address(account, "usernamed-urusa77@gmail.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_package_name(account, "usernamed-com.samsung.account");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_icon_path(account, "usernamed-icon-path");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_access_token(account, "usernamed-ACCESSTOKEN");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_auth_type( account, ACCOUNT_AUTH_TYPE_OAUTH );
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_secret( account, ACCOUNT_SECRECY_INVISIBLE);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_source(account, "updated-URUSA inc.");

	int i;

	for(i=0;i<USER_TXT_CNT;i++)
	{
		ret = account_set_user_text(account, i, "usernamed-user txt");
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}

	}

	for(i=0;i<USER_INT_CNT;i++)
	{
		ret = account_set_user_int(account, i, 111);
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}
	}


	ret = account_set_capability(account, ACCOUNT_CAPABILITY_PHOTO, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_VIDEO, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_VOIP, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_SAMSUNG_APPS, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_STATUS_POST, ACCOUNT_CAPABILITY_ENABLED);

	ret = account_update_to_db_by_user_name(account, "updated-wy1115", "updated-com.samsung.account");


	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	return ret;

}


static int _account_test_capability()
{
	GSList* list = NULL;
	GSList* iter = NULL;
	int glist_len = 0;

	list = g_slist_append(list, strdup("test 1"));
	list = g_slist_append(list, strdup("test 2"));
	list = g_slist_append(list, strdup("test 3"));
	list = g_slist_append(list, strdup("test 4"));
	list = g_slist_append(list, strdup("test 5"));
	list = g_slist_append(list, strdup("test 6"));

	glist_len =  g_slist_length( list );

	int capability_count = g_slist_length( list );

	int binding_bufsize = 2+2*(capability_count*2)+1;

	int binding_buf_index = 0;
	char* binding_buf = (char*)malloc(sizeof(char)*binding_bufsize);

	memset(binding_buf, 0, sizeof(char)*binding_bufsize);

	binding_buf[binding_buf_index++]='(';

	printf("(%d)Binding buf = %s\n", __LINE__,binding_buf);

	int i;

	for(i=0;i<capability_count;i++)
	{
		/*snprintf(binding_buf+binding_buf_index, strlen(binding_buf)+2, "%s?,", binding_buf);*/
		binding_buf[binding_buf_index++]='?';
		if(i != capability_count -1)
			binding_buf[binding_buf_index++]=',';
		printf("(%d)Binding buf = %s\n", __LINE__,binding_buf);
	}

	binding_buf[binding_buf_index]=')';

	printf("(%d)Binding buf = %s\n", __LINE__,binding_buf);

	printf("Length %d\n", glist_len);

	for (iter = list; iter != NULL; iter = g_slist_next(iter))
	{
		char* capa;
		capa = (char *)iter->data;
		printf("%s\n", (char *)iter->data);
		g_free(capa);
	}


	g_slist_free(list);
	return 0;
}


static int utc1_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return account_id;
}

static int utc2_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_set_user_name(account, NULL);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return account_id;
}




static int utc3_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_set_user_name(account, "11111111111111111111111111111111111 \
		2222222222222222222222222222222222222222222222222 \
		3333333333333333333333333333333333333333333333333\
		4444444444444444444444444444444444444444444444444\
		5555555555555555555555555555555555555555555555555\
		666666666666666666666666666666666666666666666666\
		777777777777777777777777777777777777777777777777\
		888888888888888888888888888888888888888888888888\
		99999999999999999999999999999999999999999999999\
		111111111111111111111111111111111111111111111111\
		22222222222222222222222222222222222222222222222\
		333333333333333333333333333333333333333333333\
		444444444444444444444444444444444444444444\
		444444444444444444444444444444444444444444444444\
		555555555555555555555555555555555555555555555555 \
		66666666666666666666666666666666666666666666666666\
		777777777777777777777777777777777777777777777777779999999999999999999999999999999999999999\
		999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999\
		999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return account_id;
}

static int utc4_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_set_user_name(account, "tarun.kr");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, NULL);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return account_id;
}

static int utc5_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_set_user_name(account, "tarun.kr");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, "tarun kumar");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_domain_name(account, NULL);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return account_id;
}

static int utc6_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_set_user_name(account, "tarun.kr");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, "tarun kumar");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_domain_name(account, "Samsung");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return account_id;
}

static int utc7_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_set_user_name(account, "tarun.kr");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, "tarun kumar");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_domain_name(account, "Samsung");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_email_address(account, NULL);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return account_id;
}

static int utc8_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_set_user_name(account, "tarun.kr");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, "tarun kumar");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_domain_name(account, "Samsung");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_email_address(account, "tarun.kr@samsung.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return account_id;
}

static int utc9_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_set_user_name(account, "tarun.kr");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, "tarun kumar");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_domain_name(account, "Samsung");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_email_address(account, "tarun.kr@samsung.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_package_name(account, NULL);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return account_id;
}

static int utc10_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_set_user_name(account, "tarun.kr");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, "tarun kumar");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_domain_name(account, "Samsung");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_email_address(account, "tarun.kr@samsung.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_package_name(account, "com.samsung.accounts");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return account_id;
}

static int utc11_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_set_user_name(account, "tarun.kr");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, "tarun kumar");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_domain_name(account, "Samsung");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_email_address(account, "tarun.kr@samsung.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_package_name(account, "com.samsung.accounts");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_icon_path(account, NULL);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return account_id;
}

static int utc12_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_set_user_name(account, "tarun.kr");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, "tarun kumar");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_domain_name(account, "Samsung");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_email_address(account, "tarun.kr@samsung.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_package_name(account, "com.samsung.accounts");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_icon_path(account, "/test/test.jpg");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return account_id;
}

static int utc13_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_set_user_name(account, "tarun.kr");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, "tarun kumar");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_domain_name(account, "Samsung");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_email_address(account, "tarun.kr@samsung.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_package_name(account, "com.samsung.accounts");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_icon_path(account, "/test/test.jpg");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_access_token(account, NULL);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}


	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return account_id;
}

static int utc14_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_set_user_name(account, "tarun.kr");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, "tarun kumar");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_domain_name(account, "Samsung");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_email_address(account, "tarun.kr@samsung.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_package_name(account, "com.samsung.accounts");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_icon_path(account, "/test/test.jpg");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_access_token(account, "xxxx/yyyy/zzzz/-&abc");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}


	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return account_id;
}

static int utc15_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_set_user_name(account, "tarun.kr");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, "tarun kumar");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_domain_name(account, "Samsung");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_email_address(account, "tarun.kr@samsung.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_package_name(account, "com.samsung.accounts");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_icon_path(account, "/test/test.jpg");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_access_token(account, "xxxx/yyyy/zzzz/-&abc");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_auth_type( account, ACCOUNT_AUTH_TYPE_OAUTH );
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_secret( account, ACCOUNT_SECRECY_INVISIBLE);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return account_id;
}

static int utc16_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_set_user_name(account, "tarun.kr");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, "tarun kumar");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_domain_name(account, "Samsung");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_email_address(account, "tarun.kr@samsung.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_package_name(account, "com.samsung.accounts");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_icon_path(account, "/test/test.jpg");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_access_token(account, "xxxx/yyyy/zzzz/-&abc");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_auth_type( account, ACCOUNT_AUTH_TYPE_OAUTH );
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_secret( account, ACCOUNT_SECRECY_INVISIBLE);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return account_id;
}

static int utc17_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_set_user_name(account, "tarun.kr");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, "tarun kumar");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_domain_name(account, "Samsung");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_email_address(account, "tarun.kr@samsung.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_package_name(account, "com.samsung.accounts");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_icon_path(account, "/test/test.jpg");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_access_token(account, "xxxx/yyyy/zzzz/-&abc");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_auth_type( account, ACCOUNT_AUTH_TYPE_OAUTH );
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_secret( account, ACCOUNT_SECRECY_INVISIBLE);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_source(account, NULL);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return account_id;
}

static int utc18_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_set_user_name(account, "tarun.kr");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, "tarun kumar");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_domain_name(account, "Samsung");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_email_address(account, "tarun.kr@samsung.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_package_name(account, "com.samsung.accounts");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_icon_path(account, "/test/test.jpg");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_access_token(account, "xxxx/yyyy/zzzz/-&abc");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_auth_type( account, ACCOUNT_AUTH_TYPE_OAUTH );
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_secret( account, ACCOUNT_SECRECY_INVISIBLE);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_source(account, "URUSA inc.");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return account_id;
}

static int utc19_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_set_user_name(account, "tarun.kr");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, "tarun kumar");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_domain_name(account, "Samsung");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_email_address(account, "tarun.kr@samsung.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_package_name(account, "com.samsung.accounts");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_icon_path(account, "/test/test.jpg");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_access_token(account, "xxxx/yyyy/zzzz/-&abc");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_auth_type( account, ACCOUNT_AUTH_TYPE_OAUTH );
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_secret( account, ACCOUNT_SECRECY_INVISIBLE);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_source(account, "URUSA inc.");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	int i;

	for(i=0; i<USER_TXT_CNT; i++)
	{
		ret = account_set_user_text(account, i, NULL);
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}

	}
	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return account_id;
}

static int utc20_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_set_user_name(account, "tarun.kr");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, "tarun kumar");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_domain_name(account, "Samsung");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_email_address(account, "tarun.kr@samsung.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_package_name(account, "com.samsung.accounts");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_icon_path(account, "/test/test.jpg");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_access_token(account, "xxxx/yyyy/zzzz/-&abc");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_auth_type( account, ACCOUNT_AUTH_TYPE_OAUTH );
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_secret( account, ACCOUNT_SECRECY_INVISIBLE);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_source(account, "URUSA inc.");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	int i;

	for(i=0; i<USER_TXT_CNT; i++)
	{
		ret = account_set_user_text(account, i, "user txt");
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account-else loop] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}

	}
	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return account_id;
}

static int utc21_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_set_user_name(account, "tarun.kr");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, "tarun kumar");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_domain_name(account, "Samsung");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_email_address(account, "tarun.kr@samsung.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_package_name(account, "com.samsung.accounts");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_icon_path(account, "/test/test.jpg");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_access_token(account, "xxxx/yyyy/zzzz/-&abc");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_auth_type( account, ACCOUNT_AUTH_TYPE_OAUTH );
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_secret( account, ACCOUNT_SECRECY_INVISIBLE);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_source(account, "URUSA inc.");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	int i;

	for(i=0; i<USER_TXT_CNT; i++)
	{
		ret = account_set_user_text(account, i, "user txt");
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}

	}
	for(i=0;i<USER_INT_CNT;i++)
	{
		ret = account_set_user_int(account, i, 000);
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}
	}
	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return account_id;
}

static int utc22_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_set_user_name(account, "tarun.kr");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, "tarun kumar");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_domain_name(account, "Samsung");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_email_address(account, "tarun.kr@samsung.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_package_name(account, "com.samsung.accounts");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_icon_path(account, "/test/test.jpg");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_access_token(account, "xxxx/yyyy/zzzz/-&abc");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_auth_type( account, ACCOUNT_AUTH_TYPE_OAUTH );
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_secret( account, ACCOUNT_SECRECY_INVISIBLE);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_source(account, "URUSA inc.");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	int i;

	for(i=0; i<USER_TXT_CNT; i++)
	{
		ret = account_set_user_text(account, i, "user txt");
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}

	}

	for(i=0;i<USER_INT_CNT;i++)
	{
		ret = account_set_user_int(account, i, 888);
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}
	}
	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return account_id;
}

static int utc23_account_insert()
{

	int ret = -1;
	account_h account;
	int account_id = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	printf("account = %p, ret = %d \n", account, ret);

	ret = account_set_user_name(account, "tarun.kr");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_display_name(account, "tarun kumar");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_domain_name(account, "Samsung");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_email_address(account, "tarun.kr@samsung.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_package_name(account, "com.samsung.accounts");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_icon_path(account, "/test/test.jpg");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_access_token(account, "xxxx/yyyy/zzzz/-&abc");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_auth_type( account, ACCOUNT_AUTH_TYPE_OAUTH );
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_secret( account, ACCOUNT_SECRECY_INVISIBLE);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_set_source(account, "URUSA inc.");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	int i;

	for(i=0; i<USER_TXT_CNT; i++)
	{
		ret = account_set_user_text(account, i, "user txt");
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}

	}

	for(i=0;i<USER_INT_CNT;i++)
	{
		ret = account_set_user_int(account, i, 888);
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}
	}

	ret = account_set_capability(account, ACCOUNT_CAPABILITY_CONTACT, ACCOUNT_CAPABILITY_ENABLED);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_STATUS_POST, ACCOUNT_CAPABILITY_ENABLED);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_CALENDAR, ACCOUNT_CAPABILITY_ENABLED);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_capability(account, -1, ACCOUNT_CAPABILITY_ENABLED);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_PHOTO, ACCOUNT_CAPABILITY_ENABLED);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_capability(account, 0, ACCOUNT_CAPABILITY_ENABLED);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_VIDEO, ACCOUNT_CAPABILITY_ENABLED);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_capability(account, -1, ACCOUNT_CAPABILITY_ENABLED);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_EMAIL, ACCOUNT_CAPABILITY_ENABLED);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_capability(account, 0, ACCOUNT_CAPABILITY_ENABLED);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_STATUS_POST, ACCOUNT_CAPABILITY_ENABLED);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_capability(account, -1, ACCOUNT_CAPABILITY_ENABLED);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_VOIP, ACCOUNT_CAPABILITY_ENABLED);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_capability(account, 2, ACCOUNT_CAPABILITY_DISABLED);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_capability(account, 0, ACCOUNT_CAPABILITY_ENABLED);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_SAMSUNG_APPS, ACCOUNT_CAPABILITY_ENABLED);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_capability(account, 3, ACCOUNT_CAPABILITY_ENABLED);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_MOBILE_TRACKER, ACCOUNT_CAPABILITY_ENABLED);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_capability(account, 5, ACCOUNT_CAPABILITY_ENABLED);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_insert_to_db(account, &account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p , account_id = %d\n", __LINE__, ret, account, account_id);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return account_id;
}

static int utc1_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_update_to_db_by_id(account, 0);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc2_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_update_to_db_by_id(account, -999);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc3_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_update_to_db_by_id(account, 555);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc4_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, NULL);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc5_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc6_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_display_name(account, NULL);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc7_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_display_name(account, "updated-tarun kumar");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}


	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc8_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_display_name(account, "updated-tarun kumar");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_domain_name(account, NULL);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_domain_name(account, NULL);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}
static int utc9_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_display_name(account, "updated-tarun kumar");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_domain_name(account, "updated-Samsung Facebook");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc10_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_display_name(account, "updated-tarun kumar");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_domain_name(account, "updated-Samsung Facebook");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_email_address(account, NULL);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc11_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_display_name(account, "updated-tarun kumar");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_domain_name(account, "updated-Samsung Facebook");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_email_address(account, "updated-urusa77@gmail.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc12_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_display_name(account, "updated-tarun kumar");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_domain_name(account, "updated-Samsung Facebook");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_email_address(account, "updated-urusa77@gmail.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_package_name(account, NULL);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc13_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_display_name(account, "updated-tarun kumar");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_domain_name(account, "updated-Samsung Facebook");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_email_address(account, "updated-urusa77@gmail.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_package_name(account, "updated-com.samsung.account");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc14_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_display_name(account, "updated-tarun kumar");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_domain_name(account, "updated-Samsung Facebook");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_email_address(account, "updated-urusa77@gmail.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_package_name(account, "updated-com.samsung.account");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_icon_path(account, NULL);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc15_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_display_name(account, "updated-tarun kumar");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_domain_name(account, "updated-Samsung Facebook");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_email_address(account, "updated-urusa77@gmail.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_package_name(account, "updated-com.samsung.account");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_icon_path(account, "updated-icon-path");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc16_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_display_name(account, "updated-tarun kumar");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_domain_name(account, "updated-Samsung Facebook");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_email_address(account, "updated-urusa77@gmail.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_package_name(account, "updated-com.samsung.account");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_icon_path(account, "updated-icon-path");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_access_token(account, NULL);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc17_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_display_name(account, "updated-tarun kumar");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_domain_name(account, "updated-Samsung Facebook");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_email_address(account, "updated-urusa77@gmail.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_package_name(account, "updated-com.samsung.account");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_icon_path(account, "updated-icon-path");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_access_token(account, "updated-ACCESSTOKEN");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc18_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_display_name(account, "updated-tarun kumar");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_domain_name(account, "updated-Samsung Facebook");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_email_address(account, "updated-urusa77@gmail.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_package_name(account, "updated-com.samsung.account");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_icon_path(account, "updated-icon-path");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_access_token(account, "updated-ACCESSTOKEN");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_auth_type( account, ACCOUNT_AUTH_TYPE_OAUTH );
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_secret( account, ACCOUNT_SECRECY_INVISIBLE);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc19_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_display_name(account, "updated-tarun kumar");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_domain_name(account, "updated-Samsung Facebook");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_email_address(account, "updated-urusa77@gmail.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_package_name(account, "updated-com.samsung.account");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_icon_path(account, "updated-icon-path");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_access_token(account, "updated-ACCESSTOKEN");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_source(account, NULL);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc20_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_display_name(account, "updated-tarun kumar");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_domain_name(account, "updated-Samsung Facebook");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_email_address(account, "updated-urusa77@gmail.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_package_name(account, "updated-com.samsung.account");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_icon_path(account, "updated-icon-path");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_access_token(account, "updated-ACCESSTOKEN");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_source(account, "updated-URUSA inc.");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc21_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_display_name(account, "updated-tarun kumar");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_domain_name(account, "updated-Samsung Facebook");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_email_address(account, "updated-urusa77@gmail.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_package_name(account, "updated-com.samsung.account");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_icon_path(account, "updated-icon-path");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_access_token(account, "updated-ACCESSTOKEN");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_source(account, "updated-URUSA inc.");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	int i;

	for(i=0;i<USER_TXT_CNT;i++)
	{
		ret = account_set_user_text(account, i, NULL);
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}

	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc22_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_display_name(account, "updated-tarun kumar");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_domain_name(account, "updated-Samsung Facebook");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_email_address(account, "updated-urusa77@gmail.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_package_name(account, "updated-com.samsung.account");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_icon_path(account, "updated-icon-path");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_access_token(account, "updated-ACCESSTOKEN");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_source(account, "updated-URUSA inc.");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	int i;
	for(i=0;i<USER_TXT_CNT;i++)
	{
		ret = account_set_user_text(account, i, "updated-user txt");
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}

	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc23_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_display_name(account, "updated-tarun kumar");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_domain_name(account, "updated-Samsung Facebook");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_email_address(account, "updated-urusa77@gmail.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_package_name(account, "updated-com.samsung.account");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_icon_path(account, "updated-icon-path");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_access_token(account, "updated-ACCESSTOKEN");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_source(account, "updated-URUSA inc.");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	int i;
	for(i=0;i<USER_TXT_CNT;i++)
	{
		ret = account_set_user_text(account, i, "updated-user txt");
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}

	}
	for(i=0;i<USER_INT_CNT;i++)
	{
		ret = account_set_user_int(account, i, -111);
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}
	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc24_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_display_name(account, "updated-tarun kumar");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_domain_name(account, "updated-Samsung Facebook");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_email_address(account, "updated-urusa77@gmail.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_package_name(account, "updated-com.samsung.account");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_icon_path(account, "updated-icon-path");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_access_token(account, "updated-ACCESSTOKEN");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_source(account, "updated-URUSA inc.");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	int i;
	for(i=0;i<USER_TXT_CNT;i++)
	{
		ret = account_set_user_text(account, i, "updated-user txt");
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}

	}
	for(i=0;i<USER_INT_CNT;i++)
	{
		ret = account_set_user_int(account, i, 0);
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}
	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc25_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_display_name(account, "updated-tarun kumar");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_domain_name(account, "updated-Samsung Facebook");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_email_address(account, "updated-urusa77@gmail.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_package_name(account, "updated-com.samsung.account");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_icon_path(account, "updated-icon-path");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_access_token(account, "updated-ACCESSTOKEN");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_source(account, "updated-URUSA inc.");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	int i;
	for(i=0;i<USER_TXT_CNT;i++)
	{
		ret = account_set_user_text(account, i, "updated-user txt");
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}

	}
	for(i=0;i<USER_INT_CNT;i++)
	{
		ret = account_set_user_int(account, i, 111);
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}
	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc26_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_display_name(account, "updated-tarun kumar");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_domain_name(account, "updated-Samsung Facebook");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_email_address(account, "updated-urusa77@gmail.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_package_name(account, "updated-com.samsung.account");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_icon_path(account, "updated-icon-path");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_access_token(account, "updated-ACCESSTOKEN");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_source(account, "updated-URUSA inc.");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	int i;
	for(i=0;i<USER_TXT_CNT;i++)
	{
		ret = account_set_user_text(account, i, "updated-user txt");
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}

	}
	for(i=0;i<USER_INT_CNT;i++)
	{
		ret = account_set_user_int(account, i, 111);
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}
	}

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc27_account_update()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	account_set_user_name(account, "updated-tarun.kr");

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_display_name(account, "updated-tarun kumar");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_domain_name(account, "updated-Samsung Facebook");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_email_address(account, "updated-urusa77@gmail.com");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_package_name(account, "updated-com.samsung.account");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_icon_path(account, "updated-icon-path");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_access_token(account, "updated-ACCESSTOKEN");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_set_source(account, "updated-URUSA inc.");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	int i;
	for(i=0;i<USER_TXT_CNT;i++)
	{
		ret = account_set_user_text(account, i, "updated-user txt");
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}

	}
	for(i=0;i<USER_INT_CNT;i++)
	{
		ret = account_set_user_int(account, i, 111);
		if(ret != ACCOUNT_ERROR_NONE)
		{
			printf("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
		}
	}
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_CONTACT, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_CALENDAR, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_PHOTO, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_VIDEO, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_EMAIL, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_STATUS_POST, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_VOIP, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_SAMSUNG_APPS, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, ACCOUNT_CAPABILITY_MOBILE_TRACKER, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, 0, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, -1, ACCOUNT_CAPABILITY_ENABLED);
	ret = account_set_capability(account, 1, ACCOUNT_CAPABILITY_ENABLED);

	ret = account_update_to_db_by_id(account, account_id);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

bool _utc1_account_get_callback(account_h handle, void* user_data)
{
	printf("_account_get_callback handle = %s , user_data = %s \n", (char *)handle, (char *)user_data);

	char **test_text = NULL;

	account_get_user_name(handle, test_text);
	printf("_account_get_callback : %s\n", *test_text);

	_account_free_text(*test_text);

	account_get_display_name(handle, test_text);
	printf("_account_get_callback : %s\n", *test_text);

	_account_free_text(*test_text);

	account_get_email_address(handle, test_text);
	printf("_account_get_callback : %s\n", *test_text);

	_account_free_text(*test_text);

	account_get_icon_path(handle, test_text);
	printf("_account_get_callback : %s\n", *test_text);

	_account_free_text(*test_text);

	account_get_source(handle, test_text);
	printf("_account_get_callback : %s\n", *test_text);

	_account_free_text(*test_text);

	account_get_package_name(handle, test_text);
	printf("_account_get_callback : %s\n", *test_text);

	_account_free_text(*test_text);

	account_get_domain_name(handle, test_text);
	printf("_account_get_callback : %s\n", *test_text);

	_account_free_text(*test_text);

	account_get_access_token(handle, test_text);
	printf("_account_get_callback : %s\n", *test_text);

	int i;

	for(i=0;i<USER_TXT_CNT;i++)
	{
		_account_free_text(*test_text);
		account_get_user_text(handle, i, test_text);
		printf("_account_get_callback : %s\n", *test_text);
	}

	int *test_int  = NULL;

	account_auth_type_e auth_type;
	account_get_auth_type(handle, &auth_type);
	printf("account_get_auth_type : %d\n", auth_type);

	account_secrecy_state_e secret;
	account_get_secret(handle, &secret);
	printf("account_get_secret : %d\n", secret);

	account_sync_state_e sync_support;
	account_get_sync_support(handle, &sync_support);
	printf("account_get_sync_support : %d\n", sync_support);

	for(i=0;i<USER_INT_CNT;i++)
	{
		account_get_user_int(handle, i, test_int);
		printf("_account_get_callback : %d\n", *test_int);
	}

	account_get_capability(handle, _capability_get_callback, NULL);

	return TRUE;
}


static int utc1_get_account_by_id()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_query_account_by_account_id(account_id, &account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	_account_get_account_info(account);

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc2_get_account_by_id()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_create(&account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_query_account_by_account_id(account_id, &account);

	_account_get_account_info(account);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_destroy(account);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc1_get_account_by_user_name()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_query_account_by_user_name(_account_get_callback, "usernamed-wy1115", NULL);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc2_get_account_by_user_name()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	char *user_data = "test user_data";

	ret = account_query_account_by_user_name(_account_get_callback, "usernamed-wy1115", (void *)user_data);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc1_get_account_by_capability()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_query_account_by_capability(_account_get_callback, ACCOUNT_CAPABILITY_PHOTO, ACCOUNT_CAPABILITY_ENABLED, NULL);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc2_get_account_by_capability()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	char *user_data = "test user_data";

	ret = account_query_account_by_capability(_account_get_callback, ACCOUNT_CAPABILITY_PHOTO, ACCOUNT_CAPABILITY_DISABLED, NULL);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc3_get_account_by_capability()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	char *user_data = "test user_data";

	ret = account_query_account_by_capability(_account_get_callback, ACCOUNT_CAPABILITY_PHOTO, -1, NULL);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc4_get_account_by_capability()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	char *user_data = "test user_data";

	ret = account_query_account_by_capability(_account_get_callback, -1, ACCOUNT_CAPABILITY_DISABLED, NULL);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc5_get_account_by_capability()
{
	int ret = - 1;

	account_h account;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	char *user_data = "test user_data";

	ret = account_query_account_by_capability(_account_get_callback, -1, -1, NULL);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret, account);
	}

	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc1_get_all_accounts()
{
	int ret = - 1;

	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_foreach_account_from_db(_account_get_callback, NULL);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, \n", __LINE__, ret);
	}

	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

static int utc2_get_all_accounts()
{
	int ret = - 1;

	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	char *user_data = "test user_data";

	ret = account_foreach_account_from_db(_account_get_callback, (void *)user_data);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, account=%p\n", __LINE__, ret);
	}

	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	return 1;
}

bool _utc_capability_get_callback(account_capability_type_e capability_type, account_capability_state_e capability_value, void* user_data)
{
	int test_type = capability_type;
	int test_value = capability_value;

	printf("_capability_get_callback test_type = : %d\n", test_type);
	printf("_capability_get_callback test_value = : %d\n", test_value);
	return TRUE;

}

static int utc1_account_get_capability_by_account_id()
{
	int ret = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_query_capability_by_account_id(_utc_capability_get_callback, account_id, NULL);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d,\n", __LINE__, ret);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, \n", __LINE__, ret);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return 1;
}

static int utc2_account_get_capability_by_account_id()
{
	int ret = -1;

	char *user_data = "test user_data";
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_query_capability_by_account_id(_utc_capability_get_callback, account_id, (void *)user_data);

	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, \n", __LINE__, ret);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return 1;
}

static int utc_account_delete_by_id()
{
	int ret = -1;

	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_delete(account_id);
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d, \n", __LINE__, ret);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return 1;
}

static int utc_account_delete_by_user_name()
{
	int ret = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_delete_from_db_by_user_name("usernamed-wy1115", "usernamed-com.samsung.account");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d \n", __LINE__, ret);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d \n", __LINE__, ret);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return 1;
}

static int utc_account_delete_by_package_name()
{
	int ret = -1;
	ret = account_connect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}

	ret = account_delete_from_db_by_package_name("usernamed-com.samsung.account");
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d\n", __LINE__, ret);
	}
	else
	{
		printf("(%d)-[Account-else loop] ret = %d\n", __LINE__, ret);
	}
	ret = account_disconnect();
	if(ret != ACCOUNT_ERROR_NONE)
	{
		printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
	}
	return 1;
}


static int utc_account_get_and_update()
{
	int ret = -1;
	account_h account;

	ret = account_connect();
	printf("%s(%d) ret=%x\n", __FUNCTION__, __LINE__, ret);

	ret = account_create(&account);
	printf("%s(%d) ret=%x\n", __FUNCTION__, __LINE__, ret);

	ret = account_query_account_by_account_id( 2, &account);
	printf("%s(%d) ret=%x\n", __FUNCTION__, __LINE__, ret);

	ret = account_set_access_token(account, "ACCESSSSSSSSSSSSSSSSSSSFASFASDGDSFFASDFASFDSDGAASDFASDGDFDFGAFASDFASFASDFASASGSGASDFASFSADDSGASGASGGASDFASDFASGSAASDFAAAG");
	printf("%s(%d) ret=%x\n", __FUNCTION__, __LINE__, ret);

	ret = account_update_to_db_by_id( account, 2);
	printf("%s(%d) ret=%x\n", __FUNCTION__, __LINE__, ret);

	ret = account_destroy(account);
	printf("%s(%d) ret=%x\n", __FUNCTION__, __LINE__, ret);

	ret = account_disconnect();
	printf("%s(%d) ret=%x\n", __FUNCTION__, __LINE__, ret);

	return 1;
}

#if 0
static int utc_notify_account_insert_update_delete()
{
	DBusMessage* msg;
	DBusConnection* conn;
   	DBusError err;

   	printf("Listening for signals\n");

   	/*initialise the errors */
   	dbus_error_init(&err);

   	/* connect to the bus and check for errors*/
   	conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
   	if  (dbus_error_is_set(&err)) {
      		fprintf(stderr, "Connection Error (%s)\n", err.message);
      		dbus_error_free(&err);
   	}
   	if (NULL == conn) {
      		exit(1);
   	}

   	/* add a rule for which messages we want to see*/
   	dbus_bus_add_match(conn, "type='signal',interface='account.signal.Type'", &err); /* see signals from the given interface*/
   	dbus_connection_flush(conn);
   	if  (dbus_error_is_set(&err)) {
      		fprintf(stderr, "Match Error (%s)\n", err.message);
      		exit(1);
   	}
   	printf("Match rule sent\n");

   	/* loop listening for signals being emmitted*/
   	while (true) {

      		/* non blocking read of the next available message*/
      		dbus_connection_read_write(conn, 0);
      		msg = dbus_connection_pop_message(conn);

      		/* loop again if we haven't read a message*/
      		if  (NULL == msg) {
         		sleep(1);
         		continue;
      		}

      		/* check if the message is a signal from the correct interface and with the correct name*/
      		if (dbus_message_is_signal(msg, ACCOUNT_DBUS_SIGNAL_INTERFACE, ACCOUNT_DBUS_NOTI_NAME_INSERT)) {
       		printf("Got Signal with name insert\n");
      		}

      		if (dbus_message_is_signal(msg, ACCOUNT_DBUS_SIGNAL_INTERFACE, ACCOUNT_DBUS_NOTI_NAME_UPDATE)) {
          		printf("Got Signal with name update\n");
      		}

		if (dbus_message_is_signal(msg, ACCOUNT_DBUS_SIGNAL_INTERFACE, ACCOUNT_DBUS_NOTI_NAME_DELETE)) {
      			printf("Got Signal with name delete\n");
      		}
      		/* free the message*/
      		dbus_message_unref(msg);
 	}
	/*close the connection*/
	dbus_connection_close(conn);
}
#endif

int main(int argc, char** argv)
{
#if 0
	utc_notify_account_insert_update_delete();
#else
	int account_id = -1;
	int cap_key = -1;
	int cap_value = -1;
	int user_resp = 0;
	char temp[128];
	memset(temp, 0x00, sizeof(temp));

	printf("\n\n**************************** Account Unit Test case Execution Suite ************************************ \n\n");
	printf("1. Press 1 for account insert test \n");
	printf("2. Press 2 for account update test \n");
	printf("3. Press 3 for account delete test \n");
	printf("4. Press 4 for query account test \n");
	scanf("%d", &user_resp);

	if (user_resp == 1) {
		printf("In Account Insert Test Mode \n");
		account_id = _account_insert_test_new();
		if(account_id > 0)
		{
			printf("(%d)-[Account] account insert test Success!! account id = %d\n", __LINE__, account_id);
		}
		utc_account_get_and_update();
	} else if (user_resp == 2) {
		printf("In Account Update Test Mode \n");
		printf("1. Press 1 to upadte account by id test \n");
		printf("2. Press 2 for update account by user name test \n");
		scanf("%d", &user_resp);
		if (user_resp == 1) {
			printf("Enter Account Id to be Updated \n");
			scanf("%d", &account_id);
			if (_account_update_test_new(account_id) == ACCOUNT_ERROR_NONE)
				printf("Account update Success \n");
			else
				printf("Account update Failed \n");
		} else if (user_resp == 2) {
			printf("Enter Account user name to be Updated \n");
			memset(temp, 0x00, sizeof(temp));
			scanf("%s", temp);
			char temp2[128];
			memset(temp2, 0x00, sizeof(temp2));
			printf("Enter Account package name to be Updated \n");
			scanf("%s", temp2);
			if (_account_update_test_new_by_user_name(temp, temp2) == ACCOUNT_ERROR_NONE)
				printf("Account update Success \n");
			else
				printf("Account update Failed \n");
		} else {
			printf("Wrong info entered \n");
			exit(0);
		}
	} else if (user_resp == 3) {
		printf("In Account Delete Test Mode \n");
		printf("1. Press 1 to delete account by id test \n");
		printf("2. Press 2 to delete account by user name test \n");
		printf("3. Press 3 to delete account by package name test \n");
		scanf("%d", &user_resp);
		if (user_resp == 1) {
			printf("Enter Account Id to be Deleted \n");
			scanf("%d", &account_id);
			int ret = account_connect();
			if(ret != ACCOUNT_ERROR_NONE)
			{
				printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
			}
			if (account_delete_from_db_by_id(account_id) == ACCOUNT_ERROR_NONE)
				printf("Account delete Success \n");
			else
				printf("Account delete Failed \n");

			ret = account_disconnect();
			if(ret != ACCOUNT_ERROR_NONE)
			{
				printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
			}
		} else if (user_resp == 2) {
			printf("Enter Account user name to be Deleted \n");
			memset(temp, 0x00, sizeof(temp));
			scanf("%s", temp);
			char temp2[128];
			memset(temp2, 0x00, sizeof(temp2));
			printf("Enter Account package name to be Deleted \n");
			scanf("%s", temp2);
			int ret = account_connect();
			if(ret != ACCOUNT_ERROR_NONE)
			{
				printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
			}
			if (account_delete_from_db_by_user_name(temp, temp2) == ACCOUNT_ERROR_NONE)
				printf("Account delete Success \n");
			else
				printf("Account delete Failed \n");
			ret = account_disconnect();
			if(ret != ACCOUNT_ERROR_NONE)
			{
				printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
			}
		} else if (user_resp == 3) {
			printf("Enter Account package name to be Deleted \n");
			memset(temp, 0x00, sizeof(temp));
			scanf("%s", temp);
			int ret = account_connect();
			if(ret != ACCOUNT_ERROR_NONE)
			{
				printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
			}
			if (account_delete_from_db_by_package_name(temp) == ACCOUNT_ERROR_NONE )
				printf("Account delete Success \n");
			else
				printf("Account delete Failed \n");
			ret = account_disconnect();
			if(ret != ACCOUNT_ERROR_NONE)
			{
				printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
			}

		} else {
			printf("Wrong info entered \n");
			exit(0);
		}

	} else if (user_resp == 4) {
		printf("In Account Query Test Mode \n");
		printf("1. Press 1 to Query account by id test \n");
		printf("2. Press 2 to Query account by user name test \n");
		printf("3. Press 3 to Query account by package name test \n");
		printf("4. Press 4 to Query account by capability test \n");
		scanf("%d", &user_resp);
		if (user_resp == 1) {
			printf("Enter Account Id to be Queried \n");
			scanf("%d", &account_id);
			account_h account;
			account_connect();
			account_create(&account);
			if (account_query_account_by_account_id(account_id, &account) == ACCOUNT_ERROR_NONE)
				printf("Account Queried Successfully \n");
			else
				printf("Account Queried failed \n");
			account_destroy(account);
			account_disconnect();
		} else if (user_resp == 2) {
			printf("Enter Account user name to be Queried \n");
			scanf("%s", temp);
			int ret = account_connect();
			if(ret != ACCOUNT_ERROR_NONE)
			{
				printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
			}
			if (account_query_account_by_user_name(_account_get_callback, temp, NULL) == ACCOUNT_ERROR_NONE)
				printf("Account Queried Successfully \n");
			else
				printf("Account Queried failed \n");

			ret = account_disconnect();
			if(ret != ACCOUNT_ERROR_NONE)
			{
				printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
			}
		} else if (user_resp == 3) {
			printf("Enter Account package name to be Queried \n");
			scanf("%s", temp);
			int ret = account_connect();
			if(ret != ACCOUNT_ERROR_NONE)
			{
				printf("(%d)-[Account] ret = %d\n", __LINE__, ret);
			}
			if (account_query_account_by_package_name(_account_get_callback, temp, NULL) == ACCOUNT_ERROR_NONE)
				printf("Account Queried Successfully \n");
			else
				printf("Account Queried failed \n");
			ret = account_disconnect();
			if(ret != ACCOUNT_ERROR_NONE)
			{
				printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
			}
		} else if (user_resp == 4) {
			printf("Enter Account capability key to be Queried \n");
			scanf("%d", &cap_key);
			printf("Enter Account capability key's value to be Queried \n");
			scanf("%d", &cap_value);
			int ret = account_connect();
			if(ret != ACCOUNT_ERROR_NONE)
			{
				printf("(%d)-[Account] ret = %d\n", __LINE__, ret);
			}
			if (account_query_account_by_capability(_account_get_callback, cap_key, cap_value, NULL) == ACCOUNT_ERROR_NONE)
				printf("Account Queried Successfully \n");
			else
				printf("Account Queried failed \n");
			ret = account_disconnect();
			if(ret != ACCOUNT_ERROR_NONE)
			{
				printf("(%d)-[Account] ret = %d, \n", __LINE__, ret);
			}
		} else {
			printf("Wrong info entered \n");
			exit(0);
		}

	} else {
			printf("Wrong info entered \n");
			exit(0);
	}

	//utc calls
#if 1
	//utc insert
	utc1_account_insert();
	utc2_account_insert();
	utc3_account_insert();
	utc4_account_insert();
	utc5_account_insert();
	utc6_account_insert();
	utc7_account_insert();
	utc8_account_insert();
	utc9_account_insert();
	utc10_account_insert();
	utc11_account_insert();
	utc12_account_insert();
	utc13_account_insert();
	utc14_account_insert();
	utc15_account_insert();
	utc16_account_insert();
	utc17_account_insert();
	utc18_account_insert();
	utc19_account_insert();
	utc20_account_insert();
	utc21_account_insert();
	utc22_account_insert();
	utc23_account_insert();
#endif

	//utc update
#if 1
	utc1_account_update();
	utc2_account_update();
	utc3_account_update();
	utc4_account_update();
	utc5_account_update();
	utc6_account_update();
	utc7_account_update();
	utc8_account_update();
	utc9_account_update();
	utc10_account_update();
	utc11_account_update();
	utc12_account_update();
	utc13_account_update();
	utc14_account_update();
	utc15_account_update();
	utc16_account_update();
	utc17_account_update();
	utc18_account_update();
	utc19_account_update();
	utc20_account_update();
	utc21_account_update();
	utc22_account_update();
	utc23_account_update();
	utc24_account_update();
	utc25_account_update();
	utc26_account_update();
	utc27_account_update();
#endif

	//utc get account by id
#if 1
	utc1_get_account_by_id();
	utc2_get_account_by_id();
#endif

	//utc get account by user name
#if 1
	utc1_get_account_by_user_name();
	utc2_get_account_by_user_name();
#endif

	//utc get account by capability
#if 1
	utc1_get_account_by_capability();
	utc2_get_account_by_capability();
	utc3_get_account_by_capability();
	utc4_get_account_by_capability();
	utc5_get_account_by_capability();
#endif

	//utc get all accounts
#if 1
	utc1_get_all_accounts();
	utc2_get_all_accounts();
#endif

	//utc get capability by account id
#if 1
	utc1_account_get_capability_by_account_id();
	utc2_account_get_capability_by_account_id();
#endif

	//utc_account_delete_by_id();
#if 1
	utc_account_delete_by_user_name();
	utc_account_delete_by_package_name();
#endif

#endif

	return 0;
}

