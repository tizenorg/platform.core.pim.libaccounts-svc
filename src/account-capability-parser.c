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
#include <account.h>
#include <glib.h>
#include <account-private.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <app.h>

#define ACCOUNT_XML_DIR "/opt/usr/share/account"
#define BUFSIZE 1024

static void _account_parse_doc(xmlDocPtr doc, xmlNodePtr cur,
							char *capability_filter, bool *supported)
{
	xmlChar *key;
	cur = cur->xmlChildrenNode;
	while ((cur != NULL) && (*supported != true)) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"capability"))) {
			key = xmlNodeGetContent(cur);
			ACCOUNT_DEBUG("Capability String is : %s\n", key);
			if (!strcmp(capability_filter, (char *)key)) {
				*supported = true;
			}
			xmlFree(key);
		}
	cur = cur->next;
	}
	return;
}

static bool _account_check_file_exists(char *f_path)
{
	FILE *file;
	if ((file = fopen(f_path, "r")) != NULL) {
		fclose(file);
		return TRUE;
	}
	return FALSE;
}

int account_get_application_capability_support(char *package_name,
								char *capability_filter, bool *supported)
{
	if (!package_name) {
		ACCOUNT_ERROR("package_name can not be NULL \n");
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	if (!capability_filter) {
		ACCOUNT_ERROR("capability_filter can not be NULL \n");
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	if (!supported) {
		ACCOUNT_ERROR("supported pointer can not be NULL \n");
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	char	xml_file_path[BUFSIZE] = {0, };
	xmlDocPtr doc = NULL;
	xmlNodePtr cur = NULL;
	*supported = false;

	ACCOUNT_MEMSET(xml_file_path, 0, sizeof(xml_file_path));
	ACCOUNT_SNPRINTF(xml_file_path, BUFSIZE, "%s/%s.account.xml",
							ACCOUNT_XML_DIR, package_name);

	ACCOUNT_INFO("xml_file_path = %s \n",xml_file_path);

	if (!_account_check_file_exists(xml_file_path)) {
		ACCOUNT_ERROR("XML file not found for %s \n", package_name);
		return ACCOUNT_ERROR_XML_FILE_NOT_FOUND;
	}

	doc = xmlParseFile(xml_file_path);
	if (doc == NULL ) {
		ACCOUNT_ERROR("Document not parsed successfully. \n");
		return ACCOUNT_ERROR_XML_PARSE_FAILED;
	}

	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		ACCOUNT_ERROR("empty document\n");
		xmlFreeDoc(doc);
		return ACCOUNT_ERROR_XML_PARSE_FAILED;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *) "account")) {
		ACCOUNT_ERROR("document of the wrong type, root node != account");
		xmlFreeDoc(doc);
		return ACCOUNT_ERROR_XML_PARSE_FAILED;
	}
	_account_parse_doc(doc, cur, capability_filter, supported);
	xmlFreeDoc(doc);

	return ACCOUNT_ERROR_NONE;
}

int account_get_application_multiple_account_support(
								char *package_name,
								bool *supported)
{
	if (!package_name) {
		ACCOUNT_ERROR("package_name can not be NULL \n");
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	if (!supported) {
		ACCOUNT_ERROR("supported pointer can not be NULL \n");
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	char	xml_file_path[BUFSIZE] = {0, };
	xmlDocPtr doc = NULL;
	xmlNodePtr cur = NULL;
	xmlChar *attr = NULL;
	*supported = false;

	ACCOUNT_MEMSET(xml_file_path, 0, sizeof(xml_file_path));
	ACCOUNT_SNPRINTF(xml_file_path, BUFSIZE, "%s/%s.account.xml",
							ACCOUNT_XML_DIR, package_name);

	ACCOUNT_VERBOSE("xml_file_path = %s \n",xml_file_path);

	if (!_account_check_file_exists(xml_file_path)) {
		ACCOUNT_ERROR("XML file not found for %s \n", package_name);
		return ACCOUNT_ERROR_XML_FILE_NOT_FOUND;
	}

	doc = xmlParseFile(xml_file_path);
	if (doc == NULL ) {
		ACCOUNT_ERROR("Document not parsed successfully. \n");
		return ACCOUNT_ERROR_XML_PARSE_FAILED;
	}

	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		ACCOUNT_ERROR("empty document\n");
		xmlFreeDoc(doc);
		return ACCOUNT_ERROR_XML_PARSE_FAILED;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *) "account")) {
		ACCOUNT_ERROR("document of the wrong type, root node != account");
		xmlFreeDoc(doc);
		return ACCOUNT_ERROR_XML_PARSE_FAILED;
	}
	attr = xmlGetProp(cur, (const xmlChar *)"multiple");
	ACCOUNT_INFO("Multiple = %s \n",(char *)attr);

	if (attr == NULL)
		*supported = false;
	else if (!strcmp("false", (char *)attr))
		*supported = false;
	else if (!strcmp("true", (char *)attr))
		*supported = true;
	else
		*supported = false;

	xmlFreeDoc(doc);

	return ACCOUNT_ERROR_NONE;
}

