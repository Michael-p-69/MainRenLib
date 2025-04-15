#include "generic_pin.h"
#include "ecbit.h"


namespace REN{
namespace DxV_HW{

TMResultM configure_ecbits(const UnsignedS Address){

    SYS.EnableDUTPower("zz_SDU_5V");

    ECBIT_BUS0.SetModAddr(Address); 
    ECBIT_BUS0.Reset();
    TIME.Wait(5ms); 
    
    const auto rev = ECBIT_BUS0.GetRev();
    std::cout << "ECBIT REV: " << rev << std::endl; 

    return TM_PASS;
}

}
}
