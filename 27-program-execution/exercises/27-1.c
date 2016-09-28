$ echo $PATH
/usr/local/bin:/usr/bin:/bin:./dir1:./dir2

$ ls -l dir1
total 8
-rw-r--r-- 1 mtk users 7860 Jun 13 11:55 xyz

$ ls -l dir2
total 28
-rwxr-xr-x 1 mtk users 27452 Jun 13 11:55 xyz

$ ./t_execlp xyz


exec fails because the file is not executable, and comes first in PATH 
(will be called before the "xyz" that is executable)
