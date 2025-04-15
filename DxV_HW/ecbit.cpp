/*---------------------------------------------------------

    Rev. 1  18nov11 Mark Saj.
    Rev. 2  24jan12 Mark Saj.   ECBIT Rev. 7 ; Using SYS_BUS_PARALLEL
    Rev. 3  25apr12 Mark Saj    ECBIT Rev. 8 ; Support for DMDx Parallel and I2C Bus.
    Rev. 4  22may12 Mark Saj    ECBIT Rev. 9 ; ECBIT_MOD_ADDR_REG = 3 ; Support for DMDx Parallel and I2C Bus.
   
---------------------------------------------------------*/
#include <Unison.h>
#include "ecbit.h"
using namespace std ;

//#########################################################
static  const   bool        USE_DRIVER_READBACK             = true ; //true ;
static  const   bool        DUTBUS_ACTIVE                   = false ; //false ; True Uses // Bus
static  const   FloatS      I2C_CLK_FREQ                    = 1.0MHz ; //400KHz;  //100KHz; 

static  const   int         ECBITS_MAX                      = 256 ;
static  const   int         ECBIT_BLOCK_NUM                 =  16 ;
static  const   int         ECBIT_REV_REG                   =   0 ;     // Register Map
static  const   int         ECBIT_BRD_CSR_REG               =   1 ;     // 2    intentionally skipped
static  const   int         ECBIT_MOD_ADDR_REG              =   3 ;
static  const   int         ECBIT_BLOCK_ADDR_REG            =   4 ;
static  const   int         ECBIT_BLOCK_CSR_REG             =   5 ;     // 6, 7 intentionally skipped
static  const   int         ECBIT_CHIP_0_HN_REG             =   8 ;
static  const   int         ECBIT_CHIP_0_LN_REG             =   9 ;
static  const   int         ECBIT_CHIP_1_HN_REG             =  10 ;
static  const   int         ECBIT_CHIP_1_LN_REG             =  11 ;                           
static  const   int         ECBIT_CHIP_0_RD_REG             =  12 ;
static  const   int         ECBIT_CHIP_1_RD_REG             =  13 ;                           

static  const   int         ECBIT_BLOCK_ADDR_WBIT           = 0x10 ;    // in Register ECBIT_BLOCK_ADDR_REG 
static  const   int         ENABLE_PDCD_CHIP_0              =  0x3 ;    // 2#0011
static  const   int         ENABLE_PDCD_CHIP_1              =  0xC ;    // 2#1100
static  const   int         DISABLE_PDCD_CHIP_0             =  0x2 ;    // 2#0010
static  const   int         DISABLE_PDCD_CHIP_1             =  0x8 ;    // 2#1000
static  const   int         ENABLE_SPLD                     =  0x8 ;    // 2#1000
static  const   int         DISABLE_SPLD                    =  0xC ;    // 2#1100
static  const   int         LOAD_ALL_CBITS_TO_RELAY_DRIVERS =  0x2 ;    // 2#10
static  const   int         RESET_BOARD                     =  0x1 ;    // 2#01
static  const   int         ECBIT_REV                       =    9 ;    // 22may12 //8 ;   // 02may12
static  const   int         I2C_ADDR_LENGTH                 =    1 ;    // To control the ECBIT module the AddressLength will alwasy be one
static  const   int         I2C_SLAVE_ADDR                  = 0x18 ;    // The ECBIT slave addres is 0x18, plus the what the three physical address linses are set, not counting the R/W bit   

//--------------------------------------------------------
static          SITE        this_site ;
static          SiteIter    site_idx ; 

static          unsigned    OnLoadSiteNum ;
static          unsigned    ECBITS_MAX_by_sites_num ;
static const    unsigned    init_lut_w_ary         [ECBIT_BLOCK_NUM] = {0xaa, 0xab, 0xae, 0xaf, 0xba, 0xbb, 0xbe, 0xbf, 0xea, 0xeb, 0xee, 0xef, 0xfa, 0xfb, 0xfe, 0xff} ;
//static const  unsigned    init_lut_r_ary         [0x56]            = {0,1,0,0,2,3,0,0,0,0,0,0,0,0,0,0,4,5,0,0,6,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,9,0,0,10,11,0,0,0,0,0,0,0,0,0,0,12,13,0,0,14,15 } ;
static const    unsigned    init_lut_r_ary         [0xff]            = {0,1,0,0,2,3,0,0,0,0,0,0,0,0,0,0,4,5,0,0,6,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,9,0,0,10,11,0,0,0,0,0,0,0,0,0,0,12,13,0,0,14,15 } ;
//static const    unsigned    init_lut_r_ary         [0xff]            = {0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,,8,,9,0,1,2,3,4,5,6,7,8,9,,0,,1,2,3,,,4,5,6,7,8 } ;
static const    unsigned    init_cbits_all_mask_ary[ECBIT_BLOCK_NUM] = {65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535} ;

//#########################################################
Ecbit    ECBIT_BUS0 (0, 0x07) ; //Instantiate a single object ECBIT_BUS0 of a class Ecbit  
//Ecbit    ECBIT_BUS1 (1, 0x00) ; //Instantiate a single object ECBIT_BUS1 of a class Ecbit  
       
//#########################################################
Ecbit::Ecbit (unsigned BusInstance_Num, UnsignedS Mod_Addr) :
lut_w_ary          (init_lut_w_ary),
lut_r_ary          (init_lut_r_ary),
cbits_all_mask_ary (init_cbits_all_mask_ary),                                    
cbits_w_ary        (ECBIT_BLOCK_NUM),
cbits_w_ary_mask   (ECBIT_BLOCK_NUM)
{

    OnLoadSiteNum               = ActiveSites.GetNumSites() ;
    ECBITS_MAX_by_sites_num = ECBITS_MAX / OnLoadSiteNum ;
    ModAddr = Mod_Addr ;
    BusInstance = BusInstance_Num; //BusInstance is cage (0-3) for XSeries, BusInstance is slot (0-39) for DiamondX

    if (DUTBUS_ACTIVE) 
    {
        BusType = SYS_BUS_PARALLEL;
        Reset () ;
    }
    else
    {
        BusType = SYS_BUS_I2C; 
        SDUI2CPin = "zz_SDUI2Cx" ; // SYS.ConfigDutBus uses a PinType, not a BusType. Setup pin "SDUI2Cx" in adpater board to match the the SDU card your working with for the I2C bus
        DmdxI2CConfig.ClockFrequency = I2C_CLK_FREQ ; // You can opeate the ECBIT module at three bus speeds, (1.0MHz,400KHz,100KHz)
        DmdxI2CConfig.SlaveID        = I2C_SLAVE_ADDR + ModAddr ; // The ECBIT slave addres is 0x18, plus the what the three physical address linses are set, not counting the R/W bit   
        DmdxI2CConfig.AddressLength  = I2C_ADDR_LENGTH ; // To control the ECBIT module the AddressLength will alwasy be one
    }
    
    CbitPinListDefer = ""; 
}

