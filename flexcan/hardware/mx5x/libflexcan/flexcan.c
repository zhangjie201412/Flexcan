#define LOG_TAG "HAL-FLEXCAN"

#include <hardware/hardware.h>
#include <hardware/flexcan.h>
#include <fcntl.h>
#include <errno.h>
#include <cutils/log.h>
#include <cutils/atomic.h>
#include <string.h>

////////////////
#include <getopt.h>
#include <libgen.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include <net/if.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <linux/can.h>
#include <linux/can/raw.h>
//////////////////

#define MODULE_NAME	"flexcan"
#define MODULE_AUTHOR	"Jay Zhang"

//open & close function
static int flexcan_device_open(const struct hw_module_t* module,const char*name,struct hw_device_t** device);
static int _flexcan_device_close(struct hw_device_t* device);

//device access interface
static int _flexcan_send(struct flexcan_device_t* flexcan_device,int *data, int id, int dlc, int extended,int rtr, int infinite, int loopcount);
static int _flexcan_dump(struct flexcan_device_t* flexcan_device,uint32_t id, uint32_t mask);

//static int _flexcan_stop_dump(struct flexcan_device_t* flexcan_device);

//module method table
static struct hw_module_methods_t flexcan_module_methods = {
	open:	flexcan_device_open
};

//module var
struct flexcan_module_t HAL_MODULE_INFO_SYM=
{
	common:	{
		tag:		HARDWARE_MODULE_TAG,
		version_major:	1,
		version_minor:	0,
		id:		FLEXCAN_HARDWARE_MODULE_ID,
		name:		MODULE_NAME,
		author:		MODULE_AUTHOR,
		methods:	&flexcan_module_methods,
	},
};

static int flexcan_device_open(const struct hw_module_t* module,const char*name,
		struct hw_device_t** device)
{
	struct flexcan_device_t* dev;
	dev = (struct flexcan_device_t*)malloc(sizeof(struct flexcan_device_t));
	if(!dev)
	{
		LOGE("[flexcan]--failed to allocate memory!\n");
		return -EFAULT;
	}
	LOGI("[flexcan]--flexcan device open!\n");
	memset(dev,0,sizeof(struct flexcan_device_t));
	dev->common.tag = HARDWARE_DEVICE_TAG;
	dev->common.version = 0;
	dev->common.module = (hw_module_t*)module;
	dev->common.close = _flexcan_device_close;
	dev->flexcan_send = _flexcan_send;
	dev->flexcan_dump = _flexcan_dump;
//	dev->flexcan_stop_dump = _flexcan_stop_dump;
	dev->running = 1;	
	//////////////////////do something init!!////////////////////
	struct ifreq ifr;
	struct sockaddr_can addr;
	char *interface = "can0";
	int family = PF_CAN, type = SOCK_RAW, proto = CAN_RAW;
	int loopcount = 1, infinite = 0;
	int s, opt, ret, i, dlc = 0, rtr = 0, extended = 0;
	int verbose = 0;

	s = socket(family, type, proto);
	dev->s = s;
	if(s < 0) {
		LOGE("[flexcan]--socket open error!!\n");
		return 0;
	}
//	LOGI("[flexcan]--socket open!!\n");


	addr.can_family = family;
	strcpy(ifr.ifr_name, interface);
	if (ioctl(s, SIOCGIFINDEX, &ifr)) {
		LOGE("[flexcan]--ioctl error!!\n");
		return 0;
	}
//	LOGI("[flexcan]--ioctl!!\n");
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		LOGE("[flexcan]--bind error!!\n");
		return 0;
	}
//	LOGI("[flexcan]--bind!!\n");
	*device=&(dev->common);
	return 0;
}

static int _flexcan_device_close(struct hw_device_t* device)
{
//	LOGI("[flexcan]--close!!\n");
	return 0;
}

