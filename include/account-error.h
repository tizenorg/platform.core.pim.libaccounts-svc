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

#ifndef __ACCOUNT_ERROR_H__
#define __ACCOUNT_ERROR_H__

#include <tizen.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @addtogroup CAPI_ACCOUNT_MANAGER_MODULE
 * @{
 */

/**
 * @file        account-error.h
 * @brief      ACCOUNT error definitions.
 */

//#define TIZEN_ERROR_ACCOUNT_CLASS SLP_ERROR_SOCIAL_CLASS | 0x90

/**
 *  @brief    Enumerations of error codes for ACOUNT APIs.
 */
typedef enum
{

	ACCOUNT_ERROR_NONE					= TIZEN_ERROR_NONE,                   /**< Successful */
	ACCOUNT_ERROR_OUT_OF_MEMORY				= TIZEN_ERROR_OUT_OF_MEMORY,              /**< Out of memory */
	ACCOUNT_ERROR_INVALID_PARAMETER			= TIZEN_ERROR_INVALID_PARAMETER,		/**< Invalid parameter */

	ACCOUNT_ERROR_DUPLICATED = TIZEN_ERROR_SOCIAL_CLASS | 0x01,
	ACCOUNT_ERROR_NO_DATA  = TIZEN_ERROR_SOCIAL_CLASS | 0x02,

	ACCOUNT_ERROR_RECORD_NOT_FOUND = TIZEN_ERROR_SOCIAL_CLASS | 0x03,
	ACCOUNT_ERROR_DB_FAILED = TIZEN_ERROR_SOCIAL_CLASS | 0x04,
	ACCOUNT_ERROR_DB_NOT_OPENED = TIZEN_ERROR_SOCIAL_CLASS | 0x05,
	ACCOUNT_ERROR_QUERY_SYNTAX_ERROR = TIZEN_ERROR_SOCIAL_CLASS | 0x06,
	ACCOUNT_ERROR_ITERATOR_END = TIZEN_ERROR_SOCIAL_CLASS | 0x07,
	ACCOUNT_ERROR_NOTI_FAILED = TIZEN_ERROR_SOCIAL_CLASS | 0x08,
	ACCOUNT_ERROR_PERMISSION_DENIED = TIZEN_ERROR_SOCIAL_CLASS | 0x09,
	ACCOUNT_ERROR_XML_PARSE_FAILED = TIZEN_ERROR_SOCIAL_CLASS | 0x0a,
	ACCOUNT_ERROR_XML_FILE_NOT_FOUND = TIZEN_ERROR_SOCIAL_CLASS | 0x0b,
	ACCOUNT_ERROR_EVENT_SUBSCRIPTION_FAIL = TIZEN_ERROR_SOCIAL_CLASS | 0x0c,
	ACCOUNT_ERROR_NOT_REGISTERED_PROVIDER = TIZEN_ERROR_SOCIAL_CLASS | 0x0d,
	ACCOUNT_ERROR_NOT_ALLOW_MULTIPLE = TIZEN_ERROR_SOCIAL_CLASS | 0x0e,
} account_error_e;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __ACCOUNT_ERROR_H__*/
