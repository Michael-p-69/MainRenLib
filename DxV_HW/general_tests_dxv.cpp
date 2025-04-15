#include "general_tests_dxv.h"

namespace REN{
namespace DxV_HW{


PinM DxV_Leak_VI100::pin_redirect(const PinM& alias) const{
    return GetGenericPin(alias).vi100(); 
}

PinM DxV_Conti_VI100::pin_redirect(const PinM& alias) const{
    return GetGenericPin(alias).vi100(); 
}

PinM DxV_p_rpx::pin_redirect(const PinM& alias) const{
    return GetGenericPin(alias).gx1_direct(); 
}


PinM DxV_p_compRamp::pin_redirect_analog(const PinM& alias) const{
    return GetGenericPin(alias).vi100(); 
}
PinM DxV_p_compRamp::pin_redirect_digital(const PinM& alias) const{

    //if( GetGenericPin(alias).get_status_tmu() == DXV_TMU_MODE::CONNECT){
    return GetGenericPin(alias).gx1_tmu_pos(); 
    //}
    //else if ( GetGenericPin(alias).get_status_connect() == DXV_CONNECT_MODE::GX1_DIRECT){
        return GetGenericPin(alias).gx1_direct(); 
    //}
    //return "ERROR_NO_DIGITAL_CONNECTED";
}

void DxV_p_compRamp::set_vih(const PinM& pin, const FloatS& vih){
   
    if( GetGenericPin(pin).get_status_tmu() == DXV_TMU_MODE::CONNECT){
        auto PPMU = GetGenericPin(pin).gx1_tmu_ref(); 
        
        VI.SetMeasureSamples(PPMU,32); 
        VI.ForceV(PPMU,vih); 
        TIME.Wait(10us); 
        
        FloatM measure;
        VI.MeasureVAverage(PPMU,measure,vih); 
        
        const auto delta = vih-measure;
        
        VI.ForceV(PPMU,vih + delta); 
    }
    else{   
        DIGITAL.SetVih(GetGenericPin(pin).gx1_direct(),vih); 
    }
}
void DxV_p_compRamp::set_vih(const PinML& pin, const FloatS1D& vih){
    //TODO: check size equal and bark if not
    PinML direct_pins = ""; 
    FloatS1D direct_vih(pin.GetNumPins());
    PinML atten_pins = "";
    FloatS1D atten_vih(pin.GetNumPins()); 
    for(size_t pin_idx = 0, vih_direct_idx = 0, vih_atten_idx =0; pin_idx < pin.GetNumPins(); ++pin_idx){
        //if( GetGenericPin(pin[pin_idx]).get_status_tmu() == DXV_TMU_MODE::CONNECT){
            atten_pins +=    pin[pin_idx];
            atten_vih[vih_atten_idx++] = vih[pin_idx]; 
        //}
        //else{
            //direct_pins +=    GetGenericPin(pin[pin_idx]).gx1_direct();
            //direct_vih[vih_direct_idx++] = vih[pin_idx]; 
        //}
    }
    direct_vih.Resize(direct_pins.GetNumPins()); 
    atten_vih.Resize(atten_pins.GetNumPins()); 
 
//    if(  direct_pins.GetNumPins() > 0){
//        DIGITAL.SetVih(direct_pins,direct_vih); 
//    }
    if( atten_pins.GetNumPins() > 0){
        VI.SetMeasureSamples(atten_pins,32); 
        VI.ForceV(atten_pins,atten_vih); 
        TIME.Wait(10us); 
        
        FloatM1D measure;
        VI.MeasureVAverage(atten_pins,measure,atten_vih); 
        
        const auto delta = atten_vih-measure;
        
        VI.ForceV(atten_pins,vih + delta); 
    }
    
    
} 


}
}


//TMFAMILY_CLASS(REN::DxV_HW::DxV_Leak_VI100)
//TMFAMILY_CLASS(REN::DxV_HW::DxV_Conti_VI100)
//TMFAMILY_CLASS(REN::DxV_HW::DxV_p_rpx)
//TMFAMILY_CLASS(REN::DxV_HW::DxV_p_compRamp)