//#########################################################
void    Ecbit::SetModAddr (UnsignedS NewAddr)
{
    if (NewAddr > 7) 
        cout << "Error Module Address not updated: The valid module addresses are (0-7)" << endl ;    
    else
        ModAddr = NewAddr;    

    if (!DUTBUS_ACTIVE) 
    {
        DmdxI2CConfig.ClockFrequency = I2C_CLK_FREQ ; // You can opeate the ECBIT module at three bus speeds, (1.0MHz,400KHz,100KHz)
        DmdxI2CConfig.SlaveID        = I2C_SLAVE_ADDR + ModAddr ; // The ECBIT slave addres is 0x18, plus the what the three physical address linses are set, not counting the R/W bit   
        DmdxI2CConfig.AddressLength  = I2C_ADDR_LENGTH ; // To control the ECBIT module the AddressLength will alwasy be one
        SYS.ConfigDUTBus (SDUI2CPin, DmdxI2CConfig) ;   // Pin is not available before the loadboard is selected and activated.
    }
}

//#########################################################
void    Ecbit::Reset ()
{
    if (!DUTBUS_ACTIVE) 
        SYS.ConfigDUTBus (SDUI2CPin, DmdxI2CConfig) ;   // Pin is not available before the loadboard is selected and activated.
    
    Set ("") ;
    CbitPinListShadow = "" ; 
    CbitPinListDefer = ""; 
}

//#########################################################
UnsignedS   Ecbit::GetRev ()
{

  //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,          MyData)
    SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_MOD_ADDR_REG, ModAddr) ;
  //SYS.ReadDUTBus  (SYSBusTypeS, BusInstance, MyAddress,          MyData,    SimulatedData) ;
    SYS.ReadDUTBus  (BusType, BusInstance,        ECBIT_REV_REG,      ecbit_rev, ECBIT_REV) ; 
    return (ecbit_rev) ;
}

//#########################################################
void    Ecbit::Close (PinML CbitPinList)
{
    PinML CbitPinListAll = CbitPinListShadow + CbitPinList ;    
    Set (CbitPinListAll) ;
}
//#########################################################
void    Ecbit::CloseDefer (PinML CbitPinList)
{
    CbitPinListDefer += (CbitPinListShadow + CbitPinList);    
}

//#########################################################
void    Ecbit::Open (PinML CbitPinList)
{
    PinML CbitPinListAll = CbitPinListShadow - CbitPinList ;    
    Set (CbitPinListAll) ;
}

//#########################################################
void    Ecbit::OpenDefer (PinML CbitPinList)
{
    CbitPinListDefer += (CbitPinListShadow - CbitPinList);    
}

//#########################################################
void  Ecbit::CloseAndOpen  (const PinML& Close, const PinML& Open){
    PinML CbitPinListAll = CbitPinListShadow += Close; 
    CbitPinListAll -= Open;
    Set (CbitPinListAll);
}


//#########################################################
void  Ecbit::ApplyDefer  (){
    PinML CbitPinListAll = CbitPinListShadow + CbitPinListDefer;  
    Set (CbitPinListAll) ;
    CbitPinListShadow = "";
}

//#########################################################
void  Ecbit::Set (PinML CbitPinList)
{
/*
--    register 0x4 - block selection
--    b7   b6   b5   b4   b3   b2   b1   b0
-- 	           W0   O3   O2   O1   O0

--    registers 0x8-0xB - ecbit selection
--    b7   b6   b5   b4   b3   b2   b1   b0
--    W3   O3   W2   O2   W1   O1   W0   O0

-- Ox = 1 to close ecbit, 0 to open ecbit
-- wx = 1 to write,       0 to do nothing ?

--        Block   1   2   3   4   5   6    7    8    9   10   11   12   13   14   15   16
-- register 0x9
--   O0 --> cbit  1, 17, 33, 49, 65, 81,  97, 113, 129, 145, 161, 177, 193, 209, 225, 241
--   O1 --> cbit  2, 18, 34, 50, 66, 82,  98, 112, 130, 146, 162, 178, 194, 210, 226, 242
--   O2 --> cbit  3, 19, 35, 51, 67, 83,  99, 113, 131, 147, 163, 179, 195, 211, 227, 243 
--   O3 --> cbit  4, 20, 36, 52, 68, 84, 100, 114, 132, 148, 164, 180, 196, 212, 228, 244
-- register 0x8         
--   O0 --> cbit  5, 21, 37, 53, 69, 85, 101, 115, 133, 149, 165, 181, 197, 213, 229, 245
--   O1 --> cbit  6, 22, 38, 54, 70, 86, 102, 116, 134, 150, 166, 182, 198, 214, 230, 246
--   O2 --> cbit  7, 23, 39, 55, 71, 87, 103, 117, 135, 151, 167, 183, 199, 215, 231, 247
--   O3 --> cbit  8, 24, 40, 56, 72, 88, 104, 118, 136, 152, 168, 184, 200, 216, 232, 248
-- register 0xB         
--   O0 --> cbit  9, 25, 41, 57, 73, 89, 105, 119, 137, 153, 169, 185, 201, 217, 233, 249
--   O1 --> cbit 10, 26, 42, 58, 74, 90, 106, 120, 138, 154, 170, 186, 202, 218, 234, 250
--   O2 --> cbit 11, 27, 43, 59, 75, 91, 107, 121, 139, 155, 171, 187, 203, 219, 235, 251
--   O3 --> cbit 12, 28, 44, 60, 76, 92, 108, 122, 140, 156, 172, 188, 204, 220, 236, 252
-- register 0xA
--   O0 --> cbit 13, 29, 45, 61, 77, 93, 109, 123, 141, 157, 173, 189, 205, 221, 237, 253
--   O1 --> cbit 14, 30, 46, 62, 78, 94, 110, 124, 142, 158, 174, 190, 206, 222, 238, 254
--   O2 --> cbit 15, 31, 47, 63, 79, 95, 111, 125, 143, 159, 175, 191, 207, 223, 239, 255
--   O3 --> cbit 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240, 256
*/
    int         PinsNum, MaskNum;
    IntM        PinChan, PinMask, PinChan_1, PinMask_1 ;
    int         this_row, this_col, this_row_mask, this_col_mask ;
    IntM        row_idx,  col_idx, row_idx_mask,  col_idx_mask ;
    PinM        NextPin, NextMask ;

//-------------------------------------------------------------
    cbits_w_ary       = 0 ;
    PinsNum           = CbitPinList.GetNumPins() ;
    CbitPinListShadow = "" ;

    if (PinsNum != 0)
    {        
        for (int i=0 ; i < PinsNum ; i++) // build set cbit array
        {
            NextPin = CbitPinList[i] ;
            PinChan = NextPin.GetResourceInstance() ; 
            if (PinChan != -1)  // if not a "null" type
            {            
                PinChan_1 = PinChan - 1 ; 
                row_idx   = PinChan_1 / 16 ;
                col_idx   = PinChan_1 % 16 ;            
                for (site_idx=ActiveSites.Begin() ; !site_idx.End() ; ++site_idx)
                { 
                    this_site = site_idx.GetValue() ;
                    this_row  = row_idx[this_site] ;
                    this_col  = col_idx[this_site] ;
                    cbits_w_ary[this_row] |= (1 << this_col) ;
                }
            }
        }

//        for (int i=0 ; i < ECBIT_BLOCK_NUM ; i++)
//        {
//        cbits_w_ary[i] = cbits_w_ary[i] & PinMask;
//        }       
      //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,          MyData)
        SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_MOD_ADDR_REG, ModAddr) ;

      //Parse bitmap into 4 8-bit registers per block.
        for (int i=0 ; i < ECBIT_BLOCK_NUM ; i++)
        {
            block_adr_w  = i + ECBIT_BLOCK_ADDR_WBIT ;

            lut_idx    = ((cbits_w_ary[i] & 0x000F) >>  0) ;         
            reg_9_data = lut_w_ary[lut_idx] ;

            lut_idx    = ((cbits_w_ary[i] & 0x00F0) >>  4) ;
            reg_8_data = lut_w_ary[lut_idx] ;

            lut_idx    = ((cbits_w_ary[i] & 0x0F00) >>  8) ;
            reg_b_data = lut_w_ary[lut_idx] ;

            lut_idx    = ((cbits_w_ary[i] & 0xF000) >> 12) ;
            reg_a_data = lut_w_ary[lut_idx] ;

          //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,            MyData)
            SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_BLOCK_ADDR_REG, block_adr_w) ;
            SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_CHIP_0_LN_REG,  reg_9_data) ;
            SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_CHIP_0_HN_REG,  reg_8_data) ;
            SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_CHIP_1_LN_REG,  reg_b_data) ;
            SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_CHIP_1_HN_REG,  reg_a_data) ;    
        }

      //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,         MyData)
        SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_BRD_CSR_REG, LOAD_ALL_CBITS_TO_RELAY_DRIVERS) ;
        TIME.Wait (12.0us) ;    // !
        CbitPinListShadow = CbitPinList ;
    }
    else 
    {   
      //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,          MyData)
        SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_MOD_ADDR_REG, ModAddr) ;
            
      //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,          MyData)
        SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_BRD_CSR_REG,  RESET_BOARD) ;
    }   
}

