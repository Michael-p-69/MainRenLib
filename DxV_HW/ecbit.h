/*---------------------------------------------------

    Rev. 1  18nov11 Mark Saj.
    Rev. 2  24jan12 Mark Saj.   ECBIT Rev. 7 ; Using SYS_BUS_PARALLEL
    Rev. 3  25apr12 Mark Saj    ECBIT Rev. 8 ; Support for DMDx Parallel and I2C Bus.
    Rev. 4  22may12 Mark Saj    ECBIT Rev. 9 ; ECBIT_MOD_ADDR_REG = 3 ; Support for DMDx Parallel and I2C Bus.
    
---------------------------------------------------*/

#include <Unison.h>

#ifndef ECBIT_H
#define ECBIT_H

//###################################################

class Ecbit
{
public:

        Ecbit (unsigned, UnsignedS) ;   // Constructor
        void        SetModAddr (UnsignedS);
        void        Reset  () ;
        UnsignedS   GetRev () ;
        void        Close  (PinML);
        void        Open   (PinML) ;
        void        CloseDefer (PinML);
        void        OpenDefer   (PinML) ;
        void        ApplyDefer(); 
        void        CloseAndOpen  (const PinML& Close, const PinML& Open) ;
        void        Set    (PinML) ;
        void        Set    (PinML, PinML) ;
        TMResultM   Read   (PinML) ;
        TMResultM   ReadActual   (PinML) ;
        TMResultM   ReadExpected (PinML) ;
        TMResultM   ReadCompare  (PinML) ;
        void        PdcdEnabled  (PinML) ;
        void        PdcdDisabled (PinML) ;
        void        SpldEnabled  () ;
        void        SpldDisabled () ;
        void        Test   (PinML) ;
        BoolS1D     Get    (PinML pins);
  
private:   

        unsigned    BusInstance ;    
        unsigned    lut_idx ;
        unsigned    block_adr_w ;
        UnsignedS   ModAddr ;
        UnsignedS   reg_8_data ;
        UnsignedS   reg_9_data ;
        UnsignedS   reg_a_data ;
        UnsignedS   reg_b_data ;
        UnsignedS   reg_c_data ;
        UnsignedS   reg_d_data ;
        UnsignedS   ecbit_rev ;
        UnsignedS1D cbits_w_ary ;
        UnsignedS1D cbits_w_ary_mask ;
const   UnsignedS1D lut_w_ary ;        
const   UnsignedS1D lut_r_ary ;
const   UnsignedS1D cbits_all_mask_ary ;
        PinML       CbitPinListShadow ;
        PinML       CbitPinListDefer;
        SYSBusTypeS BusType;
        PinM        SDUI2CPin ;
        SYSBusConfigInfoStruct  DmdxI2CConfig ;

} ;

extern  Ecbit ECBIT_BUS0 ;     // BusInstance 0
//extern  Ecbit ECBIT_BUS1 ;     // BusInstance 1       

//#####################################################

#endif
