/*
 * Gadget Driver for Android
 *
 * Copyright (C) 2008 Google, Inc.
 * Author: Mike Lockwood <lockwood@android.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/* #define DEBUG */
/* #define VERBOSE_DEBUG */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/utsname.h>
#include <linux/platform_device.h>

#include <linux/usb/android_composite.h>
#include <linux/usb/ch9.h>
#include <linux/usb/composite.h>
#include <linux/usb/gadget.h>

#include "gadget_chips.h"

/*
 * Kbuild is not very cooperative with respect to linking separately
 * compiled library objects into one module.  So for now we won't use
 * separate compilation ... ensuring init/exit sections work to shrink
 * the runtime footprint, and giving us at least some parts of what
 * a "gcc --combine ... part1.c part2.c part3.c ... " build would.
 */
#include "usbstring.c"
#include "config.c"
#include "epautoconf.c"
#include "composite.c"

#define LGE_SYS_USB_PID 

MODULE_AUTHOR("Mike Lockwood");
MODULE_DESCRIPTION("Android Composite USB Driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

static const char longname[] = "Gadget Android";

//20110624 yongman.kwon@lge.com [LS855] for setting serial string value by meid [START]
#ifdef CONFIG_LGE_USB_GADGET_DRIVER
char meid_buf[20] =  "0123456789ABCDEF";
#endif
//20110624 yongman.kwon@lge.com [LS855] for setting serial string value by meid [END]

#ifdef CONFIG_LGE_USB_GADGET_DRIVER
#if 0//def LGE_SYS_USB_PID
/* product id */
u16 product_id;
static ssize_t android_set_pid(struct device *dev, struct device_attribute *attr,char *buf);
static ssize_t android_get_pid(struct device *dev, struct device_attribute *attr,char *buf);

static DEVICE_ATTR(product_id, 0664, android_get_pid, android_set_pid);
#else
/* product id */
u16 product_id;

int android_set_pid(const char *val, struct kernel_param *kp);
static int android_get_pid(char *buffer, struct kernel_param *kp);
static int android_force_enum(char *val, struct kernel_param *kp);

module_param_call(product_id, android_set_pid, android_get_pid,
					&product_id, 0664);

MODULE_PARM_DESC(product_id, "USB device product id");
#endif
#endif
#ifdef CONFIG_USB_SUPPORT_LGE_ANDROID_AUTORUN
#ifdef CONFIG_USB_SUPPORT_LGE_ANDROID_AUTORUN_CGO
const u16 lg_charge_only_pid = 0xFFFF;
#define lg_charge_only_pid_string "FFFF"
#endif
static u16 autorun_user_mode;
static int android_set_usermode(const char *val, struct kernel_param *kp);
module_param_call(user_mode, android_set_usermode, NULL,
					&autorun_user_mode, 0664);
MODULE_PARM_DESC(user_mode, "USB Autorun user mode");
#endif
#ifdef CONFIG_USB_SUPPORT_LGE_ANDROID_AUTORUN
#define MAX_USB_MODE_LEN 32
static char usb_mode[MAX_USB_MODE_LEN] = "init_mode";
int android_set_usb_mode(const char *val, struct kernel_param *kp);
static int android_get_usb_mode(char *buffer, struct kernel_param *kp);
module_param_call(usb_mode, android_set_usb_mode, android_get_usb_mode,
					&usb_mode, 0664);
MODULE_PARM_DESC(usb_mode, "USB device connection mode");
#endif
#ifdef CONFIG_USB_SUPPORT_LGE_ANDROID_AUTORUN
const u16 lg_autorun_pid	= 0x6325;
#define lg_autorun_pid_string "6325"
/* 2011.05.13 jaeho.cho@lge.com generate ADB USB uevent for gingerbread*/
int adb_disable = 1;
//extern int manual_mode;
#endif
#ifdef CONFIG_LGE_USB_GADGET_NDIS_DRIVER
#ifdef CONFIG_LGE_USB_GADGET_NDIS_VZW_DRIVER
const u16 lg_ndis_pid = 0x6200;
#define lg_ndis_pid_string "6200"
#elif defined(CONFIG_LGE_USB_GADGET_NDIS_UNITED_DRIVER)
const u16 lg_ndis_pid = 0x61A1;
#define lg_ndis_pid_string "61A1"
#elif defined(CONFIG_LGE_USB_GADGET_NDIS_UNITED_DRIVER_NET_MODE)
const u16 lg_ndis_pid = 0x61FE;
#define lg_ndis_pid_string "61FE"
#elif defined(CONFIG_LGE_USB_GADGET_NDIS_UNITED_DRIVER_UMS_MODE)
const u16 lg_ndis_pid = 0x61FC;
#define lg_ndis_pid_string "61FC"
#endif
#elif defined(CONFIG_LGE_USB_GADGET_ECM_DRIVER)
const u16 lg_ecm_pid = 0x6200;
#define lg_ecm_pid_string "6200"
#elif defined(CONFIG_LGE_USB_GADGET_RNDIS_DRIVER)
const u16 lg_rndis_pid = 0x6200;
#define lg_rndis_pid_string "0x6200"
#else
#ifdef CONFIG_LGE_USB_GADGET_PLATFORM_DRIVER
const u16 lg_android_pid = 0x618E;
#define lg_android_pid_string "618E"
#else
const u16 lg_rmnet_pid = 0x61CF;
#define lg_rmnet_pid_string "61CF"
#endif
#endif
#ifdef CONFIG_LGE_USB_GADGET_DRIVER
#ifdef CONFIG_LGE_USB_GADGET_NDIS_DRIVER
#ifdef CONFIG_LGE_USB_GADGET_NDIS_VZW_DRIVER
const u16 lg_default_pid	= 0x6200;
#define lg_default_pid_string "6200"
#elif defined(CONFIG_LGE_USB_GADGET_NDIS_UNITED_DRIVER)
const u16 lg_default_pid	= 0x61A1;
#define lg_default_pid_string "61A1"
#elif defined(CONFIG_LGE_USB_GADGET_NDIS_UNITED_DRIVER_NET_MODE)
const u16 lg_default_pid	= 0x61FE;
#define lg_default_pid_string "61FE"
#elif defined(CONFIG_LGE_USB_GADGET_NDIS_UNITED_DRIVER_UMS_MODE)
const u16 lg_default_pid	= 0x61FC;
#define lg_default_pid_string "61FC"
#endif
#else
#ifdef CONFIG_LGE_USB_GADGET_PLATFORM_DRIVER
const u16 lg_default_pid	= 0x618E;
#define lg_default_pid_string "618E"
#else
const u16 lg_default_pid	= 0x61CF;
#define lg_default_pid_string "61CF"
#endif
#endif

const u16 lg_ums_pid		= 0x630F;
#define lg_ums_pid_string "630F"

#ifdef CONFIG_LGE_USB_GADGET_SUPPORT_FACTORY_USB
static int cable;
extern int get_msm_cable_type(void);
#endif
#endif

/* Default vendor and product IDs, overridden by platform data */
#define VENDOR_ID		0x1004	//0x0bb4

//20110325 yongman.kwon@lge.com [LS855] Support ndis drivers [START]
#ifdef CONFIG_LGE_USB_GADGET_NDIS_DRIVER
#define PRODUCT_ID		0x61FC	//0x0c03
#else
#define PRODUCT_ID		0x618E	//0x0c03
#endif
//20110325 yongman.kwon@lge.com [LS855] Support ndis drivers [END]

//20110325 yongman.kwon@lge.com [LS855] Support ndis drivers [START]
#ifdef CONFIG_LGE_USB_GADGET_NDIS_DRIVER
#define NDIS_PRODUCT_ID 0x61FC
#endif
//20110325 yongman.kwon@lge.com [LS855] Support ndis drivers [END]

struct android_dev {
	struct usb_composite_dev *cdev;
	struct usb_configuration *config;
	int num_products;
	struct android_usb_product *products;
	int num_functions;
	char **functions;

	int product_id;
	int version;
#ifdef CONFIG_LGE_USB_GADGET_DRIVER
	struct mutex lock;
#endif
#ifdef CONFIG_LGE_USB_GADGET_FUNC_BIND_ONLY_INIT
    unique_usb_function unique_function;
#endif
};

static struct android_dev *_android_dev;

/* string IDs are assigned dynamically */

#define STRING_MANUFACTURER_IDX		0
#define STRING_PRODUCT_IDX		1
#define STRING_SERIAL_IDX		2

#ifdef CONFIG_LGE_USB_GADGET_SUPPORT_FACTORY_USB
#include <linux/lge_extpwr_type.h>
const u16 lg_factory_pid = 0x6000;
#define LGE_FACTORY_USB_PID 	    0x6000
#define LGE_FACTORY_USB_PID_STRING  "0x6000"

static int cable_type;
extern struct android_usb_platform_data android_usb_pdata_factory;
#endif

/* String Table */
static struct usb_string strings_dev[] = {
	/* These dummy values should be overridden by platform data */
	[STRING_MANUFACTURER_IDX].s = "Android",
	[STRING_PRODUCT_IDX].s = "Android",
	[STRING_SERIAL_IDX].s = "0123456789ABCDEF",
	{  }			/* end of list */
};

static struct usb_gadget_strings stringtab_dev = {
	.language	= 0x0409,	/* en-us */
	.strings	= strings_dev,
};

static struct usb_gadget_strings *dev_strings[] = {
	&stringtab_dev,
	NULL,
};

static struct usb_device_descriptor device_desc = {
	.bLength              = sizeof(device_desc),
	.bDescriptorType      = USB_DT_DEVICE,
	.bcdUSB               = __constant_cpu_to_le16(0x0200),
	.bDeviceClass         = USB_CLASS_PER_INTERFACE,
	.idVendor             = __constant_cpu_to_le16(VENDOR_ID),
	.idProduct            = __constant_cpu_to_le16(PRODUCT_ID),
	.bcdDevice            = __constant_cpu_to_le16(0xffff),
	.bNumConfigurations   = 1,
};

static struct list_head _functions = LIST_HEAD_INIT(_functions);
static int _registered_function_count = 0;

//20110810 yongman.kwon@lge.com [LS855] force enum [START]
static int android_check_cable_type = NO_INIT_CABLE; 
extern int twl_detect_usb_irq;
extern int otg_detect_usb_irq;
char force_reset;

int android_force_reset(char *val, struct kernel_param *kp)
{
	int ret;

	//printk("%s\n", __func__);

	android_check_cable_type = get_ext_pwr_type();

	//check device reset when factory cable detect.
	if(android_check_cable_type==LT_CABLE_56K || android_check_cable_type==LT_CABLE_130K || android_check_cable_type==LT_CABLE_910K)
	{
		if((twl_detect_usb_irq==1) &&(otg_detect_usb_irq==0)){
			printk("%s : force USB enum\n", __func__);
			//	usb_composite_force_reset(dev->cdev);
			force_reset = 1;
		}
		else
			force_reset = 0;
	}
	else
		force_reset = 0;

	mutex_lock(&_android_dev->lock);
	ret = sprintf(val, "%d", force_reset);
	mutex_unlock(&_android_dev->lock);


	//printk("force_reset value : %s\n", val);
	
	return ret;
}

module_param_call(force_reset, android_force_reset, android_force_reset,
					&force_reset, 0664);

//20110810 yongman.kwon@lge.com [LS855] force enum [END]

#ifdef CONFIG_LGE_USB_GADGET_FUNC_BIND_ONLY_INIT
void android_set_device_class(u16 pid);
#endif
static void android_set_default_product(int product_id);
static struct android_usb_function *get_function(const char *name)
{
	struct android_usb_function	*f;
	list_for_each_entry(f, &_functions, list) {
		if (!strcmp(name, f->name))
			return f;
	}
	return 0;
}

static void bind_functions(struct android_dev *dev)
{
	struct android_usb_function	*f;
	char **functions = dev->functions;
	int i;

	for (i = 0; i < dev->num_functions; i++) {
		char *name = *functions++;
		f = get_function(name);
		if (f)
			f->bind_config(dev->config);
		else
			printk(KERN_ERR "function %s not found in bind_functions\n", name);
	}
}

static int android_bind_config(struct usb_configuration *c)
{
	struct android_dev *dev = _android_dev;

	printk(KERN_DEBUG "android_bind_config\n");
	dev->config = c;

	/* bind our functions if they have all registered */
	if (_registered_function_count == dev->num_functions)
		bind_functions(dev);

	return 0;
}

static int android_setup_config(struct usb_configuration *c,
		const struct usb_ctrlrequest *ctrl);

static struct usb_configuration android_config_driver = {
	.label		= "android",
	.bind		= android_bind_config,
	.setup		= android_setup_config,
	.bConfigurationValue = 1,
	.bmAttributes	= USB_CONFIG_ATT_ONE | USB_CONFIG_ATT_SELFPOWER,
	.bMaxPower	= 0xFA, /* 500ma */
};

static int android_setup_config(struct usb_configuration *c,
		const struct usb_ctrlrequest *ctrl)
{
	int i;
	int ret = -EOPNOTSUPP;

	for (i = 0; i < android_config_driver.next_interface_id; i++) {
		if (android_config_driver.interface[i]->setup) {
			ret = android_config_driver.interface[i]->setup(
				android_config_driver.interface[i], ctrl);
			if (ret >= 0)
				return ret;
		}
	}
	return ret;
}

static int product_has_function(struct android_usb_product *p,
		struct usb_function *f)
{
	char **functions = p->functions;
	int count = p->num_functions;
	const char *name = f->name;
	int i;

	for (i = 0; i < count; i++) {
		if (!strcmp(name, *functions++))
			return 1;
	}
	return 0;
}

static int product_matches_functions(struct android_usb_product *p)
{
	struct usb_function		*f;
	list_for_each_entry(f, &android_config_driver.functions, list) 
	{
		if (product_has_function(p, f) == !!f->disabled)
			return 0;
	}
	return 1;
}

#ifdef CONFIG_LGE_USB_GADGET_FUNC_BIND_ONLY_INIT
static int product_matches_unique_functions(struct android_dev *dev, struct android_usb_product *p)
{
    if(p->unique_function == dev->unique_function)
		return 1;
	else
		return 0;
}

static int get_init_product_id(struct android_dev *dev)
{
	struct android_usb_product *p = dev->products;
	int count = dev->num_products;
	int i;

    if (p) {
		for (i = 0; i < count; i++, p++) {  
			if (product_matches_unique_functions(dev,p))
			{
//				USB_DBG("dev->product_id 0x%x\n", p->product_id);
				return p->product_id;
			}
		}
    }

//	USB_DBG("dev->product_id 0x%x\n", dev->product_id);
	/* use default product ID */
	return dev->product_id;
}

#endif
static int get_product_id(struct android_dev *dev)
{
	struct android_usb_product *p = dev->products;
	int count = dev->num_products;
	int i;

#ifdef CONFIG_LGE_USB_GADGET_SUPPORT_FACTORY_USB
	if(product_id == LGE_FACTORY_USB_PID)
		return product_id;
	else if(p) {
#else
	if (p) {
#endif
		for (i = 0; i < count; i++, p++) {
			if (product_matches_functions(p))
				return p->product_id;
		}
	}
	/* use default product ID */
	return dev->product_id;
}

static int android_bind(struct usb_composite_dev *cdev)
{
	struct android_dev *dev = _android_dev;
	struct usb_gadget	*gadget = cdev->gadget;
	int			gcnum, id, product_id, ret;

	printk(KERN_INFO "android_bind\n");

	/* Allocate string descriptor numbers ... note that string
	 * contents can be overridden by the composite_dev glue.
	 */
	id = usb_string_id(cdev);
	if (id < 0)
		return id;
	strings_dev[STRING_MANUFACTURER_IDX].id = id;
	device_desc.iManufacturer = id;

	id = usb_string_id(cdev);
	if (id < 0)
		return id;
	strings_dev[STRING_PRODUCT_IDX].id = id;
	device_desc.iProduct = id;

	id = usb_string_id(cdev);
	if (id < 0)
		return id;
//20110329 yongman.kwon@lge.com [LS855] for factory pid [START]
#ifdef CONFIG_LGE_USB_GADGET_SUPPORT_FACTORY_USB	
	if (cable_type==LT_CABLE_56K || cable_type==LT_CABLE_130K || cable_type==LT_CABLE_910K)
	{
		strings_dev[STRING_SERIAL_IDX].id = 0;
		device_desc.iSerialNumber = 0;
	}
	else
	{
		strings_dev[STRING_SERIAL_IDX].id = id;
		device_desc.iSerialNumber = id;
	}
#else
	strings_dev[STRING_SERIAL_IDX].id = id;
	device_desc.iSerialNumber = id;
#endif
//20110329 yongman.kwon@lge.com [LS855] for factory pid [END]

	/* register our configuration */
	ret = usb_add_config(cdev, &android_config_driver);
	if (ret) {
		printk(KERN_ERR "usb_add_config failed\n");
		return ret;
	}

	gcnum = usb_gadget_controller_number(gadget);
	if (gcnum >= 0)
		device_desc.bcdDevice = cpu_to_le16(0x0200 + gcnum);
	else {
		/* gadget zero is so simple (for now, no altsettings) that
		 * it SHOULD NOT have problems with bulk-capable hardware.
		 * so just warn about unrcognized controllers -- don't panic.
		 *
		 * things like configuration and altsetting numbering
		 * can need hardware-specific attention though.
		 */
		pr_warning("%s: controller '%s' not recognized\n",
			longname, gadget->name);
		device_desc.bcdDevice = __constant_cpu_to_le16(0x9999);
	}

	usb_gadget_set_selfpowered(gadget);
	dev->cdev = cdev;
#ifdef CONFIG_LGE_USB_GADGET_FUNC_BIND_ONLY_INIT
	product_id = get_init_product_id(dev);
#else
	product_id = get_product_id(dev);
#endif
	device_desc.idProduct = __constant_cpu_to_le16(product_id);
	cdev->desc.idProduct = device_desc.idProduct;

//20110325 yongman.kwon@lge.com [LS855] Support ndis drivers [START]
#ifdef CONFIG_LGE_USB_GADGET_NDIS_DRIVER
	if(product_id == NDIS_PRODUCT_ID)
	{
		device_desc.bDeviceClass		 = USB_CLASS_MISC;
		device_desc.bDeviceSubClass 	 = 0x02;
		device_desc.bDeviceProtocol 	 = 0x01;
	}
	else
	{
		device_desc.bDeviceClass		 = USB_CLASS_COMM;
		device_desc.bDeviceSubClass 	 = 0x00;
		device_desc.bDeviceProtocol 	 = 0x00;
	}
#endif
//20110325 yongman.kwon@lge.com [LS855] Support ndis drivers [END]

	return 0;
}

static struct usb_composite_driver android_usb_driver = {
	.name		= "android_usb",
	.dev		= &device_desc,
	.strings	= dev_strings,
	.bind		= android_bind,
	.enable_function = android_enable_function,
};

void android_register_function(struct android_usb_function *f)
{
	struct android_dev *dev = _android_dev;

	printk(KERN_INFO "android_register_function %s\n", f->name);
	list_add_tail(&f->list, &_functions);
	_registered_function_count++;

	/* bind our functions if they have all registered
	 * and the main driver has bound.
	 */
	if (dev->config && _registered_function_count == dev->num_functions) {
		bind_functions(dev);
		android_set_default_product(dev->product_id);
	}
}

/**
 * android_set_function_mask() - enables functions based on selected pid.
 * @up: selected product id pointer
 *
 * This function enables functions related with selected product id.
 */
static void android_set_function_mask(struct android_usb_product *up)
{
	int index, found = 0;
	struct usb_function *func;
/* 2011.05.13 jaeho.cho@lge.com generate ADB USB uevent for gingerbread*/
#ifdef CONFIG_USB_SUPPORT_LGE_ANDROID_AUTORUN
    static int autorun_started = 0;
#endif
	list_for_each_entry(func, &android_config_driver.functions, list) {
		/* adb function enable/disable handled separetely */
		//yongman.kwonif (!strcmp(func->name, "adb"))
		if ((strcmp(func->name, "adb")==0)&&(up->product_id!=lg_factory_pid))
/* 2011.05.13 jaeho.cho@lge.com generate ADB USB uevent for gingerbread*/
#ifdef CONFIG_USB_SUPPORT_LGE_ANDROID_AUTORUN
        {
			if(autorun_started)
			{
                if(up->product_id == lg_ndis_pid)
                {
                    usb_function_set_enabled(func,!adb_disable);
                }
			    else
			    {
			        usb_function_set_enabled(func,0);
			    }

				continue;
			}
			else
			{
#ifdef CONFIG_LGE_USB_GADGET_LLDM_DRIVER
			    if(up->product_id != lg_ndis_pid && up->product_id != lg_factory_pid && up->product_id != lg_lldm_sdio_pid)
#else
			    if(up->product_id != lg_ndis_pid && up->product_id != lg_factory_pid)
#endif
                {
                    autorun_started = 1;
                }
			    else
			    {
			        continue;
			    }
			}
		}
#else
		continue;
#endif
		for (index = 0; index < up->num_functions; index++) {
			if (!strcmp(up->functions[index], func->name)) {
				found = 1;
				break;
			}
		}

		if (found) { /* func is part of product. */
			/* if func is disabled, enable the same. */
			if (func->disabled)
				usb_function_set_enabled(func, 1);
			found = 0;
		} else { /* func is not part if product. */
			/* if func is enabled, disable the same. */
			if (!func->disabled)
				usb_function_set_enabled(func, 0);
		}
	}
}

/**
 * android_set_defaut_product() - selects default product id and enables
 * required functions
 * @product_id: default product id
 *
 * This function selects default product id using pdata information and
 * enables functions for same.
*/
static void android_set_default_product(int pid)
{
	struct android_dev *dev = _android_dev;
	struct android_usb_product *up = dev->products;
	int index;

	for (index = 0; index < dev->num_products; index++, up++) {
		if (pid == up->product_id)
			break;
	}
	android_set_function_mask(up);
}

#if defined (CONFIG_USB_ANDROID_RNDIS) || defined (CONFIG_LGE_USB_GADGET_NDIS_DRIVER)
static void android_config_functions(struct usb_function *f, int enable)
{
	struct android_dev *dev = _android_dev;
	struct android_usb_product *up = dev->products;
	int index;
	char **functions;

	/* Searches for product id having function at first index */
	if (enable) {
		for (index = 0; index < dev->num_products; index++, up++) {
			functions = up->functions;
			if (!strcmp(*functions, f->name))
				break;
		}
		android_set_function_mask(up);
	} else
		android_set_default_product(dev->product_id);
}
#endif

int android_switch_composition(u16 pid)
{
	struct android_dev *dev = _android_dev;
	int ret = -EINVAL;

#ifdef CONFIG_LGE_USB_GADGET_FUNC_BIND_ONLY_INIT
    android_set_device_class(pid);
#endif

#ifdef CONFIG_USB_SUPPORT_LGE_ANDROID_AUTORUN_CGO
    usb_charge_only_softconnect();
#endif
#ifdef CONFIG_USB_SUPPORT_LGE_ANDROID_AUTORUN_CGO
	if (pid == lg_charge_only_pid) {
		product_id = pid;		
		device_desc.idProduct = __constant_cpu_to_le16(pid);
		if (dev->cdev)
			dev->cdev->desc.idProduct = device_desc.idProduct;
		/* If we are in charge only pid, disconnect android gadget */
		usb_gadget_disconnect(dev->cdev->gadget);
		return 0;
	}
#endif

	device_desc.idProduct = __constant_cpu_to_le16(pid);
	android_set_default_product(pid);
#ifdef CONFIG_LGE_USB_GADGET_DRIVER
	product_id = pid;
#endif

	if (dev->cdev)
		dev->cdev->desc.idProduct = device_desc.idProduct;

#ifdef CONFIG_LGE_USB_GADGET_SUPPORT_FACTORY_USB
	if(pid==LGE_FACTORY_USB_PID)
	{
		device_desc.bDeviceClass		 = USB_CLASS_COMM;
		device_desc.bDeviceSubClass 	 = 0x00;
		device_desc.bDeviceProtocol 	 = 0x00;

		strings_dev[STRING_SERIAL_IDX].id = 0;
		device_desc.iSerialNumber = 0;
		strings_dev[STRING_SERIAL_IDX].s = "\0";
	}
#endif


	if (dev->cdev)
		dev->cdev->desc.iSerialNumber = device_desc.iSerialNumber;

	usb_composite_force_reset(dev->cdev);

    return ret;
}
#ifdef CONFIG_LGE_USB_GADGET_FUNC_BIND_ONLY_INIT
void android_set_device_class(u16 pid)
{
	struct android_dev *dev = _android_dev;
    int deviceclass = -1;
#ifdef CONFIG_LGE_USB_GADGET_NDIS_DRIVER
#else
#ifdef CONFIG_LGE_USB_GADGET_PLATFORM_DRIVER
	if(pid == lg_android_pid) 
#else
    if(pid == lg_rmnet_pid) 
#endif
    {
	  deviceclass = USB_CLASS_COMM;
	  goto SetClass;
    }
#endif
#ifdef CONFIG_LGE_USB_GADGET_SUPPORT_FACTORY_USB
    if(pid == lg_factory_pid) 
    {
      deviceclass = USB_CLASS_COMM;
	  goto SetClass;
    }
#endif
#if defined(CONFIG_LGE_USB_GADGET_NDIS_DRIVER)
    if(pid == lg_ndis_pid) 
    {
  	  deviceclass = USB_CLASS_MISC;
  	  goto SetClass;
    }
#ifdef CONFIG_LGE_USB_GADGET_NDIS_VZW_DRIVER
    if(pid == lg_ums_pid) 
    {
  	  deviceclass = USB_CLASS_PER_INTERFACE;
  	  goto SetClass;
    }
#endif
#elif defined(CONFIG_LGE_USB_GADGET_ECM_DRIVER)
    if(pid == lg_ecm_pid) 
    {
  	  deviceclass = USB_CLASS_MISC;
  	  goto SetClass;
    }
#elif defined(CONFIG_LGE_USB_GADGET_RNDIS_DRIVER)
    if(pid == lg_rndis_pid) 
    {
  	  deviceclass = USB_CLASS_MISC;
  	  goto SetClass;
    }
#endif
#ifdef CONFIG_LGE_USB_GADGET_DRIVER
#ifdef CONFIG_LGE_USB_GADGET_NDIS_VZW_DRIVER
#else
    if(pid == lg_ums_pid) 
    {
  	  deviceclass = USB_CLASS_PER_INTERFACE;
  	  goto SetClass;
    }
#endif
#endif
#ifdef CONFIG_LGE_USB_GADGET_MTP_DRIVER
    if(pid == lg_mtp_pid) 
    {
  	  deviceclass = USB_CLASS_PER_INTERFACE;
  	  goto SetClass;
    }
#endif
#ifdef CONFIG_USB_SUPPORT_LGE_ANDROID_AUTORUN
    if(pid == lg_autorun_pid) 
    {
  	  deviceclass = USB_CLASS_PER_INTERFACE;
  	  goto SetClass;
    }
#endif
#ifdef CONFIG_USB_SUPPORT_LGE_ANDROID_AUTORUN_CGO
    if(pid == lg_charge_only_pid) 
    {
  	  deviceclass = USB_CLASS_PER_INTERFACE;
  	  goto SetClass;
    }
#endif

#ifdef CONFIG_LGE_USB_GADGET_LLDM_DRIVER
if(pid == lg_lldm_sdio_pid) 
    {
  deviceclass = USB_CLASS_MISC;
  goto SetClass;
    }
#endif  

SetClass:
	if(deviceclass == USB_CLASS_COMM)
	{
  		dev->cdev->desc.bDeviceClass = USB_CLASS_COMM;
		dev->cdev->desc.bDeviceSubClass      = 0x00;
		dev->cdev->desc.bDeviceProtocol      = 0x00;
	}
	else if(deviceclass == USB_CLASS_MISC)
	{
	  	dev->cdev->desc.bDeviceClass = USB_CLASS_MISC;
		dev->cdev->desc.bDeviceSubClass      = 0x02;
		dev->cdev->desc.bDeviceProtocol      = 0x01;
	}
	else if(deviceclass == USB_CLASS_PER_INTERFACE)
	{
		dev->cdev->desc.bDeviceClass = USB_CLASS_PER_INTERFACE;
		dev->cdev->desc.bDeviceSubClass      = 0x00;
		dev->cdev->desc.bDeviceProtocol      = 0x00;
	}
	else
	{
		dev->cdev->desc.bDeviceClass = USB_CLASS_PER_INTERFACE;
		dev->cdev->desc.bDeviceSubClass      = 0x00;
		dev->cdev->desc.bDeviceProtocol      = 0x00;
	}
}
#endif
void android_enable_function(struct usb_function *f, int enable)
{
	struct android_dev *dev = _android_dev;
	int disable = !enable;
	int product_id;

	if (!!f->disabled != disable) {
		usb_function_set_enabled(f, !disable);
#ifdef CONFIG_LGE_USB_GADGET_NDIS_DRIVER
	if (!strcmp(f->name, "cdc_ethernet")) {

		/* We need to specify the COMM class in the device descriptor
		 * if we are using CDC-ECM.
		 */
		dev->cdev->desc.bDeviceClass = USB_CLASS_MISC;
		dev->cdev->desc.bDeviceSubClass      = 0x02;
		dev->cdev->desc.bDeviceProtocol      = 0x01;

		android_config_functions(f, enable);
	}
#else
#ifdef CONFIG_USB_ANDROID_RNDIS
		if (!strcmp(f->name, "rndis")) {
			struct usb_function		*func;

			/* We need to specify the COMM class in the device descriptor
			 * if we are using RNDIS.
			 */
			if (enable)
#ifdef CONFIG_USB_ANDROID_RNDIS_WCEIS
				dev->cdev->desc.bDeviceClass = USB_CLASS_WIRELESS_CONTROLLER;
#else
				dev->cdev->desc.bDeviceClass = USB_CLASS_COMM;
#endif
			else
				dev->cdev->desc.bDeviceClass = USB_CLASS_PER_INTERFACE;

			/* Windows does not support other interfaces when RNDIS is enabled,
			 * so we disable UMS and MTP when RNDIS is on.
			 */
			list_for_each_entry(func, &android_config_driver.functions, list) {
				if (!strcmp(func->name, "usb_mass_storage")
					|| !strcmp(func->name, "mtp")) {
					usb_function_set_enabled(func, !enable);
				}
			}
		}
#endif
#endif
		product_id = get_product_id(dev);
		device_desc.idProduct = __constant_cpu_to_le16(product_id);
		if (dev->cdev)
			dev->cdev->desc.idProduct = device_desc.idProduct;
		usb_composite_force_reset(dev->cdev);
	}
}

#ifdef CONFIG_USB_SUPPORT_LGE_ANDROID_AUTORUN
static int android_set_usermode(const char *val, struct kernel_param *kp)
{
	int ret = 0;
	unsigned long tmp;

	ret = strict_strtoul(val, 16, &tmp);
	if (ret)
		return ret;

	autorun_user_mode = (unsigned int)tmp;
	pr_info("autorun user mode : %d\n", autorun_user_mode);

	return ret;
}

int get_autorun_user_mode(void)
{
	return autorun_user_mode;
}
EXPORT_SYMBOL(get_autorun_user_mode);
#endif
#if 0//def LGE_SYS_USB_PID
static ssize_t android_set_pid(struct device *dev, struct device_attribute *attr,char *buf)
{
	int ret = 0;
	unsigned long tmp;

#if 0//def CONFIG_LGE_USB_GADGET_NDIS_DRIVER
    return ret;
#endif

#ifdef LGE_SYS_USB_PID	
	ret = strict_strtoul(buf, 16, &tmp);	
#else
	ret = strict_strtoul(val, 16, &tmp);
#endif

	if (ret)
		goto out;

	/* We come here even before android_probe, when product id
	 * is passed via kernel command line.
	 */
	if (!_android_dev) {
		device_desc.idProduct = tmp;
		goto out;
	}

	if (device_desc.idProduct == tmp) {
		pr_info("[%s] Requested product id is same(%lx), ignore it\n", __func__, tmp);
		goto out;
	}

#ifdef CONFIG_LGE_USB_GADGET_SUPPORT_FACTORY_USB
	/* If cable is factory cable, we ignore request from user space */
	if (device_desc.idProduct == LGE_FACTORY_USB_PID) {
		pr_info("[%s] Factory USB cable is connected, ignore it\n", __func__);
		goto out;
	}
#endif

	mutex_lock(&_android_dev->lock);
	pr_info("[%s] user set product id - %lx begin\n", __func__, tmp);
	ret = android_switch_composition(tmp);
	pr_info("[%s] user set product id - %lx complete\n", __func__, tmp);
	mutex_unlock(&_android_dev->lock);
out:
	return ret;
}
static ssize_t android_get_pid(struct device *dev, struct device_attribute *attr,char *buf)
{
	int ret;
    pr_debug("[%s] get product id - %d\n", __func__, device_desc.idProduct);
	mutex_lock(&_android_dev->lock);
#ifdef LGE_SYS_USB_PID	
	ret = sprintf(buf, "%x", device_desc.idProduct);
#else
	ret = sprintf(buffer, "%x", device_desc.idProduct);
#endif
	mutex_unlock(&_android_dev->lock);
	return ret;
}

#else
int android_set_pid(const char *val, struct kernel_param *kp)
{
	int ret = 0;
	unsigned long tmp;

	ret = strict_strtoul(val, 16, &tmp);
	if (ret)
		goto out;

	/* We come here even before android_probe, when product id
	 * is passed via kernel command line.
	 */
	if (!_android_dev) {
		device_desc.idProduct = tmp;
		goto out;
	}

#ifdef CONFIG_LGE_USB_GADGET_NDIS_DRIVER
	if (device_desc.idProduct == tmp) {
		pr_info("[%s] Requested product id is same(%lx), ignore it\n", __func__, tmp);
		goto out;
	}
#else
	/* [yk.kim@lge.com] 2010-01-03, prevents from mode switching init time */
	if (device_desc.idProduct == tmp) {
		pr_info("[%s] Requested product id is same(%lx), ignore it\n", __func__, tmp);
		goto out;
	}
#endif

/* 2011.7.22 jaeho.cho@lge.com comment this routine to avoid a case that user cable is detected
as factory cable. However, it should be restored later with the condition of manual test mode NV
for the purpose of manufacturing. FIXME  */
#if 0//def CONFIG_LGE_USB_GADGET_SUPPORT_FACTORY_USB
	/* If cable is factory cable, we ignore request from user space */
	if (device_desc.idProduct == LGE_FACTORY_USB_PID && manual_mode) {
#ifdef CONFIG_LGE_DIAGTEST
        if(!get_allow_usb_switch_mode())
#endif
        {
		    pr_info("[%s] Factory USB cable is connected, ignore it\n", __func__);
		    goto out;
        }
#ifdef CONFIG_LGE_DIAGTEST
        else
        {
            factory_allow_usb_switch = 1;
        }
#endif
	}
#endif

	mutex_lock(&_android_dev->lock);
	pr_info("[%s] user set product id - %lx begin\n", __func__, tmp);
	ret = android_switch_composition(tmp);
	pr_info("[%s] user set product id - %lx complete\n", __func__, tmp);
	mutex_unlock(&_android_dev->lock);
out:
	return ret;
}
static int android_get_pid(char *buffer, struct kernel_param *kp)
{
	int ret;
    pr_debug("[%s] get product id - %d\n", __func__, device_desc.idProduct);
	mutex_lock(&_android_dev->lock);
	ret = sprintf(buffer, "%x", device_desc.idProduct);
	mutex_unlock(&_android_dev->lock);
	return ret;
}

#endif

#ifdef CONFIG_USB_SUPPORT_LGE_ANDROID_AUTORUN
int android_set_usb_mode(const char *val, struct kernel_param *kp)
{
	int ret = 0;
	memset(usb_mode, 0, MAX_USB_MODE_LEN);
	pr_info("[%s] request connection mode : [%s]\n", __func__,val);

#ifdef CONFIG_LGE_USB_GADGET_LLDM_DRIVER
	if (get_lldm_sdio_mode())
	{
		ret = android_set_pid(lg_lldm_sdio_pid_string, NULL);
		return ret;
	}			
#endif
#ifdef CONFIG_USB_SUPPORT_LGE_ANDROID_AUTORUN_CGO
	if (strstr(val, "charge_only")) {
		strcpy(usb_mode, "charge_only");
		ret = android_set_pid(lg_charge_only_pid_string, NULL);
		return ret;
	}
#endif
#ifdef CONFIG_USB_SUPPORT_LGE_ANDROID_AUTORUN_CGO
	else if (strstr(val, "mass_storage")) {
#else
	if (strstr(val, "mass_storage")) {
#endif
		strcpy(usb_mode, "mass_storage");
		ret = android_set_pid(lg_ums_pid_string, NULL);
		return ret;
	}
#ifdef CONFIG_LGE_USB_GADGET_MTP_DRIVER
	else if (strstr(val, "windows_media_sync")) {
		strcpy(usb_mode, "windows_media_sync");
		ret = android_set_pid(lg_mtp_pid_string, NULL);
		return ret;
	}
#endif
	else if (strstr(val, "internet_connection")) {
		strcpy(usb_mode, "internet_connection");
		ret = android_set_pid(lg_default_pid_string, NULL);
		return ret;
	}
	else if (strstr(val, "auto_run")) {
		strcpy(usb_mode, "auto_run");
		ret = android_set_pid(lg_autorun_pid_string, NULL);
		return ret;
	}
	else {
		pr_info("[%s] undefined connection mode, ignore it : [%s]\n", __func__,val);
		return -EINVAL;
	}
}

static int android_get_usb_mode(char *buffer, struct kernel_param *kp)
{
	int ret;
	pr_info("[%s][%s] get usb connection mode\n", __func__, usb_mode);
	mutex_lock(&_android_dev->lock);
	ret = sprintf(buffer, "%s", usb_mode);
	mutex_unlock(&_android_dev->lock);
	return ret;
}
#endif
#ifdef CONFIG_LGE_USB_GADGET_DRIVER
u16 android_get_product_id(void)
{
	if(device_desc.idProduct != 0x0000 && device_desc.idProduct != 0x0001)
	{
		pr_info("LG_FW : 0x%x\n", device_desc.idProduct);
		return device_desc.idProduct;
	}
	else
	{
		pr_info("LG_FW : product_id is not initialized : device_desc.idProduct = 0x%x\n", device_desc.idProduct);
		return lg_default_pid;
	}
}
#endif

static int android_probe(struct platform_device *pdev)
{
	struct android_usb_platform_data *pdata = pdev->dev.platform_data;
	struct android_dev *dev = _android_dev;

#ifdef LGE_SYS_USB_PID
	int ret;
#endif
	printk(KERN_INFO "android_probe pdata: %p\n", pdata);

//20110329 yongman.kwon@lge.com [LS855] for factory pid [START]
#ifdef CONFIG_LGE_USB_GADGET_SUPPORT_FACTORY_USB
	cable_type = get_ext_pwr_type();//LG_NORMAL_USB_CABLE;
	
	if (cable_type==LT_CABLE_56K || cable_type==LT_CABLE_130K || cable_type==LT_CABLE_910K)
	{
		pdev->dev.platform_data = &android_usb_pdata_factory;
		pdata = pdev->dev.platform_data;
	}
#endif
//20110329 yongman.kwon@lge.com [LS855] for factory pid [END]

	if (pdata) {
		dev->products = pdata->products;
		dev->num_products = pdata->num_products;
		dev->functions = pdata->functions;
		dev->num_functions = pdata->num_functions;
#ifdef CONFIG_LGE_USB_GADGET_FUNC_BIND_ONLY_INIT
        dev->unique_function = pdata->unique_function;
#endif
		if (pdata->vendor_id)
			device_desc.idVendor =
				__constant_cpu_to_le16(pdata->vendor_id);
		if (pdata->product_id) {
			dev->product_id = pdata->product_id;
			device_desc.idProduct =
				__constant_cpu_to_le16(pdata->product_id);

#ifdef CONFIG_LGE_USB_GADGET_DRIVER
			product_id = pdata->product_id;
#endif			
		}
		if (pdata->version)
			dev->version = pdata->version;

		if (pdata->product_name)
			strings_dev[STRING_PRODUCT_IDX].s = pdata->product_name;
		if (pdata->manufacturer_name)
			strings_dev[STRING_MANUFACTURER_IDX].s =
					pdata->manufacturer_name;
//20110329 yongman.kwon@lge.com [LS855] for factory pid [START]
#ifdef CONFIG_LGE_USB_GADGET_SUPPORT_FACTORY_USB
		if (cable_type==LT_CABLE_56K || cable_type==LT_CABLE_130K || cable_type==LT_CABLE_910K)
		{
			strings_dev[STRING_SERIAL_IDX].s = "\0";
		}
		else
		{
#ifdef CONFIG_LGE_USB_GADGET_DRIVER		
			//meid not active or no data about meid in eMMC.
			//printk("meid_buf[0] : %d\n", meid_buf[0]);
			//printk("meid_buf[1] : %d\n", meid_buf[1]);

			if((!strcmp(meid_buf,"00000000000000"))||(meid_buf[0]==0xFF))
			{
			//	printk("set serial number by default\n");
				if (pdata->serial_number)
					strings_dev[STRING_SERIAL_IDX].s = pdata->serial_number;	
			}
			else
			{
			//	printk("set serial number by meid\n");
				strings_dev[STRING_SERIAL_IDX].s = meid_buf;					
			}
#else
			if (pdata->serial_number)
				strings_dev[STRING_SERIAL_IDX].s = pdata->serial_number;
#endif			
		}
#else		
		if (pdata->serial_number)	
			strings_dev[STRING_SERIAL_IDX].s = pdata->serial_number;
#endif	
//20110329 yongman.kwon@lge.com [LS855] for factory pid [END]

	}

#if 0//def LGE_SYS_USB_PID
	ret = device_create_file(&pdev->dev, &dev_attr_product_id);

	if(ret < 0)
		return -1;
#endif

	return usb_composite_register(&android_usb_driver);
}

static struct platform_driver android_platform_driver = {
	.driver = { .name = "android_usb", },
	.probe = android_probe,
};

//20110624 yongman.kwon@lge.com [LS855] for setting serial string value by meid [START]s
#ifdef CONFIG_LGE_USB_GADGET_DRIVER
static s32 __init uboot_read_meid(char *str)
{
	sscanf(str, "%s", meid_buf);
	printk(KERN_INFO "%s: meid value %s\n", __func__, meid_buf);
	return 0;
}
__setup("meid=", uboot_read_meid);
#endif
//20110624 yongman.kwon@lge.com [LS855] for setting serial string value by meid [END]

static int __init init(void)
{
	struct android_dev *dev;

	printk(KERN_INFO "android init\n");

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return -ENOMEM;

	/* set default values, which should be overridden by platform data */
	dev->product_id = PRODUCT_ID;
	_android_dev = dev;

#ifdef CONFIG_LGE_USB_GADGET_DRIVER
    mutex_init(&dev->lock);
#endif
	return platform_driver_register(&android_platform_driver);
}
module_init(init);

static void __exit cleanup(void)
{
	usb_composite_unregister(&android_usb_driver);
	platform_driver_unregister(&android_platform_driver);
	kfree(_android_dev);
	_android_dev = NULL;
}
module_exit(cleanup);