//#########################################################
void  Ecbit::Set (PinML CbitPinList, PinML MaskList)
{
    int         PinsNum, MaskNum;
    IntM        PinChan, PinMask, PinChan_1, PinMask_1 ;
    int         this_row, this_col, this_row_mask, this_col_mask ;
    IntM        row_idx,  col_idx, row_idx_mask,  col_idx_mask ;
    PinM        NextPin, NextMask ;

//-------------------------------------------------------------
    cbits_w_ary       = 0 ;
    cbits_w_ary_mask  = 0 ;
    PinsNum           = CbitPinList.GetNumPins() ;
    MaskNum           = MaskList.GetNumPins() ;
    CbitPinListShadow = "" ;

    if (PinsNum != 0)
    {
        for (int i=0 ; i < MaskNum ; i++) // build mask array
        {
            NextMask = MaskList[i] ;
            PinMask = NextMask.GetResourceInstance() ;
            if (PinMask != -1)  // if not a "null" type
            {            
                PinMask_1 = PinMask - 1 ; 
                row_idx_mask   = PinMask_1 / 16 ;
                col_idx_mask   = PinMask_1 % 16 ;            
                for (site_idx=ActiveSites.Begin() ; !site_idx.End() ; ++site_idx)
                { 
                    this_site = site_idx.GetValue() ;
                    this_row_mask  = row_idx_mask[this_site] ;
                    this_col_mask  = col_idx_mask[this_site] ;
                    cbits_w_ary_mask[this_row_mask] |= (1 << this_col_mask) ;
                }
            }
        }   
        
        for (int i=0 ; i < PinsNum ; i++) // build set cbit array
        {
            NextPin = CbitPinList[i] ;
            PinChan = NextPin.GetResourceInstance() ; 
            if (PinChan != -1)  // if not a "null" type
            {            
                PinChan_1 = PinChan - 1 ; 
                row_idx   = PinChan_1 / 16 ;
                col_idx   = PinChan_1 % 16 ;            
                for (site_idx=ActiveSites.Begin() ; !site_idx.End() ; ++site_idx)
                { 
                    this_site = site_idx.GetValue() ;
                    this_row  = row_idx[this_site] ;
                    this_col  = col_idx[this_site] ;
                    cbits_w_ary[this_row] |= (1 << this_col) ;
                    cbits_w_ary[this_row] = cbits_w_ary[this_row] & cbits_w_ary_mask[this_row]; 
                }
            }
        }

//        for (int i=0 ; i < ECBIT_BLOCK_NUM ; i++)
//        {
//        cbits_w_ary[i] = cbits_w_ary[i] & PinMask;
//        }       
      //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,          MyData)
        SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_MOD_ADDR_REG, ModAddr) ;

      //Parse bitmap into 4 8-bit registers per block.
        for (int i=0 ; i < ECBIT_BLOCK_NUM ; i++)
        {
            block_adr_w  = i + ECBIT_BLOCK_ADDR_WBIT ;

            lut_idx    = ((cbits_w_ary[i] & 0x000F) >>  0) ;         
            reg_9_data = lut_w_ary[lut_idx] ;

            lut_idx    = ((cbits_w_ary[i] & 0x00F0) >>  4) ;
            reg_8_data = lut_w_ary[lut_idx] ;

            lut_idx    = ((cbits_w_ary[i] & 0x0F00) >>  8) ;
            reg_b_data = lut_w_ary[lut_idx] ;

            lut_idx    = ((cbits_w_ary[i] & 0xF000) >> 12) ;
            reg_a_data = lut_w_ary[lut_idx] ;

          //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,            MyData)
            SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_BLOCK_ADDR_REG, block_adr_w) ;
            SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_CHIP_0_LN_REG,  reg_9_data) ;
            SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_CHIP_0_HN_REG,  reg_8_data) ;
            SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_CHIP_1_LN_REG,  reg_b_data) ;
            SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_CHIP_1_HN_REG,  reg_a_data) ;    
        }

      //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,         MyData)
        SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_BRD_CSR_REG, LOAD_ALL_CBITS_TO_RELAY_DRIVERS) ;
        TIME.Wait (12.0us) ;    // !
        CbitPinListShadow = CbitPinList ;
    }
    else 
    {   
      //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,          MyData)
        SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_MOD_ADDR_REG, ModAddr) ;
            
      //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,          MyData)
        SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_BRD_CSR_REG,  RESET_BOARD) ;
    }   
}
//#########################################################
TMResultM   Ecbit::Read (PinML CbitPinList)
{
        TMResultM   fflag_l_ms = TM_PASS ;
        TMResultM   fflag_ms   = TM_PASS ;
        BoolM       ORFail     = false ;
        StringS     FailBin    = "" ;
        StringS     TestName   = "" ;

        UnsignedS   reg_8_cbits, reg_9_cbits, reg_a_cbits, reg_b_cbits ;
        UnsignedS   block_adr_r ;
        UnsignedS1D cbits_shadow_ary    (ECBIT_BLOCK_NUM) ;
        UnsignedM1D cbits_shadow_ary_ms (ECBITS_MAX_by_sites_num) ;
        UnsignedS1D cbits_r_ary         (ECBIT_BLOCK_NUM) ;
        UnsignedS1D cbits_r_driver_ary  (ECBIT_BLOCK_NUM) ;
        UnsignedM1D cbits_ary_ms        (ECBITS_MAX_by_sites_num) ;
        UnsignedS1D cbits_delta_ary     (ECBIT_BLOCK_NUM) ;
        UnsignedM   cbit_state_ms ;
        UnsignedM   cbit_shadow_ms ;

        int         PinsNum, PinsNum_1 ;        
        IntM        PinChan, PinChan_1 ; 
        int         this_row, this_col, this_pin, this_chan ; 
        IntM        row_idx, col_idx ;
        PinM        NextPin ;

//-------------------------------------------------------------
  //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,            MyData)
    SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_MOD_ADDR_REG,   ModAddr) ;
    
    for (int i=0 ; i < ECBIT_BLOCK_NUM ; i++)
    {
        block_adr_w = i + ECBIT_BLOCK_ADDR_WBIT ;

      //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,            MyData)
        SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_BLOCK_ADDR_REG, block_adr_w) ;      

        if (!USE_DRIVER_READBACK)
        {            
            SYS.ReadDUTBus  (BusType, BusInstance,    ECBIT_CHIP_0_LN_REG,  reg_9_data,  0) ;
            SYS.ReadDUTBus  (BusType, BusInstance,    ECBIT_CHIP_0_HN_REG,  reg_8_data,  0) ;
            SYS.ReadDUTBus  (BusType, BusInstance,    ECBIT_CHIP_1_LN_REG,  reg_b_data,  0) ;
            SYS.ReadDUTBus  (BusType, BusInstance,    ECBIT_CHIP_1_HN_REG,  reg_a_data,  0) ;

            lut_idx     = reg_8_data ;
            reg_8_cbits = lut_r_ary[lut_idx] ;
            lut_idx     = reg_9_data ;
            reg_9_cbits = lut_r_ary[lut_idx] ;
            lut_idx     = reg_a_data ;
            reg_a_cbits = lut_r_ary[lut_idx] ;
            lut_idx     = reg_b_data ;
            reg_b_cbits = lut_r_ary[lut_idx] ;        

            cbits_r_ary[i]     = (reg_a_cbits << 12) | (reg_b_cbits << 8) | (reg_8_cbits << 4) | reg_9_cbits ;
            cbits_delta_ary[i] = cbits_r_ary[i] ^ cbits_w_ary[i] ;  
        } else
        {
          //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,         MyData)
            SYS.WriteDUTBus (BusType, BusInstance,    ECBIT_BRD_CSR_REG, LOAD_ALL_CBITS_TO_RELAY_DRIVERS) ;
            TIME.Wait (12.0us) ;    // !

          //SYS.ReadDUTBus  (SYSBusTypeS, BusInstance, MyAddress,           MyData,     SimulatedData) ;
            SYS.ReadDUTBus  (BusType, BusInstance,        ECBIT_CHIP_0_RD_REG, reg_c_data, 0) ;
            SYS.ReadDUTBus  (BusType, BusInstance,        ECBIT_CHIP_1_RD_REG, reg_d_data, 0) ;
    
            cbits_r_driver_ary[i] = (reg_d_data << 8) + reg_c_data ; 

          //Mask out all unused (no-pullup) cbits
            cbits_r_driver_ary = cbits_r_driver_ary & cbits_all_mask_ary ;
            cbits_delta_ary    = cbits_r_driver_ary ^ cbits_w_ary ;
        }
    }

    cbits_ary_ms        = 0 ;
    cbits_shadow_ary    = 0 ;
    cbits_shadow_ary_ms = 0 ;

    PinsNum = CbitPinListShadow.GetNumPins() ;
    if (PinsNum != 0)
    {
        for (int i=0 ; i < PinsNum ; i++)
        {
            NextPin = CbitPinListShadow[i] ;
            PinChan = NextPin.GetResourceInstance() ;
 
            if (PinChan != -1)  // if not a "null" type
            {
                PinChan_1   = PinChan - 1 ; 
                row_idx     = PinChan_1 / 16 ;
                col_idx     = PinChan_1 % 16 ; 

                for (site_idx=ActiveSites.Begin() ; !site_idx.End() ; ++site_idx)
                { 
                    this_site = site_idx.GetValue() ;
                    this_row  = row_idx  [this_site] ;
                    this_col  = col_idx  [this_site] ;
                    this_chan = PinChan_1[this_site] ;

                    this_pin = this_chan % ECBITS_MAX_by_sites_num ;   // Vertical   pin arrangement!
                    cbits_shadow_ary[this_row] |= (1 << this_col) ; 
                   
                    if (USE_DRIVER_READBACK)
                        cbits_ary_ms[this_site][this_pin] = (cbits_r_driver_ary[this_row] >> this_col) & 0x1 ;
                    else
                        cbits_ary_ms[this_site][this_pin] = (cbits_r_ary[this_row] >> this_col) & 0x1 ;
                    
                    cbits_shadow_ary_ms[this_site][this_pin] = (cbits_shadow_ary[this_row] >> this_col) & 0x1 ;
                }
            }
        }
    }

    FailBin = "BIN_FAIL_ECBIT" ;
    ORFail  = false ;

    PinsNum = CbitPinList.GetNumPins() ;
    for (IntS i=0 ; i < PinsNum ; ++i)
    {
        NextPin = CbitPinList[i] ;
        PinChan = NextPin.GetResourceInstance() ;
        if (PinChan != -1)  // if not a "null" type
        {
            PinChan_1 = PinChan - 1 ;
            for (site_idx=ActiveSites.Begin() ; !site_idx.End() ; ++site_idx)
            { 
                this_site = site_idx.GetValue() ;
                this_chan = PinChan_1[this_site] ;
                this_pin = this_chan % ECBITS_MAX_by_sites_num ;   // Vertical   pin arrangement!

                cbit_state_ms[this_site] = cbits_ary_ms[this_site][this_pin] ;
                cbit_shadow_ms[this_site] = cbits_shadow_ary_ms[this_site][this_pin] ;
            }
    
            TestName   = "ECBIT_" + CONV.IntToString (i+1) + "_STATE" ;
          //Result     = DLOG.Value (Pin(s),  Value,         LowLimit,       HighLimit,      Units,    Comment,  FailBin, ORFail,   MinorID, ExpectedResult, DLOGEnable);
            fflag_l_ms = DLOG.Value (NextPin, cbit_state_ms, cbit_shadow_ms, cbit_shadow_ms, UTL_VOID, TestName, FailBin, UTL_VOID, (i+1),   ER_PASS,        true) ;
            fflag_ms   = DLOG.AccumulateResults (fflag_ms, fflag_l_ms) ;
        }
    }

    return (fflag_ms) ;
}

