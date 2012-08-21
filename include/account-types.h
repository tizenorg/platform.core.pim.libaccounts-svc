/*
 *  account
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

#ifndef __ACCOUNT_TYPES_H__
#define __ACCOUNT_TYPES_H__

#include <db-util.h>


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @addtogroup CAPI_ACCOUNT_MODULE
 * @{
 */

/**
 * @file		account-types.h
 * @ingroup 	CAPI_ACCOUNT
 * @brief		This file defines common types and enums of ACCOUNT.
 */

/**
 * @brief	Enumerations for the result types of capability.
 */
typedef enum
{
	ACCOUNT_CAPABILITY_CONTACT = 1,
	ACCOUNT_CAPABILITY_CALENDAR,
	ACCOUNT_CAPABILITY_PHOTO,
	ACCOUNT_CAPABILITY_VIDEO,
	ACCOUNT_CAPABILITY_EMAIL,
	ACCOUNT_CAPABILITY_STATUS_POST,
	ACCOUNT_CAPABILITY_VOIP,
	ACCOUNT_CAPABILITY_SAMSUNG_APPS,
	ACCOUNT_CAPABILITY_TASK,
	ACCOUNT_CAPABILITY_MOBILE_TRACKER
} account_capability_type_e;

/**
 * @brief	Enumerations for the state of capability.
 */
typedef enum
{
	ACCOUNT_CAPABILITY_DISABLED = 0x00,
	ACCOUNT_CAPABILITY_ENABLED
}
account_capability_state_e;

/**
 * @brief	Enumerations for the state of account secrecy.
 */
typedef enum
{
	ACCOUNT_SECRECY_INVISIBLE = 0x00,
	ACCOUNT_SECRECY_VISIBLE
}
account_secrecy_state_e;

/**
 * @brief	Enumerations for the account supported sync or not.
 * ACCOUNT_NOT_SUPPORTS_SYNC: sync not support
 * ACCOUNT_SUPPORTS_SYNC_IDLE: sync support and sync status is idle
 * ACCOUNT_SUPPORTS_SYNC_ACTIVE: sync support and sync status id active
 * ACCOUNT_SUPPORTS_SYNC: NOT USED, WILL BE REMOVED. TO PREVENT BUILD ERROR.
 */
typedef enum
{
	ACCOUNT_NOT_SUPPORTS_SYNC = 0x00,
	ACCOUNT_SUPPORTS_SYNC_IDLE,
	ACCOUNT_SUPPORTS_SYNC_ACTIVE,
	ACCOUNT_SUPPORTS_SYNC
}
account_sync_state_e;

typedef enum
{
	ACCOUNT_AUTH_TYPE_XAUTH,
	ACCOUNT_AUTH_TYPE_OAUTH,
	ACCOUNT_AUTH_TYPE_CLIENT_LOGIN
}account_auth_type_e;

/**
 * @brief Account handle.
 */
typedef struct account_s* account_h;

/**
 * @brief Account capability handle.
 */
typedef struct account_capability_s* account_capability_h;

#define ACCOUNT_DATA_USERNAME	"http://tizen.org/account/data/username"
#define ACCOUNT_OPERATION_SIGNIN	"http://tizen.org/account/operation/signin"
#define ACCOUNT_OPERATION_VIEW		"http://tizen.org/account/operation/view"
#define ACCOUNT_SUPPORTS_CAPABILITY_CONTACT		"http://tizen.org/account/capability/contact"
#define ACCOUNT_SUPPORTS_CAPABILITY_CALENDER		"http://tizen.org/account/capability/calender"
#define ACCOUNT_SUPPORTS_CAPABILITY_PHOTO		"http://tizen.org/account/capability/photo"
#define ACCOUNT_SUPPORTS_CAPABILITY_VIDEO		"http://tizen.org/account/capability/video"
#define ACCOUNT_SUPPORTS_CAPABILITY_EMAIL		"http://tizen.org/account/capability/email"
#define ACCOUNT_SUPPORTS_CAPABILITY_POST		"http://tizen.org/account/capability/post"
#define ACCOUNT_SUPPORTS_CAPABILITY_VOIP		"http://tizen.org/account/capability/voip"
#define ACCOUNT_SUPPORTS_CAPABILITY_SAMSUNG_APPS		"http://tizen.org/account/capability/samsungapps"
#define ACCOUNT_SUPPORTS_CAPABILITY_TASK		"http://tizen.org/account/capability/task"
#define ACCOUNT_SUPPORTS_CAPABILITY_MOBILE_TRACKER		"http://tizen.org/account/capability/mobiletracker"

#define USER_TXT_CNT 5
#define USER_INT_CNT 5

#define ACCOUNT_DBUS_SIGNAL_INTERFACE		"account.signal.Type"
#define ACCOUNT_DBUS_NOTI_NAME_INSERT		"insert"
#define ACCOUNT_DBUS_NOTI_NAME_UPDATE		"update"
#define ACCOUNT_DBUS_NOTI_NAME_DELETE		"delete"
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __ACCOUNT_TYPES_H__*/

