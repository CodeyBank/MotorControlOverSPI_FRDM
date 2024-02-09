/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/
/*
 * How to setup clock using clock driver functions:
 *
 * 1. Setup clock sources.
 *
 * 2. Set up wait states of the flash.
 *
 * 3. Set up all dividers.
 *
 * 4. Set up all selectors to provide selected clocks.
 *
 */

/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Clocks v13.0
processor: MCXN947
package_id: MCXN947VDF
mcu_data: ksdk2_0
processor_version: 15.0.0
board: FRDM-MCXN947
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

#include "fsl_clock.h"
#include "clock_config.h"
#include "fsl_spc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* System clock frequency. */
extern uint32_t SystemCoreClock;

/*******************************************************************************
 ************************ BOARD_InitBootClocks function ************************
 ******************************************************************************/
void BOARD_InitBootClocks(void)
{
    BOARD_BootClockPLL150M();
}

/*******************************************************************************
 ******************** Configuration BOARD_BootClockFRO12M **********************
 ******************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!Configuration
name: BOARD_BootClockFRO12M
outputs:
- {id: CLK_144M_clock.outFreq, value: 144 MHz}
- {id: CLK_48M_clock.outFreq, value: 48 MHz}
- {id: FRO_12M_clock.outFreq, value: 12 MHz}
- {id: MAIN_clock.outFreq, value: 12 MHz}
- {id: Slow_clock.outFreq, value: 3 MHz}
- {id: System_clock.outFreq, value: 12 MHz}
- {id: gdet_clock.outFreq, value: 48 MHz}
- {id: trng_clock.outFreq, value: 48 MHz}
settings:
- {id: RunPowerMode, value: OD}
- {id: SCGMode, value: SIRC}
- {id: SCG.SCSSEL.sel, value: SCG.SIRC}
- {id: SCG_FIRCCSR_FIRCEN_CFG, value: Disabled}
- {id: SYSCON.FREQMEREFCLKSEL.sel, value: SYSCON.evtg_out0a}
- {id: SYSCON.FREQMETARGETCLKSEL.sel, value: SYSCON.evtg_out0a}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/*******************************************************************************
 * Variables for BOARD_BootClockFRO12M configuration
 ******************************************************************************/
/*******************************************************************************
 * Code for BOARD_BootClockFRO12M configuration
 ******************************************************************************/
void BOARD_BootClockFRO12M(void)
{
    CLOCK_EnableClock(kCLOCK_Scg);                     /*!< Enable SCG clock */

    /* FRO OSC setup - begin, attach FRO12M to MainClock for safety switching */
    CLOCK_AttachClk(kFRO12M_to_MAIN_CLK);              /*!< Switch to FRO 12M first to ensure we can change the clock setting */

    /* Set the DCDC VDD regulator to 1.2 V voltage level */
    spc_active_mode_dcdc_option_t dcdcOpt = {
      .DCDCVoltage       = kSPC_DCDC_OverdriveVoltage,
      .DCDCDriveStrength = kSPC_DCDC_NormalDriveStrength,
    };
    SPC_SetActiveModeDCDCRegulatorConfig(SPC0, &dcdcOpt);
    /* Set the LDO_CORE VDD regulator to 1.2 V voltage level */
    spc_active_mode_core_ldo_option_t ldoOpt = {
      .CoreLDOVoltage       = kSPC_CoreLDO_OverDriveVoltage,
      .CoreLDODriveStrength = kSPC_CoreLDO_NormalDriveStrength,
    };
    SPC_SetActiveModeCoreLDORegulatorConfig(SPC0, &ldoOpt);
    /* Configure Flash wait-states to support 1.2V voltage level and 12000000Hz frequency */;
    FMU0->FCTRL = (FMU0->FCTRL & ~((uint32_t)FMU_FCTRL_RWSC_MASK)) | (FMU_FCTRL_RWSC(0x0U));
    /* Specifies the 1.2V operating voltage for the SRAM's read/write timing margin */
    spc_sram_voltage_config_t sramCfg = {
      .operateVoltage       = kSPC_sramOperateAt1P2V,
      .requestVoltageUpdate = true,
    };
    SPC_SetSRAMOperateVoltage(SPC0, &sramCfg);

    /*!< Set up clock selectors  */

    /*!< Set up dividers */
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U);           /*!< Set AHBCLKDIV divider to value 1 */

    /* Set SystemCoreClock variable */
    SystemCoreClock = BOARD_BOOTCLOCKFRO12M_CORE_CLOCK;
}

