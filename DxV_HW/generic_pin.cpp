#include "generic_pin.h"
#include "ecbit.h"


namespace REN{
namespace DxV_HW{

GenericPin& GetGenericPin(const PinM& alias_pin){

    static std::array<GenericPin,33> GenericPins= {
        GenericPin(GP_AVAILIBLE::INVALID),
        GenericPin(GP_AVAILIBLE::P01),
        GenericPin(GP_AVAILIBLE::P02),
        GenericPin(GP_AVAILIBLE::P03),
        GenericPin(GP_AVAILIBLE::P04),
        GenericPin(GP_AVAILIBLE::P05),
        GenericPin(GP_AVAILIBLE::P06),
        GenericPin(GP_AVAILIBLE::P07),
        GenericPin(GP_AVAILIBLE::P08),
        GenericPin(GP_AVAILIBLE::P09),
        GenericPin(GP_AVAILIBLE::P10),
        GenericPin(GP_AVAILIBLE::P11),
        GenericPin(GP_AVAILIBLE::P12),
        GenericPin(GP_AVAILIBLE::P13),
        GenericPin(GP_AVAILIBLE::P14),
        GenericPin(GP_AVAILIBLE::P15),
        GenericPin(GP_AVAILIBLE::P16),
        GenericPin(GP_AVAILIBLE::P17),
        GenericPin(GP_AVAILIBLE::P18),
        GenericPin(GP_AVAILIBLE::P19),
        GenericPin(GP_AVAILIBLE::P20),
        GenericPin(GP_AVAILIBLE::P21),
        GenericPin(GP_AVAILIBLE::P22),
        GenericPin(GP_AVAILIBLE::P23),
        GenericPin(GP_AVAILIBLE::P24),
        GenericPin(GP_AVAILIBLE::P25),
        GenericPin(GP_AVAILIBLE::P26),
        GenericPin(GP_AVAILIBLE::P27),
        GenericPin(GP_AVAILIBLE::P28),
        GenericPin(GP_AVAILIBLE::P29),
        GenericPin(GP_AVAILIBLE::P30),
        GenericPin(GP_AVAILIBLE::P31),
        GenericPin(GP_AVAILIBLE::P32)
        
    };
    
    Adapterboard thisAdapter = TestProg.GetActiveAdapterboard();
    
    const AdapterPinInfo pin_info = thisAdapter.GetPinInfo(alias_pin); 
    const auto PPID = pin_info.PPId; 
    
    if(PPID == "P01") return GenericPins[1]; 
    if(PPID == "P02") return GenericPins[2]; 
    if(PPID == "P03") return GenericPins[3]; 
    if(PPID == "P04") return GenericPins[4]; 
    if(PPID == "P05") return GenericPins[5]; 
    if(PPID == "P06") return GenericPins[6]; 
    if(PPID == "P07") return GenericPins[7]; 
    if(PPID == "P08") return GenericPins[8]; 
    if(PPID == "P09") return GenericPins[9]; 
    if(PPID == "P10") return GenericPins[10]; 
    if(PPID == "P11") return GenericPins[11]; 
    if(PPID == "P12") return GenericPins[12]; 
    if(PPID == "P13") return GenericPins[13]; 
    if(PPID == "P14") return GenericPins[14]; 
    if(PPID == "P15") return GenericPins[15]; 
    if(PPID == "P16") return GenericPins[16]; 
    if(PPID == "P17") return GenericPins[17]; 
    if(PPID == "P18") return GenericPins[18]; 
    if(PPID == "P19") return GenericPins[19]; 
    if(PPID == "P20") return GenericPins[20]; 
    if(PPID == "P21") return GenericPins[21]; 
    if(PPID == "P22") return GenericPins[22]; 
    if(PPID == "P23") return GenericPins[23]; 
    if(PPID == "P24") return GenericPins[24]; 
    if(PPID == "P25") return GenericPins[25]; 
    if(PPID == "P26") return GenericPins[26]; 
    if(PPID == "P27") return GenericPins[27]; 
    if(PPID == "P28") return GenericPins[28]; 
    if(PPID == "P29") return GenericPins[29]; 
    if(PPID == "P30") return GenericPins[30]; 
    if(PPID == "P31") return GenericPins[31]; 
    if(PPID == "P32") return GenericPins[32]; 
    
    //THROW ERROR
    return GenericPins[0];
}

GenericPin::GenericPin(const GP_AVAILIBLE gp):
    assignment(gp)
{}

TMFAMILY_CLASS(REN::DxV_HW::s_ConfigureGenericPins)

DXV_CONNECT_MODE GenericPin::get_status_connect() const{

    const auto gx1_connected    = ECBIT_BUS0.Get(this->xe_gx1_direct()); 
    const auto force_connected  = ECBIT_BUS0.Get(this->xe_force_isolate()); 
    const auto sense_connected  = ECBIT_BUS0.Get(this->xe_sense_isolate());
    const auto groudned         = ECBIT_BUS0.Get(this->xe_ground_force());
    
    if(groudned[0])        return REN::DxV_HW::DXV_CONNECT_MODE::GROUND; 
    if(force_connected[0]) return REN::DxV_HW::DXV_CONNECT_MODE::VI_100_KELVIN; 
    if(gx1_connected[0])   return REN::DxV_HW::DXV_CONNECT_MODE::GX1_DIRECT; 
    if(sense_connected[0]) return REN::DxV_HW::DXV_CONNECT_MODE::VI_100_SENSE_ONLY; 

    return REN::DxV_HW::DXV_CONNECT_MODE::OPEN;
}

DXV_BULK_MODE GenericPin::get_status_bulk() const{

    const auto bulk_connect    = ECBIT_BUS0.Get(this->xe_bulk_connect()); 
    const auto bulk_discharge  = ECBIT_BUS0.Get(this->xe_bulk_discharge()); 
    
    if(bulk_connect[0] && bulk_discharge[0] )   return REN::DxV_HW::DXV_BULK_MODE::CONNECT_DISCHARGE; 
    if(bulk_connect[0]                      )   return REN::DxV_HW::DXV_BULK_MODE::CONNECT_NO_DISCHARGE; 
    if(bulk_discharge[0]                    )   return REN::DxV_HW::DXV_BULK_MODE::OPEN_DISCHARGE; 

    return REN::DxV_HW::DXV_BULK_MODE::OPEN_NO_DISCHARGE;
}


DXV_TMU_MODE        GenericPin::get_status_tmu() const{
    
    const auto tmu_connected  = ECBIT_BUS0.Get(this->xe_tmu_connect()); 
    
    if(tmu_connected[0])  return REN::DxV_HW::DXV_TMU_MODE::CONNECT; 
    return REN::DxV_HW::DXV_TMU_MODE::OPEN; 

}
    
DXV_SHORT_MODE GenericPin::get_status_short() const{
    const auto short_a  = ECBIT_BUS0.Get(this->xe_short_A()); 
    const auto short_b  = ECBIT_BUS0.Get(this->xe_short_B()); 
    
    if(short_a[0] && short_b[0] )   return REN::DxV_HW::DXV_SHORT_MODE::SHORT_A_AND_B; 
    if(short_a[0]               )   return REN::DxV_HW::DXV_SHORT_MODE::SHORT_A; 
    if(short_b[0]               )   return REN::DxV_HW::DXV_SHORT_MODE::SHORT_B; 

    return REN::DxV_HW::DXV_SHORT_MODE::OPEN;

}

TM_RETURN s_ConfigureGenericPins::Execute()
    {
       Profiler P(__PRETTY_FUNCTION__);      
       
        
        PinML OpenEbits;
        PinML CloseEbits;
        
        PinML OpenCbits;
        PinML CloseCbits;

        for(size_t entry = 0; entry < DUTPins.GetSize(); ++entry){
            for(size_t pin = 0; pin < DUTPins[entry].GetNumPins(); ++pin){

                GenericPin& aliased_pin = GetGenericPin(DUTPins[entry][pin]);
               
                
                switch(connect[entry]){
                case(DXV_CONNECT_MODE::OPEN):
                    OpenEbits += aliased_pin.xe_force_isolate(); 
                    OpenEbits += aliased_pin.xe_sense_isolate(); 
                    //OpenEbits += aliased_pin.xe_ground_force(); 
                    //OpenEbits += aliased_pin.xe_gx1_direct(); 
                    OpenCbits += aliased_pin.xe_ground_force(); 
                    OpenCbits += aliased_pin.xe_gx1_direct(); 
                    break; 
                case(DXV_CONNECT_MODE::GROUND):
                    CloseEbits += aliased_pin.xe_force_isolate(); 
                    OpenEbits += aliased_pin.xe_sense_isolate(); 
                    //CloseEbits += aliased_pin.xe_ground_force();
                    CloseCbits += aliased_pin.xe_ground_force(); 
                    //OpenEbits += aliased_pin.xe_gx1_direct(); 
                    OpenCbits += aliased_pin.xe_gx1_direct(); 
                    break; 
                case(DXV_CONNECT_MODE::GX1_DIRECT):
                    OpenEbits += aliased_pin.xe_force_isolate(); 
                    CloseEbits += aliased_pin.xe_sense_isolate(); 
                    //OpenEbits += aliased_pin.xe_ground_force(); 
                    OpenCbits += aliased_pin.xe_ground_force(); 
                    //CloseEbits += aliased_pin.xe_gx1_direct(); 
                    CloseCbits += aliased_pin.xe_gx1_direct(); 
                    break; 
                case(DXV_CONNECT_MODE::VI_100_KELVIN):
                    CloseEbits += aliased_pin.xe_force_isolate(); 
                    CloseEbits += aliased_pin.xe_sense_isolate(); 
                    //OpenEbits += aliased_pin.xe_ground_force();
                    OpenCbits += aliased_pin.xe_ground_force(); 
                    //OpenEbits += aliased_pin.xe_gx1_direct();
                    OpenCbits += aliased_pin.xe_gx1_direct(); 
                    break;                    
                case(DXV_CONNECT_MODE::VI_100_SENSE_ONLY):
                    OpenEbits += aliased_pin.xe_force_isolate(); 
                    CloseEbits += aliased_pin.xe_sense_isolate(); 
                    //OpenEbits += aliased_pin.xe_ground_force();
                    OpenCbits += aliased_pin.xe_ground_force();
                    //OpenEbits += aliased_pin.xe_gx1_direct(); 
                    OpenCbits += aliased_pin.xe_gx1_direct(); 
                    break; 
                
                }
                
                switch(bulk_mode[entry]){
                case(DXV_BULK_MODE::OPEN_NO_DISCHARGE):
                    OpenEbits += aliased_pin.xe_bulk_discharge();
                    OpenEbits += aliased_pin.xe_bulk_connect();
                    break; 
                case(DXV_BULK_MODE::OPEN_DISCHARGE):
                    CloseEbits += aliased_pin.xe_bulk_discharge();
                    OpenEbits += aliased_pin.xe_bulk_connect();
                    break; 
                case(DXV_BULK_MODE::CONNECT_NO_DISCHARGE):
                    OpenEbits += aliased_pin.xe_bulk_discharge();
                    CloseEbits += aliased_pin.xe_bulk_connect();
                    break; 
                case(DXV_BULK_MODE::CONNECT_DISCHARGE):
                    CloseEbits += aliased_pin.xe_bulk_discharge();
                    CloseEbits += aliased_pin.xe_bulk_connect(); 
                    break;                                   
                }
                
                switch(tmu_mode[entry]){
                case(DXV_TMU_MODE::OPEN):
                    OpenEbits += aliased_pin.xe_tmu_connect();
                    break; 
                case(DXV_TMU_MODE::CONNECT):
                    CloseEbits += aliased_pin.xe_tmu_connect();
                    break;              
                }
                
                //LJ16 only
                switch(bypass_mode[entry]){
                case(BYPASS_CONNECT_MODE::OPEN):
                    OpenEbits += aliased_pin.xe_bypass();
                    break; 
                case(BYPASS_CONNECT_MODE::CONNECT):
                    CloseEbits += aliased_pin.xe_bypass();
                    break;              
                }
                
                switch(short_mode[entry]){
                case(DXV_SHORT_MODE::OPEN):
                    OpenEbits += aliased_pin.xe_short_A();
                    OpenEbits += aliased_pin.xe_short_B();
                    break; 
                case(DXV_SHORT_MODE::SHORT_A):
                    CloseEbits += aliased_pin.xe_short_A();
                    OpenEbits += aliased_pin.xe_short_B();
                    break; 
                case(DXV_SHORT_MODE::SHORT_B):
                    OpenEbits += aliased_pin.xe_short_A();
                    CloseEbits += aliased_pin.xe_short_B();
                    break; 
                case(DXV_SHORT_MODE::SHORT_A_AND_B):
                    CloseEbits += aliased_pin.xe_short_A();
                    CloseEbits += aliased_pin.xe_short_B();
                    break;                                  
                }
                
            }
        }
        ECBIT_BUS0.CloseAndOpen(CloseEbits, OpenEbits); 
        
//        for(int pin = 0; pin < CloseEbits.GetNumPins(); ++pin){
//            std::cout << CloseEbits[pin] << std::endl;
//        }
//        for(int pin = 0; pin < CloseCbits.GetNumPins(); ++pin){
//            std::cout << CloseCbits[pin] << std::endl;
//        }
        
        //ECBIT_BUS0.Read(CloseEbits);
        
        //for LJ16 only 
        if(CloseCbits.GetNumPins() > 0){
            CBIT.Close(CloseCbits); 
            
        }
        if(OpenCbits.GetNumPins() > 0){
            CBIT.Open(OpenCbits); 
        }
        SetResult(TM_PASS); 
        return TM_HAS_RESULT;
    }
    
