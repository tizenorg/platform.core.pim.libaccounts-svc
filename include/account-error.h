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

#ifndef __ACCOUNT_ERROR_H__
#define __ACCOUNT_ERROR_H__

#include <tizen.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @addtogroup CAPI_ACCOUNT_MODULE
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

} account_error_e;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __ACCOUNT_ERROR_H__*/