/*******************************************************************************
 ******************* Configuration BOARD_BootClockFROHF48M *********************
 ******************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!Configuration
name: BOARD_BootClockFROHF48M
outputs:
- {id: CLK_144M_clock.outFreq, value: 144 MHz}
- {id: CLK_48M_clock.outFreq, value: 48 MHz}
- {id: FRO_12M_clock.outFreq, value: 12 MHz}
- {id: FRO_HF_clock.outFreq, value: 48 MHz}
- {id: MAIN_clock.outFreq, value: 48 MHz}
- {id: Slow_clock.outFreq, value: 12 MHz}
- {id: System_clock.outFreq, value: 48 MHz}
- {id: gdet_clock.outFreq, value: 48 MHz}
- {id: trng_clock.outFreq, value: 48 MHz}
settings:
- {id: RunPowerMode, value: OD}
- {id: SYSCON.FLEXSPICLKSEL.sel, value: NO_CLOCK}
- {id: SYSCON.FREQMEREFCLKSEL.sel, value: SYSCON.evtg_out0a}
- {id: SYSCON.FREQMETARGETCLKSEL.sel, value: SYSCON.evtg_out0a}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/*******************************************************************************
 * Variables for BOARD_BootClockFROHF48M configuration
 ******************************************************************************/
/*******************************************************************************
 * Code for BOARD_BootClockFROHF48M configuration
 ******************************************************************************/
void BOARD_BootClockFROHF48M(void)
{
    CLOCK_EnableClock(kCLOCK_Scg);                     /*!< Enable SCG clock */

    /* FRO OSC setup - begin, attach FRO12M to MainClock for safety switching */
    CLOCK_AttachClk(kFRO12M_to_MAIN_CLK);              /*!< Switch to FRO 12M first to ensure we can change the clock setting */

    /* Set the DCDC VDD regulator to 1.2 V voltage level */
    spc_active_mode_dcdc_option_t dcdcOpt = {
      .DCDCVoltage       = kSPC_DCDC_OverdriveVoltage,
      .DCDCDriveStrength = kSPC_DCDC_NormalDriveStrength,
    };
    SPC_SetActiveModeDCDCRegulatorConfig(SPC0, &dcdcOpt);
    /* Set the LDO_CORE VDD regulator to 1.2 V voltage level */
    spc_active_mode_core_ldo_option_t ldoOpt = {
      .CoreLDOVoltage       = kSPC_CoreLDO_OverDriveVoltage,
      .CoreLDODriveStrength = kSPC_CoreLDO_NormalDriveStrength,
    };
    SPC_SetActiveModeCoreLDORegulatorConfig(SPC0, &ldoOpt);
    /* Configure Flash wait-states to support 1.2V voltage level and 48000000Hz frequency */;
    FMU0->FCTRL = (FMU0->FCTRL & ~((uint32_t)FMU_FCTRL_RWSC_MASK)) | (FMU_FCTRL_RWSC(0x1U));
    /* Specifies the 1.2V operating voltage for the SRAM's read/write timing margin */
    spc_sram_voltage_config_t sramCfg = {
      .operateVoltage       = kSPC_sramOperateAt1P2V,
      .requestVoltageUpdate = true,
    };
    SPC_SetSRAMOperateVoltage(SPC0, &sramCfg);

    CLOCK_SetupFROHFClocking(48000000U);               /*!< Enable FRO HF(48MHz) output */
    /*!< Set up clock selectors  */
    CLOCK_AttachClk(kFRO_HF_to_MAIN_CLK);

    /*!< Set up dividers */
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U);           /*!< Set AHBCLKDIV divider to value 1 */

    /* Set SystemCoreClock variable */
    SystemCoreClock = BOARD_BOOTCLOCKFROHF48M_CORE_CLOCK;
}

