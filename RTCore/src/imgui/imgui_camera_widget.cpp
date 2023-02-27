/*
* Copyright (c) 2014-2021, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/


#include <tinygltf/json.hpp>

#include <Imgui/imgui.h>
#include "imgui/imgui_camera_widget.h"
#include "imgui_helper.h"
#include "nvh/cameramanipulator.hpp"
#include "nvh/misc.hpp"
#include <fstream>
#include <sstream>


namespace ImGuiH {

using nlohmann::json;
using Gui = ImGuiH::Control;

//--------------------------------------------------------------------------------------------------
// Holds all saved cameras in a vector of Cameras
// - The first camera in the list is the HOME camera, the one that was set before this is called.
// - The update function will check if something has changed and will save the JSON to disk, only
//  once in a while.
// - Adding a camera will be added only if it is different from all other saved cameras
// - load/save Setting will load next to the executable, the "jsonFilename" + ".json"
//
struct CameraManager
{
  CameraManager()  = default;
  ~CameraManager() { saveSetting(nvh::CameraManipulator::Singleton()); };


  // update setting, load or save
  void update(nvh::CameraManipulator& cameraM)
  {
    // Push the HOME camera and load default setting
    if(cameras.empty())
    {
      cameras.emplace_back(cameraM.getCamera());
    }
    if(doLoadSetting)
      loadSetting(cameraM);

    // Save settings (with a delay after the last modification, so we don't spam disk too much)
    auto& IO = ImGui::GetIO();
    if(settingsDirtyTimer > 0.0f)
    {
      settingsDirtyTimer -= IO.DeltaTime;
      if(settingsDirtyTimer <= 0.0f)
      {
        saveSetting(cameraM);
        settingsDirtyTimer = 0.0f;
      }
    }
  }

  // Clear all cameras except the HOME
  void removedSavedCameras()
  {
    if(cameras.size() > 1)
      cameras.erase(cameras.begin() + 1, cameras.end());
  }

  void setCameraJsonFile(const std::string& filename)
  {
    jsonFilename = NVPSystem::exePath() + filename + ".json";
    doLoadSetting = true;
    removedSavedCameras();
  }


  void setHomeCamera(const nvh::CameraManipulator::Camera& camera)
  {
    if(cameras.empty())
      cameras.resize(1);
    cameras[0] = camera;
  }

  // Adding a camera only if it different from all the saved ones
  void addCamera(const nvh::CameraManipulator::Camera& camera)
  {
    bool unique = true;
    for(const auto& c : cameras)
    {
      if(c == camera)
      {
        unique = false;
        break;
      }
    }
    if(unique)
    {
      cameras.emplace_back(camera);
      markIniSettingsDirty();
    }
  }

  void markIniSettingsDirty()
  {
    auto& IO = ImGui::GetIO();
    if(settingsDirtyTimer <= 0.0f)
      settingsDirtyTimer = IO.IniSavingRate / 2.0f;
  }

  template <typename T>
  bool getJsonValue(const json& j, const std::string& name, T& value)
  {
    auto fieldIt = j.find(name);
    if(fieldIt != j.end())
    {
      value = (*fieldIt);
      return true;
    }
    LOGE("Could not find JSON field %s", name.c_str());
    return false;
  }

  template <typename T>
  bool getJsonArray(const json& j, const std::string& name, T& value)
  {
    auto fieldIt = j.find(name);
    if(fieldIt != j.end())
    {
      value = T((*fieldIt).begin(), (*fieldIt).end());
      return true;
    }
    LOGE("Could not find JSON field %s", name.c_str());
    return false;
  }


  void loadSetting(nvh::CameraManipulator& cameraM)
  {
    if(jsonFilename.empty() || cameras.empty() || doLoadSetting == false)
      return;

    try
    {
      doLoadSetting = false;

      // Clear all cameras except the HOME
      removedSavedCameras();

      std::ifstream i(jsonFilename);
      if(!i.is_open())
        return;

      // Parsing the file
      json j;
      i >> j;

      // Temp
      int                iVal;
      float              fVal;
      std::vector<float> vfVal;

      // Settings
      if(getJsonValue(j, "mode", iVal))
        cameraM.setMode(static_cast<nvh::CameraManipulator::Modes>(iVal));
      if(getJsonValue(j, "speed", fVal))
        cameraM.setSpeed(fVal);
      if(getJsonValue(j, "anim_duration", fVal))
        cameraM.setAnimationDuration(fVal);

      // All cameras
      std::vector<json> cc;
      getJsonArray(j, "cameras", cc);
      for(auto& c : cc)
      {
        nvh::CameraManipulator::Camera camera;
        if(getJsonArray(c, "eye", vfVal))
          camera.eye = {vfVal[0], vfVal[1], vfVal[2]};
        if(getJsonArray(c, "ctr", vfVal))
          camera.ctr = {vfVal[0], vfVal[1], vfVal[2]};
        if(getJsonArray(c, "up", vfVal))
          camera.up = {vfVal[0], vfVal[1], vfVal[2]};
        if(getJsonValue(c, "fov", fVal))
          camera.fov = fVal;
        cameras.emplace_back(camera);
      }
      i.close();
    }
    catch(...)
    {
      return;
    }
  }

  void saveSetting(nvh::CameraManipulator& cameraM)
  {
    if(jsonFilename.empty())
      return;

    json j;
    j["mode"]          = cameraM.getMode();
    j["speed"]         = cameraM.getSpeed();
    j["anim_duration"] = cameraM.getAnimationDuration();

    // Save all extra cameras
    json cc = json::array();
    for(size_t n = 1; n < cameras.size(); n++)
    {
      auto& c   = cameras[n];
      json  jo  = json::object();
      jo["eye"] = std::vector<float>{c.eye.X, c.eye.Y, c.eye.Z};
      jo["up"]  = std::vector<float>{c.up.X, c.up.Y, c.up.Z};
      jo["ctr"] = std::vector<float>{c.ctr.X, c.ctr.Y, c.ctr.Z};
      jo["fov"] = c.fov;
      cc.push_back(jo);
    }
    j["cameras"] = cc;

    std::ofstream o(jsonFilename);
    if(o.is_open())
    {
      o << j.dump(2) << std::endl;
      o.close();
    }
  }

  // Holds all cameras. [0] == HOME
  std::vector<nvh::CameraManipulator::Camera> cameras;
  float                                       settingsDirtyTimer{0};
  std::string                                 jsonFilename;
  bool                                        doLoadSetting{true};
};

static std::unique_ptr<CameraManager> sCamMgr;


//--------------------------------------------------------------------------------------------------
// Display the values of the current camera: position, center, up and FOV
//
void CurrentCameraTab(nvh::CameraManipulator& cameraM, nvh::CameraManipulator::Camera& camera, bool& changed, bool& instantSet)
{

  bool y_is_up = camera.up.Y == 1;

  PropertyEditor::begin();
  PropertyEditor::entry(
      "Eye", [&] { return ImGui::InputFloat3("##Eye", &camera.eye.X, "%.5f"); }, "Position of the Camera");
  changed |= ImGui::IsItemDeactivatedAfterEdit();
  PropertyEditor::entry(
      "Center", [&] { return ImGui::InputFloat3("##Ctr", &camera.ctr.X, "%.5f"); }, "Center of camera interest");
  changed |= ImGui::IsItemDeactivatedAfterEdit();
  changed |= PropertyEditor::entry(
      "Y is UP", [&] { return ImGui::Checkbox("##Y", &y_is_up); }, "Is Y pointing up or Z?");
  if(PropertyEditor::entry(
         "FOV", [&] { return ImGui::DragFloat("##Y", &camera.fov); }, "Field of view in degrees?"))
  {
    instantSet = true;
    changed    = true;
  }

  if(PropertyEditor::treeNode("Clip planes"))
  {
    vec2 clip = cameraM.getClipPlanes();
    PropertyEditor::entry("Near", [&] { return ImGui::InputFloat("##CN", &clip.X); });
    changed |= ImGui::IsItemDeactivatedAfterEdit();
    PropertyEditor::entry("Far", [&] { return ImGui::InputFloat("##CF", &clip.Y); });
    changed |= ImGui::IsItemDeactivatedAfterEdit();
    PropertyEditor::treePop();
    cameraM.setClipPlanes(clip);
  }

  camera.up = y_is_up ? vec3(0, 1, 0) : vec3(0, 0, 1);

  if(cameraM.isAnimated())
  {
    // Ignoring any changes while the camera is moving to the goal.
    // The camera has to be in the new position before setting a new value.
    changed = false;
  }

  ImGui::TableNextRow();
  ImGui::TableNextColumn();

  ImGui::TextDisabled("(?)");
  ImGuiH::tooltip(cameraM.getHelp().c_str(), false, 0.0f);
  ImGui::TableNextColumn();
  if(ImGui::SmallButton("Copy"))
  {
    std::string text = nvh::stringFormat("{%.5f, %.5f, %.5f}, {%.5f, %.5f, %.5f}, {%.5f, %.5f, %.5f}",  //
                                         camera.eye.X, camera.eye.Y, camera.eye.Z,                      //
                                         camera.ctr.X, camera.ctr.Y, camera.ctr.Z,                      //
                                         camera.up.X, camera.up.Y, camera.up.Z);
    ImGui::SetClipboardText(text.c_str());
  }
  ImGuiH::tooltip("Copy to the clipboard the current camera: {eye}, {ctr}, {up}");
  ImGui::SameLine();
  const char* pPastedString;
  if(ImGui::SmallButton("Paste") && (pPastedString = ImGui::GetClipboardText()))
  {
    float val[9];
    int   result = sscanf(pPastedString, "{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}", &val[0], &val[1], &val[2], &val[3],
                          &val[4], &val[5], &val[6], &val[7], &val[8]);
    if(result == 9)  // 9 value properly scanned
    {
      camera.eye = vec3{val[0], val[1], val[2]};
      camera.ctr = vec3{val[3], val[4], val[5]};
      camera.up  = vec3{val[6], val[7], val[8]};
      changed    = true;
    }
  }
  ImGuiH::tooltip("Paste from the clipboard the current camera: {eye}, {ctr}, {up}");
  PropertyEditor::end();
}

//--------------------------------------------------------------------------------------------------
// Display buttons for all saved cameras. Allow to create and delete saved cameras
//
void SavedCameraTab(nvh::CameraManipulator& cameraM, nvh::CameraManipulator::Camera& camera, bool& changed)
{
  // Dummy
  ImVec2      button_sz(50, 30);
  char        label[128];
  ImGuiStyle& style             = ImGui::GetStyle();
  int         buttons_count     = (int)sCamMgr->cameras.size();
  float       window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

  // The HOME camera button, different from the other ones
  if(ImGui::Button("Home", ImVec2(ImGui::GetWindowContentRegionMax().x, 50)))
  {
    camera  = sCamMgr->cameras[0];
    changed = true;
  }
  ImGuiH::tooltip("Reset the camera to its origin");

  // Display all the saved camera in an array of buttons
  int delete_item = -1;
  for(int n = 1; n < buttons_count; n++)
  {
    ImGui::PushID(n);
    sprintf(label, "# %d", n);
    if(ImGui::Button(label, button_sz))
    {
      camera  = sCamMgr->cameras[n];
      changed = true;
    }

    // Middle click to delete a camera
    if(ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked[NVPWindow::MOUSE_BUTTON_MIDDLE])
      delete_item = n;

    // Displaying the position of the camera when hovering the button
    sprintf(label, "Pos: %3.5f, %3.5f, %3.5f", sCamMgr->cameras[n].eye.X, sCamMgr->cameras[n].eye.Y,
            sCamMgr->cameras[n].eye.Z);
    ImGuiH::tooltip(label);

    // Wrapping all buttons (see ImGUI Demo)
    float last_button_x2 = ImGui::GetItemRectMax().x;
    float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x;  // Expected position if next button was on same line
    if(n + 1 < buttons_count && next_button_x2 < window_visible_x2)
      ImGui::SameLine();

    ImGui::PopID();
  }

  // Adding a camera button
  if(ImGui::Button("+"))
  {
    sCamMgr->addCamera(cameraM.getCamera());
    sCamMgr->markIniSettingsDirty();
  }
  ImGuiH::tooltip("Add a new saved camera");
  ImGui::SameLine();
  ImGui::TextDisabled("(?)");
  ImGuiH::tooltip("Middle-click a camera to delete it", false, 0.0f);

  // Remove element
  if(delete_item > 0)
  {
    sCamMgr->cameras.erase(sCamMgr->cameras.begin() + delete_item);
    sCamMgr->markIniSettingsDirty();
  }
}

//--------------------------------------------------------------------------------------------------
// This holds all camera setting, like the speed, the movement mode, transition duration
//
void CameraExtraTab(nvh::CameraManipulator& cameraM, bool& changed)
{
  // Navigation Mode
  PropertyEditor::begin();
  auto mode     = cameraM.getMode();
  auto speed    = cameraM.getSpeed();
  auto duration = static_cast<float>(cameraM.getAnimationDuration());

  changed |= PropertyEditor::entry(
      "Navigation",
      [&] {
        int rmode = static_cast<int>(mode);
        changed |= ImGui::RadioButton("Examine", &rmode, nvh::CameraManipulator::Examine);
        ImGuiH::tooltip("The camera orbit around a point of interest");
        changed |= ImGui::RadioButton("Fly", &rmode, nvh::CameraManipulator::Fly);
        ImGuiH::tooltip("The camera is free and move toward the looking direction");
        changed |= ImGui::RadioButton("Walk", &rmode, nvh::CameraManipulator::Walk);
        ImGuiH::tooltip("The camera is free but stay on a plane");
        cameraM.setMode(static_cast<nvh::CameraManipulator::Modes>(rmode));
        return changed;
      },
      "Camera Navigation Mode");

  changed |= PropertyEditor::entry(
      "Speed", [&] { return ImGui::SliderFloat("##S", &speed, 0.01F, 10.0F); }, "Changing the default speed movement");
  changed |= PropertyEditor::entry(
      "Transition", [&] { return ImGui::SliderFloat("##S", &duration, 0.0F, 2.0F); }, "Nb seconds to move to new position");

  cameraM.setSpeed(speed);
  cameraM.setAnimationDuration(duration);

  PropertyEditor::end();
}

//--------------------------------------------------------------------------------------------------
// Display the camera eye and center of interest position of the camera (nvh::CameraManipulator)
// Allow also to modify the field-of-view (FOV)
// And basic control information is displayed
bool CameraWidget(nvh::CameraManipulator& cameraM /*= nvh::CameraManipulator::Singleton()*/)
{
  if(!sCamMgr)
    sCamMgr = std::make_unique<CameraManager>();

  bool changed{false};
  bool instantSet{false};
  auto camera = cameraM.getCamera();

  // Updating the camera manager
  sCamMgr->update(cameraM);

  // Starting UI
  if(ImGui::BeginTabBar("Hello"))
  {
    if(ImGui::BeginTabItem("Current"))
    {
      CurrentCameraTab(cameraM, camera, changed, instantSet);
      ImGui::EndTabItem();
    }

    if(ImGui::BeginTabItem("Cameras"))
    {
      SavedCameraTab(cameraM, camera, changed);
      ImGui::EndTabItem();
    }

    if(ImGui::BeginTabItem("Extra"))
    {
      CameraExtraTab(cameraM, changed);
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }

  // Apply the change back to the camera
  if(changed)
  {
    cameraM.setCamera(camera, instantSet);
    sCamMgr->markIniSettingsDirty();
  }
  ImGui::Separator();

  return changed;
}

void SetCameraJsonFile(const std::string& filename)
{
  if(!sCamMgr)
    sCamMgr = std::make_unique<CameraManager>();
  sCamMgr->setCameraJsonFile(filename);
}

void SetHomeCamera(const nvh::CameraManipulator::Camera& camera)
{
  if(!sCamMgr)
    sCamMgr = std::make_unique<CameraManager>();
  sCamMgr->setHomeCamera(camera);
}

void AddCamera(const nvh::CameraManipulator::Camera& camera)
{
  if(!sCamMgr)
    sCamMgr = std::make_unique<CameraManager>();
  sCamMgr->addCamera(camera);
}

}  // namespace ImGuiH
