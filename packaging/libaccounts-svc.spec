
Name:       libaccounts-svc
Summary:    Account DB library
Version:    0.2.52
Release:    1
Group:      Social & Content/Utilities
License:    Apache-2.0
Source0:    libaccounts-svc-%{version}.tar.gz
Source1001: 	libaccounts-svc.manifest
Source1002:     accounts_DB.sh

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
BuildRequires:  pkgconfig(libtzplatform-config)
Requires: libtzplatform-config
Requires(post): /sbin/ldconfig
Requires(post): /usr/bin/sqlite3
Requires(postun): /sbin/ldconfig

%description
Account DB libraryXB-Public-Package: no

%package devel
Summary:    Development files for %{name}
Requires:   %{name} = %{version}-%{release}
%description devel
Development files for %{name}

%prep
%setup -q
cp %{SOURCE1001} %{SOURCE1002} .

%build
export CFLAGS="${CFLAGS} -fPIC -fvisibility=hidden"
%cmake .

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

install -D -m 0750 %{SOURCE1002} %{buildroot}%{_datadir}/%{name}/accounts_DB.sh

rm -rf %{buildroot}%{_libdir}/accounts-svc

%post

/sbin/ldconfig
%{_datadir}/%{name}/accounts_DB.sh


#set message key value to NULL
vconftool set -t string db/account/msg '' -g 6514

#smack labeling
%postun -p /sbin/ldconfig

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/*.so.*
%{_bindir}/account-xml-verify
%{_datadir}/%{name}/accounts_DB.sh

%files devel
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/*.so
%{_libdir}/pkgconfig/accounts-svc.pc
%{_includedir}/*.h
%{_datadir}/%{name}/accounts_DB.sh
%{_bindir}/account-xml-verify