/*******************************************************************************
 ******************* Configuration BOARD_BootClockFROHF144M ********************
 ******************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!Configuration
name: BOARD_BootClockFROHF144M
outputs:
- {id: CLK_144M_clock.outFreq, value: 144 MHz}
- {id: CLK_48M_clock.outFreq, value: 48 MHz}
- {id: FRO_12M_clock.outFreq, value: 12 MHz}
- {id: FRO_HF_clock.outFreq, value: 144 MHz}
- {id: MAIN_clock.outFreq, value: 144 MHz}
- {id: Slow_clock.outFreq, value: 36 MHz}
- {id: System_clock.outFreq, value: 144 MHz}
- {id: gdet_clock.outFreq, value: 48 MHz}
- {id: trng_clock.outFreq, value: 48 MHz}
settings:
- {id: RunPowerMode, value: OD}
- {id: SYSCON.AHBCLKDIV.scale, value: '1', locked: true}
- {id: SYSCON.FLEXSPICLKSEL.sel, value: NO_CLOCK}
- {id: SYSCON.FREQMEREFCLKSEL.sel, value: SYSCON.evtg_out0a}
- {id: SYSCON.FREQMETARGETCLKSEL.sel, value: SYSCON.evtg_out0a}
sources:
- {id: SCG.FIRC.outFreq, value: 144 MHz}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/*******************************************************************************
 * Variables for BOARD_BootClockFROHF144M configuration
 ******************************************************************************/
/*******************************************************************************
 * Code for BOARD_BootClockFROHF144M configuration
 ******************************************************************************/
void BOARD_BootClockFROHF144M(void)
{
    CLOCK_EnableClock(kCLOCK_Scg);                     /*!< Enable SCG clock */

    /* FRO OSC setup - begin, attach FRO12M to MainClock for safety switching */
    CLOCK_AttachClk(kFRO12M_to_MAIN_CLK);              /*!< Switch to FRO 12M first to ensure we can change the clock setting */

    /* Set the DCDC VDD regulator to 1.2 V voltage level */
    spc_active_mode_dcdc_option_t dcdcOpt = {
      .DCDCVoltage       = kSPC_DCDC_OverdriveVoltage,
      .DCDCDriveStrength = kSPC_DCDC_NormalDriveStrength,
    };
    SPC_SetActiveModeDCDCRegulatorConfig(SPC0, &dcdcOpt);
    /* Set the LDO_CORE VDD regulator to 1.2 V voltage level */
    spc_active_mode_core_ldo_option_t ldoOpt = {
      .CoreLDOVoltage       = kSPC_CoreLDO_OverDriveVoltage,
      .CoreLDODriveStrength = kSPC_CoreLDO_NormalDriveStrength,
    };
    SPC_SetActiveModeCoreLDORegulatorConfig(SPC0, &ldoOpt);
    /* Configure Flash wait-states to support 1.2V voltage level and 144000000Hz frequency */;
    FMU0->FCTRL = (FMU0->FCTRL & ~((uint32_t)FMU_FCTRL_RWSC_MASK)) | (FMU_FCTRL_RWSC(0x3U));
    /* Specifies the 1.2V operating voltage for the SRAM's read/write timing margin */
    spc_sram_voltage_config_t sramCfg = {
      .operateVoltage       = kSPC_sramOperateAt1P2V,
      .requestVoltageUpdate = true,
    };
    SPC_SetSRAMOperateVoltage(SPC0, &sramCfg);

    CLOCK_SetupFROHFClocking(144000000U);               /*!< Enable FRO HF(144MHz) output */
    /*!< Set up clock selectors  */
    CLOCK_AttachClk(kFRO_HF_to_MAIN_CLK);

    /*!< Set up dividers */
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U);           /*!< Set AHBCLKDIV divider to value 1 */

    /* Set SystemCoreClock variable */
    SystemCoreClock = BOARD_BOOTCLOCKFROHF144M_CORE_CLOCK;
}

