#pragma once
#include <Unison.h>
#include <TMFamily.h>
#include "p_leak_vi_100.h"
#include "p_conti_vi_100.h"
#include "p_rpx.h"
#include "p_comparator_ramp.h"
#include "generic_pin.h"

namespace REN{
namespace DxV_HW{

class DxV_Leak_VI100 : public REN::GeneralTests::leakVI100
{
    virtual PinM pin_redirect(const PinM& alias) const; 

};

class DxV_Conti_VI100 : public REN::GeneralTests::contiVI100
{
    virtual PinM pin_redirect(const PinM& alias) const; 

};

class DxV_p_rpx : public REN::GeneralTests::p_rpx
{
    virtual PinM pin_redirect(const PinM& alias) const; 

};

class DxV_p_compRamp : public REN::GeneralTests::compRampVi100
{
    virtual PinM pin_redirect_analog(const PinM& alias) const;  
    virtual PinM pin_redirect_digital(const PinM& alias) const; 
        
    virtual void set_vih(const PinM& pin, const FloatS& vih); 
    virtual void set_vih(const PinML& pin, const FloatS1D& vih); 

};


}
}
