/*
 * Code for AM335X EVM.
 *
 * Copyright (C) 2011 Texas Instruments, Inc. - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/i2c/at24.h>
#include <linux/phy.h>
#include <linux/gpio.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>
#include <linux/input/matrix_keypad.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/export.h>
#include <linux/wl12xx.h>
#include <linux/ethtool.h>
#include <linux/mfd/tps65910.h>
#include <linux/mfd/tps65217.h>
#include <linux/pwm_backlight.h>
#include <linux/input/ti_tsc.h>
#include <linux/platform_data/ti_adc.h>
#include <linux/mfd/ti_tscadc.h>
#include <linux/reboot.h>
#include <linux/pwm/pwm.h>
#include <linux/rtc/rtc-omap.h>
#include <linux/opp.h>
#include <linux/i2c/pca953x.h>
#include <linux/serial_8250.h>
#include <linux/serial_core.h>
#include <linux/wtd_adm706.h>
#include <linux/buzzer.h>

/* LCD controller is similar to DA850 */
#include <video/da8xx-fb.h>

#include <mach/hardware.h>
#include <mach/board-am335xevm.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/hardware/asp.h>

#include <plat/omap_device.h>
#include <plat/omap-pm.h>
#include <plat/irqs.h>
#include <plat/board.h>
#include <plat/common.h>
#include <plat/lcdc.h>
#include <plat/usb.h>
#include <plat/mmc.h>
#include <plat/emif.h>
#include <plat/nand.h>
#include <mach/com335x.h>

#include "board-flash.h"
#include "cpuidle33xx.h"
#include "mux.h"
#include "devices.h"
#include "hsmmc.h"
#include "common.h"

/* Convert GPIO signal to GPIO pin number */
#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))


static struct omap_board_config_kernel com335x_config[] __initdata = {
};

/* module pin mux structure */
struct pinmux_config {
	const char *string_name; /* signal name format */
	int val; /* Options for the mux register value */
};

/*
* @pin_mux - single module pin-mux structure which defines pin-mux
*			details for all its pins.
*/
static void setup_pin_mux(struct pinmux_config *pin_mux)
{
	int i;

	for (i = 0; pin_mux->string_name != NULL; pin_mux++)
		omap_mux_init_signal(pin_mux->string_name, pin_mux->val);

}

/**********TSc controller**********/
#include <linux/lis3lv02d.h>


#ifdef CONFIG_OMAP_MUX
static struct omap_board_mux board_mux[] __initdata = {
	AM33XX_MUX(I2C0_SDA, OMAP_MUX_MODE0 | AM33XX_SLEWCTRL_SLOW |
			AM33XX_INPUT_EN | AM33XX_PIN_OUTPUT),
	AM33XX_MUX(I2C0_SCL, OMAP_MUX_MODE0 | AM33XX_SLEWCTRL_SLOW |
			AM33XX_INPUT_EN | AM33XX_PIN_OUTPUT),
	{ .reg_offset = OMAP_MUX_TERMINATOR },
};
#else
#define	board_mux	NULL
#endif

/* Module pin mux for LCDC */


/* Pin mux for gpmc */

/* Module pin mux for SPI0 */
static struct pinmux_config spi0_pin_mux[] = {
	{"spi0_sclk.spi0_sclk", OMAP_MUX_MODE0 | AM33XX_PULL_ENBL
							| AM33XX_INPUT_EN},
	{"spi0_d0.spi0_d0", OMAP_MUX_MODE0 | AM33XX_PULL_ENBL | AM33XX_PULL_UP
							| AM33XX_INPUT_EN},
	{"spi0_d1.spi0_d1", OMAP_MUX_MODE0 | AM33XX_PULL_ENBL
							| AM33XX_INPUT_EN},
	{"spi0_cs0.spi0_cs0", OMAP_MUX_MODE0 | AM33XX_PULL_ENBL | AM33XX_PULL_UP
							| AM33XX_INPUT_EN},
	{NULL, 0},
};
#if 0
/* Module pin mux for uart0 */
static struct pinmux_config uart2_pin_mux[] = {
	{"uart0_rxd.uart0_rxd", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"uart0_txd.uart0_txd", OMAP_MUX_MODE0 | AM33XX_PULL_ENBL},
	{NULL, 0},
};

/**************** uart1 ****************/
static struct pinmux_config uart2_pin_mux[] = {
	{"uart1_rxd.uart1_rxd", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"uart1_txd.uart1_txd", OMAP_MUX_MODE0 | AM33XX_PULL_ENBL},
	{NULL, 0},
};
static void uart1_init(void)
{
	setup_pin_mux(uart1_pin_mux);
	return;
}


/**************** uart2 ****************/
static struct pinmux_config uart2_pin_mux[] = {
	{"gmii1_rxer.uart2_txd", OMAP_MUX_MODE6 | AM33XX_PIN_OUTPUT},
	{"gmii1_crs.uart2_rxd", OMAP_MUX_MODE6 | AM33XX_PIN_INPUT_PULLUP},
	{NULL, 0},
};
static void uart2_init(void)
{
	setup_pin_mux(uart2_pin_mux);
	return;
}

/**************** uart5 ***************/
static struct pinmux_config uart2_pin_mux[] = {
	{"rmii1_refclk.uart2_txd", OMAP_MUX_MODE3 | AM33XX_PIN_OUTPUT},
	{"gmii1_col.uart2_rxd", OMAP_MUX_MODE3 | AM33XX_PIN_INPUT_PULLUP},
	{NULL, 0},
};

static void uart5_init(void)
{
	setup_pin_mux(uart5_pin_mux);
	return;
}
#endif

/* Pin mux for i2c 2 */
static struct pinmux_config i2c2_pin_mux[] = {
	{"uart1_ctsn.i2c2_sda",    OMAP_MUX_MODE3 | AM33XX_SLEWCTRL_SLOW |
					AM33XX_PULL_UP | AM33XX_INPUT_EN},
	{"uart1_rtsn.i2c2_scl",   OMAP_MUX_MODE3 | AM33XX_SLEWCTRL_SLOW |
					AM33XX_PULL_UP | AM33XX_INPUT_EN},
	{NULL, 0},
};

static struct i2c_board_info __initdata com335x_i2c2_boardinfo[] = {

	{
		I2C_BOARD_INFO("isl1208", 0x6f),
	},
	
	{
		I2C_BOARD_INFO("rx8025", 0x32),
		.irq = -EINVAL,
	},

};

static void com335x_i2c2_init(void)
{
	setup_pin_mux(i2c2_pin_mux);
	omap_register_i2c_bus(3, 400, com335x_i2c2_boardinfo, ARRAY_SIZE(com335x_i2c2_boardinfo));
}
/**************** serial ****************/
static struct omap_device_pad serial0_pads[] __initdata = {
	OMAP_MUX_STATIC("uart0_rxd.uart0_rxd", AM33XX_PIN_INPUT_PULLUP | OMAP_MUX_MODE0),
	OMAP_MUX_STATIC("uart0_txd.uart0_txd", AM33XX_PULL_ENBL | OMAP_MUX_MODE0),
};

static struct omap_device_pad serial1_pads[] __initdata = {
	OMAP_MUX_STATIC("uart1_rxd.uart1_rxd", AM33XX_PIN_INPUT_PULLUP | OMAP_MUX_MODE0),
	OMAP_MUX_STATIC("uart1_txd.uart1_txd", AM33XX_PULL_ENBL | OMAP_MUX_MODE0),
};

static struct omap_device_pad serial2_pads[] __initdata = {
	OMAP_MUX_STATIC("mii1_rxerr.uart2_txd", OMAP_MUX_MODE6 | AM33XX_PIN_OUTPUT),
	OMAP_MUX_STATIC("mii1_crs.uart2_rxd", OMAP_MUX_MODE6 | AM33XX_PIN_INPUT_PULLUP),
};

static struct omap_device_pad serial3_pads[] __initdata = {
	OMAP_MUX_STATIC("ecap0_in_pwm0_out.uart3_txd", OMAP_MUX_MODE1 | AM33XX_PIN_OUTPUT),
	OMAP_MUX_STATIC("spi0_cs1.uart3_rxd", OMAP_MUX_MODE1 | AM33XX_PIN_INPUT_PULLUP),
};

