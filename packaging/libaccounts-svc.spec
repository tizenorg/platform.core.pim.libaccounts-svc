
Name:       libaccounts-svc
Summary:    Account DB library
Version:    0.0.73
Release:    1
Group:      TO_BE/FILLED_IN
License:    TO BE FILLED IN
Source0:    libaccounts-svc-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(db-util)
BuildRequires:  pkgconfig(capi-base-common)
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
if [ ! -f /opt/dbspace/.account-svc.db ]
rm -rf /opt/dbspace/.account-svc.db*
then
        sqlite3 /opt/dbspace/.account-svc.db 'PRAGMA journal_mode = PERSIST;
        CREATE TABLE if not exists accounts (_id INTEGER PRIMARY KEY AUTOINCREMENT, email_address TEXT, user_name TEXT, display_name TEXT, icon_path TEXT,
        service_type INTEGER, source TEXT, library_name TEXT, is_default INTEGER, domain_name TEXT,
        key TEXT, secret TEXT, save_screen_name INTEGER, save_token INTEGER, save_user_name INTEGER,
        save_password INTEGER, key_values TEXT, capability TEXT,
        int_custom0 INTEGER, int_custom1 INTEGER, int_custom2 INTEGER, int_custom3 INTEGER, int_custom4 INTEGER,
        txt_custom0 TEXT, txt_custom1 TEXT, txt_custom2 TEXT, txt_custom3 TEXT, txt_custom4 TEXT);
        CREATE TABLE if not exists capabilities (_id INTEGER PRIMARY KEY AUTOINCREMENT, key TEXT, value TEXT, app_id INTEGER, account_id INTEGER, FOREIGN KEY (account_id) REFERENCES accounts(_id));
        CREATE TABLE if not exists service (_service_id INTEGER PRIMARY KEY AUTOINCREMENT, service_name TEXT, library_name TEXT, version TEXT, type TEXT, icon_path TEXT, auth_type TEXT);
        INSERT INTO service ( _service_id, service_name, library_name, version, type) values (1, "Samsung", "samsung-sso-efl", "0.0.1", "ui-gadget");
        INSERT INTO service ( _service_id, service_name, library_name, version, type) values (3, "Google", "email-setting-efl", "0.0.1", "ui-gadget");
        INSERT INTO service ( _service_id, service_name, library_name, version, type) values (4, "MSN", "email-setting-efl", "0.0.1", "ui-gadget");
        CREATE TABLE if not exists feature (_feature_id INTEGER PRIMARY KEY AUTOINCREMENT, feature_name TEXT, description TEXT, service_id INTEGER, FOREIGN KEY (service_id) REFERENCES service(_service_id));'
fi

chown 5000:5000 /opt/dbspace/.account-svc.db
chown 5000:5000 /opt/dbspace/.account-svc.db-journal

chmod 660 /opt/dbspace/.account-svc.db
chmod 660 /opt/dbspace/.account-svc.db-journal

%postun -p /sbin/ldconfig




%files
%defattr(-,root,root,-)
%{_libdir}/*.so.*

%files devel
%defattr(-,root,root,-)
%{_libdir}/*.so
%{_libdir}/pkgconfig/accounts-svc.pc
%{_includedir}/*.h