//#########################################################
TMResultM   Ecbit::ReadActual (PinML CbitPinList)
{
        TMResultM   fflag_l_ms = TM_PASS ;
        TMResultM   fflag_ms   = TM_PASS ;
        BoolM       ORFail     = false ;
        StringS     FailBin    = "" ;
        StringS     TestName   = "" ;
        UnsignedS   block_adr_r ;
        UnsignedS1D cbits_shadow_ary    (ECBIT_BLOCK_NUM) ;
        UnsignedM1D cbits_shadow_ary_ms (ECBITS_MAX_by_sites_num) ;
        UnsignedS1D cbits_r_ary         (ECBIT_BLOCK_NUM) ;
        UnsignedS1D cbits_r_driver_ary  (ECBIT_BLOCK_NUM) ;
        UnsignedM1D cbits_ary_ms        (ECBITS_MAX_by_sites_num) ;
        UnsignedS1D cbits_delta_ary     (ECBIT_BLOCK_NUM) ;
        UnsignedM   cbit_state_ms ;
        UnsignedM   cbit_shadow_ms ;
        int         PinsNum, PinsNum_1 ;        
        IntM        PinChan, PinChan_1 ; 
        int         this_row, this_col, this_pin, this_chan ; 
        IntM        row_idx, col_idx ;
        PinM        NextPin ;

  //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,            MyData)
    SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_MOD_ADDR_REG,   ModAddr) ;
    
    for (int i=0 ; i < ECBIT_BLOCK_NUM ; i++)
    {
        block_adr_w = i + ECBIT_BLOCK_ADDR_WBIT ;

      //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,            MyData)
      SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_BLOCK_ADDR_REG, block_adr_w) ;      
      SYS.WriteDUTBus (BusType, BusInstance,    ECBIT_BRD_CSR_REG, LOAD_ALL_CBITS_TO_RELAY_DRIVERS) ;
      TIME.Wait (12.0us) ;  

      //SYS.ReadDUTBus  (SYSBusTypeS, BusInstance, MyAddress,           MyData,     SimulatedData) ;
      SYS.ReadDUTBus  (BusType, BusInstance,        ECBIT_CHIP_0_RD_REG, reg_c_data, 0) ;
      SYS.ReadDUTBus  (BusType, BusInstance,        ECBIT_CHIP_1_RD_REG, reg_d_data, 0) ;
    
      cbits_r_driver_ary[i] = (reg_d_data << 8) + reg_c_data ; 
      //Mask out all unused (no-pullup) cbits
      cbits_r_driver_ary = cbits_r_driver_ary & cbits_all_mask_ary ;
      cbits_delta_ary    = cbits_r_driver_ary ^ cbits_w_ary ;      
    }

    cbits_ary_ms        = 0 ;
    cbits_shadow_ary    = 0 ;
    cbits_shadow_ary_ms = 0 ;

    PinsNum = CbitPinListShadow.GetNumPins() ;
    if (PinsNum != 0)
    {
        for (int i=0 ; i < PinsNum ; i++)
        {
            NextPin = CbitPinListShadow[i] ;
            PinChan = NextPin.GetResourceInstance() ; 
            if (PinChan != -1)  // if not a "null" type
            {
                PinChan_1   = PinChan - 1 ; 
                row_idx     = PinChan_1 / 16 ;
                col_idx     = PinChan_1 % 16 ; 
                for (site_idx=ActiveSites.Begin() ; !site_idx.End() ; ++site_idx)
                { 
                    this_site = site_idx.GetValue() ;
                    this_row  = row_idx  [this_site] ;
                    this_col  = col_idx  [this_site] ;
                    this_chan = PinChan_1[this_site] ;

                    this_pin = this_chan % ECBITS_MAX_by_sites_num ;   // Vertical   pin arrangement!
                    cbits_shadow_ary[this_row] |= (1 << this_col) ;                    
                    cbits_ary_ms[this_site][this_pin] = (cbits_r_driver_ary[this_row] >> this_col) & 0x1 ;                    
                    cbits_shadow_ary_ms[this_site][this_pin] = (cbits_shadow_ary[this_row] >> this_col) & 0x1 ;
                }
            }
        }
    }

    FailBin = "BIN_FAIL_ECBIT" ;
    ORFail  = false ;
    PinsNum = CbitPinList.GetNumPins() ;
    for (IntS i=0 ; i < PinsNum ; ++i)
    {
        NextPin = CbitPinList[i] ;
        PinChan = NextPin.GetResourceInstance() ;
        if (PinChan != -1)  // if not a "null" type
        {
            PinChan_1 = PinChan - 1 ;
            for (site_idx=ActiveSites.Begin() ; !site_idx.End() ; ++site_idx)
            { 
                this_site = site_idx.GetValue() ;
                this_chan = PinChan_1[this_site] ;

                this_pin = this_chan % ECBITS_MAX_by_sites_num ;   // Vertical   pin arrangement!
                cbit_state_ms[this_site] = cbits_ary_ms[this_site][this_pin] ;
                cbit_shadow_ms[this_site] = cbits_shadow_ary_ms[this_site][this_pin] ;
            }    
            TestName   = "ECBIT_" + CONV.IntToString (i+1) + "_STATE_ACTUAL" ;

          //Result     = DLOG.Value (Pin(s),  Value,         LowLimit,       HighLimit,      Units,    Comment,  FailBin, ORFail,   MinorID, ExpectedResult, DLOGEnable);
            fflag_l_ms = DLOG.Value (NextPin, cbit_state_ms, cbit_shadow_ms, cbit_shadow_ms, UTL_VOID, TestName, FailBin, UTL_VOID, (i+1),   ER_PASS,        true) ;
            fflag_ms   = DLOG.AccumulateResults (fflag_ms, fflag_l_ms) ;
        }
    }
    return (fflag_ms) ;
} 