int account_get_application_icon_path(char *package_name,
											char **icon_path)
{
	if (!package_name) {
		ACCOUNT_ERROR("package_name can not be NULL \n");
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	if (!icon_path) {
		ACCOUNT_ERROR("icon_path pointer can not be NULL \n");
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	char	xml_file_path[BUFSIZE] = {0, };
	xmlDocPtr doc = NULL;
	xmlNodePtr cur = NULL;
	xmlChar *attr = NULL;
	*icon_path = NULL;

	ACCOUNT_MEMSET(xml_file_path, 0, sizeof(xml_file_path));
	ACCOUNT_SNPRINTF(xml_file_path, BUFSIZE, "%s/%s.account.xml",
							ACCOUNT_XML_DIR, package_name);

	ACCOUNT_INFO("xml_file_path = %s \n",xml_file_path);

	if (!_account_check_file_exists(xml_file_path)) {
		ACCOUNT_ERROR("XML file not found for %s \n", package_name);
		return ACCOUNT_ERROR_XML_FILE_NOT_FOUND;
	}

	doc = xmlParseFile(xml_file_path);
	if (doc == NULL ) {
		ACCOUNT_ERROR("Document not parsed successfully. \n");
		return ACCOUNT_ERROR_XML_PARSE_FAILED;
	}

	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		ACCOUNT_ERROR("empty document\n");
		xmlFreeDoc(doc);
		return ACCOUNT_ERROR_XML_PARSE_FAILED;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *) "account")) {
		ACCOUNT_ERROR("document of the wrong type, root node != account");
		xmlFreeDoc(doc);
		return ACCOUNT_ERROR_XML_PARSE_FAILED;
	}
	attr = xmlGetProp(cur, (const xmlChar *)"icon_path");
	ACCOUNT_DEBUG("icon_path = %s \n",(char *)attr);

	if (attr == NULL)
		*icon_path = NULL;
	else
		*icon_path = strdup((char *)attr);

	xmlFreeDoc(doc);

	return ACCOUNT_ERROR_NONE;
}

static bool __service_iter_func(service_h service, const char* pkg_name,
								void *data)
{
	if (!pkg_name) {
		ACCOUNT_ERROR("__service_iter_func: pkg name NULL \n");
		return TRUE;
	}
	GList **pkg_list = (GList **)data;
	char *pkgname = NULL;
	pkgname = strdup(pkg_name);
	*pkg_list = g_list_append(*pkg_list, pkgname);
	return TRUE;
}