static struct omap_device_pad serial4_pads[] __initdata = {
	OMAP_MUX_STATIC("uart0_rtsn.uart4_txd", OMAP_MUX_MODE1 | AM33XX_PIN_OUTPUT),
	OMAP_MUX_STATIC("uart0_ctsn.uart4_rxd", OMAP_MUX_MODE1 | AM33XX_PIN_INPUT_PULLUP),
};

static struct omap_device_pad serial5_pads[] __initdata = {
	OMAP_MUX_STATIC("rmii1_refclk.uart5_txd", OMAP_MUX_MODE3 | AM33XX_PIN_OUTPUT),
	OMAP_MUX_STATIC("mii1_col.uart5_rxd", OMAP_MUX_MODE3 | AM33XX_PIN_INPUT_PULLUP),
};


static struct omap_board_data serial0_data __initdata = {
	.id             = 0,
	.pads           = serial0_pads,
	.pads_cnt       = ARRAY_SIZE(serial0_pads),
};

static struct omap_board_data serial1_data __initdata = {
	.id             = 1,
	.pads           = serial1_pads,
	.pads_cnt       = ARRAY_SIZE(serial1_pads),
};

static struct omap_board_data serial2_data __initdata = {
	.id             = 2,
	.pads           = serial2_pads,
	.pads_cnt       = ARRAY_SIZE(serial2_pads),
};

static struct omap_board_data serial3_data __initdata = {
	.id             = 3,
	.pads           = serial3_pads,
	.pads_cnt       = ARRAY_SIZE(serial3_pads),
};

static struct omap_board_data serial4_data __initdata = {
	.id             = 4,
	.pads           = serial4_pads,
	.pads_cnt       = ARRAY_SIZE(serial4_pads),
};

static struct omap_board_data serial5_data __initdata = {
	.id             = 5,
	.pads           = serial5_pads,
	.pads_cnt       = ARRAY_SIZE(serial5_pads),
};

static inline void com335x_serial_init(void)
{
	omap_serial_init_port(&serial0_data, NULL);
	omap_serial_init_port(&serial1_data, NULL);
	omap_serial_init_port(&serial2_data, NULL);
	omap_serial_init_port(&serial3_data, NULL);
	omap_serial_init_port(&serial4_data, NULL);
	omap_serial_init_port(&serial5_data, NULL);
}

/**************** RGMII1 Setup ****************/
/* COM335X EVM Phy ID and Debug Registers */
#define COM335X_EVM_PHY_ID		0x004dd072
#define COM335X_PHY_MASK		0xffffffef
#define AR8035_PHY_DEBUG_ADDR_REG	0x1d
#define AR8035_PHY_DEBUG_DATA_REG	0x1e
#define AR8035_DEBUG_RGMII_CLK_DLY_REG	0x5
#define AR8035_RGMII_TX_CLK_DLY		BIT(8)

static struct pinmux_config rgmii1_pin_mux[] = {
	{"mii1_txen.rgmii1_tctl", OMAP_MUX_MODE2 | AM33XX_PIN_OUTPUT},
	{"mii1_rxdv.rgmii1_rctl", OMAP_MUX_MODE2 | AM33XX_PIN_INPUT_PULLDOWN},
	{"mii1_txd3.rgmii1_td3", OMAP_MUX_MODE2 | AM33XX_PIN_OUTPUT},
	{"mii1_txd2.rgmii1_td2", OMAP_MUX_MODE2 | AM33XX_PIN_OUTPUT},
	{"mii1_txd1.rgmii1_td1", OMAP_MUX_MODE2 | AM33XX_PIN_OUTPUT},
	{"mii1_txd0.rgmii1_td0", OMAP_MUX_MODE2 | AM33XX_PIN_OUTPUT},
	{"mii1_txclk.rgmii1_tclk", OMAP_MUX_MODE2 | AM33XX_PIN_OUTPUT},
	{"mii1_rxclk.rgmii1_rclk", OMAP_MUX_MODE2 | AM33XX_PIN_INPUT_PULLDOWN},
	{"mii1_rxd3.rgmii1_rd3", OMAP_MUX_MODE2 | AM33XX_PIN_INPUT_PULLDOWN},
	{"mii1_rxd2.rgmii1_rd2", OMAP_MUX_MODE2 | AM33XX_PIN_INPUT_PULLDOWN},
	{"mii1_rxd1.rgmii1_rd1", OMAP_MUX_MODE2 | AM33XX_PIN_INPUT_PULLDOWN},
	{"mii1_rxd0.rgmii1_rd0", OMAP_MUX_MODE2 | AM33XX_PIN_INPUT_PULLDOWN},
	{"mdio_data.mdio_data", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"mdio_clk.mdio_clk", OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT_PULLUP},
	{NULL, 0},
};

static int am33xx_tx_clk_dly_phy_fixup(struct phy_device *phydev)
{
	phy_write(phydev, AR8035_PHY_DEBUG_ADDR_REG,
		  AR8035_DEBUG_RGMII_CLK_DLY_REG);
	phy_write(phydev, AR8035_PHY_DEBUG_DATA_REG, AR8035_RGMII_TX_CLK_DLY);

	return 0;
}

static void com335x_eth_init(void)
{
	setup_pin_mux(rgmii1_pin_mux);
	am33xx_cpsw_init(AM33XX_CPSW_MODE_RGMII, "0:04", NULL);
	phy_register_fixup_for_uid(COM335X_EVM_PHY_ID, COM335X_PHY_MASK, am33xx_tx_clk_dly_phy_fixup);
	return;
}

/**************** touch screen *******************************/
static struct tsc_data am335x_touchscreen_data  = {
	.wires  = 4,
	.x_plate_resistance = 200,
	.steps_to_configure = 5,
};

static struct adc_data am335x_adc_data = {
	.adc_channels = 4,
};

static struct mfd_tscadc_board tscadc = {
	.tsc_init = &am335x_touchscreen_data,
	.adc_init = &am335x_adc_data,
};

#if 0
static struct pinmux_config tsc_pin_mux[] = {
	{"ain0.ain0",           OMAP_MUX_MODE0 | AM33XX_INPUT_EN},
	{"ain1.ain1",           OMAP_MUX_MODE0 | AM33XX_INPUT_EN},
	{"ain2.ain2",           OMAP_MUX_MODE0 | AM33XX_INPUT_EN},
	{"ain3.ain3",           OMAP_MUX_MODE0 | AM33XX_INPUT_EN},
	{"ain4.ain4",           OMAP_MUX_MODE0 | AM33XX_INPUT_EN},
	{"ain5.ain5",           OMAP_MUX_MODE0 | AM33XX_INPUT_EN},
	{"ain6.ain6",           OMAP_MUX_MODE0 | AM33XX_INPUT_EN},
	{"ain7.ain7",           OMAP_MUX_MODE0 | AM33XX_INPUT_EN},
	{"vrefp.vrefp",         OMAP_MUX_MODE0 | AM33XX_INPUT_EN},
	{"vrefn.vrefn",         OMAP_MUX_MODE0 | AM33XX_INPUT_EN},
	{NULL, 0},
};
#endif

static void com335x_tsc_init(void)
{
	int err;
#if 0
	setup_pin_mux(tsc_pin_mux);
#endif

	err = am33xx_register_mfd_tscadc(&tscadc);
	if (err)
		pr_err("failed to register touchscreen device\n");
}


/************* LCD *************************/
static struct pinmux_config lcdc_pin_mux[] = {
	{"gpmc_ad8.lcd_data23",		OMAP_MUX_MODE1 | AM33XX_PIN_OUTPUT},
	{"gpmc_ad9.lcd_data22",		OMAP_MUX_MODE1 | AM33XX_PIN_OUTPUT},
	{"gpmc_ad10.lcd_data21",	OMAP_MUX_MODE1 | AM33XX_PIN_OUTPUT},
	{"gpmc_ad11.lcd_data20",	OMAP_MUX_MODE1 | AM33XX_PIN_OUTPUT},
	{"gpmc_ad12.lcd_data19",	OMAP_MUX_MODE1 | AM33XX_PIN_OUTPUT},
	{"gpmc_ad13.lcd_data18",	OMAP_MUX_MODE1 | AM33XX_PIN_OUTPUT},
	{"gpmc_ad14.lcd_data17",	OMAP_MUX_MODE1 | AM33XX_PIN_OUTPUT},
	{"gpmc_ad15.lcd_data16",	OMAP_MUX_MODE1 | AM33XX_PIN_OUTPUT},
	{NULL, 0},
};

