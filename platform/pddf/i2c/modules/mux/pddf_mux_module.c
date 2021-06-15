/*
 * Copyright 2019 Broadcom. 
 * The term “Broadcom” refers to Broadcom Inc. and/or its subsidiaries.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *
 * A pddf kernel module to create I2C client for pca954x type of multiplexer
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/dmi.h>
#include <linux/kobject.h>
#include "pddf_client_defs.h"
#include "pddf_mux_defs.h"


static ssize_t do_device_operation(struct device *dev, struct device_attribute *da, const char *buf, size_t count);
extern void* get_device_table(char *name);
extern void delete_device_table(char *name);


MUX_DATA mux_data = {0};

/* MUX CLIENT DATA */
PDDF_DATA_ATTR(virt_bus, S_IWUSR|S_IRUGO, show_pddf_data, store_pddf_data, PDDF_INT_HEX, sizeof(int), (void*)&mux_data.virt_bus, NULL);
PDDF_DATA_ATTR(dev_ops, S_IWUSR, NULL, do_device_operation, PDDF_CHAR, 8, (void*)&mux_data, (void*)&pddf_data);



static struct attribute *mux_attributes[] = {
	&attr_virt_bus.dev_attr.attr,
	&attr_dev_ops.dev_attr.attr,
	NULL
};

static const struct attribute_group pddf_mux_client_data_group = {
	.attrs = mux_attributes,
};

struct i2c_board_info *i2c_get_mux_board_info(MUX_DATA* mdata, NEW_DEV_ATTR *device_data)
{
	static struct i2c_board_info board_info;
	static struct pca954x_platform_mode platform_modes[8];
	static struct pca954x_platform_data mux_platform_data;
	int num_modes, i;

	if (strncmp(device_data->dev_type, "pca9548", strlen("pca9548")) == 0)
		num_modes = 8;
	else if (strncmp(device_data->dev_type, "pca9546", strlen("pca9546")) == 0)
		num_modes = 6;
	else
	{
		printk(KERN_ERR "%s: Unknown type of mux device\n", __FUNCTION__);
		return NULL;
	}
	
	for(i = 0; i < num_modes; i++) {
		platform_modes[i] = (struct pca954x_platform_mode) {
			.adap_id = (mdata->virt_bus + i),
			.deselect_on_exit = 1,
		};
	}

	mux_platform_data = (struct pca954x_platform_data) {
		.modes = platform_modes,
		.num_modes = num_modes,
	};
	
	board_info = (struct i2c_board_info) {
		.platform_data = &mux_platform_data,
	};

	board_info.addr = device_data->dev_addr;
	strcpy(board_info.type, device_data->dev_type);

	return &board_info;
}


static ssize_t do_device_operation(struct device *dev, struct device_attribute *da, const char *buf, size_t count)
{
	PDDF_ATTR *ptr = (PDDF_ATTR *)da;
	MUX_DATA *mux_ptr = (MUX_DATA *)(ptr->addr);
	NEW_DEV_ATTR *device_ptr = (NEW_DEV_ATTR *)(ptr->data);
	struct i2c_adapter *adapter;
	struct i2c_board_info *board_info;
	struct i2c_client *client_ptr;

	if (strncmp(buf, "add", strlen(buf)-1)==0)
	{
		adapter = i2c_get_adapter(device_ptr->parent_bus);
		board_info = i2c_get_mux_board_info(mux_ptr, device_ptr);

		client_ptr = i2c_new_device(adapter, board_info);

		if (client_ptr != NULL)
		{
			i2c_put_adapter(adapter);
			pddf_dbg(MUX, KERN_ERR "Created %s client: 0x%p\n", device_ptr->i2c_name, (void *)client_ptr);
			add_device_table(device_ptr->i2c_name, (void*)client_ptr);
		}
		else 
		{
			i2c_put_adapter(adapter);
			goto free_data;
		}
	}
	else if (strncmp(buf, "delete", strlen(buf)-1)==0)
	{
		/*Get the i2c_client handle for the created client*/
		client_ptr = (struct i2c_client *)get_device_table(device_ptr->i2c_name);
		if (client_ptr)
		{
			pddf_dbg(MUX, KERN_ERR "Removing %s client: 0x%p\n", device_ptr->i2c_name, (void *)client_ptr);
			i2c_unregister_device(client_ptr);
			/*TODO: Nullyfy the platform data*/
			delete_device_table(device_ptr->i2c_name);
		}
		else
		{
			printk(KERN_ERR "Unable to get the client handle for %s\n", device_ptr->i2c_name);
		}
	}
	else
	{
		printk(KERN_ERR "PDDF_ERROR: %s: Invalid value for dev_ops %s", __FUNCTION__, buf);
	}

free_data:
	memset(mux_ptr, 0, sizeof(MUX_DATA));
	/*TODO: free the device_ptr->data is dynamically allocated*/
	memset(device_ptr, 0 , sizeof(NEW_DEV_ATTR));

	return count;
}


static struct kobject *mux_kobj;

int __init mux_data_init(void)
{
	struct kobject *device_kobj;
	int ret = 0;


	pddf_dbg(MUX, "MUX_DATA MODULE.. init\n");

	device_kobj = get_device_i2c_kobj();
	if(!device_kobj) 
		return -ENOMEM;

	mux_kobj = kobject_create_and_add("mux", device_kobj);
	if(!mux_kobj) 
		return -ENOMEM;
	
	
	ret = sysfs_create_group(mux_kobj, &pddf_clients_data_group);
	if (ret)
    {
        kobject_put(mux_kobj);
        return ret;
    }
	pddf_dbg(MUX, "CREATED PDDF I2C CLIENTS CREATION SYSFS GROUP\n");

	ret = sysfs_create_group(mux_kobj, &pddf_mux_client_data_group);
	if (ret)
    {
		sysfs_remove_group(mux_kobj, &pddf_clients_data_group);
        kobject_put(mux_kobj);
        return ret;
    }
	pddf_dbg(MUX, "CREATED MUX DATA SYSFS GROUP\n");

	return ret;
}

void __exit mux_data_exit(void)
{
	pddf_dbg(MUX, "MUX_DATA MODULE.. exit\n");
	sysfs_remove_group(mux_kobj, &pddf_mux_client_data_group);
	sysfs_remove_group(mux_kobj, &pddf_clients_data_group);
	kobject_put(mux_kobj);
	pddf_dbg(MUX, KERN_ERR "%s: Removed the kobjects for 'mux'\n",__FUNCTION__);
	return;
}

module_init(mux_data_init);
module_exit(mux_data_exit);

MODULE_AUTHOR("Broadcom");
MODULE_DESCRIPTION("mux platform data");
MODULE_LICENSE("GPL");