    StringS print_gp(GP_AVAILIBLE pin_num){
        StringS mapping[] = {"INVALID_ERROR", 
        "01", "02", "03", "04", "05", "06", "07", "08", "09", "10",
        "11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
        "21", "22", "23", "24", "25", "26", "27", "28", "29", "30",
        "31", "32"};
        return mapping[static_cast<size_t>(pin_num)];
    }


    
      

    BoolM s_ConfigureGenericPins::Initialize()
    { 
        Profiler P(__PRETTY_FUNCTION__);

        BoolM initialize_status = true;

       
        return initialize_status;
    }
 
    BoolM s_ConfigureGenericPins::ParamCheck()
    {
        StringS error_message;
        BoolM param_check_status = true;
        return param_check_status;
    }
    
    s_ConfigureGenericPins::s_ConfigureGenericPins()
    {
        AddInputParameter(DisplayHardwareDoc,"DisplayHardwareDoc", "'Right Click Above For Documentation'");
        SetParameterAttribute("DisplayHardwareDoc","parameter-group","Hardware Documentation");
        SetParameterAttribute("DisplayHardwareDoc","description",
            "<h3>This method enables seamless control of the Renesas DxV generic hardware.</h3> <br><br>\
            The generic hardware is designed to support up to 32 pin devices. Each device has an idential circuit as shown below:<br><br>\
            <img src=\"/home/rich/programs/DxV_Template/Libraries/DxV_HW/Doc//universal_hw_32.jpg\" alt=\"Schematic\"><br><br>\
            Each DUT pin is capable of exclusively connecting to the following drive instruments: <ul>\
            <li>The <b>A</b> output of a VI100 with a full kelvin connection <i>(100V Capable)</i></li>\
            <li>The <b>A</b> output of a VI100 with a sense only connection</li>\
            <li>Direclty to a dedicated Gx1x Digital pin for drive or recieve <i>(limited to 5.7 V)</i></li>\
            <li>Directly to Tester Ground</li>\
            <li>None of the above (Open Circuit)</li>\
            </ul><br>\
            Additionally, the pin may connect to any of the flowing independantly of the instrument connection:<ul>\
            <li> A bulk capacitor of 1 uF\
            <ul>\
                <li>The circuit has independent control of the discharge resistor</li>\
                <li>The discharge resistor should be used to ensure the cap is fully discharged before it is switched in while it is open</li>\
                <li>The resistor and capacitor may be connected together to appl a 10k Ohm pulldown to the DUT pin</li>\
            </ul>\
            </li>\
            <li> An actively attenuated 20x divider to use the Gx1x Recieve or TMU <i>above 5.7 V</i>\
            <ul>\
                <li>The TMU Reference is set with a kelvin Gx1x pair</li>\
                <li>The LVDS output of the comparator must be baised to 1.2V using the 50 Ohm drivers of the Gx1x connected to the output</li>\
                <li>The Gx1x on the output can be used to strobe the comparator with a very high effective bandwidth</li>\
            </ul>\
            </li>\
            <li> Up to two <b>Shorting Buses</b> that connect to every pin\
            <ul>\
                <li><b>Short-A</b> and <b>Short-B</b></li>\
                <li>There is a load bank of 12 compoennts between <b>Short-A</b> and <b>Short-B</b></li>\
                <li>If the FPVI is installed into the DxV then it can either force each short bus indpeendantly, or be strapped across the short buses as a floating instrument.</li>\
            </ul>\
            </li>\
            </ul><br><br>\
            For individual relay documentation for each enumerated type, please see the description field of each enum header.\
            <br><br>\
            All of the relays that are controlled on the hardware are controlled via the Cohu designed ECBIT2 module. This module uses the\
            SDU I2C bus to control up to 256 ECBITS each. This leaves the 32 SDU3 CBITs to pass directly to the interposer to control interposer specific circuits.\
            While independent control of each relay is possible, the library restricts some connections intentionally to reduce programing erors. For example,\
            You may not connect a force instrument to ground directly. <br><br>\
            As each DUT pin has many possible connections, there is an alias library that has been created to use the DUT pin to look up the PinM object for each\
            desired instrument connection. These pins are assigned via the adapter board and use the <b>PPID</b> field to assign the correct pin number to find the appropriate channels.\
            <br>\
            The DUT Pin list must all use the GP <i>(Generic Pin)</i> pin type. Each pin must be an alias pin. Using a non aliased pin will result in an error!\
            <br>\
            The ECBIT library will close or open all the relays required to create the state created within this block as quickly as possible. The shadow variables\
            will not be disturbed, however no gauntee of execution order is made. If you require a specific execution order use two calls to this method!\
            Only the pins where changes are made will see a ECBIT write.\
            "
        );
        SetParameterAttribute("DisplayHardwareDoc","tooltip",
            "Right click and view to see detailed documentation on the hardware and this setup method"
        );
        

        AddInputParameter(DUTPins,"DUTPins");
        SetParameterAttribute("DUTPins","parameter-group","Pin Configure Burst");
        SetParameterAttribute("DUTPins","tooltip",
            "The list of Pin Alias that will be connected with the given condition. Use only GP Pin types!"
        );
        
        AddInputParameter(connect,"InstrumentConnect","DXV_CONNECT_MODE:OPEN");
        SetParameterAttribute("InstrumentConnect","parameter-group","Pin Configure Burst" );
        SetParameterAttribute("InstrumentConnect","description",
            "<h3>The Relay Connections of the DXV_CONNECT_MODE enumerated type are illustrated below:</h3> <br><br>\
            <img src=\"/home/rich/programs/DxV_Template/Libraries/DxV_HW/Doc/universal_hw_connect_states.jpg\" alt=\"Schematic\">"
         );
        SetParameterAttribute("InstrumentConnect","tooltip",
            "Right Click to View Relay Connections!"
        );
        
        AddInputParameter(bypass_mode,"BypassCapMode","BYPASS_CONNECT_MODE:OPEN");
        SetParameterAttribute("BypassCapMode","parameter-group","Pin Configure Burst" );
        SetParameterAttribute("BypassCapMode","tooltip",
            "Connect or disconnect 100 nF Bypass cap."
        );
        
        AddInputParameter(bulk_mode,"BulkCapMode","DXV_BULK_MODE:OPEN_DISCHARGE");
        SetParameterAttribute("BulkCapMode","parameter-group","Pin Configure Burst" );
        SetParameterAttribute("BulkCapMode","description",
            "<h3>The Relay Connections of the DXV_BULK_MODE enumerated type are illustrated below:</h3> <br><br>\
            <img src=\"/home/rich/programs/DxV_Template/Libraries/DxV_HW/Doc/universal_hw_bulk_states.jpg\" alt=\"Schematic\">"
         );
        SetParameterAttribute("BulkCapMode","tooltip",
            "Right Click to View Relay Connections!"
        );
        
        AddInputParameter(tmu_mode,"TmuAttenuatorConnected","DXV_TMU_MODE:OPEN");
        SetParameterAttribute("TmuAttenuatorConnected","parameter-group","Pin Configure Burst" );
        SetParameterAttribute("TmuAttenuatorConnected","description",
            "<h3>The Relay Connections of the DXV_TMU_MODE enumerated type are illustrated below:</h3> <br><br>\
            <img src=\"/home/rich/programs/DxV_Template/Libraries/DxV_HW/Doc/universal_hw_tmu_states.jpg\" alt=\"Schematic\">"
         );
        SetParameterAttribute("TmuAttenuatorConnected","tooltip",
            "Right Click to View Relay Connections!"
        );

        AddInputParameter(short_mode,"ShortBusControl","DXV_SHORT_MODE:OPEN");
        SetParameterAttribute("ShortBusControl","parameter-group","Pin Configure Burst" );
        SetParameterAttribute("ShortBusControl","description",
            "<h3>The Relay Connections of the DXV_SHORT_MODE enumerated type are illustrated below:</h3> <br><br>\
            <img src=\"/home/rich/programs/DxV_Template/Libraries/DxV_HW/Doc/universal_hw_short_states.jpg\" alt=\"Schematic\">"
         );
        SetParameterAttribute("ShortBusControl","tooltip",
            "Right Click to View Relay Connections!"
        );
        



    }


 


}
}
