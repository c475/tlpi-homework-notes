/*
	Question:
	Log in as a normal, unprivileged user, create an executable file (or copy an existing
	file such as /bin/sleep ), and enable the set-user-ID permission bit on that file (chmod
	u+s). Try modifying the file (e.g., cat >> file). What happens to the file permissions
	as a result (ls –l)? Why does this happen?


	The set-user-ID bit is cleared. 
	This is a security feature so an unprivileged user can't modify a program 
	that potentially (probably) runs with elevated privileges.

	If an unprivileged user could modify a set-user-id-root program it would be trivial 
	to gain root access to the system by creating a program that exec()s/fork()s a shell, 
	or calls system(), and copying its contents to the SUID-root program.
*/