static struct pinmux_config lcdc16_pin_mux[] = {
	{"lcd_data0.lcd_data0",		OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT
        | AM33XX_PULL_DISA},
	{"lcd_data1.lcd_data1",		OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT
        | AM33XX_PULL_DISA},
	{"lcd_data2.lcd_data2",		OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT
        | AM33XX_PULL_DISA},
	{"lcd_data3.lcd_data3",		OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT
        | AM33XX_PULL_DISA},
	{"lcd_data4.lcd_data4",		OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT
        | AM33XX_PULL_DISA},
	{"lcd_data5.lcd_data5",		OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT
        | AM33XX_PULL_DISA},
	{"lcd_data6.lcd_data6",		OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT
        | AM33XX_PULL_DISA},
	{"lcd_data7.lcd_data7",		OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT
        | AM33XX_PULL_DISA},
	{"lcd_data8.lcd_data8",		OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT
        | AM33XX_PULL_DISA},
	{"lcd_data9.lcd_data9",		OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT
        | AM33XX_PULL_DISA},
	{"lcd_data10.lcd_data10",	OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT
        | AM33XX_PULL_DISA},
	{"lcd_data11.lcd_data11",	OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT
        | AM33XX_PULL_DISA},
	{"lcd_data12.lcd_data12",	OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT
        | AM33XX_PULL_DISA},
	{"lcd_data13.lcd_data13",	OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT
        | AM33XX_PULL_DISA},
	{"lcd_data14.lcd_data14",	OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT
        | AM33XX_PULL_DISA},
	{"lcd_data15.lcd_data15",	OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT
        | AM33XX_PULL_DISA},
	{"lcd_vsync.lcd_vsync",		OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
	{"lcd_hsync.lcd_hsync",		OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
	{"lcd_pclk.lcd_pclk",		OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
	{"lcd_ac_bias_en.lcd_ac_bias_en", OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
	{NULL, 0},
};
static int __init conf_disp_pll(int rate)
{
	struct clk *disp_pll;
	int ret = -EINVAL;

	disp_pll = clk_get(NULL, "dpll_disp_ck");
	if (IS_ERR(disp_pll)) {
		pr_err("Cannot clk_get disp_pll\n");
		goto out;
	}

	ret = clk_set_rate(disp_pll, rate);
	clk_put(disp_pll);
out:
	return ret;
}

static const struct display_panel com335x_panel = {
	WVGA,
	16,
	16,
	COLOR_ACTIVE,
};

static struct lcd_ctrl_config lcd_cfg = {
	&com335x_panel,
	.ac_bias		= 255,
	.ac_bias_intrpt		= 0,
	.dma_burst_sz		= 16,
	.bpp			= 16,
	.fdd			= 0x80,
	.tft_alt_mode		= 0,
	.stn_565_mode		= 0,
	.mono_8bit_mode		= 0,
	.invert_line_clock	= 1,
	.invert_frm_clock	= 1,
	.sync_edge		= 0,
	.sync_ctrl		= 1,
	.raster_order		= 0,
};


struct da8xx_lcdc_platform_data com335x_lcd_pdata[] = {
	[0] = {
		.manu_name = "AUO",
		.controller_data = &lcd_cfg,
		.type = "AUO_G121SN01",
	},

	[1] = {
		.manu_name = "AUO",
		.controller_data = &lcd_cfg,
		.type = "AUO_G156XW01",
	},
	
	[2] = {
		.manu_name = "AUO",
		.controller_data = &lcd_cfg,
		.type = "AUO_AT070TN94",
	},
/*
   Add by Michael,Date:2015/04/23.
   Add and modify support lcd screen models and parameters.  
*/

	[3] = {
		.manu_name = "LVDS",
		.controller_data = &lcd_cfg,
		.type = "LVDS_1024x768",
	},

	[4] = { 
		.manu_name = "LVDS",
		.controller_data = &lcd_cfg,
		.type = "INX_G150XGE",
	},

	[5] = { 
		.manu_name = "LVDS",
		.controller_data = &lcd_cfg,
		.type = "LVDS_800x600",
	},

	[6] = { 
		.manu_name = "LVDS",
		.controller_data = &lcd_cfg,
		.type = "LVDS_1366x768",
	},

	[7] = { 
		.manu_name = "LVDS",
		.controller_data = &lcd_cfg,
		.type = "LVDS_1024x600",
	},
/************************* end *******************************/

};

struct da8xx_lcdc_platform_data *lcdc_pdata = NULL;

static void sgx_init(void)
{
	if (omap3_has_sgx()) {
		am33xx_gpu_init();
	}
}


static int __init com335x_setup_lcd_type(char *str)
{
	unsigned int i;
	struct da8xx_lcdc_platform_data *pdata = com335x_lcd_pdata;
	
	for(i=0; i < sizeof(com335x_lcd_pdata)/sizeof(struct da8xx_lcdc_platform_data); i++)
	{
		if(!strcmp(str, pdata->type))
		{
			lcdc_pdata = pdata;
			return 0;
		}
		pdata++;
	}
	
	return 0;
}

__setup("lcdtype=", com335x_setup_lcd_type);


static void com335x_lcdc_init(void)
{
//	struct da8xx_lcdc_platform_data *lcdc_pdata;
	if(lcdc_pdata)
	{
		setup_pin_mux(lcdc16_pin_mux);
//		setup_pin_mux(lcdc_pin_mux);

		if (conf_disp_pll(300000000)) {
			pr_info("Failed configure display PLL, not attempting to"
				"register LCDC\n");
			return;
		}

//		lcdc_pdata = &g156xw01_pdata;
		lcdc_pdata->get_context_loss_count = omap_pm_get_dev_context_loss_count;

		if (am33xx_register_lcdc(lcdc_pdata))
			pr_info("Failed to register LCDC device\n");

		sgx_init();
	}
	return;
}


/*********************** Setup McASP 1*********************************** */
/* Module pin mux for mcasp0 */
static struct pinmux_config mcasp1_pin_mux[] = {
	{"mcasp0_aclkr.mcasp1_aclkx", OMAP_MUX_MODE3 |AM33XX_PIN_INPUT_PULLDOWN},	/* BCLK */
	{"mcasp0_fsr.mcasp1_fsx", OMAP_MUX_MODE3 | AM33XX_PIN_INPUT_PULLDOWN},		/* LRCLK */
	{"mcasp0_axr1.mcasp1_axr0", OMAP_MUX_MODE3 | AM33XX_PIN_INPUT_PULLDOWN},	/* Data out */
	{"mcasp0_ahclkx.mcasp1_axr1", OMAP_MUX_MODE3 | AM33XX_PIN_INPUT_PULLDOWN},	/* Data in */
	{NULL, 0},
};

static u8 am335x_iis_serializer_direction1[] = {
	TX_MODE,	RX_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,	INACTIVE_MODE,
};
static struct snd_platform_data com335x_snd_data = {
	.tx_dma_offset	= 0x46400000,	/* McASP1*/
	.rx_dma_offset	= 0x46400000,
	.op_mode	= DAVINCI_MCASP_IIS_MODE,
	.num_serializer	= ARRAY_SIZE(am335x_iis_serializer_direction1),
	.tdm_slots	= 2,
	.serial_dir	= am335x_iis_serializer_direction1,
	.asp_chan_q	= EVENTQ_2,
	.version	= MCASP_VERSION_3,
	.txnumevt	= 1,
	.rxnumevt	= 1,
//	.get_context_loss_count	=
//			omap_pm_get_dev_context_loss_count,
};

#if 0
static void mcasp1_init(void)
{
	printk("com335x: mcasp0 init\n");
	/* Configure McASP */
	setup_pin_mux(mcasp1_pin_mux);
	am335x_register_mcasp(&com335x_snd_data, 0);
	return;
}
#endif

static void com335x_audio_init(void)
{
	printk("com335x: mcasp1 init\n");
	/* Configure McASP */
	setup_pin_mux(mcasp1_pin_mux);
	am335x_register_mcasp(&com335x_snd_data, 1);
	return;
}
/***************************** Setup nand flash**************** */
static struct mtd_partition com335x_nand_partitions[] = {
/* All the partition sizes are listed in terms of NAND block size */
	{
		.name           = "SPL",
		.offset         = 0,			/* Offset = 0x0 */
		.size           = SZ_128K,
	},
	{
		.name           = "SPL.backup1",
		.offset         = MTDPART_OFS_APPEND,	/* Offset = 0x20000 */
		.size           = SZ_128K,
	},
	{
		.name           = "SPL.backup2",
		.offset         = MTDPART_OFS_APPEND,	/* Offset = 0x40000 */
		.size           = SZ_128K,
	},
	{
		.name           = "SPL.backup3",
		.offset         = MTDPART_OFS_APPEND,	/* Offset = 0x60000 */
		.size           = SZ_128K,
	},
	{
		.name           = "U-Boot",
		.offset         = MTDPART_OFS_APPEND,   /* Offset = 0x80000 */
		.size           = 15 * SZ_128K,
	},
	{
		.name           = "U-Boot Env",
		.offset         = MTDPART_OFS_APPEND,   /* Offset = 0x260000 */
		.size           = 1 * SZ_128K,
	},
	{
		.name           = "logo",
		.offset         = MTDPART_OFS_APPEND,   /* Offset = 0x280000 */
		.size           = 24 * SZ_128K,
	},
	{
		.name           = "Kernel",
		.offset         = MTDPART_OFS_APPEND,   /* Offset = 0x580000 */
		.size           = 40 * SZ_128K,
	},
	{
		.name           = "File System",
		.offset         = MTDPART_OFS_APPEND,   /* Offset = 0xA80000 */
		.size           = MTDPART_SIZ_FULL,
	},
};
static struct pinmux_config nand_pin_mux[] = {
	{"gpmc_ad0.gpmc_ad0",	  OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ad1.gpmc_ad1",	  OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ad2.gpmc_ad2",	  OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ad3.gpmc_ad3",	  OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ad4.gpmc_ad4",	  OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ad5.gpmc_ad5",	  OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ad6.gpmc_ad6",	  OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ad7.gpmc_ad7",	  OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_wait0.gpmc_wait0", OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_wpn.gpmc_wpn",	  OMAP_MUX_MODE7 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_csn0.gpmc_csn0",	  OMAP_MUX_MODE0 | AM33XX_PULL_DISA},
	{"gpmc_advn_ale.gpmc_advn_ale",  OMAP_MUX_MODE0 | AM33XX_PULL_DISA},
	{"gpmc_oen_ren.gpmc_oen_ren",	 OMAP_MUX_MODE0 | AM33XX_PULL_DISA},
	{"gpmc_wen.gpmc_wen",     OMAP_MUX_MODE0 | AM33XX_PULL_DISA},
	{"gpmc_ben0_cle.gpmc_ben0_cle",	 OMAP_MUX_MODE0 | AM33XX_PULL_DISA},
	{NULL, 0},
};

static struct gpmc_timings am335x_nand_timings = {
	.sync_clk = 0,

	.cs_on = 0,
	.cs_rd_off = 44,
	.cs_wr_off = 44,

	.adv_on = 6,
	.adv_rd_off = 34,
	.adv_wr_off = 44,
	.we_off = 40,
	.oe_off = 54,

	.access = 64,
	.rd_cycle = 82,
	.wr_cycle = 82,

	.wr_access = 40,
	.wr_data_mux_bus = 0,
};

static void com335x_nand_init(void)
{
	struct omap_nand_platform_data *pdata;
	struct gpmc_devices_info gpmc_device[2] = {
		{ NULL, 0 },
		{ NULL, GPMC_DEVICE_NOR},
	};

	setup_pin_mux(nand_pin_mux);
	pdata = omap_nand_init(com335x_nand_partitions,
		ARRAY_SIZE(com335x_nand_partitions), 0, 0,
		&am335x_nand_timings);
	if (!pdata)
		return;
	pdata->ecc_opt =OMAP_ECC_BCH8_CODE_HW;
	pdata->elm_used = true;
	gpmc_device[0].pdata = pdata;
	gpmc_device[0].flag = GPMC_DEVICE_NAND;

	omap_init_gpmc(gpmc_device, sizeof(gpmc_device));
	omap_init_elm();
}

/********************************usb ************************/
static struct pinmux_config usb0_pin_mux[] = {
	{"usb0_drvvbus.usb0_drvvbus",    OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
	{NULL, 0},
};

static struct pinmux_config usb1_pin_mux[] = {
	{"usb1_drvvbus.usb1_drvvbus",    OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
	{NULL, 0},
};

static void usb0_init(void)
{
	setup_pin_mux(usb0_pin_mux);
	return;
}

static void usb1_init(void)
{
	setup_pin_mux(usb1_pin_mux);
	return;
}

static struct omap_musb_board_data musb_board_data = {
	.interface_type	= MUSB_INTERFACE_ULPI,
	/*
	 * mode[0:3] = USB0PORT's mode
	 * mode[4:7] = USB1PORT's mode
	 * AM335X beta EVM has USB0 in OTG mode and USB1 in host mode.
	 */
	.mode           = (MUSB_HOST << 4) | MUSB_OTG,
	.power		= 500,
	.instances	= 1,
};


/**************** mmc ****************/
static struct pinmux_config mmc0_common_pin_mux[] = {
	{"mmc0_dat3.mmc0_dat3",	OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"mmc0_dat2.mmc0_dat2",	OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"mmc0_dat1.mmc0_dat1",	OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"mmc0_dat0.mmc0_dat0",	OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"mmc0_clk.mmc0_clk",	OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"mmc0_cmd.mmc0_cmd",	OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{NULL, 0},
};

static struct pinmux_config mmc0_cd_pin_mux[]= {
	{"mcasp0_ahclkr.gpio3_17", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT_PULLUP},
	{NULL, 0},

};

static struct omap2_hsmmc_info com335x_mmc[] __initdata = {
	{
		.mmc            = 1,
		.caps           = MMC_CAP_4_BIT_DATA,
		.gpio_cd        = GPIO_TO_PIN(3,16),
		.gpio_wp        = -EINVAL,
		.ocr_mask       = MMC_VDD_32_33 | MMC_VDD_33_34, /* 3V3 */
	},
	{}      /* Terminator */
};

static void com335x_mmc_init(void)
{
	setup_pin_mux(mmc0_common_pin_mux);
	setup_pin_mux(mmc0_cd_pin_mux);
	omap2_hsmmc_init(com335x_mmc);
	return;
}

/******************* GPIO button ***************/
#define COM335X_KEYS_DEBOUNCE_MS	10
#define COM335X_GPIO_KEYS_POLL_MS	200
static struct gpio_keys_button com335x_gpio_keys[] = {

	{
		.type			= EV_KEY,
		.active_low		= 1,
		.wakeup			= 0,
		.debounce_interval	= COM335X_KEYS_DEBOUNCE_MS,
		.code			= KEY_UP,
		.gpio			= COM335X_EXT_GPIO(15),
		.desc			= "key_up"
	},

	{
		.type			= EV_KEY,
		.active_low		= 1,
		.wakeup			= 0,
		.debounce_interval	= COM335X_KEYS_DEBOUNCE_MS,
		.code			= KEY_LEFT,
		.gpio			= COM335X_EXT_GPIO(13),
		.desc			= "key_left"
	},

	{
		.type			= EV_KEY,
		.active_low		= 1,
		.wakeup			= 0,
		.debounce_interval	= COM335X_KEYS_DEBOUNCE_MS,
		.code			= KEY_RIGHT,
		.gpio			= COM335X_EXT_GPIO(12),
		.desc			= "key_right"
	},

	{
		.type			= EV_KEY,
		.active_low		= 1,
		.wakeup			= 0,
		.debounce_interval	= COM335X_KEYS_DEBOUNCE_MS,
		.code			= KEY_DOWN,
		.gpio			= COM335X_EXT_GPIO(14),
		.desc			= "key_down"
	},

	{
		.type			= EV_KEY,
		.active_low		= 1,
		.wakeup			= 0,
		.debounce_interval	= COM335X_KEYS_DEBOUNCE_MS,
		.code			= KEY_ENTER,
		.gpio			= COM335X_EXT_GPIO(10),
		.desc			= "key_ok"
	},

	{
		.type			= EV_KEY,
		.active_low		= 1,
		.wakeup			= 0,
		.debounce_interval	= COM335X_KEYS_DEBOUNCE_MS,
		.code			= KEY_ESC,
		.gpio			= COM335X_EXT_GPIO(11),
		.desc			= "key_esc"
	},
};

static struct gpio_keys_platform_data com335x_gpio_keys_pdata = {
	.buttons = com335x_gpio_keys,
	.nbuttons = ARRAY_SIZE(com335x_gpio_keys),
	.poll_interval = COM335X_GPIO_KEYS_POLL_MS,
};

static struct platform_device com335x_gpio_keys_device = {
	.name = "gpio-keys-polled",
	.id = 0,
	.dev = {
		.platform_data = &com335x_gpio_keys_pdata
	},
};

static void __init com335x_add_buttons(void)
{
	platform_device_register(&com335x_gpio_keys_device);
}
/********************** GPIO led ****************/
static struct pinmux_config core_led_pin_mux[] = {

	{"emu0.gpio3_7",           OMAP_MUX_MODE7 | AM33XX_PIN_OUTPUT},
	{NULL, 0},
};

static struct gpio_led com335x_leds[] = {
	{	/* core board */
		.name			= "system-led:yellow",
		.gpio			= GPIO_TO_PIN(3, 7),
		.active_low		= 0,
		.default_trigger	= "heartbeat",
	},
	{	/* base board */
		.name			= "user-led:red",
		.gpio			= COM335X_EXT_GPIO(2),
		.active_low		= 0,
		.default_trigger	= "none",
	},
};

static struct gpio_led_platform_data com335x_leds_info = {
	.leds		= com335x_leds,
	.num_leds	= ARRAY_SIZE(com335x_leds),
};

static struct platform_device com335x_leds_device = {
	.name		= "leds-gpio",
	.id		= -1,
	.dev		= {
		.platform_data	= &com335x_leds_info,
	},
};

static void __init com335x_add_leds(void)
{
	setup_pin_mux(core_led_pin_mux);

	platform_device_register(&com335x_leds_device);
}

/******************* tps65217 ********************************/
#if 0
static struct tps65217_rdelay dcdc2_ramp_delay = {
	.ramp_delay = 70000,
};
#endif


/* TPS65217 voltage regulator support */

/* 1.8V */
static struct regulator_consumer_supply tps65217_dcdc1_consumers[] = {
	{
		.supply = "vdds_osc",
	},
	{
		.supply = "vdds_pll_ddr",
	},
	{
		.supply = "vdds_pll_mpu",
	},
	{
		.supply = "vdds_pll_core_lcd",
	},
	{
		.supply = "vdds_sram_mpu_bb",
	},
	{
		.supply = "vdds_sram_core_bg",
	},
	{
		.supply = "vdda_usb0_1p8v",
	},
	{
		.supply = "vdds_ddr",
	},
	{
		.supply = "vdds",
	},
	{
		.supply = "vdds_hvx_1p8v",
	},
	{
		.supply = "vdda_adc",
	},
	{
		.supply = "ddr2",
	},
};

/* 1.1V */
static struct regulator_consumer_supply tps65217_dcdc2_consumers[] = {
	{
		.supply = "vdd_mpu",
	},
};

/* 1.1V */
static struct regulator_consumer_supply tps65217_dcdc3_consumers[] = {
	{
		.supply = "vdd_core",
	},
};

/* 1.8V LDO */
static struct regulator_consumer_supply tps65217_ldo1_consumers[] = {
	{
		.supply = "vdds_rtc",
	},
};

/* 3.3V LDO */
static struct regulator_consumer_supply tps65217_ldo2_consumers[] = {
	{
		.supply = "vdds_any_pn",
	},
};

/* 3.3V LDO */
static struct regulator_consumer_supply tps65217_ldo3_consumers[] = {
	{
		.supply = "vdds_hvx_ldo3_3p3v",
	},
	{
		.supply = "vdda_usb0_3p3v",
	},
};

/* 3.3V LDO */
static struct regulator_consumer_supply tps65217_ldo4_consumers[] = {
	{
		.supply = "vdds_hvx_ldo4_3p3v",
	},
};

static struct regulator_init_data tps65217_regulator_data[] = {
	/* dcdc1 */
	{
		.constraints = {
			.min_uV = 900000,
			.max_uV = 1800000,
			.boot_on = 1,
			.always_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65217_dcdc1_consumers),
		.consumer_supplies = tps65217_dcdc1_consumers,
	},

	/* dcdc2 */
	{
		.constraints = {
			.min_uV = 900000,
			.max_uV = 3300000,
			.valid_ops_mask = (REGULATOR_CHANGE_VOLTAGE |
				REGULATOR_CHANGE_STATUS),
			.boot_on = 1,
			.always_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65217_dcdc2_consumers),
		.consumer_supplies = tps65217_dcdc2_consumers,
	},

	/* dcdc3 */
	{
		.constraints = {
			.min_uV = 900000,
			.max_uV = 1500000,
			.valid_ops_mask = (REGULATOR_CHANGE_VOLTAGE |
				REGULATOR_CHANGE_STATUS),
			.boot_on = 1,
			.always_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65217_dcdc3_consumers),
		.consumer_supplies = tps65217_dcdc3_consumers,
	},

	/* ldo1 */
	{
		.constraints = {
			.min_uV = 1000000,
			.max_uV = 3300000,
			.valid_ops_mask = REGULATOR_CHANGE_STATUS,
			.boot_on = 1,
			.always_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65217_ldo1_consumers),
		.consumer_supplies = tps65217_ldo1_consumers,
	},

	/* ldo2 */
	{
		.constraints = {
			.min_uV = 900000,
			.max_uV = 3300000,
			.valid_ops_mask = (REGULATOR_CHANGE_VOLTAGE |
				REGULATOR_CHANGE_STATUS),
			.boot_on = 1,
			.always_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65217_ldo2_consumers),
		.consumer_supplies = tps65217_ldo2_consumers,
	},

	/* ldo3 */
	{
		.constraints = {
			.min_uV = 1800000,
			.max_uV = 3300000,
			.valid_ops_mask = (REGULATOR_CHANGE_VOLTAGE |
				REGULATOR_CHANGE_STATUS),
			.boot_on = 1,
			.always_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65217_ldo3_consumers),
		.consumer_supplies = tps65217_ldo3_consumers,
	},

	/* ldo4 */
	{
		.constraints = {
			.min_uV = 1800000,
			.max_uV = 3300000,
			.valid_ops_mask = (REGULATOR_CHANGE_VOLTAGE |
				REGULATOR_CHANGE_STATUS),
			.boot_on = 1,
			.always_on = 1,
		},
		.num_consumer_supplies = ARRAY_SIZE(tps65217_ldo4_consumers),
		.consumer_supplies = tps65217_ldo4_consumers,
	},
};

static struct tps65217_board com335x_tps65217_info = {
	.tps65217_init_data = &tps65217_regulator_data[0],
};

static struct i2c_board_info tps65217_i2c_boardinfo[] = {
	{
		I2C_BOARD_INFO("tps65217", TPS65217_I2C_ID),
		.platform_data  = &com335x_tps65217_info,
	},
};

#if 0
static void tps65217_init(void)
{
	struct i2c_adapter *adapter;
	struct i2c_client *client;
	struct device *mpu_dev;
	struct tps65217 *tps;
	unsigned int val;
	int ret;

	mpu_dev = omap_device_get_by_hwmod_name("mpu");
	if (!mpu_dev)
		pr_warning("%s: unable to get the mpu device\n", __func__);

	/* I2C0 adapter request */
	adapter = i2c_get_adapter(1);
	if (!adapter) {
		pr_err("failed to get adapter i2c1\n");
		return;
	}

	client = i2c_new_device(adapter, tps65217_i2c_boardinfo);
	if (!client)
		pr_err("failed to register tps65217 to i2c1\n");

	i2c_put_adapter(adapter);

	tps = (struct tps65217 *)i2c_get_clientdata(client);

	ret = tps65217_reg_read(tps, TPS65217_REG_STATUS, &val);
	if (ret) {
		pr_err("failed to read tps65217 status reg\n");
		return;
	}

	if (!(val & TPS65217_STATUS_ACPWR)) {
		/* If powered by USB then disable OPP120 and OPPTURBO */
		pr_info("Maximum current provided by the USB port is 500mA"
			" which is not sufficient when operating @OPP120 and"
			" OPPTURBO. The current requirement for some use-cases"
			" using OPP100 might also exceed the maximum current"
			" that the USB port can provide. Unless you are fully"
			" confident that the current requirements for OPP100"
			" use-case don't exceed the USB limits, switching to"
			" AC power is recommended.\n");
		opp_disable(mpu_dev, 600000000);
		opp_disable(mpu_dev, 720000000);
	}
}
#endif

/*
 * external Watchdog
*/
static struct adm706_platform_data com335x_adm706_pdata = {
	.enable_pin = GPIO_TO_PIN(3,14),
	.feed_pin	= GPIO_TO_PIN(3,15),
};

static struct platform_device com335x_adm706_device = {
	.name		= "adm706",
	.id		= -1,
	.dev = {
		.platform_data = &com335x_adm706_pdata,
	}
};

static struct pinmux_config adm706_pin[] = 
{
	{"mcasp0_aclkx.gpio3_14", OMAP_MUX_MODE7 | AM33XX_PIN_OUTPUT_PULLUP},	/* enable pin */
	{"mcasp0_fsx.gpio3_15", OMAP_MUX_MODE7 | AM33XX_PIN_OUTPUT_PULLUP},	/* feed pin */
	{NULL, 0},
};

static void com335x_add_adm706(void){
	struct adm706_platform_data *adm706_pdata;

	adm706_pdata = &com335x_adm706_pdata;

	setup_pin_mux(adm706_pin);
	
	if(gpio_request_one(adm706_pdata->enable_pin, GPIOF_INIT_HIGH, "Watchdog enable") < 0)
	{
		printk("Failed to request watchdog enable gpio.\n");
		return;
	}
	
	if(gpio_request_one(adm706_pdata->feed_pin, GPIOF_INIT_HIGH, "Watchdog feed") < 0)
	{
		printk("Failed to request watchdog feed gpio.\n");
		return;
	}

	platform_device_register(&com335x_adm706_device);
}
/**************************buzzer*************************/
static struct buzzer_platform_data com335x_buzzer_pdata = {
	.bzr_pin = -1,
	.active_low = 0,
};

static struct platform_device com335x_buzzer_device = {
	.name	= "buzzer-gpio",
	.id	= -1,
	.dev	= {
			.platform_data = &com335x_buzzer_pdata,
	}
};

static void com335x_add_buzzer(void){
	struct buzzer_platform_data *bzr_pdata;

	bzr_pdata = com335x_buzzer_device.dev.platform_data;
	if(gpio_request_one(COM335X_EXT_GPIO(1), 0/*GPIOF_INIT_HIGH*/, "buzzer gpio") < 0)
	{
		printk("Failed to request buzzer gpio.\n");
		return;
	}
	bzr_pdata->bzr_pin = COM335X_EXT_GPIO(1);

	platform_device_register(&com335x_buzzer_device);
};

static void com335x_reset_3g(void){

	if(gpio_request_one(COM335X_EXT_GPIO(5), GPIOF_INIT_HIGH, "3g-reset") < 0)
	{
		printk("Failed to request 3g-reset pin.\n");
		return;
	}
	
	if(gpio_request_one(COM335X_EXT_GPIO(6), 0/*GPIOF_INIT_HIGH*/, "3g-disable") < 0)
	{
		printk("Failed to request 3g-disable pin.\n");
		return;
	}
	
	/* reset 3g */
	gpio_set_value(COM335X_EXT_GPIO(5), 0);
	msleep(150);
	gpio_set_value(COM335X_EXT_GPIO(5), 1);

	/* Enable 3g RF */
	gpio_set_value(COM335X_EXT_GPIO(6), 1);
}
/**************** PCA9555 ****************/

int com335x_pac9555_setup(struct i2c_client *client, unsigned gpio, unsigned ngpio, void *context)
{
//	com335x_leds[1].gpio = COM335X_EXT_GPIO(2);
//	com335x_mmc[0].gpio_cd = COM335X_EXT_GPIO(9);
//	gpio_direction_output(com335x_leds[1].gpio, 1);
/* 	com335x_add_adm706(); */
	com335x_add_buzzer();
	com335x_reset_3g();
	/* WIFI power enable */
	if(gpio_request_one(COM335X_EXT_GPIO(4), GPIOF_INIT_HIGH, "wifi power") < 0)
	{
		printk("Failed to enable wifi power\n");
	}
	return 0;
}
static struct pca953x_platform_data com335x_pca9555_info = {
	.gpio_base		= COM335X_EXT_GPIO_BASE,
	.irq_base		= 396,	//(128 + 128),	/* Not sure.we assume as 128(AM335x int vector) + 128(GPIO int) */
	.setup		= com335x_pac9555_setup,
};
/**************** i2c0 ****************/
static void __iomem *am33xx_i2c0_base;

int am33xx_map_i2c0(void)
{
	am33xx_i2c0_base = ioremap(AM33XX_I2C0_BASE, SZ_4K);

	if (!am33xx_i2c0_base)
		return -ENOMEM;

	return 0;
}

void __iomem *am33xx_get_i2c0_base(void)
{
	return am33xx_i2c0_base;
}

/* Pin mux for i2c 0 */
static struct pinmux_config i2c0_pin_mux[] = {
	{"i2c0_sda.i2c0_sda",    OMAP_MUX_MODE0 | AM33XX_SLEWCTRL_SLOW |
					AM33XX_PULL_UP | AM33XX_INPUT_EN},
	{"i2c0_scl.i2c0_scl",   OMAP_MUX_MODE0 | AM33XX_SLEWCTRL_SLOW |
					AM33XX_PULL_UP | AM33XX_INPUT_EN},
	{NULL, 0},
};

static struct at24_platform_data com335x_eeprom_info = {
	.byte_len       = (8*1024) / 8,
	.page_size      = 16,
};

static struct i2c_board_info __initdata com335x_i2c0_boardinfo[] = {
	{
		I2C_BOARD_INFO("pca9539", 0x74),
		.platform_data = &com335x_pca9555_info,
	},
	
	{
		I2C_BOARD_INFO("tps65217", TPS65217_I2C_ID),
		.platform_data  = &com335x_tps65217_info,
	},

	{
		I2C_BOARD_INFO("ds1339", 0x68),
		.irq = -EINVAL,
	},
	
	{
		I2C_BOARD_INFO("tlv320aic3x", 0x1b),
	},
	
	{
		/* Daughter Board EEPROM */
		I2C_BOARD_INFO("24c08", 0x50),
		.platform_data  = &com335x_eeprom_info,
	},

};

static struct pinmux_config pca9555_irq_mux[] = {
	{"xdma_event_intr0.gpio0_19", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT_PULLUP},
	{NULL, 0},
};

static void com335x_i2c0_init(void)
{
//	setup_pin_mux(i2c0_pin_mux);

//	setup_pin_mux(pca9555_irq_mux);

//	com335x_i2c0_boardinfo[0].irq = gpio_to_irq(GPIO_TO_PIN(0, 19));

	omap_register_i2c_bus(1, 400, com335x_i2c0_boardinfo, ARRAY_SIZE(com335x_i2c0_boardinfo));
}

/******************************************i2c1**************************/
static struct pinmux_config i2c1_pin_mux[] = {
	{"spi0_d1.i2c1_sda",    OMAP_MUX_MODE2 | AM33XX_SLEWCTRL_SLOW |
					AM33XX_PULL_ENBL | AM33XX_INPUT_EN},
	{"spi0_cs0.i2c1_scl",   OMAP_MUX_MODE2 | AM33XX_SLEWCTRL_SLOW |
					AM33XX_PULL_ENBL | AM33XX_INPUT_EN},
	{NULL, 0},
};

static struct pinmux_config ft5x16_irq_pin_mux[] = {
	{"xdma_event_intr1.gpio0_20", OMAP_MUX_MODE7 | AM33XX_PIN_INPUT_PULLUP},
	{NULL, 0},
};

static struct i2c_board_info __initdata com335x_i2c1_boardinfo[] = {
	[0] = {
		I2C_BOARD_INFO("ft5x16_ts", 0x38),
		.irq = -EINVAL,
	},
};

static void com335x_i2c1_init(void)
{
	setup_pin_mux(i2c1_pin_mux);
	setup_pin_mux(ft5x16_irq_pin_mux);

	if(gpio_request(GPIO_TO_PIN(0,20), "ft5x16_ts_irq") < 0)
	{
		printk("Can't request gpio for ft5x16_ts irq\n");
		return;
	}
	gpio_direction_input(GPIO_TO_PIN(0,20));
	com335x_i2c1_boardinfo[0].irq = gpio_to_irq(GPIO_TO_PIN(0,20));
	omap_register_i2c_bus(2, 400, com335x_i2c1_boardinfo, ARRAY_SIZE(com335x_i2c1_boardinfo));
}
/******************************can bus*********************************/
static struct pinmux_config d_can0_pin_mux[] = {
       {"uart1_ctsn.d_can0_tx", OMAP_MUX_MODE2 | AM33XX_PULL_ENBL},
       {"uart1_rtsn.d_can0_rx", OMAP_MUX_MODE2 | AM33XX_PIN_INPUT_PULLUP},
       {NULL, 0},
};
static inline void can0_init(void)
{
	setup_pin_mux(d_can0_pin_mux);
	am33xx_d_can_init(0);
}

static struct pinmux_config d_can1_pin_mux[] = {
       {"uart1_rxd.d_can1_tx", OMAP_MUX_MODE2 | AM33XX_PULL_ENBL},
       {"uart1_txd.d_can1_rx", OMAP_MUX_MODE2 | AM33XX_PIN_INPUT_PULLUP},
       {NULL, 0},
};
static inline void can1_init(void)
{
	setup_pin_mux(d_can1_pin_mux);
	am33xx_d_can_init(1);
}


static void com335x_can_init(void)
{
	can0_init();
	can1_init();
}
/*********************************** GPMC for 8-bit*****************************/
static struct pinmux_config gpmc_pin_mux[] = {
	/********* 8-bit data bus **************/
	{"gpmc_ad0.gpmc_ad0",			OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ad1.gpmc_ad1",			OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ad2.gpmc_ad2",			OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ad3.gpmc_ad3",			OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ad4.gpmc_ad4",			OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ad5.gpmc_ad5",			OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ad6.gpmc_ad6",			OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ad7.gpmc_ad7",			OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},

	/**************** 8-bit address bus ****************/
	{"gpmc_a0.gpmc_a0",			OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
	{"gpmc_a1.gpmc_a1",			OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
	{"gpmc_a2.gpmc_a2",			OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
	{"gpmc_a3.gpmc_a3",			OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
	{"gpmc_a4.gpmc_a4",			OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
	{"gpmc_a5.gpmc_a5",			OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
	{"gpmc_a6.gpmc_a6",			OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
	{"gpmc_a7.gpmc_a7",			OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
#if 0
	{"gpmc_a8.gpmc_a8",			OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
	{"gpmc_a9.gpmc_a9",			OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
	{"gpmc_a10.gpmc_a10",		OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},
	{"gpmc_a11.gpmc_a11",		OMAP_MUX_MODE0 | AM33XX_PIN_OUTPUT},


	{"gpmc_wait0.gpmc_wait0",		OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
#endif	
	{"gpmc_csn2.gpmc_csn2",			OMAP_MUX_MODE0 | AM33XX_PULL_DISA},
#if 0
	{"gpmc_advn_ale.gpmc_advn_ale",	OMAP_MUX_MODE0 | AM33XX_PULL_DISA},
#endif
	{"gpmc_oen_ren.gpmc_oen_ren",		OMAP_MUX_MODE0 | AM33XX_PULL_DISA},
	{"gpmc_wen.gpmc_wen",			OMAP_MUX_MODE0 | AM33XX_PULL_DISA},
#if 0
	{"gpmc_ben0_cle.gpmc_ben0_cle",	OMAP_MUX_MODE0 | AM33XX_PULL_DISA},
#endif
/*
   Add by Michael ,date:2015/07/07.
   add gpmc_clk and gpmc_ben1 configure of gpmc_pin_Mux for 10111 sysboot 
*/
#if 1
	{"gpmc_clk.gpmc_clk",      OMAP_MUX_MODE0 | AM33XX_PIN_INPUT_PULLUP},
	{"gpmc_ben1.gpmc_be1n",    OMAP_MUX_MODE0 | AM33XX_PULL_DISA},
#endif
/********************************************************************/
	{NULL, 0},
};

static struct pinmux_config ext_uart_irq_pin_mux[] = {
	{"gpmc_a8.gpio1_24",			OMAP_MUX_MODE7 | AM33XX_PIN_INPUT_PULLDOWN},
	{"gpmc_a9.gpio1_25",			OMAP_MUX_MODE7 | AM33XX_PIN_INPUT_PULLDOWN},
	{"gpmc_a10.gpio1_26",			OMAP_MUX_MODE7 | AM33XX_PIN_INPUT_PULLDOWN},
	{"gpmc_a11.gpio1_27",			OMAP_MUX_MODE7 | AM33XX_PIN_INPUT_PULLDOWN},
	{NULL, 0},
};

/***********************************
All following timing according to Exar ST16554 datasheet
************************************/
static struct gpmc_timings ext_uart_timings = {
	/* Minimum clock period for synchronous mode (in picoseconds) */
	.sync_clk = 0,

	/* CS signal timings corresponding to GPMC_CONFIG2 */
	.cs_on = 10,				/* T6s */
	.cs_rd_off = 85,			/* oe_off + T7h */
	.cs_wr_off = 60,			/* Tcs for write: we_off + T13h */

	/* ADV signal timings corresponding to GPMC_CONFIG3 */
//	.adv_on = 0,			/* Address setup Tas*/
//	.adv_rd_off = 110,		/* Read deassertion time  Trd + Tdd*/
//	.adv_wr_off = 90,		/* Write deassertion time  Twr + Tdh*/

	/* WE signals timings corresponding to GPMC_CONFIG4 */
	.we_on = 20,		/* WE assertion time: cs_on + T13d */
	.we_off = 60,		/* WE deassertion time: we_on + T13w */

	/* OE signals timings corresponding to GPMC_CONFIG4 */
	.oe_on = 25,		/* OE assertion time: cs_on + T7d */
	.oe_off = 75,		/* OE deassertion time: oe_on + T7w */


	/* Access time and cycle time timings corresponding to GPMC_CONFIG5 */
//	.page_burst_access = 2 * 10,	/* Multiple access word delay */
	.access = 65,		/* Start-cycle to first data valid delay: at leaset oe_on + T12d */
	.rd_cycle = 115,		/* Total read cycle time: cs_rd_off + (T9d - T7h) - oe_on */
	.wr_cycle = 90,		/* Total write cycle time: cs_wr_off + (T15d - T13h) - we_on */

	/* The following are only on OMAP3430 */
	.wr_access = 40,		/* WRACCESSTIME: max we_off - T16s  */
	.wr_data_mux_bus = 0,	/* WRDATAONADMUXBUS */

};

static struct plat_serial8250_port com335x_ext_uart_data[] = {
	{	/******** Port A ********/
		.mapbase	= -1,
		.irq		= -1,
		.uartclk	= 1843200,
		.iotype	= UPIO_MEM,
		.regshift	= 0,
		.flags	= UPF_BOOT_AUTOCONF | UPF_IOREMAP,
		.irqflags	= IRQF_TRIGGER_HIGH,
	},
	
	{	/******** Port B ********/
		.mapbase	= -1,
		.irq		= -1,
		.uartclk	= 1843200,
		.iotype	= UPIO_MEM,
		.regshift	= 0,
		.flags	= UPF_BOOT_AUTOCONF | UPF_IOREMAP,
		.irqflags	= IRQF_TRIGGER_HIGH,
	},

	{	/******** Port C ********/
		.mapbase	= -1,
		.irq		= -1,
		.uartclk	= 1843200,
		.iotype	= UPIO_MEM,
		.regshift	= 0,
		.flags	= UPF_BOOT_AUTOCONF | UPF_IOREMAP,
		.irqflags	= IRQF_TRIGGER_HIGH,
	},

	{	/******** Port D ********/
		.mapbase	= -1,
		.irq		= -1,
		.uartclk	= 1843200,
		.iotype	= UPIO_MEM,
		.regshift	= 0,
		.flags	= UPF_BOOT_AUTOCONF | UPF_IOREMAP,
		.irqflags	= IRQF_TRIGGER_HIGH,
	},
};

static struct platform_device com335x_ext_uart_device = {
	.name			= "serial8250",
	.id			= 0,
	.dev			= {
		.platform_data	= com335x_ext_uart_data,
	},
};

static int gpmc_cssp_init(void)
{
	int cs = 2; /* Chip Select on GPMC bus */
	int val;
	unsigned long serial_gpmc_mem_base_phys;

	if (gpmc_cs_request(cs, SZ_16M, &serial_gpmc_mem_base_phys) < 0) {
			printk(KERN_ERR "[ext_uart platform init]: gpmc_cs_request failed\n");
			return -1;
	}

	printk("[ext_uart platform init]: gpmc mem base is 0x%08x\n", serial_gpmc_mem_base_phys);
	/******** Port A ********/
	com335x_ext_uart_data[0].mapbase	= serial_gpmc_mem_base_phys;
	com335x_ext_uart_data[0].irq		= gpio_to_irq(GPIO_TO_PIN(1, 24));
	/******** Port B ********/
	com335x_ext_uart_data[1].mapbase	= serial_gpmc_mem_base_phys + 0x08;
	com335x_ext_uart_data[1].irq		= gpio_to_irq(GPIO_TO_PIN(1, 25));
	/******** Port C ********/
	com335x_ext_uart_data[2].mapbase	= serial_gpmc_mem_base_phys + 0x10;
	com335x_ext_uart_data[2].irq		= gpio_to_irq(GPIO_TO_PIN(1, 26));
	/******** Port D ********/
	com335x_ext_uart_data[3].mapbase	= serial_gpmc_mem_base_phys + 0x18;
	com335x_ext_uart_data[3].irq		= gpio_to_irq(GPIO_TO_PIN(1, 27));
	
	if (gpmc_cs_configure(cs, GPMC_CONFIG_DEV_TYPE, GPMC_DEVICETYPE_NOR) < 0) {
			printk(KERN_ERR "[ext_uart platform init]: gpmc_cs_configure failed\n");
			return -1;
	}	

//	val = GPMC_CONFIG1_READMULTIPLE_SUPP;
	val = GPMC_CONFIG1_READTYPE_SYNC;
	val |= GPMC_CONFIG1_WRITETYPE_SYNC;
//	val |= GPMC_CONFIG1_CLKACTIVATIONTIME(2);
//	val |= GPMC_CONFIG1_PAGE_LEN(2);
//	val |= GPMC_CONFIG1_DEVICESIZE(0);	/* 8-bit */
//	val |= GPMC_CONFIG1_DEVICETYPE_NOR;
//	val |= GPMC_CONFIG1_MUXADDDATA;
//	val |= GPMC_CONFIG1_WAIT_READ_MON;
	gpmc_cs_write_reg(cs, GPMC_CS_CONFIG1, val);

	if (gpmc_cs_set_timings(cs, &ext_uart_timings) < 0) {
		printk(KERN_ERR "Failed gpmc_cs_set_timings for ST16C554 device\n");
		goto free;
	}

#if 0
	val = gpmc_cs_read_reg(cs, GPMC_CS_CONFIG6);
	val &= 0xe0000000;
	val |= ((1<<7)|(2<<8)|(3<<24)| 30);
	gpmc_cs_write_reg(cs, GPMC_CS_CONFIG6, val);
#endif
	printk(KERN_INFO "gpmc_cssp_init for ST16C554 device succeeded\n");

	return 0;

free:
	gpmc_cs_free(cs);

	printk(KERN_ERR "Could not initialize ST16C554 device\n");

	return -1;
}

static void com335x_gpmc_init(void)
{
	int status;
	
	struct gpmc_devices_info gpmc_device[2] = {
			{ NULL, GPMC_DEVICE_NOR },
		};
	
	setup_pin_mux(gpmc_pin_mux);
	setup_pin_mux(ext_uart_irq_pin_mux);

//	omap_init_gpmc(gpmc_device, sizeof(gpmc_device));
	gpmc_cssp_init();
	platform_device_register(&com335x_ext_uart_device);
}
/*******************************************************************************************/
void __iomem *am33xx_emif_base;

void __iomem * __init am33xx_get_mem_ctlr(void)
{

	am33xx_emif_base = ioremap(AM33XX_EMIF0_BASE, SZ_32K);

	if (!am33xx_emif_base)
		pr_warning("%s: Unable to map DDR2 controller",	__func__);

	return am33xx_emif_base;
}

void __iomem *am33xx_get_ram_base(void)
{
	return am33xx_emif_base;
}

void __iomem *am33xx_gpio0_base;

void __iomem *am33xx_get_gpio0_base(void)
{
	am33xx_gpio0_base = ioremap(AM33XX_GPIO0_BASE, SZ_4K);

	return am33xx_gpio0_base;
}
/*******************************************************************************/
static struct resource am33xx_cpuidle_resources[] = {
	{
		.start		= AM33XX_EMIF0_BASE,
		.end		= AM33XX_EMIF0_BASE + SZ_32K - 1,
		.flags		= IORESOURCE_MEM,
	},
};

static struct am33xx_cpuidle_config am33xx_cpuidle_pdata = {
};

static struct platform_device am33xx_cpuidle_device = {
	.name			= "cpuidle-am33xx",
	.num_resources		= ARRAY_SIZE(am33xx_cpuidle_resources),
	.resource		= am33xx_cpuidle_resources,
	.dev = {
		.platform_data	= &am33xx_cpuidle_pdata,
	},
};

static void __init am33xx_cpuidle_init(void)
{
	int ret;

	am33xx_cpuidle_pdata.emif_base = am33xx_get_mem_ctlr();

	ret = platform_device_register(&am33xx_cpuidle_device);

	if (ret)
		pr_warning("AM33XX cpuidle registration failed\n");

}
/***************************************************************************/
/*
* am335x_evm_get_id - returns Board Type (EVM/BB/EVM-SK ...)
*
* Note:
*	returns -EINVAL if Board detection hasn't happened yet.
*/
int am335x_evm_get_id(void)
{
	return -EINVAL;
}
EXPORT_SYMBOL(am335x_evm_get_id);
/*******************************************************************************/
static void __init com335x_init(void)
{
	am33xx_cpuidle_init();
	am33xx_mux_init(board_mux);

	usb0_init();
	usb1_init();

	com335x_serial_init();
	com335x_i2c0_init();
	com335x_nand_init();
	com335x_mmc_init();
	com335x_tsc_init();
	com335x_add_leds();
	com335x_add_buttons();
	com335x_eth_init();
//	com335x_can_init();
	com335x_i2c1_init();
	com335x_gpmc_init();
	com335x_audio_init();
	com335x_lcdc_init();
	com335x_add_adm706();
	com335x_i2c2_init();
	
	omap_sdrc_init(NULL, NULL);
	usb_musb_init(&musb_board_data);
	omap_board_config = com335x_config;
	omap_board_config_size = ARRAY_SIZE(com335x_config);
	/* Create an alias for icss clock */
	if (clk_add_alias("pruss", NULL, "pruss_uart_gclk", NULL))
		pr_warn("failed to create an alias: icss_uart_gclk --> pruss\n");
	/* Create an alias for gfx/sgx clock */
	if (clk_add_alias("sgx_ck", NULL, "gfx_fclk", NULL))
		pr_warn("failed to create an alias: gfx_fclk --> sgx_ck\n");
}

static void __init com335x_map_io(void)
{
	omap2_set_globals_am33xx();
	omapam33xx_map_common_io();
}

MACHINE_START(COM335X, "com335x")
	/* Maintainer: eac-systems */
	/* ronggangxt@163.com */
	.atag_offset	= 0x100,
	.map_io		= com335x_map_io,
	.init_early	= am33xx_init_early,
	.init_irq	= ti81xx_init_irq,
	.handle_irq     = omap3_intc_handle_irq,
	.timer		= &omap3_am33xx_timer,
	.init_machine	= com335x_init,
MACHINE_END