/*******************************************************************************
 ******************** Configuration BOARD_BootClockPLL150M *********************
 ******************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!Configuration
name: BOARD_BootClockPLL150M
called_from_default_init: true
outputs:
- {id: CLK16K0_toVBAT_clock.outFreq, value: 16.384 kHz}
- {id: CLK_144M_clock.outFreq, value: 144 MHz}
- {id: CLK_1M_clock.outFreq, value: 1 MHz}
- {id: CLK_48M_clock.outFreq, value: 48 MHz}
- {id: CTIMER4_clock.outFreq, value: 150 MHz}
- {id: FLEXCOMM0_clock.outFreq, value: 24 MHz}
- {id: FLEXCOMM3_clock.outFreq, value: 48 MHz}
- {id: FRO_12M_clock.outFreq, value: 12 MHz}
- {id: FRO_HF_DIV_clock.outFreq, value: 48 MHz}
- {id: FRO_HF_clock.outFreq, value: 48 MHz}
- {id: LP_OSC_clock.outFreq, value: 16.384 kHz}
- {id: MAIN_clock.outFreq, value: 150 MHz}
- {id: PLL0_CLK_clock.outFreq, value: 150 MHz}
- {id: Slow_clock.outFreq, value: 37.5 MHz}
- {id: System_clock.outFreq, value: 150 MHz}
- {id: gdet_clock.outFreq, value: 48 MHz}
- {id: trng_clock.outFreq, value: 48 MHz}
settings:
- {id: PLL0_Mode, value: Normal}
- {id: RunPowerMode, value: OD}
- {id: SCGMode, value: PLL0}
- {id: CLK16K0_VBAT_EN, value: Enabled}
- {id: CTIMER4CLKDIV_HALT, value: Enable}
- {id: FLEXCOMM0CLKDIV_HALT, value: Enable}
- {id: FLEXCOMM3CLKDIV_HALT, value: Enable}
- {id: FROHFDIV_HALT, value: Enable}
- {id: SCG.PLL0M_MULT.scale, value: '50', locked: true}
- {id: SCG.PLL0SRCSEL.sel, value: SCG.FIRC_48M}
- {id: SCG.PLL0_NDIV.scale, value: '8', locked: true}
- {id: SCG.SCSSEL.sel, value: SCG.PLL0_CLK}
- {id: SYSCON.CTIMER4CLKDIV.scale, value: '1', locked: true}
- {id: SYSCON.CTIMERCLKSEL4.sel, value: SCG.PLL0_CLK}
- {id: SYSCON.FCCLKSEL0.sel, value: SYSCON.FRO_HF_DIV_CLK}
- {id: SYSCON.FCCLKSEL3.sel, value: SYSCON.FRO_HF_DIV_CLK}
- {id: SYSCON.FLEXCOMM0CLKDIV.scale, value: '2', locked: true}
- {id: SYSCON.FLEXSPICLKSEL.sel, value: NO_CLOCK}
- {id: SYSCON.FREQMEREFCLKSEL.sel, value: SYSCON.evtg_out0a}
- {id: SYSCON.FREQMETARGETCLKSEL.sel, value: SYSCON.evtg_out0a}
- {id: SYSCON_CLOCK_CTRL_FRO1MHZ_CLK_ENA_CFG, value: Enabled}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/*******************************************************************************
 * Variables for BOARD_BootClockPLL150M configuration
 ******************************************************************************/
/*******************************************************************************
 * Code for BOARD_BootClockPLL150M configuration
 ******************************************************************************/
