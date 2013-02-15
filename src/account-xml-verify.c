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
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <account-types.h>
#include <account.h>

static char *_account_list_get_capablity_string_value(char *key)
{
	if (!strcmp(ACCOUNT_SUPPORTS_CAPABILITY_CONTACT, key))
		return "Contact";
	else if (!strcmp(ACCOUNT_SUPPORTS_CAPABILITY_CALENDAR, key))
		return "Calendar";
	else if (!strcmp(ACCOUNT_SUPPORTS_CAPABILITY_PHOTO, key))
		return "Photo";
	else if (!strcmp(ACCOUNT_SUPPORTS_CAPABILITY_VIDEO, key))
		return "Video";
	else if (!strcmp(ACCOUNT_SUPPORTS_CAPABILITY_EMAIL, key))
		return "Email";
	else if (!strcmp(ACCOUNT_SUPPORTS_CAPABILITY_POST, key))
		return "Post";
	else if (!strcmp(ACCOUNT_SUPPORTS_CAPABILITY_VOIP, key))
		return "VOIP";
	else if (!strcmp(ACCOUNT_SUPPORTS_CAPABILITY_SAMSUNG_APPS, key))
		return "Samsung Apps";
	else if (!strcmp(ACCOUNT_SUPPORTS_CAPABILITY_MOBILE_TRACKER, key))
		return "Mobile Tracker";
	else if (!strcmp(ACCOUNT_SUPPORTS_CAPABILITY_TASK, key))
		return "Task";
	else if (!strcmp(ACCOUNT_SUPPORTS_CAPABILITY_S_NOTE, key))
		return "S note";
	else if (!strcmp(ACCOUNT_SUPPORTS_CAPABILITY_GALLERY, key))
		return "Gallery";
	else if (!strcmp(ACCOUNT_SUPPORTS_CAPABILITY_MUSIC, key))
		return "Music";
	else if (!strcmp(ACCOUNT_SUPPORTS_CAPABILITY_MEMO, key))
		return "Memo";
	else if (!strcmp(ACCOUNT_SUPPORTS_CAPABILITY_MESSAGE, key))
		return "Message";
	else if (!strcmp(ACCOUNT_SUPPORTS_CAPABILITY_CHAT, key))
		return "Chat";
	else if (!strcmp(ACCOUNT_SUPPORTS_CAPABILITY_BOOKMARK, key))
		return "Bookmark";
	else if (!strcmp(ACCOUNT_SUPPORTS_CAPABILITY_TIZEN_EMAIL, key))
		return "Tizen Email";
	else
		return "::ERROR::Unkown:Recheck String !!!!";
}

void parseCap (xmlDocPtr doc, xmlNodePtr cur)
{
	xmlChar *key;
	cur = cur->xmlChildrenNode;
	printf("\n List of supported Capabilities is :\n");
	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"capability"))) {
			key = xmlNodeGetContent(cur);
			printf("\n :: %s :: \n", _account_list_get_capablity_string_value((char *)key));
			xmlFree(key);
		}
		cur = cur->next;
	}
	return;
}

static void parseDoc(char *docname)
{
	xmlDocPtr doc = NULL;
	xmlNodePtr cur = NULL;
	xmlChar *attr = NULL;
	xmlChar *icon_path = NULL;

	doc = xmlParseFile(docname);

	if (doc == NULL ) {
		fprintf(stderr,"::ERROR::Document not parsed successfully. \n");
		return;
	}

	cur = xmlDocGetRootElement(doc);

	if (cur == NULL) {
		fprintf(stderr,"::ERROR::empty document.\n");
		xmlFreeDoc(doc);
		return;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *) "account")) {
		fprintf(stderr,"::ERROR::document of the wrong type, root node != account.");
		xmlFreeDoc(doc);
		return;
	}
	attr = xmlGetProp(cur, (const xmlChar *)"multiple");

	if (attr == NULL)
		printf("\n multiple attribute not set, by default it will be taken as false.\n");
	else if (!strcmp("false", (char *)attr))
		printf("\n :: multiple = %s  ::\n",attr);
	else if (!strcmp("true", (char *)attr))
		printf("\n :: multiple = %s  :: \n",attr);
	else
		fprintf(stderr,"::ERROR:: Only multiple can be set as false or true. \n");

	icon_path = xmlGetProp(cur, (const xmlChar *)"icon_path");

	if (icon_path == NULL)
		printf("\n icon path is NULL.\n");
	else
		printf("\n :: icon path = %s  ::\n",icon_path);


	parseCap(doc, cur);
	xmlFreeDoc(doc);
	return;
}

int main(int argc, char **argv)
{
	char *docname = NULL;

	if (argc <= 1) {
		printf("Usage: %s xml_file_path\n", argv[0]);
		return(0);
	}
	printf("\n #############################################################################\n");
	docname = argv[1];
	parseDoc (docname);
	printf("\n #############################################################################\n");
	return (1);
}
