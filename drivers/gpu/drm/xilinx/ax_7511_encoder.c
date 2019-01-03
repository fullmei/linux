/*
 * ax_7511_encoder.c 
 *
 * Copyright (C) 2017 
 * Author: meisq
 *
 *
 */

#include <drm/drmP.h>
#include <drm/drm_edid.h>
#include <drm/drm_encoder_slave.h>

#include <linux/device.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/of_gpio.h>
#include <linux/gpio/consumer.h>

#define ENC_MAX_FREQ 	150000
#define ENC_MAX_H 		1920
#define ENC_MAX_V 		1080
#define ENC_PREF_H 		1280
#define ENC_PREF_V 		720
//72
#define TX_SLV0 					0x39
//7A
#define TX_SLV1 					0x3D
								
struct ax_7511_encoder {
	struct drm_encoder *encoder;
	struct i2c_adapter *i2c_bus;
  	bool i2c_present;
};


static inline struct ax_7511_encoder *to_ax_7511_encoder(struct drm_encoder *encoder)
{

	return to_encoder_slave(encoder)->slave_priv;
}

static bool ax_7511_mode_fixup(struct drm_encoder *encoder,
			   const struct drm_display_mode *mode,
			   struct drm_display_mode *adjusted_mode)
{
	return true;
}

static void ax_7511_encoder_mode_set(struct drm_encoder *encoder,
				 struct drm_display_mode *mode,
				 struct drm_display_mode *adjusted_mode)
{

}

static void
ax_7511_encoder_dpms(struct drm_encoder *encoder, int mode)
{
}

static void ax_7511_encoder_save(struct drm_encoder *encoder)
{
}

static void ax_7511_encoder_restore(struct drm_encoder *encoder)
{
}

static int ax_7511_encoder_mode_valid(struct drm_encoder *encoder,
				    struct drm_display_mode *mode)
{

   if (mode && 
      !(mode->flags & ((DRM_MODE_FLAG_INTERLACE | DRM_MODE_FLAG_DBLCLK) | DRM_MODE_FLAG_3D_MASK)) &&
      (mode->clock <= ENC_MAX_FREQ) &&
      (mode->hdisplay <= ENC_MAX_H) && 
      (mode->vdisplay <= ENC_MAX_V)) 
         return MODE_OK;
   return MODE_BAD;
}

static unsigned char ax_readbyte_8b(struct i2c_adapter *i2c_bus,unsigned char slaveAddr, unsigned char regAddr)
{
	unsigned char rdbuf = 0;
	int error;
	
	struct i2c_msg wrmsg[2] = {
		{
			.addr = slaveAddr,
			.flags = 0,
			.len = 1,
			.buf = &regAddr,
		}, {
			.addr = slaveAddr,
			.flags = I2C_M_RD,
			.len = 1,
			.buf = &rdbuf,
		}
	};

	if(!i2c_bus)
	{
		printk(KERN_ALERT "ax_readbyte_8b i2c_bus error\n");
		return 0;
	}
	
	error = i2c_transfer(i2c_bus,wrmsg,2);
	if(error < 0)
	{
		printk(KERN_ALERT "ax_readbyte_8b transfer failed\n");		
		return 0;
	}

	return rdbuf;
}

static void ax_writebyte_8b(struct i2c_adapter *i2c_bus,unsigned char slaveAddr,unsigned char regAddr,unsigned char data)
{
	int error;
	unsigned char buffer[2];
	struct i2c_msg wrmsg = {	
		.addr = slaveAddr,
		.flags = 0,
		.len = 2,
		.buf = buffer,
	};
	
	buffer[0] = regAddr;
	buffer[1] = data;

	if(!i2c_bus)
	{
		printk(KERN_ALERT "ax_writebyte_8b i2c_bus error\n");
		return ;
	}
	
	error = i2c_transfer(i2c_bus,&wrmsg,1);
	if(error < 0)
	{
		printk(KERN_ALERT "ax_writebyte_8b transfer failed\n");		
		return;
	}

}


static int ax_7511_encoder_get_modes(struct drm_encoder *encoder,
				   struct drm_connector *connector)
{
   struct ax_7511_encoder *ax_7511 = to_ax_7511_encoder(encoder);
   struct edid *edid;
   int num_modes = 0;
   
   if (ax_7511->i2c_present)
   {
      ax_writebyte_8b(ax_7511->i2c_bus, TX_SLV0, 0xc9, 0x03);
      ax_writebyte_8b(ax_7511->i2c_bus, TX_SLV0, 0xc9, 0x13);
      msleep(200);
      edid = drm_get_edid(connector, ax_7511->i2c_bus);
      drm_mode_connector_update_edid_property(connector, edid);
      if (edid) 
      {
         num_modes = drm_add_edid_modes(connector, edid);
         kfree(edid);
      }
   }
   else
   {
      num_modes = drm_add_modes_noedid(connector, ENC_MAX_H, ENC_MAX_V);
      drm_set_preferred_mode(connector, ENC_PREF_H, ENC_PREF_V);
   }   
	return num_modes;
}

static enum drm_connector_status ax_7511_encoder_detect(struct drm_encoder *encoder,
		     struct drm_connector *connector)
{
   struct ax_7511_encoder *ax_7511 = to_ax_7511_encoder(encoder);
   if (ax_7511->i2c_present)
   {
      if (drm_probe_ddc(ax_7511->i2c_bus))
         return connector_status_connected;
      return connector_status_disconnected;
   }
   else
      return connector_status_unknown; 
}

