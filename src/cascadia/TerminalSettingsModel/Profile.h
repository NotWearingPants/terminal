/*++
Copyright (c) Microsoft Corporation
Licensed under the MIT license.

Module Name:
- Profile.hpp

Abstract:
- A profile acts as a single set of terminal settings. Many tabs or panes could
  exist side-by-side with different profiles simultaneously.
- Profiles could also specify their appearance when unfocused, this is what
  the inheritance tree looks like for unfocused settings:

                +-------------------+
                |                   |
                |Profile.defaults   |
                |                   |
                |DefaultAppearance  |
                |                   |
                +-------------------+
                   ^             ^
                   |             |
+------------------++           ++------------------+
|                   |           |                   |
|MyProfile          |           |Profile.defaults   |
|                   |           |                   |
|DefaultAppearance  |           |UnfocusedAppearance|
|                   |           |                   |
+-------------------+           +-------------------+
                   ^
                   |
+------------------++
|                   |
|MyProfile          |
|                   |
|UnfocusedAppearance|
|                   |
+-------------------+


Author(s):
- Mike Griese - March 2019

--*/
#pragma once

#include "Profile.g.h"
#include "IInheritable.h"

#include "../inc/cppwinrt_utils.h"
#include "JsonUtils.h"
#include <DefaultSettings.h>
#include "AppearanceConfig.h"
#include "FontConfig.h"

// fwdecl unittest classes
namespace SettingsModelLocalTests
{
    class DeserializationTests;
    class ProfileTests;
    class ColorSchemeTests;
    class KeyBindingsTests;
};
namespace TerminalAppUnitTests
{
    class DynamicProfileTests;
    class JsonTests;
};

// GUID used for generating GUIDs at runtime, for profiles that did not have a
// GUID specified manually.
constexpr GUID RUNTIME_GENERATED_PROFILE_NAMESPACE_GUID = { 0xf65ddb7e, 0x706b, 0x4499, { 0x8a, 0x50, 0x40, 0x31, 0x3c, 0xaf, 0x51, 0x0a } };

namespace winrt::Microsoft::Terminal::Settings::Model::implementation
{
    struct Profile : ProfileT<Profile>, IInheritable<Profile>
    {
    public:
        Profile() noexcept = default;
        Profile(guid guid) noexcept;

        void CreateUnfocusedAppearance();
        void DeleteUnfocusedAppearance();

        hstring ToString()
        {
            return Name();
        }

        static void CopyInheritanceGraphs(std::unordered_map<const Profile*, winrt::com_ptr<Profile>>& visited, const std::vector<winrt::com_ptr<Profile>>& source, std::vector<winrt::com_ptr<Profile>>& target);
        winrt::com_ptr<Profile>& CopyInheritanceGraph(std::unordered_map<const Profile*, winrt::com_ptr<Profile>>& visited) const;
        winrt::com_ptr<Profile> CopySettings() const;

        static com_ptr<Profile> FromJson(const Json::Value& json);
        void LayerJson(const Json::Value& json);
        Json::Value ToJson() const;

        hstring EvaluatedStartingDirectory() const;

        Model::IAppearanceConfig DefaultAppearance();
        Model::FontConfig FontInfo();

        void _FinalizeInheritance() override;

        WINRT_PROPERTY(bool, Deleted, false);
        WINRT_PROPERTY(OriginTag, Origin, OriginTag::None);

        WINRT_PROPERTY(guid, Updates);
        INHERITABLE_SETTING(Model::Profile, guid, Guid, _GenerateGuidForProfile(Name(), Source()));
        INHERITABLE_SETTING(Model::Profile, hstring, Name, L"Default");
        INHERITABLE_SETTING(Model::Profile, hstring, Source);
        INHERITABLE_SETTING(Model::Profile, bool, Hidden, false);
        INHERITABLE_SETTING(Model::Profile, guid, ConnectionType);

        // Default Icon: Segoe MDL2 CommandPrompt icon
        INHERITABLE_SETTING(Model::Profile, hstring, Icon, L"\uE756");

        INHERITABLE_SETTING(Model::Profile, CloseOnExitMode, CloseOnExit, CloseOnExitMode::Graceful);
        INHERITABLE_SETTING(Model::Profile, hstring, TabTitle);
        INHERITABLE_NULLABLE_SETTING(Model::Profile, Microsoft::Terminal::Core::Color, TabColor, nullptr);
        INHERITABLE_SETTING(Model::Profile, bool, SuppressApplicationTitle, false);

        INHERITABLE_SETTING(Model::Profile, bool, UseAcrylic, false);
        INHERITABLE_SETTING(Model::Profile, Microsoft::Terminal::Control::ScrollbarState, ScrollState, Microsoft::Terminal::Control::ScrollbarState::Visible);

        INHERITABLE_SETTING(Model::Profile, hstring, Padding, DEFAULT_PADDING);

        INHERITABLE_SETTING(Model::Profile, hstring, Commandline, L"cmd.exe");
        INHERITABLE_SETTING(Model::Profile, hstring, StartingDirectory);

        INHERITABLE_SETTING(Model::Profile, Microsoft::Terminal::Control::TextAntialiasingMode, AntialiasingMode, Microsoft::Terminal::Control::TextAntialiasingMode::Grayscale);
        INHERITABLE_SETTING(Model::Profile, bool, ForceFullRepaintRendering, false);
        INHERITABLE_SETTING(Model::Profile, bool, SoftwareRendering, false);

        INHERITABLE_SETTING(Model::Profile, int32_t, HistorySize, DEFAULT_HISTORY_SIZE);
        INHERITABLE_SETTING(Model::Profile, bool, SnapOnInput, true);
        INHERITABLE_SETTING(Model::Profile, bool, AltGrAliasing, true);

        INHERITABLE_SETTING(Model::Profile, Model::BellStyle, BellStyle, BellStyle::Audible);

        INHERITABLE_SETTING(Model::Profile, Model::IAppearanceConfig, UnfocusedAppearance, nullptr);

    private:
        Model::IAppearanceConfig _DefaultAppearance{ winrt::make<AppearanceConfig>(weak_ref<Model::Profile>(*this)) };
        Model::FontConfig _FontInfo{ winrt::make<FontConfig>(weak_ref<Model::Profile>(*this)) };
        static std::wstring EvaluateStartingDirectory(const std::wstring& directory);

        static guid _GenerateGuidForProfile(const std::wstring_view& name, const std::wstring_view& source) noexcept;

        friend class SettingsModelLocalTests::DeserializationTests;
        friend class SettingsModelLocalTests::ProfileTests;
        friend class SettingsModelLocalTests::ColorSchemeTests;
        friend class SettingsModelLocalTests::KeyBindingsTests;
        friend class TerminalAppUnitTests::DynamicProfileTests;
        friend class TerminalAppUnitTests::JsonTests;
    };
}

namespace winrt::Microsoft::Terminal::Settings::Model::factory_implementation
{
    BASIC_FACTORY(Profile);
}
