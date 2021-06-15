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
 * Description of various APIs related to transciever component
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
#include "pddf_xcvr_defs.h"

/*#define SFP_DEBUG*/
#ifdef SFP_DEBUG
#define sfp_dbg(...) printk(__VA_ARGS__)
#else
#define sfp_dbg(...)
#endif

extern XCVR_SYSFS_ATTR_OPS xcvr_ops[];
extern void *get_device_table(char *name);

int get_xcvr_module_attr_data(struct i2c_client *client, struct device *dev,
                            struct device_attribute *da);

int xcvr_i2c_cpld_read(XCVR_ATTR *info)
{
    int status = -1;

    if (info!=NULL)
    {
        if (info->len==1)
        {
            status = board_i2c_cpld_read(info->devaddr , info->offset);
        }
        else
        {
            /* Get the I2C client for the CPLD */
            struct i2c_client *client_ptr=NULL;
            client_ptr = (struct i2c_client *)get_device_table(info->devname);
            if (client_ptr)
            {
                if (info->len==2)
                {       
                    status = i2c_smbus_read_word_swapped(client_ptr, info->offset);
                }
                else
                    printk(KERN_ERR "PDDF_XCVR: Doesn't support block CPLD read yet");
            }
            else
                printk(KERN_ERR "Unable to get the client handle for %s\n", info->devname);
        }

    }

    return status;
}

int sonic_i2c_get_mod_pres(struct i2c_client *client, XCVR_ATTR *info, struct xcvr_data *data)
{
    int status = 0;
    uint32_t modpres = 0;

    if ( strcmp(info->devtype, "cpld") == 0)
    {
        status = xcvr_i2c_cpld_read(info);

        if (status < 0)
            return status;
        else
        {
            modpres = ((status & BIT_INDEX(info->mask)) == info->cmpval) ? 1 : 0;
            sfp_dbg(KERN_INFO "\nMod presence :0x%x, reg_value = 0x%x, devaddr=0x%x, mask=0x%x, offset=0x%x\n", modpres, status, info->devaddr, info->mask, info->offset);
        }
    }
    else if(strcmp(info->devtype, "eeprom") == 0)
    {
        /* get client client for eeprom -  Not Applicable */
    }
    data->modpres = modpres;

    return 0;
}

int sonic_i2c_get_mod_reset(struct i2c_client *client, XCVR_ATTR *info, struct xcvr_data *data)
{
    int status = 0;
    uint32_t modreset=0;

    if (strcmp(info->devtype, "cpld") == 0)
    {
        status = xcvr_i2c_cpld_read(info);
        if (status < 0)
            return status;
        else
        {
            modreset = ((status & BIT_INDEX(info->mask)) == info->cmpval) ? 1 : 0;
            sfp_dbg(KERN_INFO "\nMod Reset :0x%x, reg_value = 0x%x\n", modreset, status);
        }
    } 
    else if(strcmp(info->devtype, "eeprom") == 0)
    {
        /* get client client for eeprom -  Not Applicable */
    }

    data->reset = modreset;
    return 0;
}

int sonic_i2c_get_mod_intr_status(struct i2c_client *client, XCVR_ATTR *info, struct xcvr_data *data)
{
    int status = 0;
    uint32_t mod_intr = 0;

    if (strcmp(info->devtype, "cpld") == 0)
    {
        status = xcvr_i2c_cpld_read(info);
        if (status < 0)
            return status;
        else
        {
            mod_intr = ((status & BIT_INDEX(info->mask)) == info->cmpval) ? 1 : 0;
            sfp_dbg(KERN_INFO "\nModule Interrupt :0x%x, reg_value = 0x%x\n", mod_intr, status);
        }
    } 
    else if(strcmp(info->devtype, "eeprom") == 0)
    {
        /* get client client for eeprom -  Not Applicable */
    }

    data->intr_status = mod_intr;
    return 0;
}


