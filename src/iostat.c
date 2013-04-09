#include "iostat.h"

struct io_stats *read_disk_statistics(char *dname)
{
	FILE fh;
	char buf[512];
	struct io_stats *iostat = NULL;
	char dev_name[512];;
	unsigned int major, minor;
	unsigned long rd_ios, rd_merges_or_rd_sec, rd_ticks_or_wr_sec, wr_ios;
	unsigned long ios_pgr, tot_ticks, rq_ticks, wr_merges, wr_ticks;
	unsigned long long rd_sec_or_wr_ios, wr_sec;
	
	if(NULL != (fh = fopen("/proc/diskstat", "r")))
	{
		while(fgets(buf, sizeof(buf), fh))
		{
			i = sscanf(buf, "%u %u %s %lu %lu %llu %lu %lu %lu %llu %lu %lu %lu %lu",
				&major, &minor, dev_name,
				&rd_ios, &rd_merges_or_rd_sec, &rd_sec_or_wr_ios, &rd_ticks_or_wr_sec,
				&wr_ios, &wr_merges, &wr_sec, &wr_ticks, &ios_pgr, &tot_ticks, &rq_ticks);
			if(0 != i)
			{
				if(!strcmp(dev_name, dname))
				{
					iostat = malloc(sizeof(*(iostat)));
					if(i == 14)
					{
						iostat.rd_sectors = rd_sec_or_wr_ios;
						iostat.wr_sectors = wr_sec;
						iostat.rd_ios = rd_ios;
						iostat.rd_merges = rd_merges_or_rd_sec;
						iostat.rd_ticks = rd_ticks_or_wr_sec;
						iostat.wr_ios = wr_ios;
						iostat.wr_merges = wr_merges;
						iostat.wr_ticks = wr_ticks;
						iostat.ios_pgr = ios_pgr;
						iostat.tot_ticks = tot_ticks;
						iostat.rq_ticks = rq_ticks;
						strncpy(iostat.dev_name, dev_name, 512);
					}
					else if (i == 7) {
						iostat.rd_ios = rd_ios;
						iostat.rd_sectors = rd_merges_or_rd_sec;
						iostat.wr_ios = rd_sec_or_wr_ios;
						iostat.wr_sectors = rd_ticks_or_wr_sec;
					}
					break;
				}
			}
			else
				continue;
		}
		fclose(fh);
	}
	else
	{
		printf("can not read /proc/diskstat!\n");
		return NULL;
	}
	return iostat;
}

int do_print(struct io_stats *iostats)
{
	cal_iostats(iostats);
		
	return 0;
}

void cal_iostats(struct io_stats *iostats)
{
	
}

int main(int argc, char *argv[])
{
	char *dname;
	struct io_stats *iostats;
	
	if(argc <= 1)
	{
		printf("no disk!\n");
		return 0;
	}
	
	dname = argv[1];
	
	//查询disk或partitions
	iostats = read_disk_statistics(dname);
	
	if(NULL != iostats)
		do_print(iostats);
	
	return 0;
};