//#########################################################
TMResultM   Ecbit::ReadExpected (PinML CbitPinList)
{       
        TMResultM   fflag_l_ms = TM_PASS ;
        TMResultM   fflag_ms   = TM_PASS ;
        BoolM       ORFail     = false ;
        StringS     FailBin    = "" ;
        StringS     TestName   = "" ;
        UnsignedS   reg_8_cbits, reg_9_cbits, reg_a_cbits, reg_b_cbits ;
        UnsignedS   block_adr_r ;
        UnsignedS1D cbits_shadow_ary    (ECBIT_BLOCK_NUM) ;
        UnsignedM1D cbits_shadow_ary_ms (ECBITS_MAX_by_sites_num) ;
        UnsignedS1D cbits_r_ary         (ECBIT_BLOCK_NUM) ;
        UnsignedS1D cbits_r_driver_ary  (ECBIT_BLOCK_NUM) ;
        UnsignedM1D cbits_ary_ms        (ECBITS_MAX_by_sites_num) ;
        UnsignedS1D cbits_delta_ary     (ECBIT_BLOCK_NUM) ;
        UnsignedM   cbit_state_ms ;
        UnsignedM   cbit_shadow_ms ;
        int         PinsNum, PinsNum_1 ;        
        IntM        PinChan, PinChan_1 ; 
        int         this_row, this_col, this_pin, this_chan ; 
        IntM        row_idx, col_idx ; 
        PinM        NextPin ;

//-------------------------------------------------------------
  //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,            MyData)
    SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_MOD_ADDR_REG,   ModAddr) ;
    
    for (int i=0 ; i < ECBIT_BLOCK_NUM ; i++)
    {
        block_adr_w = i + ECBIT_BLOCK_ADDR_WBIT ;

      //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,            MyData)
        SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_BLOCK_ADDR_REG, block_adr_w) ;      

      //SYS.ReadDUTBus  (SYSBusTypeS, BusInstance, MyAddress,            MyData,      SimulatedData) ;
        SYS.ReadDUTBus  (BusType, BusInstance,    ECBIT_CHIP_0_LN_REG,  reg_9_data,  0) ;
        SYS.ReadDUTBus  (BusType, BusInstance,    ECBIT_CHIP_0_HN_REG,  reg_8_data,  0) ;
        SYS.ReadDUTBus  (BusType, BusInstance,    ECBIT_CHIP_1_LN_REG,  reg_b_data,  0) ;
        SYS.ReadDUTBus  (BusType, BusInstance,    ECBIT_CHIP_1_HN_REG,  reg_a_data,  0) ;

        lut_idx     = reg_8_data ;
        reg_8_cbits = lut_r_ary[lut_idx] ;
        lut_idx     = reg_9_data ;
        reg_9_cbits = lut_r_ary[lut_idx] ;
        lut_idx     = reg_a_data ;
        reg_a_cbits = lut_r_ary[lut_idx] ;
        lut_idx     = reg_b_data ;
        reg_b_cbits = lut_r_ary[lut_idx] ;        

        cbits_r_ary[i]     = (reg_a_cbits << 12) | (reg_b_cbits << 8) | (reg_8_cbits << 4) | reg_9_cbits ;
