#ifndef VPX_TOOLTIPS_H
#define VPX_TOOLTIPS_H

#include <map>
#include <string>

static std::map<std::string, std::string> VPX_TOOLTIPS = {
    {"VPRegPath",
        "This can be used to override the VPinball table settings directory location. "
        "If set to ./, the current table path will be used."
    },
    {"AltSound",
        "Controls sound format between 'Legacy', 'Altsound' and 'G-Sound'.\n"
        "1 - If the folder contains an 'altsound.csv' or 'g-sound.csv' file.\n"
        "1 - If the folder contains subfolders with names like 'jingle', 'single', 'voice', etc. and you are NOT running on PinSound sound hardware.\n"
        "Note: this only works for the unencrypted versions of the PinSound libraries, which are now quite old.\n"
        "2 - If the folder contains subfolders with names like 'jingle', 'single', 'voice', etc. and you are running on PinSound sound hardware.\n"
        "3 - ??"
    },
    {"PinMAMEPath",
        "Specifies the directory where the PinMAME emulator is located. PinMAME is an emulator for classic pinball machines.\n"
        "Default is ~/.pinmame: If not specified, the default location for the PinMAME folder is ~/.pinmame."
    },
    {"PinMAMEWindow",
        "Controls the visibility of the PinMAME DMD (Dot Matrix Display) window.\n"
        "0 - Turns the DMD window off: Disables the PinMAME DMD window, which is often used for displaying the score and game information.\n"
        "1 - Turns the DMD window on: Enables the PinMAME DMD window to display relevant game information. (Default is 1)"
    },
    {"PinMAMEWindowX",
        "Sets the X pixel coordinate of the top-left corner of the PinMAME DMD window on the screen.\n"
        "This controls the horizontal position where the DMD window appears on the screen."
    },
    {"PinMAMEWindowY",
        "Sets the Y pixel coordinate of the top-left corner of the PinMAME DMD window on the screen.\n"
        "This controls the vertical position where the DMD window appears on the screen."
    },
    {"PinMAMEWindowWidth",
        "Controls the width of the PinMAME DMD window.\n"
        "You can resize the DMD window by specifying the width in pixels."
    },
    {"PinMAMEWindowHeight",
        "Controls the height of the PinMAME DMD window.\n"
        "You can resize the DMD window by specifying the height in pixels."
    },
    {"PinMAMEWindowRotation",
        "Controls the rotation of the PinMAME DMD window.\n"
        "The values 0, 1, 2, and 3 correspond to rotation angles of 0°, 90°, 180°, and 270°, respectively.\n"
        "The default value is 0, which represents no rotation."
    },
    {"FlexDMDWindow",
        "Controls the visibility of the FlexDMD (Dot Matrix Display) window.\n"
        "0 - Turns the DMD window off: Disables the FlexDMD window, which is often used for displaying the score and game information.\n"
        "1 - Turns the DMD window on: Enables the FlexDMD window to display relevant game information. (Default is 1)"
    },
    {"FlexDMDWindowX",
        "Sets the X pixel coordinate of the top-left corner of the FlexDMD window on the screen.\n"
        "This controls the horizontal position where the DMD window appears on the screen."
    },
    {"FlexDMDWindowY",
        "Sets the Y pixel coordinate of the top-left corner of the FlexDMD window on the screen.\n"
        "This controls the vertical position where the DMD window appears on the screen."
    },
    {"FlexDMDWindowWidth",
        "Controls the width of the FlexDMD window.\n"
        "You can resize the DMD window by specifying the width in pixels."
    },
    {"FlexDMDWindowHeight",
        "Controls the height of the FlexDMD window.\n"
        "You can resize the DMD window by specifying the height in pixels."
    },
    {"FlexDMDWindowRotation",
        "Controls the rotation of the FlexDMD window.\n"
        "The values 0, 1, 2, and 3 correspond to rotation angles of 0°, 90°, 180°, and 270°, respectively.\n"
        "The default value is 0, which represents no rotation."
    },
    {"B2SHideGrill",
        "Controls the visibility of the grill in the backbox of the cabinet setup.\n"
        "0 - Show grill (if it exists): Displays the grill, a common component in pinball machines.\n"
        "1 - Hide grill: Hides the grill."
    },
    {"B2SHideB2SDMD",
        "Controls the visibility of the extra DMD frame in the backbox.\n"
        "0 - Show extra DMD frame (if it exists): Displays the DMD frame around the Dot Matrix Display.\n"
        "1 - Hide extra DMD frame: Hides the DMD frame, leaving just the DMD window."
    },
    {"B2SHideB2SBackglass",
        "Controls the visibility of the backglass in a cabinet setup.\n"
        "0 - Show backglass: Displays the backglass, which often shows game artwork.\n"
        "1 - Hide backglass: Hides the backglass, potentially saving space in a multi-display setup."
    },
    {"B2SWindows",
        "Controls the visibility of various B2S windows such as the grill, DMD frame, and backglass.\n"
        "0 - Hide all B2S windows: Hides the grill, DMD frame, and backglass.\n"
        "1 - Show B2S windows according to other settings: Displays the grill, DMD frame, and backglass according to the other settings."
    },
    {"B2SBackglassX",
        "Sets the X pixel coordinate of the top-left corner of the backglass window.\n"
        "This allows you to position the backglass at a specific horizontal position on the screen."
    },
    {"B2SBackglassY",
        "Sets the Y pixel coordinate of the top-left corner of the backglass window.\n"
        "This allows you to position the backglass at a specific vertical position on the screen."
    },
    {"B2SBackglassWidth",
        "Controls the pixel width of the backglass window.\n"
        "This defines how wide the backglass window will appear on the screen."
    },
    {"B2SBackglassHeight",
        "Controls the pixel height of the backglass window.\n"
        "This defines how tall the backglass window will appear on the screen."
    },
    {"B2SBackglassRotation",
        "Controls the rotation of the backglass window.\n"
        "The values 0, 1, 2, and 3 correspond to rotation angles of 0°, 90°, 180°, and 270°, respectively.\n"
        "The default value is 0, which represents no rotation."
    },
    {"B2SDMDX",
        "Sets the X pixel coordinate of the top-left corner of the B2SDMD frame.\n"
        "This controls the horizontal positioning of the B2SDMD frame relative to the screen."
    },
    {"B2SDMDY",
        "Sets the Y pixel coordinate of the top-left corner of the B2SDMD frame.\n"
        "This controls the vertical positioning of the B2SDMD frame relative to the screen."
    },
    {"B2SDMDWidth",
        "Controls the pixel width of the B2SDMD frame.\n"
        "This defines how wide the B2SDMD frame will appear on the screen."
    },
    {"B2SDMDHeight",
        "Controls the pixel height of the B2SDMD frame.\n"
        "This defines how tall the B2SDMD frame will appear on the screen."
    },
    {"B2SDMDRotation",
        "Controls the rotation of the B2SDMD frame.\n"
        "The values 0, 1, 2, and 3 correspond to rotation angles of 0°, 90°, 180°, and 270°, respectively.\n"
        "The default value is 0, which represents no rotation."
    },
    {"B2SDMDFlipY",
        "Controls whether the B2SDMD frame is flipped vertically.\n"
        "0 - No flip: The B2SDMD frame appears normally.\n"
        "1 - Flip vertically: Flips the B2SDMD frame vertically, which may be useful for specific screen orientations."
    },
    {"PUPTopperWindow",
        "Controls the visibility of the PUPTopper window.\n"
        "0 - Hide PUPTopper window.\n"
        "1 - Show PUPTopper window."
    },
    {"PUPTopperWindowX",
        "Sets the X pixel coordinate of the top-left corner of the PUPTopper window.\n"
        "This allows you to position the PUPTopper at a specific horizontal position on the screen."
    },
    {"PUPTopperWindowY",
        "Sets the Y pixel coordinate of the top-left corner of the PUPTopper window.\n"
        "This allows you to position the PUPTopper at a specific vertical position on the screen."
    },
    {"PUPTopperWindowWidth",
        "Controls the pixel width of the PUPTopper window.\n"
        "This defines how wide the PUPTopper window will appear on the screen."
    },
    {"PUPTopperWindowHeight",
        "Controls the pixel height of the PUPTopper window.\n"
        "This defines how tall the PUPTopper window will appear on the screen."
    },
    {"PUPTopperWindowRotation",
        "Controls the rotation of the PUPTopper window.\n"
        "The values 0, 1, 2, and 3 correspond to rotation angles of 0°, 90°, 180°, and 270°, respectively.\n"
        "The default value is 0, which represents no rotation."
    },
    {"PUPBackglassWindow",
        "Controls the visibility of the PUPBackglass window.\n"
        "0 - Hide PUPBackglass window.\n"
        "1 - Show PUPBackglass window."
    },
    {"PUPBackglassWindowX",
        "Sets the X pixel coordinate of the top-left corner of the PUPBackglass window.\n"
        "This allows you to position the PUPBackglass at a specific horizontal position on the screen."
    },
    {"PUPBackglassWindowY",
        "Sets the Y pixel coordinate of the top-left corner of the PUPBackglass window.\n"
        "This allows you to position the PUPBackglass at a specific vertical position on the screen."
    },
    {"PUPBackglassWindowWidth",
        "Controls the pixel width of the PUPBackglass window.\n"
        "This defines how wide the PUPBackglass window will appear on the screen."
    },
    {"PUPBackglassWindowHeight",
        "Controls the pixel height of the PUPBackglass window.\n"
        "This defines how tall the PUPBackglass window will appear on the screen."
    },
    {"PUPBackglassWindowRotation",
        "Controls the rotation of the PUPBackglass window.\n"
        "The values 0, 1, 2, and 3 correspond to rotation angles of 0°, 90°, 180°, and 270°, respectively.\n"
        "The default value is 0, which represents no rotation."
    },
    {"PUPDMDWindow",
        "Controls the visibility of the PUPDMD window.\n"
        "0 - Hide PUPDMD window.\n"
        "1 - Show PUPDMD window."
    },
    {"PUPDMDWindowX",
        "Sets the X pixel coordinate of the top-left corner of the PUPDMD window.\n"
        "This allows you to position the PUPDMD at a specific horizontal position on the screen."
    },
    {"PUPDMDWindowY",
        "Sets the Y pixel coordinate of the top-left corner of the PUPDMD window.\n"
        "This allows you to position the PUPDMD at a specific vertical position on the screen."
    },
    {"PUPDMDWindowWidth",
        "Controls the pixel width of the PUPDMD window.\n"
        "This defines how wide the PUPDMD window will appear on the screen."
    },
    {"PUPDMDWindowHeight",
        "Controls the pixel height of the PUPDMD window.\n"
        "This defines how tall the PUPDMD window will appear on the screen."
    },
    {"PUPDMDWindowRotation",
        "Controls the rotation of the PUPDMD window.\n"
        "The values 0, 1, 2, and 3 correspond to rotation angles of 0°, 90°, 180°, and 270°, respectively.\n"
        "The default value is 0, which represents no rotation."
    },
    {"PUPPlayfieldWindow",
        "Controls the visibility of the PUPPlayfield window.\n"
        "0 - Hide PUPPlayfield window.\n"
        "1 - Show PUPPlayfield window."
    },
    {"PUPPlayfieldWindowX",
        "Sets the X pixel coordinate of the top-left corner of the PUPPlayfield window.\n"
        "This allows you to position the PUPPlayfield at a specific horizontal position on the screen."
    },
    {"PUPPlayfieldWindowY",
        "Sets the Y pixel coordinate of the top-left corner of the PUPPlayfield window.\n"
        "This allows you to position the PUPPlayfield at a specific vertical position on the screen."
    },
    {"PUPPlayfieldWindowWidth",
        "Controls the pixel width of the PUPPlayfield window.\n"
        "This defines how wide the PUPPlayfield window will appear on the screen."
    },
    {"PUPPlayfieldWindowHeight",
        "Controls the pixel height of the PUPPlayfield window.\n"
        "This defines how tall the PUPPlayfield window will appear on the screen."
    },
    {"PUPPlayfieldWindowRotation",
        "Controls the rotation of the PUPPlayfield window.\n"
        "The values 0, 1, 2, and 3 correspond to rotation angles of 0°, 90°, 180°, and 270°, respectively.\n"
        "The default value is 0, which represents no rotation."
    },
    {"PUPFullDMDWindow",
        "Controls the visibility of the PUPFullDMD window.\n"
        "0 - Hide PUPFullDMD window.\n"
        "1 - Show PUPFullDMD window."
    },
    {"PUPFullDMDWindowX",
        "Sets the X pixel coordinate of the top-left corner of the PUPFullDMD window.\n"
        "This allows you to position the PUPFullDMD at a specific horizontal position on the screen."
    },
    {"PUPFullDMDWindowY",
        "Sets the Y pixel coordinate of the top-left corner of the PUPFullDMD window.\n"
        "This allows you to position the PUPFullDMD at a specific vertical position on the screen."
    },
    {"PUPFullDMDWindowWidth",
        "Controls the pixel width of the PUPFullDMD window.\n"
        "This defines how wide the PUPFullDMD window will appear on the screen."
    },
    {"PUPFullDMDWindowHeight",
        "Controls the pixel height of the PUPFullDMD window.\n"
        "This defines how tall the PUPFullDMD window will appear on the screen."
    },
    {"PUPFullDMDWindowRotation",
        "Controls the rotation of the PUPFullDMD window.\n"
        "The values 0, 1, 2, and 3 correspond to rotation angles of 0°, 90°, 180°, and 270°, respectively.\n"
        "The default value is 0, which represents no rotation."
    },
    {"ZeDMDDebug",
        "0 to disable debug messages for an attached ZeDMD (the default), 1 to enable."
    },
    {"ZeDMDWiFiAddr",
        "Default value is zedmd-wifi.local."
    },
    {"DMDServerAddr",
        "Default value is localhost."
    },
    {"DMDServerPort",
        "Default value is 6789."
    },
    {"Sound3D",
        "This is an integer value that selects how VPinball generates its audio. Six options are available.\n"
        "0 - Standard 2 channel (default)\n"
        "1 - Surround (All effects to rear channels)\n"
        "2 - Surround (Front is rear of cab)\n"
        "3 - Surround (Front is front of cab)\n"
        "4 - 7.1 Surround (Front is rear, back is side, backbox is front)\n"
        "5 - 7.1 Surround Sound Feedback (SSF) This exaggerates the positional feel of the playfield sound effects when played in a cabinet with exciter pairs positioned at each end of the cabinet"
    },
    {"SoundDevice",
        "This is the text name of the main sound device that will be used by VPinball for Table mechanical sounds like flippers and bumpers\n"
        "Determine this name by running VPinball with the -listsnd parameter."
    },
    {"SoundDeviceBG",
        "This is the text name of the main sound device that will be used by VPinball for Music and Backglass sounds, including those from PUP packs and PinMAME.\n"
        "If you set SoundDevice to a non-default value and leave this setting blank (to use the default), that effectively tells VPinball to use two different devices and that isn't supported. So either both SoundDevice and SoundDeviceBG should both be left blank to use your system's default, or they should both be set to exactly the same value."
    },
    {"PlayMusic",
        "Setting this to 0 disables Music from being played, 1 enables Music (the default).\n"
        "Music sounds are those routed through SoundDeviceBG and includes not only music but backglass sounds plus PinMAME and PUP audio."
    },
    {"PlaySound",
        "Setting this to 0 disables Table sounds from being played, 1 enables Table sounds (the default).\n"
        "Table sounds are those sounds routed through SoundDevice like those from flipper and bumpers."
    },
    {"MusicVolume",
        "An integer number in the range of 0 - 100 for those sounds defined as Music above.\n"
        "The default value is 100 (maximum volume)."
    },
    {"SoundVolume",
        "An integer number in the range of 0 - 100 for the Table mechanical sounds as defined above.\n"
        "The default value is 100 (full volume)"
    },
    {"BGSet",
        "Controls the display mode for the backglass and the playfield.\n"
        "0 - Desktop (default): The default desktop mode for standard use.\n"
        "1 - Fullscreen: For cabinet use or multi-window on desktop setups.\n"
        "2 - Full Single Screen (FSS): Uses the entire screen for both playfield and backglass. Falls back to desktop view if unavailable."
    },
    {"PBWEnabled",
        "This enables analog nudging from hardware acceleration sensors, both from purpose-built controllers like the KL25Z-based Pinscape or from video game console controllers like the Playstation 4 Dualshock.\n"
        "Setting this to 0 disables the acceleration sensor, 1 enables it (the default)."
    },
    {"PBWAccelGainX",
        "This adjusts the accelerometer sensitivity on the X-axis representing left & right nudging expressed as a percentage in the range of 0 - 150%. The default value is the maximum of 150% (or multiply the value by 1.5x).\n"
        "Set this value to zero to disable the accelerometer along the X-axis entirely."
    },
    {"PBWAccelGainY",
        "The accelerometer sensitivy on the Y-axis representing forward nudging. It is otherwise analogous to PBWAccelGainX. Setting this value to zero will disable forward nudging."
    },
    {"VisualLatencyCorrection",
        "Correction expressed in milliseconds applied to ball rendering to account for visual latency.\n"
        "Setting this value to 0 disables this correction, while 1 (the default) derives an appropriate value from the target FPS rate."
    },
    {"MaxPrerenderedFrames",
        "Pre-rendering frames is a technique used to help reduce lag in games.\n"
        "Leave this value at the default of 0 if you have enabled 'Low Latency' or 'Anti Lag' settings in your graphics driver. Otherwise experiment with a value of 1 or 2 for a chance of lag reduction at the cost of a small framerate reduction."
    },
    {"SyncMode",
        "Controls the synchronization method for the video display.\n"
        "0 - None: No synchronization, potentially resulting in tearing or stuttering.\n"
        "1 - Vertical Sync: Synchronizes the video output with the display refresh rate, preventing screen tearing at the cost of input latency.\n"
        "2 - Adaptive Sync: Syncs video output to the display refresh rate, except for late frames, which may result in occasional tearing.\n"
        "3 - Frame Pacing (default): Synchronizes the simulation with the video frame rate while keeping input latency low and dynamically adjusting frame rates."
    },
    {"OverrideTableEmissionScale",
        "Replace table's scene lighting emission scale setup, eventually based on automatic Day/Night computed from geographic position."
    },
    {"MaxFramerate",
        "Sets the maximum frame rate for the game.\n"
        "Defaults to the playfield display refresh rate, ensuring a smooth experience.\n"
        "0 - Unbound frame rate: Removes any frame rate cap, allowing the game to run as fast as possible based on hardware.\n"
        "The frame rate will never go below 24fps, ensuring at least minimal smoothness."
    },
    {"FXAA",
        "Controls the method used for anti-aliasing to smooth out jagged edges in the game visuals.\n"
        "0 - Disabled: No anti-aliasing applied, which may result in jagged edges.\n"
        "1 - Fast FXAA: Fast FXAA (Fast Approximate Anti-Aliasing) method for anti-aliasing, offering a good balance between performance and visual quality.\n"
        "2 - Standard FXAA: Standard FXAA method, offering better anti-aliasing than fast FXAA but with slightly more performance cost.\n"
        "3 - Quality FXAA: High-quality FXAA, providing better image smoothing at the cost of performance.\n"
        "4 - Fast NFAA: Fast NVIDIA Fast Approximate Anti-Aliasing (NFAA), a technique for smoother visuals on NVIDIA hardware.\n"
        "5 - Standard DLLA: Standard Dynamic Line Anti-Aliasing, a method designed for dynamic rendering of lines and edges.\n"
        "6 - Quality SMAA: High-quality Subpixel Morphological Anti-Aliasing (SMAA), a more advanced anti-aliasing method with high visual quality."
    },
    {"Sharpen",
        "Controls the sharpening effect applied to the game's visuals.\n"
        "0 - Disabled: No sharpening effect applied to the visuals.\n"
        "1 - CAS: Contrast Adaptive Sharpening, a method to enhance image details by adjusting contrast and edges.\n"
        "2 - Bilateral CAS: Bilateral Contrast Adaptive Sharpening, a variant of CAS that reduces noise and artifacts in the image while sharpening."
    },
    {"DisableAO",
        "Determines whether ambient occlusion (AO) is enabled or disabled. AO simulates realistic shading and lighting by darkening areas that are less exposed to light.\n"
        "0 = Enabled: Ambient occlusion is enabled, providing more realistic shadowing and depth.\n"
        "1 = Disabled: Ambient occlusion is disabled, potentially improving performance but sacrificing visual realism."
    },
    {"DynamicAO",
        "Controls the type of ambient occlusion used. This setting takes effect only when DisableAO is set to 0.\n"
        "0 = Static: Static ambient occlusion, where shadows are precomputed and don't change in real-time.\n"
        "1 = Dynamic: Dynamic ambient occlusion, where shadows and lighting change in real-time based on the scene's lighting conditions."
    },
    {"SSRefl",
        "Controls whether screen space reflections (SSR) are enabled or disabled. SSR is a technique for simulating reflections on shiny surfaces.\n"
        "0 - Disable screen space reflections: Disables SSR, resulting in no reflections on reflective surfaces.\n"
        "1 - Enable screen space reflections: Enables SSR, allowing for more realistic reflections on surfaces like water and mirrors."
    },
    {"PFReflection",
        "Determines the type of reflections on the playfield (game surface). The reflection settings apply to the ball and other game elements that interact with the playfield.\n"
        "0 - Disable reflections (default?): Disables reflections entirely on the playfield.\n"
        "1 - Balls only: Only the ball will reflect on the playfield surface.\n"
        "2 - Static only: Only static objects (such as the table or scenery) will have reflections.\n"
        "3 - Static and balls: Both static objects and the ball will reflect.\n"
        "4 - Static and unsynced dynamics: Static objects and unsynchronized dynamics (like the ball's interaction with moving elements) will have reflections.\n"
        "5 - Dynamic: Dynamic reflections will be applied to all game objects in the scene."
    },
    {"MaxTexDimension",
        "Sets the maximum dimension for textures. Higher values may improve visual quality but can decrease performance.\n"
        "0 - Unlimited (default): Textures can be any size, limited only by hardware capabilities."
    },
    {"AAFactor",
        "Controls the level of supersampling applied for anti-aliasing. Supersampling can improve visual quality by rendering the image at a higher resolution and then downsampling it.\n"
        "0.00 to 2.00: Specifies the supersampling factor, with higher values improving quality but requiring more performance."
    },
    {"MSAASamples",
        "Controls the number of samples for Multi-Sample Anti-Aliasing (MSAA), which helps reduce jagged edges and improves visual quality.\n"
        "0 - None: No MSAA applied, leading to possible jagged edges.\n"
        "4 - 4 samples: Applies 4 samples for MSAA, offering a balance of performance and quality.\n"
        "6 - 6 samples: Applies 6 samples for MSAA, improving image quality further but with a performance hit.\n"
        "8 - 8 samples: Applies 8 samples for the highest quality MSAA, with a significant performance cost."
    },
    {"ForceBloomOff",
        "Controls whether bloom effects (a lighting effect that creates a glowing halo around bright areas) are enabled or disabled.\n"
        "0 - Enabled: Bloom effects are enabled, enhancing bright areas of the screen for a more dramatic visual effect.\n"
        "1 - Disabled: Bloom effects are disabled, removing the glowing halo effect from bright areas."
    },
    {"ForceMotionBlurOff",
        "Controls whether motion blur is applied to the ball during gameplay. Motion blur can make fast-moving objects appear smoother but may reduce visual clarity.\n"
        "0 - Enabled: Ball motion blur is enabled, making the ball's movement appear smoother.\n"
        "1 - Disabled: Ball motion blur is disabled, resulting in sharper visuals but potentially making fast-moving objects look more jagged."
    },
    {"ForceAnisotropicFiltering",
        "Determines whether anisotropic filtering (AF) is enabled or disabled. AF improves the quality of textures viewed at steep angles, making them appear sharper and clearer.\n"
        "0 - Disabled: Anisotropic filtering is disabled, leading to potentially blurry textures at certain angles.\n"
        "1 - Enabled: Anisotropic filtering is enabled, improving the quality of textures, especially at angles."
    },
    {"BAMHeadTracking",
        "Makes the game appear as a 3D image without the need for 3D glasses.\n"
        "Note: BAM Head Tracking uses a PS3 Eye Camera and a 3 point Infrared LED hat.\n"
        "0 - Disabled.\n"
        "1 - Enabled."
    },
    {"GfxBackend",
        "Valid values are: Noop, Agc, Direct3D11, Direct3D12, Gnm, Metal, Nvn, OpenGLES, OpenGL, Vulkan, Default"
    },
    {"BallTrail",
        "Determines whether the ball is drawn with a trail behind it to more easily follow its motion.\n"
        "A setting of 0 disables ball trails, 1 turns them on (the default)."
    },
    {"BallTrailStrength",
        "Determines the length and intensity of the trail drawn behind the ball's path.\n"
        "It is a floating point value between 0.0 and 1.0, with a default of 0.5.\n"
        "The higher the value, the more intensely the ball's trail is drawn and the easier it is to follow. It only has an effect if BallTrail is enabled."
    },
    {"DynamicDayNight",
        "Activate this to switch the table brightness automatically based on your PC's clock and your geographic location.\n"
        "0 disables this feature (the default), 1 enables it. You will need to fill in the latitude and longitude for your location for this to work correctly.\n"
        "You may use openstreetmap.org or Google Maps to get these values in the correct format."
    },
    {"Latitude",
        "The latitude of your location. Specify only if you have DynamicDayNight enabled. Use a negative value if you are in the southern hemisphere."
    },
    {"Longitude",
        "The longitude of your location. Specify only if you have DynamicDayNight enabled. Use a negative value if you are in the western hemisphere."
    },
    {"NudgeStrength",
        "Changes the visual effect / screen shaking when nudging the table.\n"
        "This does not change the actual strength of the nudge. The default setting is 0.02."
    }
};

#endif // VPX_TOOLTIPS_H
