#include "iostat.h"
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static struct io_stats *head;
static unsigned int hz;
static unsigned long long uptime;

void get_cpu_stat()
{
	FILE *fh;
	char buf[8192];
	unsigned long long cpu_user, cpu_nice, cpu_sys, cpu_idle;
	unsigned long long cpu_iowait, cpu_hardirq, cpu_softirq, cpu_steal;
	
	if(NULL != (fh = fopen("/proc/stat", "r")))
	{
		while(fgets(buf, sizeof(buf), fh))
		{
			if (!strncmp(buf, "cpu ", 4)) {
				sscanf(buf + 5, "%llu %llu %llu %llu %llu %llu %llu %llu",
						&cpu_user,
						&cpu_nice,
						&cpu_sys,
						&cpu_idle,
						&cpu_iowait,
						&cpu_hardirq,
						&cpu_softirq,
						&cpu_steal
				);
				uptime = cpu_user + cpu_nice + cpu_sys 
					+ cpu_idle + cpu_iowait + cpu_hardirq 
					+ cpu_softirq + cpu_steal;
				break;
			}
		}
		fclose(fh);
	}
};

void save_io_stat(struct io_stats *iostats)
{
	if(head->next)
	{
		iostats->next = head->next;
		head->next = iostats;
	}
	else
	{
		head->next = iostats;
	}
};

struct io_stats *read_disk_statistics()
{
	FILE *fh;
	char buf[512];
	struct io_stats *iostat = NULL;
	char dev_name[512];;
	unsigned int major, minor;
	unsigned long rd_ios, rd_merges_or_rd_sec, rd_ticks_or_wr_sec, wr_ios;
	unsigned long ios_pgr, tot_ticks, rq_ticks, wr_merges, wr_ticks;
	unsigned long long rd_sec_or_wr_ios, wr_sec;
	int i;
	
	if(NULL != (fh = fopen("/proc/diskstats", "r")))
	{
		while(fgets(buf, sizeof(buf), fh))
		{
			i = sscanf(buf, "%u %u %s %lu %lu %llu %lu %lu %lu %llu %lu %lu %lu %lu",
				&major, &minor, dev_name,
				&rd_ios, &rd_merges_or_rd_sec, &rd_sec_or_wr_ios, &rd_ticks_or_wr_sec,
				&wr_ios, &wr_merges, &wr_sec, &wr_ticks, &ios_pgr, &tot_ticks, &rq_ticks);
			if(0 != i)
			{
				iostat = malloc(sizeof(*(iostat)));
				if(i == 14)
				{
					iostat->rd_sectors = rd_sec_or_wr_ios;
					iostat->wr_sectors = wr_sec;
					iostat->rd_ios = rd_ios;
					iostat->rd_merges = rd_merges_or_rd_sec;
					iostat->rd_ticks = rd_ticks_or_wr_sec;
					iostat->wr_ios = wr_ios;
					iostat->wr_merges = wr_merges;
					iostat->wr_ticks = wr_ticks;
					iostat->ios_pgr = ios_pgr;
					iostat->tot_ticks = tot_ticks;
					iostat->rq_ticks = rq_ticks;
					strncpy(iostat->dev_name, dev_name, 512);
				}
				else if (i == 7) {
					iostat->rd_ios = rd_ios;
					iostat->rd_sectors = rd_merges_or_rd_sec;
					iostat->wr_ios = rd_sec_or_wr_ios;
					iostat->wr_sectors = rd_ticks_or_wr_sec;
				}
				save_io_stat(iostat);
			}
			else
				continue;
		}
		fclose(fh);
	}
	else
	{
		printf("can not read /proc/diskstats!\n");
		return NULL;
	}
	return iostat;
}

int do_print()
{
	struct io_stats *l;
	l = head->next;
	if(l)
	{
		while(l)
		{
			if(!l->rd_ios && !l->wr_ios) {
				l = l->next;
				continue;
			}
			write_iostats(l);
			l = l->next;
		}
	}
	else
		printf("no disk info");
	
	return 0;
}

//(((double) ((n) - (m))) / (p) * HZ)
/*
double ll_s_value(unsigned long long value1, unsigned long long value2,
			unsigned long long itv)
{
	if ((value2 < value1) && (value1 <= 0xffffffff))
		return ((double) ((value2 - value1) & 0xffffffff)) / itv * HZ;
	else
		return S_VALUE(value1, value2, itv);
}
*/
void write_iostats(struct io_stats *iostats)
{
	unsigned long long itv = uptime;
	unsigned long long rd_sec, wr_sec;
	int fctr = 1;

	rd_sec = iostats->rd_sectors;
	wr_sec = iostats->wr_sectors;
	printf("%-13s", iostats->dev_name);
	printf(" %8.2f %12.2f %12.2f %10llu %10llu\n",
		(double)(iostats->rd_ios + iostats->wr_ios) / itv * hz,
		(double)(iostats->rd_sectors) / itv * hz,
		(double)(iostats->wr_sectors) / itv * hz,
		(unsigned long long) rd_sec / fctr,
		(unsigned long long) wr_sec / fctr);
}

void get_HZ(void)
{
	long ticks;

	if ((ticks = sysconf(_SC_CLK_TCK)) == -1) {
		perror("sysconf");
	}

	hz = (unsigned int) ticks;
}

int main(int argc, char *argv[])
{
	head = malloc(sizeof(*(head)));
	get_HZ();
	//查询cpu数据
	get_cpu_stat();
	//查询disk或partitions
	if(NULL != read_disk_statistics())
		//打印
		do_print();
	
	return 0;
};