//        cbits_delta_ary[i] = cbits_r_ary[i] ^ cbits_w_ary[i] ;  
    }

    cbits_ary_ms        = 0 ;
    cbits_shadow_ary    = 0 ;
    cbits_shadow_ary_ms = 0 ;

    PinsNum = CbitPinListShadow.GetNumPins() ;
    if (PinsNum != 0)
    {
        for (int i=0 ; i < PinsNum ; i++)
        {
            NextPin = CbitPinListShadow[i] ;
            PinChan = NextPin.GetResourceInstance() ;
 
            if (PinChan != -1)  // if not a "null" type
            {
                PinChan_1   = PinChan - 1 ; 
                row_idx     = PinChan_1 / 16 ;
                col_idx     = PinChan_1 % 16 ; 

                for (site_idx=ActiveSites.Begin() ; !site_idx.End() ; ++site_idx)
                { 
                    this_site = site_idx.GetValue() ;
                    this_row  = row_idx  [this_site] ;
                    this_col  = col_idx  [this_site] ;
                    this_chan = PinChan_1[this_site] ;

                    this_pin = this_chan % ECBITS_MAX_by_sites_num ;   // Vertical   pin arrangement!
                    cbits_shadow_ary[this_row] |= (1 << this_col) ;                   
                    cbits_ary_ms[this_site][this_pin] = (cbits_r_ary[this_row] >> this_col) & 0x1 ;                    
                    cbits_shadow_ary_ms[this_site][this_pin] = (cbits_shadow_ary[this_row] >> this_col) & 0x1 ;
                }
            }
        }
    }

    FailBin = "FAIL" ;
    ORFail  = false ;
    PinsNum = CbitPinList.GetNumPins() ;
    for (IntS i=0 ; i < PinsNum ; ++i)
    {
        NextPin = CbitPinList[i] ;
        PinChan = NextPin.GetResourceInstance() ;

        if (PinChan != -1)  // if not a "null" type
        {
            PinChan_1 = PinChan - 1 ;
            for (site_idx=ActiveSites.Begin() ; !site_idx.End() ; ++site_idx)
            { 
                this_site = site_idx.GetValue() ;
                this_chan = PinChan_1[this_site] ;

                this_pin = this_chan % ECBITS_MAX_by_sites_num ;   // Vertical   pin arrangement!
                cbit_state_ms[this_site] = cbits_ary_ms[this_site][this_pin] ;
                cbit_shadow_ms[this_site] = cbits_shadow_ary_ms[this_site][this_pin] ;
            }
    
            TestName   = "ECBIT_" + CONV.IntToString (i+1) + "_STATE_EXPECTED" ;

          //Result     = DLOG.Value (Pin(s),  Value,         LowLimit,       HighLimit,      Units,    Comment,  FailBin, ORFail,   MinorID, ExpectedResult, DLOGEnable);
            fflag_l_ms = DLOG.Value (NextPin, cbit_state_ms, cbit_shadow_ms, cbit_shadow_ms, UTL_VOID, TestName, FailBin, UTL_VOID, (i+1),   ER_PASS,        true) ;
            fflag_ms   = DLOG.AccumulateResults (fflag_ms, fflag_l_ms) ;
        }
    }
    return (fflag_ms) ;
}

//#########################################################
TMResultM   Ecbit::ReadCompare (PinML CbitPinList)
{
        TMResultM   fflag_l_ms = TM_PASS ;
        TMResultM   fflag_ms   = TM_PASS ;
        BoolM       ORFail     = false ;
        StringS     FailBin    = "" ;
        StringS     TestName_actual   = "" ;
        StringS     TestName_expected   = "" ;
        UnsignedS   reg_8_cbits, reg_9_cbits, reg_a_cbits, reg_b_cbits ;
        UnsignedS   block_adr_r, brd_csr_r, blk_csr_r ;
        UnsignedS1D cbits_shadow_ary    (ECBIT_BLOCK_NUM) ;
        UnsignedM1D cbits_shadow_ary_ms (ECBITS_MAX_by_sites_num) ;
        UnsignedS1D cbits_r_ary         (ECBIT_BLOCK_NUM) ;
        UnsignedS1D cbits_r_driver_ary  (ECBIT_BLOCK_NUM) ;
        UnsignedM1D cbits_ary_actual_ms (ECBITS_MAX_by_sites_num) ;
        UnsignedM1D cbits_ary_expected_ms (ECBITS_MAX_by_sites_num) ;
        UnsignedS1D cbits_delta_ary     (ECBIT_BLOCK_NUM) ;
        UnsignedM   cbit_state_actual_ms ;
        UnsignedM   cbit_state_expected_ms ;
        UnsignedM   cbit_shadow_ms ;
        int         PinsNum, PinsNum_1 ;        
        IntM        PinChan, PinChan_1 ; 
        int         this_row, this_col, this_pin, this_chan ; 
        IntM        row_idx, col_idx ; 
        PinM        NextPin ;

  //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,            MyData)
    SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_MOD_ADDR_REG,   ModAddr) ;    
    for (int i=0 ; i < ECBIT_BLOCK_NUM ; i++)
    {
        block_adr_w = i + ECBIT_BLOCK_ADDR_WBIT ;
      //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,            MyData)
        SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_BLOCK_ADDR_REG, block_adr_w) ;      

      //SYS.ReadDUTBus  (SYSBusTypeS, BusInstance, MyAddress,            MyData,      SimulatedData) ;
            SYS.ReadDUTBus  (BusType, BusInstance,    ECBIT_CHIP_0_LN_REG,  reg_9_data,  0) ;
            SYS.ReadDUTBus  (BusType, BusInstance,    ECBIT_CHIP_0_HN_REG,  reg_8_data,  0) ;
            SYS.ReadDUTBus  (BusType, BusInstance,    ECBIT_CHIP_1_LN_REG,  reg_b_data,  0) ;
            SYS.ReadDUTBus  (BusType, BusInstance,    ECBIT_CHIP_1_HN_REG,  reg_a_data,  0) ;

            lut_idx     = reg_8_data ;
            reg_8_cbits = lut_r_ary[lut_idx] ;
            lut_idx     = reg_9_data ;
            reg_9_cbits = lut_r_ary[lut_idx] ;
            lut_idx     = reg_a_data ;
            reg_a_cbits = lut_r_ary[lut_idx] ;
            lut_idx     = reg_b_data ;
            reg_b_cbits = lut_r_ary[lut_idx] ;        

            cbits_r_ary[i]     = (reg_a_cbits << 12) | (reg_b_cbits << 8) | (reg_8_cbits << 4) | reg_9_cbits ;