int sonic_i2c_get_mod_lpmode(struct i2c_client *client, XCVR_ATTR *info, struct xcvr_data *data)
{
    int status = 0;
    uint32_t lpmode = 0;

    if (strcmp(info->devtype, "cpld") == 0)
    {
        status = xcvr_i2c_cpld_read(info);
        if (status < 0)
            return status;
        else
        {
            lpmode = ((status & BIT_INDEX(info->mask)) == info->cmpval) ? 1 : 0;
            sfp_dbg(KERN_INFO "\nModule LPmode :0x%x, reg_value = 0x%x\n", lpmode, status);
        }
    }
    else if (strcmp(info->devtype, "eeprom") == 0)
    {
        /* get client client for eeprom -  Not Applicable */
    }
    
    data->lpmode = lpmode;
    return 0;
}

int sonic_i2c_get_mod_rxlos(struct i2c_client *client, XCVR_ATTR *info, struct xcvr_data *data)
{
    int status = 0;
    uint32_t rxlos = 0;


    if (strcmp(info->devtype, "cpld") == 0)
    {
        status = xcvr_i2c_cpld_read(info);
        if (status < 0)
            return status;
        else
        {
            rxlos = ((status & BIT_INDEX(info->mask)) == info->cmpval) ? 1 : 0;
            sfp_dbg(KERN_INFO "\nModule RxLOS :0x%x, reg_value = 0x%x\n", rxlos, status);
        }
    } 
    data->rxlos = rxlos;

    return 0;
}

int sonic_i2c_get_mod_txdisable(struct i2c_client *client, XCVR_ATTR *info, struct xcvr_data *data)
{
    int status = 0;
    uint32_t txdis = 0;

    if (strcmp(info->devtype, "cpld") == 0)
    {
        status = xcvr_i2c_cpld_read(info);
        if (status < 0)
            return status;
        else
        {
            txdis = ((status & BIT_INDEX(info->mask)) == info->cmpval) ? 1 : 0;
            sfp_dbg(KERN_INFO "\nModule TxDisable :0x%x, reg_value = 0x%x\n", txdis, status);
        }
    }
    data->txdisable = txdis;

    return 0;
}

int sonic_i2c_get_mod_txfault(struct i2c_client *client, XCVR_ATTR *info, struct xcvr_data *data)
{
    int status = 0;
    uint32_t txflt = 0;

    if (strcmp(info->devtype, "cpld") == 0)
    {
        status = xcvr_i2c_cpld_read(info);
        if (status < 0)
            return status;
        else
        {
            txflt = ((status & BIT_INDEX(info->mask)) == info->cmpval) ? 1 : 0;
            sfp_dbg(KERN_INFO "\nModule TxFault :0x%x, reg_value = 0x%x\n", txflt, status);
        }

    } 
    data->txfault = txflt;

    return 0;
}

int sonic_i2c_set_mod_reset(struct i2c_client *client, XCVR_ATTR *info, struct xcvr_data *data)
{
    int status = 0;
    unsigned int val_mask = 0, dnd_value = 0;
    uint32_t reg;
    struct i2c_client *client_ptr=NULL;

    if (strcmp(info->devtype, "cpld") == 0)
    {
        val_mask = BIT_INDEX(info->mask);
        /* Get the I2C client for the CPLD */
        client_ptr = (struct i2c_client *)get_device_table(info->devname);

        if (client_ptr)
        {
            if (info->len == 1)
                status = board_i2c_cpld_read(info->devaddr , info->offset);
            else if (info->len == 2)
                status = i2c_smbus_read_word_data(client_ptr, info->offset);
            else
            {
                printk(KERN_ERR "PDDF_XCVR: Doesn't support block CPLD read yet");
                status = -1;
            }
        }
        else
        {
            printk(KERN_ERR "Unable to get the client handle for %s\n", info->devname);
            status = -1;
        }
        /*printk(KERN_ERR "sonic_i2c_set_mod_reset:client_ptr=0x%x, status=0x%x, offset=0x%x, len=%d\n", client_ptr, status, info->offset, info->len);*/

        if (status < 0)
            return status;
        else
        {
            dnd_value = status & ~val_mask;
            if (((data->reset == 1) && (info->cmpval != 0)) || ((data->reset == 0) && (info->cmpval == 0)))
                reg = dnd_value | val_mask;
            else
                reg = dnd_value;
            if (info->len == 1)
                status = board_i2c_cpld_write(info->devaddr, info->offset, (uint8_t)reg);
            else if (info->len == 2)
                status = i2c_smbus_write_word_swapped(client_ptr, info->offset, (uint16_t)reg);
            else
            {
                printk(KERN_ERR "PDDF_XCVR: Doesn't support block CPLD write yet");
                status = -1;
            }
        }
    }

    return status;
}