static int _flexcan_send(struct flexcan_device_t* flexcan_device,int *data, int id, int dlc, int extended,int rtr, int infinite, int loopcount)
{
	int s = flexcan_device->s;
	struct can_frame frame;
	int i;
	int ret;
	for(i = 0; i < dlc; i++)
	{
		frame.data[i] = (char)data[i];
//		LOGI("[flexcan]--%d = %d .",i,data[i]);
		if(i == 8)
			break;
	}
	frame.can_id = id;
	frame.can_dlc = dlc;

	if (extended) {
		frame.can_id &= CAN_EFF_MASK;
		frame.can_id |= CAN_EFF_FLAG;
	} else {
		frame.can_id &= CAN_SFF_MASK;
	}

	if (rtr)
		frame.can_id |= CAN_RTR_FLAG;

	while (infinite || loopcount--) {
		ret = write(s, &frame, sizeof(frame));
		if (ret == -1) {
			perror("write");
			break;
		}
	}

//	close(s);
	return 0;
}

#define BUF_SIZ	(255)

static int _flexcan_dump(struct flexcan_device_t* flexcan_device,uint32_t id, uint32_t mask)
{
	struct can_frame frame;
	struct ifreq ifr;
	struct sockaddr_can addr;
	FILE *out = stdout;
	char *interface = "can0";
	char *optout = NULL;
	char *ptr;
	char buf[BUF_SIZ];
	int n = 0, err;
	int nbytes, i;
	int opt, optdaemon = 0;


	struct can_filter *filter = NULL;
	int filter_count = 0;

	int s = flexcan_device->s;

	///////filter///////
	filter = realloc(filter, sizeof(struct can_filter) * (filter_count + 1));
	if(!filter)
		return -1;

	filter[filter_count].can_id = id;
	filter[filter_count].can_mask = mask;
	filter_count++;
	//////////////////
	if (filter) {
		if (setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, filter,
			       filter_count * sizeof(struct can_filter)) != 0) {
			LOGE("[flexcan]--setsockopt!!\n");
			return 0;
		}
	}
//	int running = flexcan_device->running;
//	while(running) {
		if ((nbytes = read(s, &frame, sizeof(struct can_frame))) < 0) {
			LOGE("[flexcan]--read!!\n");
			return 1;
		} else {
#ifdef FLEX_DEBUG
			if (frame.can_id & CAN_EFF_FLAG)
				n = snprintf(buf, BUF_SIZ, "<0x%08x> ", frame.can_id & CAN_EFF_MASK);
			else
				n = snprintf(buf, BUF_SIZ, "<0x%03x> ", frame.can_id & CAN_SFF_MASK);

			n += snprintf(buf + n, BUF_SIZ - n, "[%d] ", frame.can_dlc);
			for (i = 0; i < frame.can_dlc; i++) {
				n += snprintf(buf + n, BUF_SIZ - n, "%02x ", frame.data[i]);
			}
			if (frame.can_id & CAN_RTR_FLAG)
				n += snprintf(buf + n, BUF_SIZ - n, "remote request");

			fprintf(out, "%s\n", buf);
/*
			do {
				err = fflush(out);
				if (err == -1 && errno == EPIPE) {
					err = -EPIPE;
					fclose(out);
					out = fopen(optout, "a");
					if (!out)
						return 0;
				}
			} while (err == -EPIPE);
*/
			n = 0;
#endif
			flexcan_device->can_id = frame.can_id;
			flexcan_device->dlc = frame.can_dlc;
			int data_tmp[8];
			for(i = 0; i< frame.can_dlc; i++)
				data_tmp[i] = (int)frame.data[i];
			flexcan_device->data = data_tmp;
		}
//		running = flexcan_device->running;
//	}
	//+++++++free struct memory
	free(filter);
	return 0;
}
/*
static int _flexcan_stop_dump(struct flexcan_device_t* flexcan_device)
{
	flexcan_device->running = 0;
	return 0;
}
*/
