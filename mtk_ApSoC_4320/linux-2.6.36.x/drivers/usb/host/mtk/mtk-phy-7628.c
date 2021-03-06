#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>

#include "mtk-phy.h"

#ifdef CONFIG_PROJECT_7628
#include "mtk-phy-7628.h"

bool phy_init_flag = false;
bool slewrate_cal_flag = false;

PHY_INT32 phy_init(struct u3phy_info *info)
{
	PHY_INT32 temp;

	if (phy_init_flag == false) {
		temp = U3PhyReadReg32(((PHY_UINT32)&info->u2phy_regs->u2phyac2));
		temp = U3PhyReadReg32(((PHY_UINT32)&info->u2phy_regs->u2phyacr0));
		temp = U3PhyReadReg32(((PHY_UINT32)&info->u2phy_regs->u2phydcr0));

		U3PhyWriteReg32(((PHY_UINT32)&info->u2phy_regs->u2phydcr0), 0xffff02);
		temp = U3PhyReadReg32(((PHY_UINT32)&info->u2phy_regs->u2phydcr0));
		U3PhyWriteReg32(((PHY_UINT32)&info->u2phy_regs->u2phydcr0), 0x555502);
		temp = U3PhyReadReg32(((PHY_UINT32)&info->u2phy_regs->u2phydcr0));
		U3PhyWriteReg32(((PHY_UINT32)&info->u2phy_regs->u2phydcr0), 0xaaaa02);
		temp = U3PhyReadReg32(((PHY_UINT32)&info->u2phy_regs->u2phydcr0));
		U3PhyWriteReg32(((PHY_UINT32)&info->u2phy_regs->u2phydcr0), 0x402);
		temp = U3PhyReadReg32(((PHY_UINT32)&info->u2phy_regs->u2phydcr0));

		U3PhyWriteReg32(((PHY_UINT32)&info->u2phy_regs->u2phyac0), 0x48086a);
		U3PhyWriteReg32(((PHY_UINT32)&info->u2phy_regs->u2phyac1), 0x4400001c);
		U3PhyWriteReg32(((PHY_UINT32)&info->u2phy_regs->u2phyacr3), 0xc0200000);
		U3PhyWriteReg32(((PHY_UINT32)&info->u2phy_regs->u2phydtm0), 0x2000000);

		phy_init_flag = true;
	}

	return PHY_TRUE;
}

PHY_INT32 phy_down(struct u3phy_info *info)
{
	// force_uart_en = 0
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs->u2phydtm0), 26, (0x1 << 26), 0);
	// RG_UART_EN = 0
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs->u2phydtm1), 16, (0x1 << 16), 0);
	// rg_usb20_gpio_ctl = 0
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs->u2phyacr4), 9, (0x1 << 9), 0);
	// usb20_gpio_mode = 0
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs->u2phyacr4), 8, (0x1 << 8), 0);

	// RG_SUSPENDM = 1
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs->u2phydtm0), 3, (0x1 << 3), 1);
	// force_suspendm = 1
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs->u2phydtm0), 18, (0x1 << 18), 1);
	
	mdelay(10);

	// RG_DPPULLDOWN = 1
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs->u2phydtm0), 6, (0x1 << 6), 1);
	// RG_DMPULLDOWN = 1
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs->u2phydtm0), 7, (0x1 << 7), 1);
	// RG_XCVRSEL[1:0] = 1
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs->u2phydtm0), 4, (0x3 << 4), 1);
	// RG_TERMSEL = 1
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs->u2phydtm0), 2, (0x1 << 2), 1);
	// RG_DATAIN[3:0] = 0
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs->u2phydtm0), 10, (0xf << 10), 0);
	// force_dp_pulldown = 1
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs->u2phydtm0), 20, (0x1 << 20), 1);
	// force_dm_pulldown = 1
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs->u2phydtm0), 21, (0x1 << 21), 1);
	// force_datain = 1
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs->u2phydtm0), 23, (0x1 << 23), 1);

	// RG_USB20_PHY_REV[7] = 0
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs->u2phyacr3), 0, (0xf << 0), 0);
	// RG_USB20_OTG_VBUSCMP_EN = 0
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs->u2phyacr2), 27, (0x1 << 27), 0);
	
	mdelay(2);

	// RG_SUSPENDM = 0
	U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs->u2phydtm0), 3, (0x1 << 3), 0);

	return PHY_TRUE;
}


//not used on SoC
PHY_INT32 phy_change_pipe_phase(struct u3phy_info *info, PHY_INT32 phy_drv, PHY_INT32 pipe_phase)
{
	return PHY_TRUE;
}

PHY_INT32 eyescan_init(struct u3phy_info *info)
{
	return PHY_TRUE;
}

PHY_INT32 phy_eyescan(struct u3phy_info *info, PHY_INT32 x_t1, PHY_INT32 y_t1, PHY_INT32 x_br, PHY_INT32 y_br, PHY_INT32 delta_x, PHY_INT32 delta_y
		, PHY_INT32 eye_cnt, PHY_INT32 num_cnt, PHY_INT32 PI_cal_en, PHY_INT32 num_ignore_cnt)
{
	return PHY_TRUE;
}