int sonic_i2c_set_mod_lpmode(struct i2c_client *client, XCVR_ATTR *info, struct xcvr_data *data)
{
    int status = 0;
    unsigned int val_mask = 0, dnd_value = 0;
    uint32_t reg;
    struct i2c_client *client_ptr=NULL;

    if (strcmp(info->devtype, "cpld") == 0)
    {
        val_mask = BIT_INDEX(info->mask);
        /* Get the I2C client for the CPLD */
        client_ptr = (struct i2c_client *)get_device_table(info->devname);

        if (client_ptr)
        {
            if (info->len == 1)
                status = board_i2c_cpld_read(info->devaddr , info->offset);
            else if (info->len == 2)
                status = i2c_smbus_read_word_data(client_ptr, info->offset);
            else
            {
                printk(KERN_ERR "PDDF_XCVR: Doesn't support block CPLD read yet");
                status = -1;
            }
        }
        else
        {
            printk(KERN_ERR "Unable to get the client handle for %s\n", info->devname);
            status = -1;
        }

        if (status < 0)
            return status;
        else
        {
            dnd_value = status & ~val_mask;
            if (((data->lpmode == 1) && (info->cmpval != 0)) || ((data->lpmode == 0) && (info->cmpval == 0)))
                reg = dnd_value | val_mask;
            else
                reg = dnd_value;
            if (info->len == 1)
                status = board_i2c_cpld_write(info->devaddr, info->offset, (uint8_t)reg);
            else if (info->len == 2)
                status = i2c_smbus_write_word_swapped(client_ptr, info->offset, (uint16_t)reg);
            else
            {
                printk(KERN_ERR "PDDF_XCVR: Doesn't support block CPLD write yet");
                status = -1;
            }
        }
    }

    return status;
}

int sonic_i2c_set_mod_txdisable(struct i2c_client *client, XCVR_ATTR *info, struct xcvr_data *data)
{
    int status = 0;
    unsigned int val_mask = 0, dnd_value = 0;
    uint32_t reg;
    struct i2c_client *client_ptr=NULL;

    if (strcmp(info->devtype, "cpld") == 0)
    {
        val_mask = BIT_INDEX(info->mask);
        /* Get the I2C client for the CPLD */
        client_ptr = (struct i2c_client *)get_device_table(info->devname);

        if (client_ptr)
        {
            if (info->len == 1)
                status = board_i2c_cpld_read(info->devaddr , info->offset);
            else if (info->len == 2)
                status = i2c_smbus_read_word_data(client_ptr, info->offset);
            else
            {
                printk(KERN_ERR "PDDF_XCVR: Doesn't support block CPLD read yet");
                status = -1;
            }
        }
        else
        {
            printk(KERN_ERR "Unable to get the client handle for %s\n", info->devname);
            status = -1;
        }

        if (status < 0)
            return status;
        else
        {
            dnd_value = status & ~val_mask;
            if (((data->txdisable == 1) && (info->cmpval != 0)) || ((data->txdisable == 0) && (info->cmpval == 0)))
                reg = dnd_value | val_mask;
            else
                reg = dnd_value;
            if (info->len == 1)
                status = board_i2c_cpld_write(info->devaddr, info->offset, (uint8_t)reg);
            else if (info->len == 2)
                status = i2c_smbus_write_word_swapped(client_ptr, info->offset, (uint16_t)reg);
            else
            {
                printk(KERN_ERR "PDDF_XCVR: Doesn't support block CPLD write yet");
                status = -1;
            }
        }
    }

    return status;
}

