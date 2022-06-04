#include "GUI.h"
//lib
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace GUI
{

const float TRA_min = -1.0f; const float TRA_max = 1.0f;  const float TRA_step = 0.1f;
const float ROT_min = 0.0f;  const float ROT_max = 360.0f;const float ROT_step = 1.0f;
const float SCA_min = 0.0f;  const float SCA_max = 10.0f; const float SCA_step = 0.1f;


bool ObjectNode(Transformations &transf, std::vector<std::string> &items, size_t &item_current_idx)
{
    assert(item_current_idx <= items.size() && " current_item out of range");
    bool retval = false;
    
    // render your GUI
    ImGui::NewFrame();

    ImGui::Begin("Model trasfromations");         
        
        {
            const char* combo_preview_value = items[item_current_idx].c_str();
            if (ImGui::BeginCombo("Model", combo_preview_value, 0))
            {
                for (int n = 0; n < items.size() ; n++)
                {
                    const bool is_selected = (item_current_idx == n);
                    if (ImGui::Selectable(items[n].c_str(), is_selected))
                        item_current_idx = n;

                    // Set the initial focus 
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }

        {
            if(ImGui::DragFloat3("translation", glm::value_ptr(transf.T), TRA_step, TRA_min, TRA_max)){
                retval = true;    
            }
            if(ImGui::DragFloat3("rotation", glm::value_ptr(transf.R), ROT_step, ROT_min, ROT_max)){
                retval = true;    
            }
            static bool uniformScale = false;
            ImGui::Checkbox("Uniform scale", &uniformScale);
            if(uniformScale){
                float scale = 1.0f; 
                if(ImGui::DragFloat("scale", &scale, SCA_step, SCA_min, SCA_max)){
                    transf.S *= scale;
                    retval = true;
                }
            } else{
                if(ImGui::DragFloat3("scale", glm::value_ptr(transf.S), SCA_step, SCA_min, SCA_max)){
                    retval = true;    
                }
            }
            
        }
    ImGui::End();
  
    ImGui::Render();

    return retval;
}
    
} // namespace name


 