void BOARD_BootClockPLL150M(void)
{
    CLOCK_EnableClock(kCLOCK_Scg);                     /*!< Enable SCG clock */

    /* FRO OSC setup - begin, attach FRO12M to MainClock for safety switching */
    CLOCK_AttachClk(kFRO12M_to_MAIN_CLK);              /*!< Switch to FRO 12M first to ensure we can change the clock setting */

    /* Set the DCDC VDD regulator to 1.2 V voltage level */
    spc_active_mode_dcdc_option_t dcdcOpt = {
      .DCDCVoltage       = kSPC_DCDC_OverdriveVoltage,
      .DCDCDriveStrength = kSPC_DCDC_NormalDriveStrength,
    };
    SPC_SetActiveModeDCDCRegulatorConfig(SPC0, &dcdcOpt);
    /* Set the LDO_CORE VDD regulator to 1.2 V voltage level */
    spc_active_mode_core_ldo_option_t ldoOpt = {
      .CoreLDOVoltage       = kSPC_CoreLDO_OverDriveVoltage,
      .CoreLDODriveStrength = kSPC_CoreLDO_NormalDriveStrength,
    };
    SPC_SetActiveModeCoreLDORegulatorConfig(SPC0, &ldoOpt);
    /* Configure Flash wait-states to support 1.2V voltage level and 150000000Hz frequency */;
    FMU0->FCTRL = (FMU0->FCTRL & ~((uint32_t)FMU_FCTRL_RWSC_MASK)) | (FMU_FCTRL_RWSC(0x3U));
    /* Specifies the 1.2V operating voltage for the SRAM's read/write timing margin */
    spc_sram_voltage_config_t sramCfg = {
      .operateVoltage       = kSPC_sramOperateAt1P2V,
      .requestVoltageUpdate = true,
    };
    SPC_SetSRAMOperateVoltage(SPC0, &sramCfg);

    CLOCK_SetupFROHFClocking(48000000U);               /*!< Enable FRO HF(48MHz) output */
    /*!< Set up PLL0 */
    const pll_setup_t pll0Setup = {
        .pllctrl = SCG_APLLCTRL_SOURCE(1U) | SCG_APLLCTRL_SELI(27U) | SCG_APLLCTRL_SELP(13U),
        .pllndiv = SCG_APLLNDIV_NDIV(8U),
        .pllpdiv = SCG_APLLPDIV_PDIV(1U),
        .pllmdiv = SCG_APLLMDIV_MDIV(50U),
        .pllRate = 150000000U
    };
    CLOCK_SetPLL0Freq(&pll0Setup);                       /*!< Configure PLL0 to the desired values */
    CLOCK_SetPll0MonitorMode(kSCG_Pll0MonitorDisable);    /* Pll0 Monitor is disabled */

    CLOCK_SetupClk16KClocking(kCLOCK_Clk16KToVbat);      /* FRO16 kHz output clock to Vbat enabled */
    SYSCON->CLOCK_CTRL |=  SYSCON_CLOCK_CTRL_FRO1MHZ_CLK_ENA_MASK;                 /*!< Enable FRO_1M is on */



    /*!< Set up clock selectors  */
    CLOCK_AttachClk(kPLL0_to_MAIN_CLK);
    CLOCK_AttachClk(kPLL0_to_CTIMER4);                 /*!< Switch CTIMER4 to PLL0 */
    CLOCK_AttachClk(kFRO_HF_DIV_to_FLEXCOMM0);                 /*!< Switch FLEXCOMM0 to FRO_HF_DIV */
    CLOCK_AttachClk(kFRO_HF_DIV_to_FLEXCOMM3);                 /*!< Switch FLEXCOMM3 to FRO_HF_DIV */
    CLOCK_AttachClk(kCLK_1M_to_WDT1);                 /*!< Switch WDT1 to CLK_1M */

    /*!< Set up dividers */
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U);           /*!< Set AHBCLKDIV divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivCtimer4Clk, 1U);           /*!< Set CTIMER4CLKDIV divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivFrohfClk, 1U);           /*!< Set FROHFDIV divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom0Clk, 2U);           /*!< Set FLEXCOMM0CLKDIV divider to value 2 */
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom3Clk, 1U);           /*!< Set FLEXCOMM3CLKDIV divider to value 1 */

    /* Set SystemCoreClock variable */
    SystemCoreClock = BOARD_BOOTCLOCKPLL150M_CORE_CLOCK;
}

/*******************************************************************************
 ******************** Configuration BOARD_BootClockPLL100M *********************
 ******************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!Configuration
name: BOARD_BootClockPLL100M
outputs:
- {id: CLK_144M_clock.outFreq, value: 144 MHz}
- {id: CLK_48M_clock.outFreq, value: 48 MHz}
- {id: CLK_IN_clock.outFreq, value: 24 MHz}
- {id: FRO_12M_clock.outFreq, value: 12 MHz}
- {id: MAIN_clock.outFreq, value: 100 MHz}
- {id: PLL1_CLK_clock.outFreq, value: 100 MHz}
- {id: Slow_clock.outFreq, value: 25 MHz}
- {id: System_clock.outFreq, value: 100 MHz}
- {id: gdet_clock.outFreq, value: 48 MHz}
- {id: trng_clock.outFreq, value: 48 MHz}
settings:
- {id: PLL1_Mode, value: Normal}
- {id: RunPowerMode, value: OD}
- {id: SCGMode, value: PLL1}
- {id: SCG.PLL1M_MULT.scale, value: '100', locked: true}
- {id: SCG.PLL1_NDIV.scale, value: '6', locked: true}
- {id: SCG.PLL1_PDIV.scale, value: '4', locked: true}
- {id: SCG.SCSSEL.sel, value: SCG.PLL1_CLK}
- {id: SCG_FIRCCSR_FIRCEN_CFG, value: Disabled}
- {id: SCG_SOSCCSR_SOSCEN_CFG, value: Enabled}
- {id: SYSCON.FREQMEREFCLKSEL.sel, value: SYSCON.evtg_out0a}
- {id: SYSCON.FREQMETARGETCLKSEL.sel, value: SYSCON.evtg_out0a}
sources:
- {id: SCG.SOSC.outFreq, value: 24 MHz, enabled: true}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/*******************************************************************************
 * Variables for BOARD_BootClockPLL100M configuration
 ******************************************************************************/