ssize_t get_module_presence(struct device *dev, struct device_attribute *da,
             char *buf)
{
    struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
    struct i2c_client *client = to_i2c_client(dev);
    struct xcvr_data *data = i2c_get_clientdata(client);
    XCVR_PDATA *pdata = (XCVR_PDATA *)(client->dev.platform_data);
    XCVR_ATTR *attr_data = NULL;
    XCVR_SYSFS_ATTR_OPS *attr_ops = NULL;
    int status = 0, i;

    for (i=0; i<pdata->len; i++)
    {
        attr_data = &pdata->xcvr_attrs[i];
        if (strcmp(attr_data->aname, attr->dev_attr.attr.name) == 0)
        {
            attr_ops = &xcvr_ops[attr->index];

            mutex_lock(&data->update_lock);
            if (attr_ops->pre_get != NULL)
            {
                status = (attr_ops->pre_get)(client, attr_data, data);
                if (status!=0)
                    printk(KERN_ERR "%s: pre_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);
            } 
            if (attr_ops->do_get != NULL)
            {
                status = (attr_ops->do_get)(client, attr_data, data);
                if (status!=0)
                    printk(KERN_ERR "%s: do_get function fails for %s attribute. ret %d\n", __FUNCTION__, attr_data->aname, status);

            }
            if (attr_ops->post_get != NULL)
            {
                status = (attr_ops->post_get)(client, attr_data, data);
                if (status!=0)
                    printk(KERN_ERR "%s: post_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);
            }
            mutex_unlock(&data->update_lock);
            return sprintf(buf, "%d\n", data->modpres);
        }
    }
    return sprintf(buf, "%s","");
}

ssize_t get_module_reset(struct device *dev, struct device_attribute *da,
             char *buf)
{
    struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
    struct i2c_client *client = to_i2c_client(dev);
    struct xcvr_data *data = i2c_get_clientdata(client);
    XCVR_PDATA *pdata = (XCVR_PDATA *)(client->dev.platform_data);
    XCVR_ATTR *attr_data = NULL;
    XCVR_SYSFS_ATTR_OPS *attr_ops = NULL;
    int status = 0, i;

    for (i=0; i<pdata->len; i++)
    {
        attr_data = &pdata->xcvr_attrs[i];
        if (strcmp(attr_data->aname, attr->dev_attr.attr.name) == 0)
        {
            attr_ops = &xcvr_ops[attr->index];

            mutex_lock(&data->update_lock);
            if (attr_ops->pre_get != NULL)
            {
                status = (attr_ops->pre_get)(client, attr_data, data);
                if (status!=0)
                    printk(KERN_ERR "%s: pre_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);
            } 
            if (attr_ops->do_get != NULL)
            {
                status = (attr_ops->do_get)(client, attr_data, data);
                if (status!=0)
                    printk(KERN_ERR "%s: do_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);

            }
            if (attr_ops->post_get != NULL)
            {
                status = (attr_ops->post_get)(client, attr_data, data);
                if (status!=0)
                    printk(KERN_ERR "%s: post_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);
            }

            mutex_unlock(&data->update_lock);

            return sprintf(buf, "%d\n", data->reset);
        }
    }
    return sprintf(buf, "%s","");
}

ssize_t set_module_reset(struct device *dev, struct device_attribute *da, const char *buf, 
        size_t count)
{
    struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
    struct i2c_client *client = to_i2c_client(dev);
    struct xcvr_data *data = i2c_get_clientdata(client);
    XCVR_PDATA *pdata = (XCVR_PDATA *)(client->dev.platform_data);
    XCVR_ATTR *attr_data = NULL;
    XCVR_SYSFS_ATTR_OPS *attr_ops = NULL;
    int status = 0, i;
    unsigned int set_value;

    for (i=0; i<pdata->len; i++)
    {
        attr_data = &pdata->xcvr_attrs[i];
        if (strcmp(attr_data->aname, attr->dev_attr.attr.name) == 0)
        {
            attr_ops = &xcvr_ops[attr->index];
            if(kstrtoint(buf, 10, &set_value))
                return -EINVAL;
            if ((set_value != 1) && (set_value != 0))
                return -EINVAL;

            data->reset = set_value;

            mutex_lock(&data->update_lock);
            
            if (attr_ops->pre_set != NULL)
            {
                status = (attr_ops->pre_set)(client, attr_data, data);
                if (status!=0)
                    printk(KERN_ERR "%s: pre_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);
                }
            if (attr_ops->do_set != NULL)
            {
                status = (attr_ops->do_set)(client, attr_data, data);
                if (status!=0)
                    printk(KERN_ERR "%s: do_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);

            }
            if (attr_ops->post_set != NULL)
            {
                status = (attr_ops->post_set)(client, attr_data, data);
                if (status!=0)
                    printk(KERN_ERR "%s: post_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);
            } 
            mutex_unlock(&data->update_lock);

            return count;
        }
    }
    return -EINVAL;
}

ssize_t get_module_intr_status(struct device *dev, struct device_attribute *da,
             char *buf)
{
    struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
    struct i2c_client *client = to_i2c_client(dev);
    struct xcvr_data *data = i2c_get_clientdata(client);
    XCVR_PDATA *pdata = (XCVR_PDATA *)(client->dev.platform_data);
    XCVR_ATTR *attr_data = NULL;
    XCVR_SYSFS_ATTR_OPS *attr_ops = NULL;
    int status = 0, i;

    for (i=0; i<pdata->len; i++)
    {
        attr_data = &pdata->xcvr_attrs[i];
        if (strcmp(attr_data->aname, attr->dev_attr.attr.name) == 0)
        {
            attr_ops = &xcvr_ops[attr->index];

            mutex_lock(&data->update_lock);
            if (attr_ops->pre_get != NULL)
            {
                status = (attr_ops->pre_get)(client, attr_data, data);
                if (status!=0)
                    printk(KERN_ERR "%s: pre_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);
            } 
            if (attr_ops->do_get != NULL)
            {
                status = (attr_ops->do_get)(client, attr_data, data);
                if (status!=0)
                    printk(KERN_ERR "%s: do_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);

            }
            if (attr_ops->post_get != NULL)
            {
                status = (attr_ops->post_get)(client, attr_data, data);
                if (status!=0)
                    printk(KERN_ERR "%s: post_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);
            }

            mutex_unlock(&data->update_lock);
            return sprintf(buf, "%d\n", data->intr_status);
        }
    }
    return sprintf(buf, "%s","");
}

int get_xcvr_module_attr_data(struct i2c_client *client, struct device *dev, 
                            struct device_attribute *da)
{
    struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
    XCVR_PDATA *pdata = (XCVR_PDATA *)(client->dev.platform_data);
    XCVR_ATTR *attr_data = NULL;
    int i;

    for (i=0; i < pdata->len; i++)
    {
        attr_data = &pdata->xcvr_attrs[i];
        if (strcmp(attr_data->aname, attr->dev_attr.attr.name) == 0)
        {
            return i;
        }
    }
    return -1;
}

ssize_t get_module_lpmode(struct device *dev, struct device_attribute *da, char *buf)
{
    struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
    struct i2c_client *client = to_i2c_client(dev);
    XCVR_PDATA *pdata = (XCVR_PDATA *)(client->dev.platform_data);
    struct xcvr_data *data = i2c_get_clientdata(client);
    XCVR_ATTR *attr_data = NULL;
    XCVR_SYSFS_ATTR_OPS *attr_ops = NULL;
    int idx, status = 0;

    idx = get_xcvr_module_attr_data(client, dev, da);

    if (idx>=0) attr_data = &pdata->xcvr_attrs[idx];
    
    if (attr_data!=NULL)
    {

        attr_ops = &xcvr_ops[attr->index];

        mutex_lock(&data->update_lock);
        if (attr_ops->pre_get != NULL)
        {
            status = (attr_ops->pre_get)(client, attr_data, data);
            if (status!=0)
                printk(KERN_ERR "%s: pre_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);
        } 
        if (attr_ops->do_get != NULL)
        {
            status = (attr_ops->do_get)(client, attr_data, data);
            if (status!=0)
                printk(KERN_ERR "%s: do_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);

        }
        if (attr_ops->post_get != NULL)
        {
            status = (attr_ops->post_get)(client, attr_data, data);
            if (status!=0)
                printk(KERN_ERR "%s: post_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);
        }
        mutex_unlock(&data->update_lock);
        return sprintf(buf, "%d\n", data->lpmode);
    }
    else
        return sprintf(buf,"%s","");
}

ssize_t set_module_lpmode(struct device *dev, struct device_attribute *da, const char *buf, 
        size_t count)
{
    struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
    struct i2c_client *client = to_i2c_client(dev);
    struct xcvr_data *data = i2c_get_clientdata(client);
    XCVR_PDATA *pdata = (XCVR_PDATA *)(client->dev.platform_data);
    int idx, status = 0;
    uint32_t set_value;
    XCVR_ATTR *attr_data = NULL;
    XCVR_SYSFS_ATTR_OPS *attr_ops = NULL;

    idx = get_xcvr_module_attr_data(client, dev, da);
    
    if (idx>=0) attr_data = &pdata->xcvr_attrs[idx];
    
    if (attr_data!=NULL)
    {
        attr_ops = &xcvr_ops[attr->index];
        if(kstrtoint(buf, 10, &set_value))
            return -EINVAL;
        if ((set_value != 1) && (set_value != 0))
            return -EINVAL;

        data->lpmode = set_value;

        mutex_lock(&data->update_lock);
        
        if (attr_ops->pre_set != NULL)
        {
            status = (attr_ops->pre_set)(client, attr_data, data);
            if (status!=0)
                printk(KERN_ERR "%s: pre_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);
            }
        if (attr_ops->do_set != NULL)
        {
            status = (attr_ops->do_set)(client, attr_data, data);
            if (status!=0)
                printk(KERN_ERR "%s: do_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);

        }
        if (attr_ops->post_set != NULL)
        {
            status = (attr_ops->post_set)(client, attr_data, data);
            if (status!=0)
                printk(KERN_ERR "%s: post_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);
        } 
        mutex_unlock(&data->update_lock);
    }
    return count;
}

ssize_t get_module_rxlos(struct device *dev, struct device_attribute *da,
             char *buf)
{
    struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
    struct i2c_client *client = to_i2c_client(dev);
    struct xcvr_data *data = i2c_get_clientdata(client);
    XCVR_PDATA *pdata = (XCVR_PDATA *)(client->dev.platform_data);
    int idx, status = 0;
    XCVR_ATTR *attr_data = NULL;
    XCVR_SYSFS_ATTR_OPS *attr_ops = NULL;

    idx = get_xcvr_module_attr_data(client, dev, da);
    
    if (idx>=0) attr_data = &pdata->xcvr_attrs[idx];
    
    if (attr_data!=NULL)
    {
        attr_ops = &xcvr_ops[attr->index];

        mutex_lock(&data->update_lock);
        if (attr_ops->pre_get != NULL)
        {
            status = (attr_ops->pre_get)(client, attr_data, data);
            if (status!=0)
                printk(KERN_ERR "%s: pre_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);
        } 
        if (attr_ops->do_get != NULL)
        {
            status = (attr_ops->do_get)(client, attr_data, data);
            if (status!=0)
                printk(KERN_ERR "%s: do_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);

        }
        if (attr_ops->post_get != NULL)
        {
            status = (attr_ops->post_get)(client, attr_data, data);
            if (status!=0)
                printk(KERN_ERR "%s: post_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);
        }
        mutex_unlock(&data->update_lock);
        return sprintf(buf, "%d\n", data->rxlos);
    }
    else
        return sprintf(buf,"%s","");
}

ssize_t get_module_txdisable(struct device *dev, struct device_attribute *da,
             char *buf)
{
    struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
    struct i2c_client *client = to_i2c_client(dev);
    struct xcvr_data *data = i2c_get_clientdata(client);
    XCVR_PDATA *pdata = (XCVR_PDATA *)(client->dev.platform_data);
    int idx, status = 0;
    XCVR_ATTR *attr_data = NULL;
    XCVR_SYSFS_ATTR_OPS *attr_ops = NULL;
    
    idx = get_xcvr_module_attr_data(client, dev, da);
    
    if (idx>=0) attr_data = &pdata->xcvr_attrs[idx];
    
    if (attr_data!=NULL)
    {
        attr_ops = &xcvr_ops[attr->index];

        mutex_lock(&data->update_lock);
        if (attr_ops->pre_get != NULL)
        {
            status = (attr_ops->pre_get)(client, attr_data, data);
            if (status!=0)
                printk(KERN_ERR "%s: pre_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);
        }
        if (attr_ops->do_get != NULL)
        {
            status = (attr_ops->do_get)(client, attr_data, data);
            if (status!=0)
                printk(KERN_ERR "%s: do_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);

        }
        if (attr_ops->post_get != NULL)
        {
            status = (attr_ops->post_get)(client, attr_data, data);
            if (status!=0)
                printk(KERN_ERR "%s: post_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);
        }
        mutex_unlock(&data->update_lock);
        return sprintf(buf, "%d\n", data->txdisable);
    }
    else
        return sprintf(buf,"%s","");
}

ssize_t set_module_txdisable(struct device *dev, struct device_attribute *da, const char *buf, 
        size_t count)
{
    struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
    struct i2c_client *client = to_i2c_client(dev);
    struct xcvr_data *data = i2c_get_clientdata(client);
    XCVR_PDATA *pdata = (XCVR_PDATA *)(client->dev.platform_data);
    int idx, status = 0;
    uint32_t set_value;
    XCVR_ATTR *attr_data = NULL;
    XCVR_SYSFS_ATTR_OPS *attr_ops = NULL;

    idx = get_xcvr_module_attr_data(client, dev, da);
    
    if (idx>=0) attr_data = &pdata->xcvr_attrs[idx];
    
    if (attr_data!=NULL)
    {
        attr_ops = &xcvr_ops[attr->index];
        if(kstrtoint(buf, 10, &set_value))
            return -EINVAL;
        if ((set_value != 1) && (set_value != 0))
            return -EINVAL;

        data->txdisable = set_value;

        mutex_lock(&data->update_lock);
        
        if (attr_ops->pre_set != NULL)
        {
            status = (attr_ops->pre_set)(client, attr_data, data);
            if (status!=0)
                printk(KERN_ERR "%s: pre_set function fails for %s attribute\n", __FUNCTION__, attr_data->aname);
            }
        if (attr_ops->do_set != NULL)
        {
            status = (attr_ops->do_set)(client, attr_data, data);
            if (status!=0)
                printk(KERN_ERR "%s: do_set function fails for %s attribute\n", __FUNCTION__, attr_data->aname);

        }
        if (attr_ops->post_set != NULL)
        {
            status = (attr_ops->post_set)(client, attr_data, data);
            if (status!=0)
                printk(KERN_ERR "%s: post_set function fails for %s attribute\n", __FUNCTION__, attr_data->aname);
        } 
        mutex_unlock(&data->update_lock);
    }
    return count;
}

ssize_t get_module_txfault(struct device *dev, struct device_attribute *da,
             char *buf)
{
    struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
    struct i2c_client *client = to_i2c_client(dev);
    XCVR_PDATA *pdata = (XCVR_PDATA *)(client->dev.platform_data);
    struct xcvr_data *data = i2c_get_clientdata(client);
    int idx, status = 0;
    XCVR_ATTR *attr_data = NULL;
    XCVR_SYSFS_ATTR_OPS *attr_ops = NULL;

    idx = get_xcvr_module_attr_data(client, dev, da);
    
    if (idx>=0) attr_data = &pdata->xcvr_attrs[idx];
    
    if (attr_data!=NULL)
    {
        attr_ops = &xcvr_ops[attr->index];

        mutex_lock(&data->update_lock);
        if (attr_ops->pre_get != NULL)
        {
            status = (attr_ops->pre_get)(client, attr_data, data);
            if (status!=0)
                printk(KERN_ERR "%s: pre_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);
        } 
        if (attr_ops->do_get != NULL)
        {
            status = (attr_ops->do_get)(client, attr_data, data);
            if (status!=0)
                printk(KERN_ERR "%s: do_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);

        }
        if (attr_ops->post_get != NULL)
        {
            status = (attr_ops->post_get)(client, attr_data, data);
            if (status!=0)
                printk(KERN_ERR "%s: post_get function fails for %s attribute\n", __FUNCTION__, attr_data->aname);
        }
        mutex_unlock(&data->update_lock);
        return sprintf(buf, "%d\n", data->txfault);
    }
    return sprintf(buf,"%s","");
}
