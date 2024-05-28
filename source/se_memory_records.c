///*
// * se_memory_records.c
// *
// *  Created on: Apr 1, 2024
// *      Author: Stanley.Ezeh
// */
//
//#include "mc_periph_init.h"
//#include "mid_sm_states.h"
//#include "m1_sm_snsless_enc.h"
//#include "se_communication.h"
//
//#ifdef MC_EXAMPLE
///* global used misc variables */
//extern uint32_t g_ui32NumberOfCycles;
//extern uint32_t g_ui32MaxNumberOfCycles;
//
///* Application and board ID  */
//extern app_ver_t g_sAppIdFM;
//
//extern ctrl_m1_mid_t g_sSpinMidSwitch;
//extern bool_t bDemoModeSpeed;
//
//#ifdef PMSM_SNSLESS_ENC
//extern bool_t bDemoModePosition;
//#endif
//
//
//#ifdef MID_EN
///* MID 2.0 control commands */
//extern mid_app_cmd_t g_eMidCmd;
//#endif
//
//#endif
//
//
//MEMORY_TABLE_DECL_START(MEM_TABLE)
//SE_MEM_REC_CREATE(g_bM1SwitchAppOnOff, sU8, RW )
//SE_MEM_REC_CREATE(g_sM1Ctrl.eState, sU8, RO)
//SE_MEM_REC_CREATE(g_eM1StateRun, sU8, RO)
//SE_MEM_REC_CREATE(g_sM1Drive.sSpeed.fltSpeedFilt, sFLT, RO)
//SE_MEM_REC_CREATE(g_sM1Drive.sSpeed.fltSpeedCmd, sFLT, RO)
//SE_MEM_REC_CREATE(g_sSpinMidSwitch.eAppState, sU8, RW)
//SE_MEM_REC_CREATE(g_sM1Drive.sFaultIdPending, sU8, RW)
//SE_MEM_REC_CREATE(g_sM1Drive.bFaultClearMan, sU8, RW)
//SE_MEM_REC_CREATE(g_sSpinMidSwitch.sFaultCtrlM1_Mid, sU16, RW)
//SE_MEM_REC_CREATE(g_sM1Drive.sFocPMSM.sIABC.fltA, sFLT, RO)
//SE_MEM_REC_CREATE(g_sM1Drive.sFocPMSM.sIABC.fltB, sFLT, RO)
//SE_MEM_REC_CREATE(g_sM1Drive.sFocPMSM.sIABC.fltC, sFLT, RO)
//SE_MEM_REC_CREATE(g_sM1Drive.sFocPMSM.fltUDcBusFilt, sFLT, RO)
//SE_MEM_REC_CREATE(g_sM1Drive.sMCATctrl.sUDQReqMCAT.fltQ, sFLT, RO)
//SE_MEM_REC_CREATE(g_sM1Drive.sMCATctrl.sUDQReqMCAT.fltD, sFLT, RO)
//MEMORY_TABLE_DECL_CLOSE()
