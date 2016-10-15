#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/acct.h>
#include <limits.h>
#include "../../lib/tlpi_hdr.h"

#define TIME_BUF_SIZE 100


// comp_t value to long long (long isn't long enough)
static long long comptToLL(comp_t ct)
{
	const int EXP_SIZE = 3; // 3-bit, base 8 exponent
	const int MANTISSA_SIZE = 13; // followed by 13-bit mantissa

	const int MANTISSA_MASK = (1 << MANTISSA_SIZE) - 1; // mask for mantissa segment
	const int EXP_MASK = (1 << EXP_SIZE) - 1; // mask for exponent segment

	long long mantissa;
	long long exp;

	mantissa = ct  & MANTISSA_MASK;
	exp = (ct >> MANTISSA_SIZE) & EXP_MASK;

	return mantissa << (exp * 3);
}

// the non-standard is showing here
// doesnt look pretty, a lot of fields dont make sense
// maybe v3 is better
int main(int argc, char *argv[])
{
	int acctFile;
	struct acct ac;
	ssize_t numRead;
	char *s;
	char timeBuf[TIME_BUF_SIZE]; // 100
	struct tm *loc;
	time_t t;

	double cpuTime;
	double elapsedTime;

	if (argc != 2 || strcmp(argv[1], "--help") == 0) {
		usageErr("%s file\n", argv[0]);
	}

	acctFile = open(argv[1], O_RDONLY);
	if (acctFile == -1) {
		errExit("open");
	}

    printf("command  flags   term.  user     "
            "start time            CPU   elapsed\n");

	while ((numRead = read(acctFile, &ac, sizeof(struct acct))) > 0) {
		if (numRead != sizeof(struct acct)) {
			fatal("partial read");
		}

        printf("%-8.8s  ", ac.ac_comm);

        printf("%c", (ac.ac_flag & AFORK) ? 'F' : '-') ;
        printf("%c", (ac.ac_flag & ASU)   ? 'S' : '-') ;
        printf("%c", (ac.ac_flag & AXSIG) ? 'X' : '-') ;
        printf("%c", (ac.ac_flag & ACORE) ? 'C' : '-') ;

#ifdef __linux__
        printf(" %#6lx   ", (unsigned long) ac.ac_exitcode);
#else   /* Many other implementations provide ac_stat instead */
        printf(" %#6lx   ", (unsigned long) ac.ac_stat);
#endif

		s = userNameFromId(ac.ac_uid);
        printf("%-8.8s ", (s == NULL) ? "???" : s);

		t = ac.ac_btime;
		loc = localtime(&t);
		if (loc == NULL) {
			printf("???Unknown time???  ");
		} else {
			strftime(timeBuf, TIME_BUF_SIZE, "%Y-%m-%d %T ", loc);
			printf("%s ", timeBuf);
		}

		cpuTime = (double)(comptToLL(ac.ac_utime) + comptToLL(ac.ac_stime)) / sysconf(_SC_CLK_TCK);
		elapsedTime = (double)comptToLL(ac.ac_etime) / sysconf(_SC_CLK_TCK);

		printf("%5.2f %7.2f ",  cpuTime, elapsedTime);
		printf("\n");
	}

	if (numRead == -1) {
		errExit("read");
	}

	exit(EXIT_SUCCESS);
}