//            cbits_delta_ary[i] = cbits_r_ary[i] ^ cbits_w_ary[i] ;  

          //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,         MyData)
            SYS.WriteDUTBus (BusType, BusInstance, ECBIT_BRD_CSR_REG, LOAD_ALL_CBITS_TO_RELAY_DRIVERS) ;
            TIME.Wait (12.0us) ;    // !

          //SYS.ReadDUTBus  (SYSBusTypeS, BusInstance, MyAddress,           MyData,     SimulatedData) ;
            SYS.ReadDUTBus  (BusType, BusInstance, ECBIT_CHIP_0_RD_REG, reg_c_data, 0) ;
            SYS.ReadDUTBus  (BusType, BusInstance, ECBIT_CHIP_1_RD_REG, reg_d_data, 0) ;
    
            cbits_r_driver_ary[i] = (reg_d_data << 8) + reg_c_data ; 
//            cbits_r_driver_ary = cbits_r_driver_ary & cbits_all_mask_ary ;
//            cbits_delta_ary    = cbits_r_driver_ary ^ cbits_w_ary ;
            
            if (cbits_r_ary[i] != cbits_r_driver_ary[i])
            {
                cout << "Mismatch between the Actual and Expected Ecbits in address block #" << i << endl ;
                SYS.ReadDUTBus  (BusType, BusInstance, ECBIT_BRD_CSR_REG, brd_csr_r, 0) ;
                SYS.ReadDUTBus  (BusType, BusInstance, ECBIT_BLOCK_CSR_REG, blk_csr_r, 0) ;
                if ((brd_csr_r & 0x80) == 0x80)
                    cout << "The Global Fault Flag was detected" << endl ;
                if ((blk_csr_r & 0x10) == 0x00)
                    cout << "Chip driver in block " << i << " has a malfunction " << endl ;
            }                                               
    }

    cbits_ary_actual_ms = 0 ;
    cbits_ary_expected_ms = 0 ;
    cbits_shadow_ary    = 0 ;
    cbits_shadow_ary_ms = 0 ;

    PinsNum = CbitPinListShadow.GetNumPins() ;
    if (PinsNum != 0)
    {
        for (int i=0 ; i < PinsNum ; i++)
        {
            NextPin = CbitPinListShadow[i] ;
            PinChan = NextPin.GetResourceInstance() ;
 
            if (PinChan != -1)  // if not a "null" type
            {
                PinChan_1   = PinChan - 1 ; 
                row_idx     = PinChan_1 / 16 ;
                col_idx     = PinChan_1 % 16 ; 

                for (site_idx=ActiveSites.Begin() ; !site_idx.End() ; ++site_idx)
                { 
                    this_site = site_idx.GetValue() ;
                    this_row  = row_idx  [this_site] ;
                    this_col  = col_idx  [this_site] ;
                    this_chan = PinChan_1[this_site] ;

                    this_pin = this_chan % ECBITS_MAX_by_sites_num ;   // Vertical   pin arrangement!
                    cbits_shadow_ary[this_row] |= (1 << this_col) ; 
                   
                    cbits_ary_actual_ms[this_site][this_pin] = (cbits_r_driver_ary[this_row] >> this_col) & 0x1 ;
                    cbits_ary_expected_ms[this_site][this_pin] = (cbits_r_ary[this_row] >> this_col) & 0x1 ;                   
                    cbits_shadow_ary_ms[this_site][this_pin] = (cbits_shadow_ary[this_row] >> this_col) & 0x1 ;
                }
            }
        }
    }

    FailBin = "FAIL" ;
    ORFail  = false ;

    PinsNum = CbitPinList.GetNumPins() ;

    for (IntS i=0 ; i < PinsNum ; ++i)
    {
        NextPin = CbitPinList[i] ;
        PinChan = NextPin.GetResourceInstance() ;

        if (PinChan != -1)  // if not a "null" type
        {
            PinChan_1 = PinChan - 1 ;

            for (site_idx=ActiveSites.Begin() ; !site_idx.End() ; ++site_idx)
            { 
                this_site = site_idx.GetValue() ;
                this_chan = PinChan_1[this_site] ;
                this_pin = this_chan % ECBITS_MAX_by_sites_num ;   // Vertical   pin arrangement!

                cbit_state_actual_ms[this_site] = cbits_ary_actual_ms[this_site][this_pin] ;
                cbit_state_expected_ms[this_site] = cbits_ary_expected_ms[this_site][this_pin] ;
                cbit_shadow_ms[this_site] = cbits_shadow_ary_ms[this_site][this_pin] ;
            }
    
            TestName_actual   = "ECBIT_" + CONV.IntToString (i+1) + "_STATE_COMPARE_ACT" ;
            TestName_expected   = "ECBIT_" + CONV.IntToString (i+1) + "_STATE_COMPARE_EXP" ;

          //Result     = DLOG.Value (Pin(s),  Value,         LowLimit,       HighLimit,      Units,    Comment,  FailBin, ORFail,   MinorID, ExpectedResult, DLOGEnable);
            fflag_l_ms = DLOG.Value (NextPin, cbit_state_actual_ms, cbit_shadow_ms, cbit_shadow_ms, UTL_VOID, TestName_actual, FailBin, UTL_VOID, (i+1),   ER_PASS,        true) ;
            fflag_l_ms = DLOG.Value (NextPin, cbit_state_expected_ms, cbit_shadow_ms, cbit_shadow_ms, UTL_VOID, TestName_expected, FailBin, UTL_VOID, (i+1),   ER_PASS,        true) ;
            fflag_ms   = DLOG.AccumulateResults (fflag_ms, fflag_l_ms) ;
        }
    }
    return (fflag_ms) ;
}       

