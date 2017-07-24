# The LazyS Copy-Paste Malware Project

## About
This is a PoC created for BSidesLV 2017.
It demonstrates how modern security tools can be easily bypassed using a malware which was created only by copying and pasting code.

### Prerequisites
vsftpd and apache2 on the C2 server.

## HOWTO
### C2 HTTP Setup
Put the file "abc.html" in apache's root folder. 
C2 address can be controlled at *ReflectiveDll.cpp*.

### C2 FTP Setup
Decide if you wish to allow anonymous access to the server.
If not (as I did) - replace *C2_user* and *C2_password* with the credentials of a user which has write privileges.
The user\pass should be edited at *FileOverWeb.cpp*

### Executing LazyS
Move the dll and executable to the victim's machine from the bin folder.
Execute the _Injector_, it will self-inject the payload and contact the C2 to get orders.
Note that the original implementation supports injection to other processes as well but this may cause errors in my implementation. 

## C2 Opcodes
### 301
NO OP, default setting

### 302
Pop PoC window - "LazyS is in da house"

### 303
Delete all the contents of C:, the windows equivalent of "rm -rf /"

### 304
Trigger a boot loop by setting a scheduled task to restart the machine every minute

### 305
"Ransomware": encrypt and rename all of the users files using Microsoft's recommended HOWTO encrypt a file

### 306
Get a file, the path to the file should be provided in the C2 html page following the string "ErrorLog::"

### 307
Get a list of all files present on the victim's machine.

### 308
Start the keylogger module.

### 309
Detect VM and sandbox environment. Unlike real malware I decided to troll users triggering this function ;)
"Real" malware will simply quit on positive detection as it wants to draw less attention.

### 310
Get persistency via the current user's registry.

### 311
Get a screenshot from the victim's machine.

### 312
Trigger BSoD by NtRaiseHardError, just for fun.

## Disclaimers
### Code Quality
**I am not a senior developer**, this code has known bugs and lacks documentation as it was mostly copied.
Do not expect it to be working perfectly as it is a PoC anyway.

### Criminal Usage
This tool and its abstract logic should be used only for legal and educational purposes.
Anyone adopting ir adapting it for illegale purposes is doing so at its own risk.

## Sources
### Injector
https://github.com/stephenfewer/ReflectiveDLLInjection
 
### Threading
https://stackoverflow.com/questions/25559918/c-stdthread-crashes-upon-execution
https://stackoverflow.com/questions/266168/simple-example-of-threading-in-c
 
### File Enumerator
https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
https://stackoverflow.com/questions/306533/how-do-i-get-a-list-of-files-in-a-directory-in-c
https://stackoverflow.com/questions/5889880/better-way-to-concatenate-multiple-strings-in-c
 
### Keylogger
http://www.cplusplus.com/forum/lounge/27569/
 
### Ransomware
https://msdn.microsoft.com/en-us/library/windows/desktop/aa382358(v=vs.85).aspx
https://stackoverflow.com/questions/25639874/recursively-searching-for-files-in-the-computer
http://www.cplusplus.com/reference/cstdio/remove/
 
### Wiper\destructive
https://stackoverflow.com/questions/9552443/running-a-cmd-command-in-c-program-file
https://stackoverflow.com/questions/12748786/delete-files-or-folder-recursively-on-windows-cmd
https://superuser.com/questions/173859/how-can-i-delete-all-files-subfolders-in-a-given-folder-via-the-command-prompt

### Screengrabbing
https://stackoverflow.com/questions/19495508/gdiplus-members-is-ambiguous
https://stackoverflow.com/questions/997175/how-can-i-take-a-screenshot-and-save-it-as-jpeg-on-windows
https://gist.github.com/ebonwheeler/3865787
 
### Upload
https://www.howtogeek.com/howto/windows/how-to-automate-ftp-uploads-from-the-windows-command-line/
http://naterice.com/ftp-upload-and-ftp-download-with-vbscript/
https://stackoverflow.com/questions/9119313/how-to-get-the-temp-folder-in-windows-7
https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
http://www.cplusplus.com/reference/regex/regex_search/
http://mathbits.com/MathBits/CompSci/Files/Name.htm
 
### Download
https://stackoverflow.com/questions/1011339/how-do-you-make-a-http-request-with-c
https://stackoverflow.com/questions/13482464/checking-if-word-exists-in-a-text-file-c

## Licenses
The core of this project is Stephan's Pherwer's great reflective DLL injector. According to the license he selected I am obliged to attach the following:
Licensed under a 3 clause BSD license, please see _Injector LICENSE.txt_ for further details.
Pafish however is licensed with GPL, see Pafish _LICENSE.txt_ for further details.
From my reading, it turns out that the entire project gets the GPL license (https://en.wikipedia.org/wiki/License_compatibility).
I am not a lawyer - feel free to do some reading about it by yourself.