static struct drm_encoder_slave_funcs ax_7511_encoder_slave_funcs = {
	.dpms 			= ax_7511_encoder_dpms,
	.save			= ax_7511_encoder_save,
	.restore		= ax_7511_encoder_restore,
	.mode_fixup 	= ax_7511_mode_fixup,
	.mode_valid		= ax_7511_encoder_mode_valid,
	.mode_set 		= ax_7511_encoder_mode_set,
	.detect			= ax_7511_encoder_detect,
	.get_modes		= ax_7511_encoder_get_modes,
};

static int ax_7511_encoder_encoder_init(struct platform_device *pdev,
				      struct drm_device *dev,
				      struct drm_encoder_slave *encoder)
{
	struct ax_7511_encoder *ax_7511 = platform_get_drvdata(pdev);
	struct device_node *sub_node;

	encoder->slave_priv = ax_7511;
	encoder->slave_funcs = &ax_7511_encoder_slave_funcs;

	ax_7511->encoder = &encoder->base;

	ax_7511->i2c_present = false;
	sub_node = of_parse_phandle(pdev->dev.of_node, "ax_7511,edid-i2c", 0);
	if (sub_node) 
	{

	   	ax_7511->i2c_bus = of_find_i2c_adapter_by_node(sub_node);
	  	if (!ax_7511->i2c_bus)
	  	{
		   	DRM_INFO("failed to get the edid i2c adapter, using default modes\n");
			return 0;
	  	}
	  	else
	  	{
		 	ax_7511->i2c_present = true;
			DRM_INFO("adv7511 chip version is %x\n",ax_readbyte_8b(ax_7511->i2c_bus, TX_SLV0,0));
			ax_writebyte_8b(ax_7511->i2c_bus, TX_SLV0, 0xF3, 0x09);
			ax_writebyte_8b(ax_7511->i2c_bus, TX_SLV0, 0x41, 0x00);   //16'h4110; 	
			ax_writebyte_8b(ax_7511->i2c_bus, TX_SLV0, 0x98, 0x03);
			ax_writebyte_8b(ax_7511->i2c_bus, TX_SLV0, 0x9a, 0xe0);	
			ax_writebyte_8b(ax_7511->i2c_bus, TX_SLV0, 0x9c, 0x30);	
			ax_writebyte_8b(ax_7511->i2c_bus, TX_SLV0, 0x9d, 0x61);	
			ax_writebyte_8b(ax_7511->i2c_bus, TX_SLV0, 0xa2, 0xa4);	
			ax_writebyte_8b(ax_7511->i2c_bus, TX_SLV0, 0xa3, 0xa4);	
			ax_writebyte_8b(ax_7511->i2c_bus, TX_SLV0, 0xe0, 0xd0);	
			ax_writebyte_8b(ax_7511->i2c_bus, TX_SLV0, 0x55, 0x12);	
			ax_writebyte_8b(ax_7511->i2c_bus, TX_SLV0, 0xf9, 0x00);
			ax_writebyte_8b(ax_7511->i2c_bus, TX_SLV0, 0x43, 0xa0);
			ax_writebyte_8b(ax_7511->i2c_bus, TX_SLV0, 0x15, 0x00);  //input id = 0x0 = 0000 = 24 bit RGB 4:4:4 or YCbCr 4:4:4 (separate syncs)
			ax_writebyte_8b(ax_7511->i2c_bus, TX_SLV0, 0xd0, 0x3c);
			ax_writebyte_8b(ax_7511->i2c_bus, TX_SLV0, 0xaf, 0x04);	
			ax_writebyte_8b(ax_7511->i2c_bus, TX_SLV0, 0x4c, 0x04);	
			ax_writebyte_8b(ax_7511->i2c_bus, TX_SLV0, 0x40, 0x00);
			ax_writebyte_8b(ax_7511->i2c_bus, TX_SLV0, 0xd6, 0xc0);

	  	}
	   	of_node_put(sub_node);
	}
	return 0;
}

static int ax_7511_encoder_probe(struct platform_device *pdev)
{
	struct ax_7511_encoder *ax_7511;
	ax_7511 = devm_kzalloc(&pdev->dev, sizeof(*ax_7511), GFP_KERNEL);
	if (!ax_7511)
		return -ENOMEM;

	platform_set_drvdata(pdev, ax_7511);

	return 0;
}

static int ax_7511_encoder_remove(struct platform_device *pdev)
{
	return 0;
}

static const struct of_device_id ax_7511_encoder_of_match[] = {
	{ .compatible = "ax_7511,drm-encoder", },
	{ /* end of table */ },
};
MODULE_DEVICE_TABLE(of, ax_7511_encoder_of_match);

static struct drm_platform_encoder_driver ax_7511_encoder_driver = {
	.platform_driver = {
		.probe			= ax_7511_encoder_probe,
		.remove			= ax_7511_encoder_remove,
		.driver			= {
			.owner		= THIS_MODULE,
			.name		= "ax_7511-drm-enc",
			.of_match_table	= ax_7511_encoder_of_match,
		},
	},

	.encoder_init = ax_7511_encoder_encoder_init,
};

static int __init ax_7511_encoder_init(void)
{
	return platform_driver_register(&ax_7511_encoder_driver.platform_driver);
}

static void __exit ax_7511_encoder_exit(void)
{
	platform_driver_unregister(&ax_7511_encoder_driver.platform_driver);
}

module_init(ax_7511_encoder_init);
module_exit(ax_7511_encoder_exit);

MODULE_AUTHOR("meisq");
MODULE_DESCRIPTION("");
MODULE_LICENSE("GPL v2");