PHY_INT32 u2_slew_rate_calibration(struct u3phy_info *info)
{
	PHY_INT32 i=0;
	//PHY_INT32 j=0;
	//PHY_INT8 u1SrCalVal = 0;
	//PHY_INT8 u1Reg_addr_HSTX_SRCAL_EN;
	PHY_INT32 fgRet = 0;	
	PHY_INT32 u4FmOut = 0;	
	PHY_INT32 u4Tmp = 0;
	//PHY_INT32 temp;

	if (slewrate_cal_flag == false) {
		slewrate_cal_flag = true;

		// => RG_USB20_HSTX_SRCAL_EN = 1
		// enable HS TX SR calibration
		U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs->u2phyacr0)
			, RG_USB20_HSTX_SRCAL_EN_OFST, RG_USB20_HSTX_SRCAL_EN, 0x1);
		DRV_MSLEEP(1);

		// => RG_FRCK_EN = 1    
		// Enable free run clock
		U3PhyWriteField32(((PHY_UINT32)&info->sifslv_fm_regs->fmmonr1)
			, RG_FRCK_EN_OFST, RG_FRCK_EN, 1);

		// MT6290 HS signal quality patch
		// => RG_CYCLECNT = 400
		// Setting cyclecnt =400
		U3PhyWriteField32(((PHY_UINT32)&info->sifslv_fm_regs->fmcr0)
			, RG_CYCLECNT_OFST, RG_CYCLECNT, 0x400);

		// => RG_FREQDET_EN = 1
		// Enable frequency meter
		U3PhyWriteField32(((PHY_UINT32)&info->sifslv_fm_regs->fmcr0)
			, RG_FREQDET_EN_OFST, RG_FREQDET_EN, 0x1);

		// wait for FM detection done, set 10ms timeout
		for(i=0; i<10; i++){
			// => u4FmOut = USB_FM_OUT
			// read FM_OUT
			u4FmOut = U3PhyReadReg32(((PHY_UINT32)&info->sifslv_fm_regs->fmmonr0));
			printk("FM_OUT value: u4FmOut = %d(0x%08X)\n", u4FmOut, u4FmOut);

			// check if FM detection done 
			if (u4FmOut != 0)
			{
				fgRet = 0;
				printk("FM detection done! loop = %d\n", i);
			
				break;
			}

			fgRet = 1;
			DRV_MSLEEP(1);
		}
		// => RG_FREQDET_EN = 0
		// disable frequency meter
		U3PhyWriteField32(((PHY_UINT32)&info->sifslv_fm_regs->fmcr0)
			, RG_FREQDET_EN_OFST, RG_FREQDET_EN, 0);

		// => RG_FRCK_EN = 0
		// disable free run clock
		U3PhyWriteField32(((PHY_UINT32)&info->sifslv_fm_regs->fmmonr1)
			, RG_FRCK_EN_OFST, RG_FRCK_EN, 0);

		// => RG_USB20_HSTX_SRCAL_EN = 0
		// disable HS TX SR calibration
		U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs->u2phyacr0)
			, RG_USB20_HSTX_SRCAL_EN_OFST, RG_USB20_HSTX_SRCAL_EN, 0);
		DRV_MSLEEP(1);

		if(u4FmOut == 0){
			U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs->u2phyacr0)
				, RG_USB20_HSTX_SRCTRL_OFST, RG_USB20_HSTX_SRCTRL, 0x4);
		
			fgRet = 1;
		}
		else{
			// set reg = (1024/FM_OUT) * 25 * 0.028 (round to the nearest digits)
			u4Tmp = (((1024 * 25 * U2_SR_COEF_7628) / u4FmOut) + 500) / 1000;
			printk("SR calibration value u1SrCalVal = %d\n", (PHY_UINT8)u4Tmp);
			U3PhyWriteField32(((PHY_UINT32)&info->u2phy_regs->u2phyacr0)
				, RG_USB20_HSTX_SRCTRL_OFST, RG_USB20_HSTX_SRCTRL, u4Tmp);
		}
	}

	return fgRet;
}

PHY_INT32 mt7628_phy_init(void)
{
	u3phy_init();
	u3phy_ops->init(u3phy);
	if (u3phy_ops->u2_slew_rate_calibration)
                u3phy_ops->u2_slew_rate_calibration(u3phy);
        else
                printk(KERN_ERR "WARN: PHY doesn't implement u2 slew rate calibration function\n");

	return PHY_TRUE;
}
EXPORT_SYMBOL_GPL(mt7628_phy_init);

PHY_INT32 mt7628_phy_down(void)
{
	phy_down(u3phy);

	return PHY_TRUE;
}
EXPORT_SYMBOL_GPL(mt7628_phy_down);
#endif
