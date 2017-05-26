# **Testa Shell**

---

Description
---

Testa is a shell or command language interpreter for the Linux operating
system. The name is Latin language meaning of “Shell” and it developed in Akademik
Delta Research Center Room 909.

Testa is a simply processor that executes commands. It provides most of GNU Utilities. It interprets the commands and also runs own scripts to automate common tasks. Testa may be used interactively or non-interactively. In interactive mode, theyaccept input typed from the keyboard. When executing non-interactively, Testa
executes commands read from a file.

Testa allows execution of GNU commands, both synchronously and asynchronously. As default, it executes synchronously but when user uses & as a parameter, Testa will execute asynchronously.

Testa has some built-in functions. So basic operations can be handled without
system call.


Installation
---
Testa Shell can easily and fastly compile and install by user.

    $ make clean
    $ make testa
    $ make install

Specifications
---
#### **Built-in Functions**

* *cd*

cd command changes directory.

***Usage:***

	prompt>cd documents
    
* *author*

author command displays author of Testa.

***Usage:*** 

	prompt>author
    
* **exit**

exit command is quits from Testa.

***Usage:*** 

	prompt>exit
    
#### **System Calls**

System commands can be call directly by Testa.

***e.g.*** 

	prompt>ls –alh

#### **Synchronously and Asynchronously Commands**

As default, it executes synchronously but when user uses & as a parameter,
Testa will execute asynchronously.

***e.g.(synchronously)*** 

	prompt>eclipce

***e.g.(asynchronously)*** 

	prompt>eclipce &
	prompt>author

#### **Running Script**

User can run the script by 2 ways:

1. Execute the script using Shebang at the begining of file

		#!/usr/local/bin/testa

2. Execute the script as second argument of Testa.
		
        $ ./testa script.ts

#### **History**

User can see last 20 previous commands with SIGQUIT (ctrl+\\) and these
commands save in home folder as testa_history.


Thanks
---
I would like to express my gratitude to *M.Ali Bursalı* for his contribution.
