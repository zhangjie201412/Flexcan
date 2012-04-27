#ifndef ANDROID_FLEXCAN_INTERFACE_H
#define ANDROID_FLEXCAN_INTERFACE_H

//head file
#include <hardware/hardware.h>

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

//define module id
#define FLEXCAN_HARDWARE_MODULE_ID	"flexcan"
#define VERSION	20120323
#define PF_CAN	29

//hardware module struct
struct flexcan_module_t
{
	struct hw_module_t common;
};

//hardware interface struct
struct flexcan_device_t
{
	struct hw_device_t common;
	int s;
	int can_id;
	int dlc;
	int *data;
	int running;
	int (*flexcan_send)(struct flexcan_device_t* flexcan_device,int *data, int id, int dlc, int extended,int rtr, int infinite, int loopcount);
	int (*flexcan_dump)(struct flexcan_device_t* flexcan_device,uint32_t id, uint32_t mask);
	int (*flexcan_stop_dump)(struct flexcan_device_t* flexcan_device);
};

__END_DECLS

#endif
