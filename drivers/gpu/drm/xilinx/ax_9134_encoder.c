/*
 * ax_9134_encoder.c 
 *
 * Copyright (C) 2017 
 * Author: Guowc
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

#define AX_9134_ENC_MAX_FREQ 		150000
#define AX_9134_ENC_MAX_H 		1920
#define AX_9134_ENC_MAX_V 		1080
#define AX_9134_ENC_PREF_H 		1280
#define AX_9134_ENC_PREF_V 		720
//72
#define TX_SLV0 					0x39
//7A
#define TX_SLV1 					0x3D
								


typedef unsigned char BYTE;
typedef unsigned int  WORD;


typedef struct {
 BYTE SlaveAddr;
 BYTE Offset;
 BYTE RegAddr;
 BYTE NBytesLSB;
 BYTE NBytesMSB;
 BYTE Dummy;
 BYTE Cmd;

} MDDCType;

static struct edid m_edid;


struct ax_9134_encoder {
	struct drm_encoder *encoder;
	struct i2c_adapter *i2c_bus;
   	u32	   reset_gpio; 
  	bool i2c_present;
};


static inline struct ax_9134_encoder *to_ax_9134_encoder(struct drm_encoder *encoder)
{

	return to_encoder_slave(encoder)->slave_priv;
}

static bool ax_9134_mode_fixup(struct drm_encoder *encoder,
			   const struct drm_display_mode *mode,
			   struct drm_display_mode *adjusted_mode)
{
	return true;
}

static void ax_9134_encoder_mode_set(struct drm_encoder *encoder,
				 struct drm_display_mode *mode,
				 struct drm_display_mode *adjusted_mode)
{

}

static void
ax_9134_encoder_dpms(struct drm_encoder *encoder, int mode)
{
}

static void ax_9134_encoder_save(struct drm_encoder *encoder)
{
}

static void ax_9134_encoder_restore(struct drm_encoder *encoder)
{
}

static int ax_9134_encoder_mode_valid(struct drm_encoder *encoder,
				    struct drm_display_mode *mode)
{

   if (mode && 
      !(mode->flags & ((DRM_MODE_FLAG_INTERLACE | DRM_MODE_FLAG_DBLCLK) | DRM_MODE_FLAG_3D_MASK)) &&
      (mode->clock <= AX_9134_ENC_MAX_FREQ) &&
      (mode->hdisplay <= AX_9134_ENC_MAX_H) && 
      (mode->vdisplay <= AX_9134_ENC_MAX_V)) 
         return MODE_OK;
   return MODE_BAD;
}

static u8 ax_readbyte_8b(struct ax_9134_encoder *axdata, BYTE slaveAddr,  BYTE regAddr)
{
	u8 rdbuf = 0;
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


	if(!axdata->i2c_bus)
	{
		printk(KERN_ALERT "ax_readbyte_8b i2c_bus error\n");
		return 0;
	}
	
	error = i2c_transfer(axdata->i2c_bus , wrmsg, 2);
	if(error < 0)
	{
		printk(KERN_ALERT "ax_readbyte_8b transfer failed\n");		
		return 0;
	}

	return rdbuf;

}

static void ax_writebyte_8b(struct ax_9134_encoder *axdata, BYTE slaveAddr,  BYTE regAddr, BYTE data)
{
	int error;

	BYTE buffer[2];

	struct i2c_msg wrmsg = {	
		.addr = slaveAddr,
		.flags = 0,
		.len = 2,
		.buf = buffer,
	};

	
	buffer[0] = regAddr;
	buffer[1] = data;



	if(!axdata->i2c_bus)
	{
		printk(KERN_ALERT "ax_writebyte_8b i2c_bus error\n");
		return ;
	}
	
	error = i2c_transfer(axdata->i2c_bus , &wrmsg, 1);
	if(error < 0)
	{
		printk(KERN_ALERT "ax_writebyte_8b transfer failed\n");		
		return;
	}

}


static void ax_writebyte_nb(struct ax_9134_encoder *axdata, BYTE slaveAddr,  BYTE regAddr,BYTE offset)
{
	int error;

	BYTE buffer[8];

	struct i2c_msg wrmsg = {	
		.addr = slaveAddr,
		.flags = 0,
		.len = 8,
		.buf = buffer,		
	};

	
	buffer[0] = regAddr;
	buffer[1] = 0xA0;
	buffer[2] = 0x00;
	buffer[3] = offset;
	buffer[4] = 16;
	buffer[5] = 0x00;
	buffer[6] = 0x00;
	buffer[7] = 0x02;



	if(!axdata->i2c_bus)
	{
		printk(KERN_ALERT "ax_writebyte_nb i2c_bus error\n");
		return ;
	}
	
	error = i2c_transfer(axdata->i2c_bus , &wrmsg, 1);
	if(error < 0)
	{
		printk(KERN_ALERT "ax_writebyte_nb transfer failed\n");		
		return;
	}

}



void ax_9134_BlockRead_MDDC(struct ax_9134_encoder *axdata, BYTE * pData)
{
	BYTE FIFO_Size,  Status;

	int datacount = 128;
	int i = 0;
	int j =0;

	ax_writebyte_8b(axdata, TX_SLV0, 0xF3, 0x09);
	udelay(200);


	ax_writebyte_8b(axdata, TX_SLV0, 0xF5, 0x10);
	udelay(1000);	


	FIFO_Size = 0;

	datacount = 16;

	for(j=0;j<8;j++)
	{
		ax_writebyte_nb(axdata, TX_SLV0,0xED, j*16);
		udelay(500);
	
		do{
			BYTE is = ax_readbyte_8b(axdata, TX_SLV0, 0xF2);
			if((is>>3) & 0x01)
				break;
			Status = (is>>4) & 0x01;
			udelay(100);				
		}while(Status);
		
		
		FIFO_Size = ax_readbyte_8b(axdata, TX_SLV0, 0xF5);
		
		if(FIFO_Size) {
			for(i=0;i<FIFO_Size;i++)
			{
				
			
				BYTE data = ax_readbyte_8b(axdata, TX_SLV0, 0xF4);	

				memcpy(pData, &data, 1);
				pData++;
				mdelay(10);				
			}
		}
		
	}



}


static void ax_9134_BlockReadEDID(struct ax_9134_encoder *axdata, BYTE *pData)
{

	ax_9134_BlockRead_MDDC(axdata, pData);

}


static int ax_9134_drm_edid_block_checksum(const u8 *raw_edid)
{
	int i;
	u8 csum = 0;
	for (i = 0; i < EDID_LENGTH; i++)
		csum += raw_edid[i];

	return csum;
}


static bool ax_9134_drm_edid_block_valid(u8 *raw_edid)
{
	u8 csum;
	int score;
	struct edid *edid = (struct edid *)raw_edid;

	if (WARN_ON(!raw_edid))
		return false;

	score = drm_edid_header_is_valid(raw_edid);
	if (score == 8) ;
	else {
		goto bad;
	}

	csum = ax_9134_drm_edid_block_checksum(raw_edid);
	if (csum) {
		if (raw_edid[0] != 0x02)
			goto bad;
	}

	switch (raw_edid[0]) {
	case 0: 
		if (edid->version != 1) {
			goto bad;
		}

		if (edid->revision > 4)
		break;

	default:
		break;
	}

	return true;

bad:

	return false;
}


static bool ax_9134_drm_edid_is_valid(struct edid *edid)
{
	int i;
	u8 *raw = (u8 *)edid; 

	if (!edid)
		return false;

	edid->checksum += edid->extensions;
	edid->extensions = 0;	

	if (!ax_9134_drm_edid_block_valid(raw + i * EDID_LENGTH))
		return false;

	return true;
}


static int ax_9134_encoder_get_modes(struct drm_encoder *encoder,
				   struct drm_connector *connector)
{

   	int num_modes = 0;
   	struct ax_9134_encoder *ax_9134 = to_ax_9134_encoder(encoder);


	if (ax_9134->i2c_present)
   	{
		BYTE editbuf[128];

		struct edid *ped = &m_edid;
		
	    memset(editbuf, 0, 128);

        ax_9134_BlockReadEDID(ax_9134, editbuf);


		if (ax_9134_drm_edid_is_valid((struct edid*)&editbuf)) {
			memcpy(ped, editbuf, 128);
			ped->checksum += ped->extensions;
			ped->extensions = 0;
			
		}


		drm_mode_connector_update_edid_property(connector, ped);
   		num_modes = drm_add_edid_modes(connector, ped);

	  
	    ax_writebyte_8b(ax_9134, TX_SLV0, 0x08, 0x35);
	    udelay(100);			
	    ax_writebyte_8b(ax_9134, TX_SLV1, 0x2f, 0x00);


	    return num_modes;
	    
   	}
	return num_modes;
}

static enum drm_connector_status ax_9134_encoder_detect(struct drm_encoder *encoder,
		     struct drm_connector *connector)
{

	BYTE ret;

   	struct ax_9134_encoder *ax_9134 = to_ax_9134_encoder(encoder);


	if(ax_9134->i2c_bus==NULL)
		return connector_status_unknown;


   	ret = ax_readbyte_8b(ax_9134, TX_SLV0, 0x09 );

	if(ret == 0x81)
	{
		memset(&m_edid, 0, 128);
		return connector_status_disconnected;
	}
   	if(ret == 0x87)
   	{
   	
   		return connector_status_connected; 		
   	}

	memset(&m_edid, 0, 128);	
	return connector_status_unknown;	

}

static struct drm_encoder_slave_funcs ax_9134_encoder_slave_funcs = {
	.dpms 			= ax_9134_encoder_dpms,
	.save			= ax_9134_encoder_save,
	.restore		= ax_9134_encoder_restore,
	.mode_fixup 		= ax_9134_mode_fixup,
	.mode_valid		= ax_9134_encoder_mode_valid,
	.mode_set 		= ax_9134_encoder_mode_set,
	.detect			= ax_9134_encoder_detect,
	.get_modes		= ax_9134_encoder_get_modes,
};

static int ax_9134_encoder_encoder_init(struct platform_device *pdev,
				      struct drm_device *dev,
				      struct drm_encoder_slave *encoder)
{
	struct ax_9134_encoder *ax_9134 = platform_get_drvdata(pdev);
	struct device_node *sub_node;
	int gpio;
	u32 reset_gpio_flags;
	
	encoder->slave_priv = ax_9134;
	encoder->slave_funcs = &ax_9134_encoder_slave_funcs;

	ax_9134->encoder = &encoder->base;

	ax_9134->i2c_present = false;
	sub_node = of_parse_phandle(pdev->dev.of_node, "ax_9134,edid-i2c", 0);
	if (sub_node) 
	{

	   	ax_9134->i2c_bus = of_find_i2c_adapter_by_node(sub_node);
	  	if (!ax_9134->i2c_bus)
	  	{
		   	DRM_INFO("failed to get the edid i2c adapter, using default modes\n");

			return 0;
		   
	  	}
	  	else
	  	{
		 	ax_9134->i2c_present = true;

	  	}

		
	   	of_node_put(sub_node);
	}

	gpio = of_get_named_gpio_flags(pdev->dev.of_node, "ax_9134,reset-gpios",0, &reset_gpio_flags);
	if (gpio < 0)
	{

		return gpio;
	}

	ax_9134->reset_gpio = gpio;	

	if(gpio_is_valid(gpio))
	{
		int i;
		int err = gpio_request(ax_9134->reset_gpio, "ax_reset_gpio");
		if (err) {
			goto free_reset_gpio;
		}		


		err = gpio_direction_output(ax_9134->reset_gpio, 0);
		if (err) {
			goto free_reset_gpio;
		}
		for(i=0;i<100;i++)
		udelay(500);
		
		err = gpio_direction_output(ax_9134->reset_gpio, 1);
		if (err) {
			goto free_reset_gpio;
		}	


	}

	return 0;

free_reset_gpio:
	if (gpio_is_valid(ax_9134->reset_gpio))
		gpio_free(ax_9134->reset_gpio);	

	return -1;

}

static int ax_9134_encoder_probe(struct platform_device *pdev)
{
	struct ax_9134_encoder *ax_9134;

	ax_9134 = devm_kzalloc(&pdev->dev, sizeof(*ax_9134), GFP_KERNEL);
	if (!ax_9134)
		return -ENOMEM;

	platform_set_drvdata(pdev, ax_9134);

	return 0;
}

static int ax_9134_encoder_remove(struct platform_device *pdev)
{
	return 0;
}

static const struct of_device_id ax_9134_encoder_of_match[] = {
	{ .compatible = "ax_9134,drm-encoder", },
	{ /* end of table */ },
};
MODULE_DEVICE_TABLE(of, ax_9134_encoder_of_match);

static struct drm_platform_encoder_driver ax_9134_encoder_driver = {
	.platform_driver = {
		.probe			= ax_9134_encoder_probe,
		.remove			= ax_9134_encoder_remove,
		.driver			= {
			.owner		= THIS_MODULE,
			.name		= "ax_9134-drm-enc",
			.of_match_table	= ax_9134_encoder_of_match,
		},
	},

	.encoder_init = ax_9134_encoder_encoder_init,
};

static int __init ax_9134_encoder_init(void)
{
	return platform_driver_register(&ax_9134_encoder_driver.platform_driver);
}

static void __exit ax_9134_encoder_exit(void)
{
	platform_driver_unregister(&ax_9134_encoder_driver.platform_driver);
}

module_init(ax_9134_encoder_init);
module_exit(ax_9134_encoder_exit);

MODULE_AUTHOR("guowc");
MODULE_DESCRIPTION("");
MODULE_LICENSE("GPL v2");