int account_query_service_list_by_capability(service_list_cb callback,
									const char* capability_type,
									void *user_data)
{
	int ret = SERVICE_ERROR_NONE;
	GList *pkg_list = NULL;
	service_h service = NULL;
	GList *iter = NULL;
	char *list_item = NULL;
	char *capability_filter = NULL;
	bool supported = false;
	char *icon_path = NULL;
	bool multiple = false;
	int error_code = ACCOUNT_ERROR_NONE;

	ACCOUNT_RETURN_VAL((capability_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("CAPABILITY TYPE IS NULL"));
	ACCOUNT_RETURN_VAL((callback != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("CALL BACK IS NULL"));

	ACCOUNT_VERBOSE("account_query_service_list_by_capability START \n");
	ret = service_create(&service);
	if (ret != SERVICE_ERROR_NONE) {
		ACCOUNT_ERROR("Owner Check : service_create fail ret=%d\n", ret);
		return ret;
	} else {
		ACCOUNT_VERBOSE("service_list_by: service_create SUCCESS!!!!!! ret=%d\n", ret);
	}
	ret = service_set_operation(service, ACCOUNT_OPERATION_SIGNIN);
	if (ret != SERVICE_ERROR_NONE) {
		ACCOUNT_ERROR("service_list_by : service_set_operation fail ret=%d\n", ret);
		service_destroy(service);
		return ret;
	} else {
		ACCOUNT_VERBOSE("service_list_by : service_set_operation SUCCESS!!!!!! ret=%d\n",
							ret);
	}
	ret = service_foreach_app_matched(service, __service_iter_func, &pkg_list);
	if (ret != SERVICE_ERROR_NONE) {
		ACCOUNT_ERROR("service_list_by : service_foreach_app_matched fail ret=%d\n",
							ret);
		service_destroy(service);
		return ret;
	} else {
		ACCOUNT_VERBOSE("service_list_by : service_foreach_app_matched SUCCESS!!!!!! ret=%d\n",
							ret);
	}
	pkg_list = g_list_append(pkg_list, strdup(FACEBOOK_PKG_NAME));
	pkg_list = g_list_append(pkg_list, strdup(EMAIL_PKG_NAME));
	pkg_list = g_list_append(pkg_list, strdup(EXCHANGE_PKG_NAME));
	pkg_list = g_list_append(pkg_list, strdup(IMS_SERVICE_PKG_NAME));
	pkg_list = g_list_append(pkg_list, strdup(SAMSUNGACCOUNT_PKG_NAME));
	pkg_list = g_list_append(pkg_list, strdup(DROPBOX_PKG_NAME));

	ret = service_destroy(service);
	if (ret != SERVICE_ERROR_NONE) {
		ACCOUNT_ERROR("service_list_by : service_destroy fail ret=%d\n", ret);
		for (iter = pkg_list; iter != NULL; iter = g_list_next(iter)) {
			list_item = (char *)iter->data;
			g_free(list_item);
		}
		g_list_free(pkg_list);
		return ret;
	} else {
		ACCOUNT_VERBOSE("service_list_by : service_destroy SUCCESS!!!!!! ret=%d\n", ret);
	}

	capability_filter = strdup(capability_type);

	for (iter = pkg_list; iter != NULL; iter = g_list_next(iter)) {
		list_item = (char *)iter->data;
		error_code = account_get_application_capability_support(list_item,
														capability_filter,
														&supported);

		if ((error_code != ACCOUNT_ERROR_NONE)
			&& (error_code != ACCOUNT_ERROR_XML_FILE_NOT_FOUND)) {
			ACCOUNT_ERROR("service_list_by : account_get_application_capability_support fail error_code=%d\n", error_code);
			for (iter = pkg_list; iter != NULL; iter = g_list_next(iter)) {
				list_item = (char *)iter->data;
				g_free(list_item);
			}
			g_list_free(pkg_list);
			_ACCOUNT_FREE(capability_filter);
			return error_code;
		}

		if (supported) {
			error_code = account_get_application_icon_path(list_item, &icon_path);
			if (error_code != ACCOUNT_ERROR_NONE) {
				ACCOUNT_ERROR("service_list_by : account_get_application_capability_support fail error_code=%d\n", error_code);
				for (iter = pkg_list; iter != NULL; iter = g_list_next(iter)) {
					list_item = (char *)iter->data;
					g_free(list_item);
				}
				g_list_free(pkg_list);
				_ACCOUNT_FREE(capability_filter);
				return error_code;
			}
			error_code = account_get_application_multiple_account_support(list_item, &multiple);
			if (error_code != ACCOUNT_ERROR_NONE) {
				ACCOUNT_ERROR("service_list_by : account_get_application_capability_support fail error_code=%d\n", error_code);
				for (iter = pkg_list; iter != NULL; iter = g_list_next(iter)) {
					list_item = (char *)iter->data;
					g_free(list_item);
				}
				g_list_free(pkg_list);
				_ACCOUNT_FREE(capability_filter);
				return error_code;
			}
			callback(list_item, icon_path, multiple, user_data);
		}
	}
	/* free list */
	for (iter = pkg_list; iter != NULL; iter = g_list_next(iter)) {
		list_item = (char *)iter->data;
		g_free(list_item);
	}
	g_list_free(pkg_list);
	_ACCOUNT_FREE(capability_filter);
	return ACCOUNT_ERROR_NONE;
}
