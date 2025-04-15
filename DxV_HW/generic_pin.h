#pragma once
#include <Unison.h>
#include <CoreFamily.h>
#include <array>

namespace REN{
namespace DxV_HW{

class ConfigureGenericPins;
class GenericPin;

enum class DXV_CONNECT_MODE{
    OPEN,
    GROUND,
    VI_100_KELVIN,
    VI_100_SENSE_ONLY,
    GX1_DIRECT
};

enum class BYPASS_CONNECT_MODE{
    OPEN,
    CONNECT
};

enum class DXV_BULK_MODE{
    OPEN_DISCHARGE,
    CONNECT_NO_DISCHARGE,
    CONNECT_DISCHARGE,
    OPEN_NO_DISCHARGE
};

enum class DXV_TMU_MODE{
    OPEN,
    CONNECT
};

enum class DXV_SHORT_MODE{
    OPEN,
    SHORT_A,
    SHORT_B,
    SHORT_A_AND_B
};




struct ShortBus{
    PinML connected_instruments; 
    
};

enum class GP_AVAILIBLE : size_t{
    INVALID = 0,
    P01 = 1,
    P02 = 2,
    P03 = 3,
    P04 = 4,
    P05 = 5,
    P06 = 6,
    P07 = 7,
    P08 = 8,
    P09 = 9,
    P10 = 10,
    P11 = 11,
    P12 = 12,
    P13 = 13,
    P14 = 14,
    P15 = 15,
    P16 = 16,
    P17 = 17,
    P18 = 18,
    P19 = 19,
    P20 = 20,
    P21 = 21,
    P22 = 22,
    P23 = 23,
    P24 = 24,
    P25 = 25,
    P26 = 26,
    P27 = 27,
    P28 = 28,
    P29 = 29,
    P30 = 30,
    P31 = 31,
    P32 = 32
};
StringS print_gp(GP_AVAILIBLE pin_num);

class GenericPin{
public:

    GenericPin(const GP_AVAILIBLE gp);

    PinM vi100()        const {return PinM("zz_" + print_gp(assignment) + "_VI100_A");}
    PinM gx1_direct()   const {return PinM("zz_" + print_gp(assignment) + "_gx1");}
    PinM gx1_tmu_pos()  const{return PinM("zz_" + print_gp(assignment) + "_gx1_tmup");}
    PinM gx1_tmu_neg()  const{return PinM("zz_" + print_gp(assignment) + "_gx1_tmun");}
    PinM gx1_tmu_ref()  const{return PinM("zz_" + print_gp(assignment) + "_gx1_ref");}
    
    //Add the getters for the ebit connection
    PinM xe_force_isolate()     const {return PinM("zz_" + print_gp(assignment) + "_xe_force_connect");}
    PinM xe_sense_isolate()     const {return PinM("zz_" + print_gp(assignment) + "_xe_sense_connect");} 
    PinM xe_ground_force()      const {return PinM("zz_" + print_gp(assignment) + "_xe_ground_f");} 
    PinM xe_gx1_direct()        const {return PinM("zz_" + print_gp(assignment) + "_xe_gx1_connect");} 
    PinM xe_tmu_connect()       const {return PinM("zz_" + print_gp(assignment) + "_xe_tmu_connect");} 
    PinM xe_short_A()           const {return PinM("zz_" + print_gp(assignment) + "_xe_short_bus_a");} 
    PinM xe_short_B()           const {return PinM("zz_" + print_gp(assignment) + "_xe_short_bus_b");} 
    PinM xe_bulk_connect()      const {return PinM("zz_" + print_gp(assignment) + "_xe_bulk_connect");} 
    PinM xe_bulk_discharge()    const {return PinM("zz_" + print_gp(assignment) + "_xe_bulk_discharge");} 
    
    PinM xe_bypass()    const {return PinM("ww_bypass_p" + print_gp(assignment));} 
    
    DXV_CONNECT_MODE get_status_connect() const; 
    DXV_BULK_MODE get_status_bulk() const; 
    DXV_TMU_MODE get_status_tmu() const; 
    DXV_SHORT_MODE get_status_short() const; 
    
    GenericPin(const GenericPin&) = delete;
    GenericPin& operator=(const GenericPin&) = delete;

protected:

    const GP_AVAILIBLE assignment; 
    
};

GenericPin& GetGenericPin(const PinM& alias_pin);


/** @file
    @brief Defines @ref ConfigureGenericPins "ConfigureGenericPins" Test Method */
/** @brief %Sets up the generic DxV pins controlled by the ECBITs. Everything done in one block is done in a single EBIT burst. */
class s_ConfigureGenericPins : public LTXC::CoreFamily
{
 
 public:
    s_ConfigureGenericPins();
    virtual ~s_ConfigureGenericPins() { }
    
    TM_RETURN        Execute();
    BoolM            ParamCheck();
    BoolM            Initialize();
 

protected:
    StringS DisplayHardwareDoc;

    // locally defined parameter variables
    ArrayOfPinML DUTPins;
    
    ArrayOfEnumS<DXV_CONNECT_MODE>  connect;
    ArrayOfEnumS<DXV_BULK_MODE>     bulk_mode;
    ArrayOfEnumS<DXV_TMU_MODE>      tmu_mode;
    ArrayOfEnumS<DXV_SHORT_MODE>    short_mode;
    
    //only for LJ16
    ArrayOfEnumS<BYPASS_CONNECT_MODE>    bypass_mode;
    
private:
    
};


}
}


