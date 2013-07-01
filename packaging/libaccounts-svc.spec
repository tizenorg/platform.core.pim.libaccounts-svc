
Name:       libaccounts-svc
Summary:    Account DB library
Version:    0.2.52
Release:    1
Group:      TO_BE/FILLED_IN
License:    Apache-2.0
Source0:    libaccounts-svc-%{version}.tar.gz
Source1001: 	libaccounts-svc.manifest

BuildRequires:  cmake
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(db-util)
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(capi-appfw-application)
BuildRequires:  pkgconfig(libxml-2.0)
BuildRequires:  pkgconfig(pkgmgr-info)
BuildRequires:  pkgconfig(aul)
Requires(post): /sbin/ldconfig
Requires(post): /usr/bin/sqlite3
Requires(postun): /sbin/ldconfig

%description
Account DB libraryXB-Public-Package: no

%package devel
Summary:    Development files for %{name}
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}
%description devel
Development files for %{name}

%prep
%setup -q
cp %{SOURCE1001} .

%build
export CFLAGS="${CFLAGS} -fPIC -fvisibility=hidden"
%cmake .

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

rm -rf %{buildroot}%{_libdir}/accounts-svc

%post
/sbin/ldconfig
if [ ! -d /opt/dbspace ]
then
        mkdir -p /opt/dbspace
fi
if [ ! -f /opt/dbspace/.account.db ]
#rm -rf /opt/dbspace/.account.db*
then
	sqlite3 /opt/dbspace/.account.db 'PRAGMA journal_mode = PERSIST;
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
fi

mkdir -p /opt/usr/share/account
chown 5000:5000 /opt/dbspace/.account.db
chown 5000:5000 /opt/dbspace/.account.db-journal
chown 5000:5000 /opt/usr/share/account

chmod 660 /opt/dbspace/.account.db
chmod 660 /opt/dbspace/.account.db-journal
chmod 755 /opt/usr/share/account

#set message key value to NULL
vconftool set -t string db/account/msg '' -g 6514

#smack labeling
%postun -p /sbin/ldconfig

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/*.so.*
%{_bindir}/account-xml-verify

%files devel
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/*.so
%{_libdir}/pkgconfig/accounts-svc.pc
%{_includedir}/*.h
%{_bindir}/account-xml-verify
