
Name:       libaccounts-svc
Summary:    Account DB library
Version:    0.0.87
Release:    1
Group:      TO_BE/FILLED_IN
License:    TO BE FILLED IN
Source0:    libaccounts-svc-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(db-util)
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(dbus-1)
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


%build
cmake . -DCMAKE_INSTALL_PREFIX=/usr

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

rm -rf %{buildroot}/usr/lib/accounts-svc


%post
/sbin/ldconfig
if [ ! -d /opt/dbspace ]
then
        mkdir -p /opt/dbspace
fi
if [ ! -f /opt/dbspace/.account.db ]
rm -rf /opt/dbspace/.account.db*
then
        sqlite3 /opt/dbspace/.account.db 'PRAGMA journal_mode = PERSIST;
        CREATE TABLE if not exists account (_id INTEGER PRIMARY KEY AUTOINCREMENT, user_name TEXT, email_address TEXT, display_name TEXT, icon_path TEXT,
        source TEXT, package_name TEXT, access_token TEXT, domain_name TEXT, auth_type INTEGER, secret INTEGER, sync_support INTEGER,
	txt_custom0 TEXT, txt_custom1 TEXT, txt_custom2 TEXT, txt_custom3 TEXT, txt_custom4 TEXT, 
        int_custom0 INTEGER, int_custom1 INTEGER, int_custom2 INTEGER, int_custom3 INTEGER, int_custom4 INTEGER);
        CREATE TABLE if not exists capabilitiy (_id INTEGER PRIMARY KEY AUTOINCREMENT, key INTEGER, value INTEGER, 
	package_name TEXT, user_name TEXT,  account_id INTEGER, FOREIGN KEY (account_id) REFERENCES account(_id));'
fi

chown 5000:5000 /opt/dbspace/.account.db
chown 5000:5000 /opt/dbspace/.account.db-journal

chmod 660 /opt/dbspace/.account.db
chmod 660 /opt/dbspace/.account.db-journal

#chsmack -a 'libaccounts-svc' /opt/dbspace/.account.db
#chsmack -a 'libaccounts-svc' /opt/dbspace/.account.db-journal

%postun -p /sbin/ldconfig




%files
%manifest libaccounts-svc.manifest
%defattr(-,root,root,-)
%{_libdir}/*.so.*

%files devel
%defattr(-,root,root,-)
%{_libdir}/*.so
%{_libdir}/pkgconfig/accounts-svc.pc
%{_includedir}/*.h
