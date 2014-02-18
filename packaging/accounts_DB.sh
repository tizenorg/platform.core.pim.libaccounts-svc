#!/bin/sh

source /etc/tizen-platform.conf

sqlite3 ${TZ_SYS_DB}/.account.db 'PRAGMA journal_mode = PERSIST;
	CREATE TABLE if not exists label (AppId TEXT, Label TEXT, Locale TEXT);
	CREATE TABLE if not exists account_type (_id INTEGER PRIMARY KEY AUTOINCREMENT, AppId TEXT,
		ServiceProviderId TEXT, IconPath TEXT, SmallIconPath TEXT, MultipleAccountSupport INT);
	CREATE TABLE if not exists account_custom (AccountId INTEGER, AppId TEXT, Key TEXT, Value TEXT);
	CREATE TABLE if not exists account (_id INTEGER PRIMARY KEY AUTOINCREMENT, user_name TEXT, email_address TEXT, display_name TEXT, icon_path TEXT,
		source TEXT, package_name TEXT, access_token TEXT, domain_name TEXT, auth_type INTEGER, secret INTEGER, sync_support INTEGER,
		txt_custom0 TEXT, txt_custom1 TEXT, txt_custom2 TEXT, txt_custom3 TEXT, txt_custom4 TEXT,
		int_custom0 INTEGER, int_custom1 INTEGER, int_custom2 INTEGER, int_custom3 INTEGER, int_custom4 INTEGER);
	CREATE TABLE if not exists capability (_id INTEGER PRIMARY KEY AUTOINCREMENT, key TEXT, value INTEGER,
		package_name TEXT, user_name TEXT,  account_id INTEGER, FOREIGN KEY (account_id) REFERENCES account(_id));
	CREATE TABLE if not exists provider_feature (app_id TEXT, key TEXT);'

chgrp ${TZ_SYS_USER_GROUP} ${TZ_SYS_DB}/.account.db
chgrp ${TZ_SYS_USER_GROUP} ${TZ_SYS_DB}/.account.db-journal

chmod 660 ${TZ_SYS_DB}/.account.db
chmod 660 ${TZ_SYS_DB}/.account.db-journal
