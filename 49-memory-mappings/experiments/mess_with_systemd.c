#include "../../lib/tlpi_hdr.h"


/*

read(5, "\21\0\0\0\0\0\0\0\1\0\0\0\307\224\0\0\350\3\0\0\0\0\0\0\0\0\0\0\0\0\0\0"..., 128) = 128
openat(AT_FDCWD, "/proc/38087/comm", O_RDONLY|O_CLOEXEC) = 67
fstat(67, {st_mode=S_IFREG|0644, st_size=0, ...}) = 0
read(67, "mess_with_syste\n", 1024)     = 16
close(67)                               = 0
read(5, 0x7ffdc14dff90, 128)            = -1 EAGAIN (Resource temporarily unavailable)
waitid(P_ALL, 0, {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=38087, si_uid=1000, si_status=0, si_utime=0, si_stime=0}, WNOHANG|WEXITED|WNOWAIT, NULL) = 0
openat(AT_FDCWD, "/proc/38087/comm", O_RDONLY|O_CLOEXEC) = 67
fstat(67, {st_mode=S_IFREG|0644, st_size=0, ...}) = 0
read(67, "mess_with_syste\n", 1024)     = 16
close(67)                               = 0
openat(AT_FDCWD, "/proc/38087/cgroup", O_RDONLY|O_CLOEXEC) = 67
fstat(67, {st_mode=S_IFREG|0444, st_size=0, ...}) = 0
read(67, "10:pids:/\n9:freezer:/\n8:blkio:/\n"..., 1024) = 183
close(67)                               = 0
kill(1044, SIG_0)                       = 0
waitid(P_PID, 38087, {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=38087, si_uid=1000, si_status=0, si_utime=0, si_stime=0}, WEXITED, NULL) = 0
waitid(P_ALL, 0, {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=38088, si_uid=1000, si_status=0, si_utime=0, si_stime=0}, WNOHANG|WEXITED|WNOWAIT, NULL) = 0
openat(AT_FDCWD, "/proc/38088/comm", O_RDONLY|O_CLOEXEC) = 67
fstat(67, {st_mode=S_IFREG|0644, st_size=0, ...}) = 0
read(67, "mess_with_syste\n", 1024)     = 16
close(67)                               = 0
openat(AT_FDCWD, "/proc/38088/cgroup", O_RDONLY|O_CLOEXEC) = 67
fstat(67, {st_mode=S_IFREG|0444, st_size=0, ...}) = 0
read(67, "10:pids:/\n9:freezer:/\n8:blkio:/\n"..., 1024) = 183
close(67)                               = 0
kill(1044, SIG_0)                       = 0
waitid(P_PID, 38088, {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=38088, si_uid=1000, si_status=0, si_utime=0, si_stime=0}, WEXITED, NULL) = 0
waitid(P_ALL, 0, {}, WNOHANG|WEXITED|WNOWAIT, NULL) = 0
epoll_pwait(4, [{EPOLLIN, {u32=4167321424, u64=94721081100112}}], 207, -1, NULL, 8) = 1
clock_gettime(CLOCK_BOOTTIME, {tv_sec=19607, tv_nsec=544948238}) = 0
read(5, "\21\0\0\0\0\0\0\0\1\0\0\0\313\224\0\0\350\3\0\0\0\0\0\0\0\0\0\0\0\0\0\0"..., 128) = 128
openat(AT_FDCWD, "/proc/38091/comm", O_RDONLY|O_CLOEXEC) = 67
fstat(67, {st_mode=S_IFREG|0644, st_size=0, ...}) = 0
read(67, "mess_with_syste\n", 1024)     = 16
close(67)                               = 0
read(5, 0x7ffdc14dff90, 128)            = -1 EAGAIN (Resource temporarily unavailable)
waitid(P_ALL, 0, {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=38090, si_uid=1000, si_status=0, si_utime=0, si_stime=0}, WNOHANG|WEXITED|WNOWAIT, NULL) = 0
openat(AT_FDCWD, "/proc/38090/comm", O_RDONLY|O_CLOEXEC) = 67
fstat(67, {st_mode=S_IFREG|0644, st_size=0, ...}) = 0
read(67, "mess_with_syste\n", 1024)     = 16
close(67)                               = 0
openat(AT_FDCWD, "/proc/38090/cgroup", O_RDONLY|O_CLOEXEC) = 67
fstat(67, {st_mode=S_IFREG|0444, st_size=0, ...}) = 0
read(67, "10:pids:/\n9:freezer:/\n8:blkio:/\n"..., 1024) = 183
close(67)                               = 0
kill(1044, SIG_0)                       = 0
waitid(P_PID, 38090, {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=38090, si_uid=1000, si_status=0, si_utime=0, si_stime=0}, WEXITED, NULL) = 0
waitid(P_ALL, 0, {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=38091, si_uid=1000, si_status=0, si_utime=0, si_stime=0}, WNOHANG|WEXITED|WNOWAIT, NULL) = 0
openat(AT_FDCWD, "/proc/38091/comm", O_RDONLY|O_CLOEXEC) = 67
fstat(67, {st_mode=S_IFREG|0644, st_size=0, ...}) = 0
read(67, "mess_with_syste\n", 1024)     = 16
close(67)                               = 0
openat(AT_FDCWD, "/proc/38091/cgroup", O_RDONLY|O_CLOEXEC) = 67
fstat(67, {st_mode=S_IFREG|0444, st_size=0, ...}) = 0
read(67, "10:pids:/\n9:freezer:/\n8:blkio:/\n"..., 1024) = 183
close(67)                               = 0
kill(1044, SIG_0)                       = 0
waitid(P_PID, 38091, {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=38091, si_uid=1000, si_status=0, si_utime=0, si_stime=0}, WEXITED, NULL) = 0
waitid(P_ALL, 0, {}, WNOHANG|WEXITED|WNOWAIT, NULL) = 0
epoll_pwait(4, [{EPOLLIN, {u32=4167815120, u64=94721081593808}}], 207, -1, NULL, 8) = 1
clock_gettime(CLOCK_BOOTTIME, {tv_sec=19625, tv_nsec=835655634}) = 0
recvmsg(14, {msg_name=NULL, msg_namelen=0, msg_iov=[{iov_base="WATCHDOG=1", iov_len=4096}], msg_iovlen=1, msg_control=[{cmsg_len=28, cmsg_level=SOL_SOCKET, cmsg_type=SCM_CREDENTIALS, cmsg_data={pid=478, uid=0, gid=0}}], msg_controllen=32, msg_flags=MSG_CMSG_CLOEXEC}, MSG_TRUNC|MSG_DONTWAIT|MSG_CMSG_CLOEXEC) = 10
openat(AT_FDCWD, "/proc/478/cgroup", O_RDONLY|O_CLOEXEC) = 67
fstat(67, {st_mode=S_IFREG|0444, st_size=0, ...}) = 0
read(67, "10:pids:/system.slice/systemd-ud"..., 1024) = 276
close(67)                               = 0
timerfd_settime(27, TFD_TIMER_ABSTIME, {it_interval={tv_sec=0, tv_nsec=0}, it_value={tv_sec=19736, tv_nsec=835443000}}, NULL) = 0
epoll_pwait(4, [{EPOLLIN, {u32=4167815120, u64=94721081593808}}], 207, -1, NULL, 8) = 1
clock_gettime(CLOCK_BOOTTIME, {tv_sec=19625, tv_nsec=836061190}) = 0
recvmsg(14, {msg_name=NULL, msg_namelen=0, msg_iov=[{iov_base="WATCHDOG=1", iov_len=4096}], msg_iovlen=1, msg_control=[{cmsg_len=28, cmsg_level=SOL_SOCKET, cmsg_type=SCM_CREDENTIALS, cmsg_data={pid=468, uid=0, gid=0}}], msg_controllen=32, msg_flags=MSG_CMSG_CLOEXEC}, MSG_TRUNC|MSG_DONTWAIT|MSG_CMSG_CLOEXEC) = 10
openat(AT_FDCWD, "/proc/468/cgroup", O_RDONLY|O_CLOEXEC) = 67
fstat(67, {st_mode=S_IFREG|0444, st_size=0, ...}) = 0
read(67, "10:pids:/system.slice/systemd-jo"..., 1024) = 288
close(67)                               = 0
timerfd_settime(27, TFD_TIMER_ABSTIME, {it_interval={tv_sec=0, tv_nsec=0}, it_value={tv_sec=19746, tv_nsec=85443000}}, NULL) = 0
epoll_pwait(4, 

*/

// strace systemd (PID 1) then make zombies to see what happens
int main(int argc, char *argv[])
{

    printf("PID: %ld\n", (long)getpid());

    switch (fork()) {
        case -1:
            errExit("fork");

        case 0:
            if (fork() == -1) {
                errExit("fork");
            } else {
                sleep(1);
            }
    }

    sleep(1000);

    exit(EXIT_SUCCESS);
}
