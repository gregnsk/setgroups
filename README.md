Setgroup wrapper
===================

This program allows a user to add or remove specified groups from his current set of groups in shell
The code is based on fusuma wrapper code from [Fusuma Suid Wrapper] (https://github.com/joshuatshaffer/fusuma-suid-wrapper)

This can be used to overcome NFS limitations of 16 GIDs without any dependency on NFSv4 or manage_gid solutions

This is a draft which can be improved by adding lists of groups allowed to join to specific users, similar to sudoers file


Installation
------------

```
git clone https://github.com/gregnsk/setgroup
cd setgroup
sudo make install
```

Usage
-----

```
setgroup -group1 -group2 +group3 +group4 <program> <args>
```
This will remove group1 and group2 and add group3 and group4 to the current list of groups,
and launch specified program with provided arguments

License
-------

This program is provided as free and open source software under the terms of the [MIT License](http://opensource.org/licenses/MIT).
