#ifndef _IOSTATDEMO_IOSTAT_H_
#define _IOSTATDEMO_IOSTAT_H_

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "c" {
#endif

struct io_stats {
	char dev_name[512];
	/* # of sectors read */
	unsigned long long rd_sectors   __attribute__ ((aligned (8)));//以8字节对齐的方式分配
	/* # of sectors written */
	unsigned long long wr_sectors   __attribute__ ((packed));//编译器取消结构在编译过程中的优化对齐,按照实际占用字节数进行对齐
	/* # of read operations issued to the device */
	unsigned long rd_ios        __attribute__ ((packed));
	/* # of read requests merged */
	unsigned long rd_merges     __attribute__ ((packed));
	/* Time of read requests in queue */
	unsigned long rd_ticks      __attribute__ ((packed));
	/* # of write operations issued to the device */
	unsigned long wr_ios        __attribute__ ((packed));
	/* # of write requests merged */
	unsigned long wr_merges     __attribute__ ((packed));
	/* Time of write requests in queue */
	unsigned long wr_ticks      __attribute__ ((packed));
	/* # of I/Os in progress */
	unsigned long ios_pgr       __attribute__ ((packed));
	/* # of ticks total (for this device) for I/O */
	unsigned long tot_ticks     __attribute__ ((packed));
	/* # of ticks requests spent in queue */
	unsigned long rq_ticks      __attribute__ ((packed));
	/* # of I/O done since last reboot */
	unsigned long dk_drive      __attribute__ ((packed));
	/* # of blocks read */
	unsigned long dk_drive_rblk __attribute__ ((packed));
	/* # of blocks written */
	unsigned long dk_drive_wblk __attribute__ ((packed));
};

struct io_stats *read_disk_statistics(char *dname);
int do_print(struct io_stats *iostats);
void cal_iostats(struct io_stats *iostats);

#ifdef __cplusplus
}
#endif

#endif