/*******************************************************************************
 * Code for BOARD_BootClockPLL100M configuration
 ******************************************************************************/
void BOARD_BootClockPLL100M(void)
{
    CLOCK_EnableClock(kCLOCK_Scg);                     /*!< Enable SCG clock */

    /* FRO OSC setup - begin, attach FRO12M to MainClock for safety switching */
    CLOCK_AttachClk(kFRO12M_to_MAIN_CLK);              /*!< Switch to FRO 12M first to ensure we can change the clock setting */

    /* Set the DCDC VDD regulator to 1.2 V voltage level */
    spc_active_mode_dcdc_option_t dcdcOpt = {
      .DCDCVoltage       = kSPC_DCDC_OverdriveVoltage,
      .DCDCDriveStrength = kSPC_DCDC_NormalDriveStrength,
    };
    SPC_SetActiveModeDCDCRegulatorConfig(SPC0, &dcdcOpt);
    /* Set the LDO_CORE VDD regulator to 1.2 V voltage level */
    spc_active_mode_core_ldo_option_t ldoOpt = {
      .CoreLDOVoltage       = kSPC_CoreLDO_OverDriveVoltage,
      .CoreLDODriveStrength = kSPC_CoreLDO_NormalDriveStrength,
    };
    SPC_SetActiveModeCoreLDORegulatorConfig(SPC0, &ldoOpt);
    /* Configure Flash wait-states to support 1.2V voltage level and 100000000Hz frequency */;
    FMU0->FCTRL = (FMU0->FCTRL & ~((uint32_t)FMU_FCTRL_RWSC_MASK)) | (FMU_FCTRL_RWSC(0x2U));
    /* Specifies the 1.2V operating voltage for the SRAM's read/write timing margin */
    spc_sram_voltage_config_t sramCfg = {
      .operateVoltage       = kSPC_sramOperateAt1P2V,
      .requestVoltageUpdate = true,
    };
    SPC_SetSRAMOperateVoltage(SPC0, &sramCfg);

    CLOCK_SetupExtRefClocking(24000000U);
    CLOCK_SetSysOscMonitorMode(kSCG_SysOscMonitorDisable);    /* System OSC Clock Monitor is disabled */

    /*!< Set up PLL1 */
    const pll_setup_t pll1Setup = {
        .pllctrl = SCG_SPLLCTRL_SOURCE(0U) | SCG_SPLLCTRL_SELI(53U) | SCG_SPLLCTRL_SELP(26U),
        .pllndiv = SCG_SPLLNDIV_NDIV(6U),
        .pllpdiv = SCG_SPLLPDIV_PDIV(2U),
        .pllmdiv = SCG_SPLLMDIV_MDIV(100U),
        .pllRate = 100000000U
    };
    CLOCK_SetPLL1Freq(&pll1Setup);                       /*!< Configure PLL1 to the desired values */
    CLOCK_SetPll1MonitorMode(kSCG_Pll1MonitorDisable);    /* Pll1 Monitor is disabled */

    /*!< Set up clock selectors  */
    CLOCK_AttachClk(kPLL1_to_MAIN_CLK);

    /*!< Set up dividers */
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U);           /*!< Set AHBCLKDIV divider to value 1 */

    /* Set SystemCoreClock variable */
    SystemCoreClock = BOARD_BOOTCLOCKPLL100M_CORE_CLOCK;
}

