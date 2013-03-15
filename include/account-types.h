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

#ifndef __ACCOUNT_TYPES_H__
#define __ACCOUNT_TYPES_H__

#include <db-util.h>

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @addtogroup CAPI_ACCOUNT_MANAGER_MODULE
 * @{
 */

/**
 * @file		account-types.h
 * @ingroup 	CAPI_ACCOUNT
 * @brief		This file defines common types and enums of ACCOUNT.
 */

/**
 * @brief	Enumerations for the state of capability.
 */
typedef enum
{
	ACCOUNT_CAPABILITY_STATE_INVALID = 0,
	ACCOUNT_CAPABILITY_DISABLED,
	ACCOUNT_CAPABILITY_ENABLED,
	ACCOUNT_CAPABILITY_STATE_MAX
}
account_capability_state_e;

/**
 * @brief	Enumerations for the state of account secrecy.
 */
typedef enum
{
	ACCOUNT_SECRECY_INVALID = 0,
	ACCOUNT_SECRECY_INVISIBLE,
	ACCOUNT_SECRECY_VISIBLE,
	ACCOUNT_SECRECY_MAX
}
account_secrecy_state_e;

/**
 * @brief	Enumerations for the account supported sync or not.
 * ACCOUNT_SYNC_NOT_SUPPORT: sync not supported
 * ACCOUNT_SYNC_STATUS_OFF: supproted sync but all synchronization functionalities are off
 * ACCOUNT_SYNC_STATUS_IDLE: sync support and sync status is idle
 * ACCOUNT_SYNC_STATUS_RUNNING: sync support and sync status is running
 * ACCOUNT_SUPPORTS_SYNC: NOT USED, WILL BE REMOVED. TO PREVENT BUILD ERROR.
 * ACCOUNT_NOT_SUPPORTS_SYNC: NOT USED, WILL BE REMOVED. TO PREVENT BUILD ERROR.
 */
typedef enum
{
	ACCOUNT_SYNC_INVALID = 0,
	ACCOUNT_SYNC_NOT_SUPPORT,
	ACCOUNT_SYNC_STATUS_OFF,
	ACCOUNT_SYNC_STATUS_IDLE,
	ACCOUNT_SYNC_STATUS_RUNNING,
	ACCOUNT_SUPPORTS_SYNC,
	ACCOUNT_NOT_SUPPORTS_SYNC,
	ACCOUNT_SYNC_MAX
}
account_sync_state_e;

typedef enum
{
	ACCOUNT_AUTH_TYPE_INVALID = 0,
	ACCOUNT_AUTH_TYPE_XAUTH,
	ACCOUNT_AUTH_TYPE_OAUTH,
	ACCOUNT_AUTH_TYPE_CLIENT_LOGIN,
	ACCOUNT_AUTH_TYPE_MAX
}account_auth_type_e;

/**
 * @brief Account handle.
 */
typedef struct account_s* account_h;

/**
 * @brief Account capability handle.
 */
typedef struct account_capability_s* account_capability_h;

/**
 * @brief Account type handle.
 */
typedef struct account_type_s* account_type_h;

/**
 * @brief Account handle.
 */
typedef struct label_s* label_h;

typedef struct account_subscribe_s* account_subscribe_h;

#define ACCOUNT_DATA_USERNAME					"http://tizen.org/account/data/username"
#define ACCOUNT_DATA_SERVICE_PROVIDER 				"http://tizen.org/account/data/service_provider"
#define ACCOUNT_DATA_ID						"http://tizen.org/appcontrol/data/account/id"
#define ACCOUNT_OPERATION_SIGNIN				"http://tizen.org/appcontrol/operation/account/add"
#define ACCOUNT_OPERATION_VIEW				"http://tizen.org/appcontrol/operation/account/configure"

#define ACCOUNT_SUPPORTS_CAPABILITY_CONTACT	"http://tizen.org/account/capability/contact"
#define ACCOUNT_SUPPORTS_CAPABILITY_CALENDAR	"http://tizen.org/account/capability/calendar"
#define ACCOUNT_SUPPORTS_CAPABILITY_PHOTO		"http://tizen.org/account/capability/photo"
#define ACCOUNT_SUPPORTS_CAPABILITY_VIDEO		"http://tizen.org/account/capability/video"
#define ACCOUNT_SUPPORTS_CAPABILITY_EMAIL		"http://tizen.org/account/capability/email"
#define ACCOUNT_SUPPORTS_CAPABILITY_POST		"http://tizen.org/account/capability/post"
#define ACCOUNT_SUPPORTS_CAPABILITY_VOIP		"http://tizen.org/account/capability/voip"
#define ACCOUNT_SUPPORTS_CAPABILITY_SAMSUNG_APPS		"http://tizen.org/account/capability/samsungapps"
#define ACCOUNT_SUPPORTS_CAPABILITY_TASK		"http://tizen.org/account/capability/task"
#define ACCOUNT_SUPPORTS_CAPABILITY_MOBILE_TRACKER		"http://tizen.org/account/capability/mobiletracker"
#define ACCOUNT_SUPPORTS_CAPABILITY_S_NOTE		"http://tizen.org/account/capability/snote"
#define ACCOUNT_SUPPORTS_CAPABILITY_GALLERY		"http://tizen.org/account/capability/gallery"
#define ACCOUNT_SUPPORTS_CAPABILITY_MUSIC		"http://tizen.org/account/capability/music"
#define ACCOUNT_SUPPORTS_CAPABILITY_MEMO		"http://tizen.org/account/capability/memo"
#define ACCOUNT_SUPPORTS_CAPABILITY_MESSAGE		"http://tizen.org/account/capability/message"
#define ACCOUNT_SUPPORTS_CAPABILITY_CHAT		"http://tizen.org/account/capability/chat"
#define ACCOUNT_SUPPORTS_CAPABILITY_BOOKMARK		"http://tizen.org/account/capability/bookmark"
#define ACCOUNT_SUPPORTS_CAPABILITY_TIZEN_EMAIL	"http://tizen.org/account/capability/tizen_email"

#define USER_TXT_CNT 5
#define USER_INT_CNT 5

#define ACCOUNT_NOTI_NAME_INSERT		"insert"
#define ACCOUNT_NOTI_NAME_UPDATE		"update"
#define ACCOUNT_NOTI_NAME_DELETE		"delete"
#define ACCOUNT_NOTI_NAME_SYNC_UPDATE		"sync_update"

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __ACCOUNT_TYPES_H__*/