//#########################################################
void    Ecbit::PdcdEnabled (PinML CbitPinList)
{
    int         PinsNum ;
    IntM        PinChan, PinChan_1 ;
    int         this_row, this_col ;
    IntM        row_idx,  col_idx ;
    PinM        NextPin ;
    cbits_w_ary       = 0 ;
    PinsNum           = CbitPinList.GetNumPins() ;
    CbitPinListShadow = "" ;

  //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,            MyData)
    SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_MOD_ADDR_REG,   ModAddr) ;    

    if (PinsNum != 0)
    {
        for (int i=0 ; i < PinsNum ; i++)
        {
            NextPin = CbitPinList[i] ;
            PinChan = NextPin.GetResourceInstance() ; 
            if (PinChan != -1)  // if not a "null" type
            {            
                PinChan_1 = PinChan - 1 ; 
                row_idx   = PinChan_1 / 16 ;
                col_idx   = PinChan_1 % 16 ;           
                for (site_idx=ActiveSites.Begin() ; !site_idx.End() ; ++site_idx)
                { 
                    this_site = site_idx.GetValue() ;
                    this_row  = row_idx[this_site] ;
                    this_col  = col_idx[this_site] ;
                    cbits_w_ary[this_row] |= (1 << this_col) ;
                }
            }
        }
      //Parse bitmap into 4 8-bit registers per block.
        for (int i=0 ; i < ECBIT_BLOCK_NUM ; i++)
        {
            block_adr_w  = i + ECBIT_BLOCK_ADDR_WBIT ;
            if ((cbits_w_ary[i] & 0x00FF) > 0)  
            {                      
          //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,            MyData)
            SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_BLOCK_ADDR_REG, block_adr_w) ;
            SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_BLOCK_CSR_REG, ENABLE_PDCD_CHIP_0) ;
            }
            if ((cbits_w_ary[i] & 0xFF00) > 0)
            {
          //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,            MyData)
            SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_BLOCK_ADDR_REG, block_adr_w) ;
            SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_BLOCK_CSR_REG, ENABLE_PDCD_CHIP_1) ;            
            }
        }
        CbitPinListShadow = CbitPinList ;
    }
}

//#########################################################
void    Ecbit::PdcdDisabled (PinML CbitPinList)
{
    int         PinsNum ;
    IntM        PinChan, PinChan_1 ;
    int         this_row, this_col ;
    IntM        row_idx,  col_idx ;
    PinM        NextPin ;
    cbits_w_ary       = 0 ;
    PinsNum           = CbitPinList.GetNumPins() ;
    CbitPinListShadow = "" ;

  //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,            MyData)
    SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_MOD_ADDR_REG,   ModAddr) ;    

    if (PinsNum != 0)
    {
        for (int i=0 ; i < PinsNum ; i++)
        {
            NextPin = CbitPinList[i] ;
            PinChan = NextPin.GetResourceInstance() ; 
            if (PinChan != -1)  // if not a "null" type
            {            
                PinChan_1 = PinChan - 1 ; 
                row_idx   = PinChan_1 / 16 ;
                col_idx   = PinChan_1 % 16 ;           
                for (site_idx=ActiveSites.Begin() ; !site_idx.End() ; ++site_idx)
                { 
                    this_site = site_idx.GetValue() ;
                    this_row  = row_idx[this_site] ;
                    this_col  = col_idx[this_site] ;
                    cbits_w_ary[this_row] |= (1 << this_col) ;
                }
            }
        }
      //Parse bitmap into 4 8-bit registers per block.
        for (int i=0 ; i < ECBIT_BLOCK_NUM ; i++)
        {
            block_adr_w  = i + ECBIT_BLOCK_ADDR_WBIT ;
            if ((cbits_w_ary[i] & 0x00FF) > 0)  
            {                      
          //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,            MyData)
            SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_BLOCK_ADDR_REG, block_adr_w) ;
            SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_BLOCK_CSR_REG, DISABLE_PDCD_CHIP_0) ;
            }
            if ((cbits_w_ary[i] & 0xFF00) > 0)
            {
          //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,            MyData)
            SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_BLOCK_ADDR_REG, block_adr_w) ;
            SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_BLOCK_CSR_REG, DISABLE_PDCD_CHIP_1) ;            
            }
        }
        CbitPinListShadow = CbitPinList ;
    }
}  

//#########################################################
void    Ecbit::SpldEnabled ()
{
  //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,          MyData)
    SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_MOD_ADDR_REG, ModAddr) ;
    SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_BRD_CSR_REG,  ENABLE_SPLD) ;
}

//#########################################################
void    Ecbit::SpldDisabled ()
{
  //SYS.WriteDUTBus (SYSBusTypeS, BusInstance, MyAddress,          MyData)
    SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_MOD_ADDR_REG, ModAddr) ;
    SYS.WriteDUTBus (BusType, BusInstance,        ECBIT_BRD_CSR_REG,  DISABLE_SPLD) ;
}

//#########################################################
void Ecbit::Test (PinML CbitPinList)
{
    int     PinsNum ;
    IntM    PinChan ;
    PinM    NextPin ;
    FloatS  DISPLAY_WAIT = 0.002;
//-------------------------------------------------------
    PinsNum = CbitPinList.GetNumPins() ;
    for (int i=0 ; i < PinsNum ; i++)
    {
        NextPin = CbitPinList[i] ;
        PinChan = NextPin.GetResourceInstance() ;
        if (PinChan != -1)  // if not a "null" type
        {
            Set (NextPin) ;    
            TIME.Wait (DISPLAY_WAIT) ;
        }
    }    
    Reset () ;

//---------------------------------------------------------
    for (int i=0 ; i < PinsNum ; i++)
    {
        NextPin = CbitPinList[i] ;
        PinChan = NextPin.GetResourceInstance() ;
        if (PinChan != -1)  // if not a "null" type
        {
            Close (NextPin) ;   
            TIME.Wait (DISPLAY_WAIT) ;
        }
    }
    
//---------------------------------------------------------
    for (int i=0 ; i < PinsNum ; i++)
    {
        NextPin = CbitPinList[i] ;
        PinChan = NextPin.GetResourceInstance() ;
        if (PinChan != -1)  // if not a "null" type
        {
            Open (NextPin) ;    
            TIME.Wait (DISPLAY_WAIT) ;
        }
    } 
    Reset () ;
}
//#########################################################
BoolS1D Ecbit::Get (PinML pins)
{
    BoolS1D state(pins.GetNumPins(), false);
    RWBoolS1DIter state_iter = state.Begin();
    for (PinMLIter pin_iter = pins.Begin(); !pin_iter.End(); ++pin_iter, ++state_iter)
    {
        if (this->CbitPinListShadow.Find(*pin_iter) != -1) *state_iter = true;
    }
    return state;
}
//#########################################################
