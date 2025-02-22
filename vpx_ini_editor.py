import os
import tkinter as tk
from tkinter import messagebox
from tkinter import ttk
from tkinter import filedialog

# Set the ini_file path here (use the full path if needed)
INI_FILE_PATH = os.path.expandvars("$HOME/.vpinball/VPinballX.ini")  # Expands $HOME

def read_ini_preserve_keys(filename):
    """
    Reads an INI file and returns a dictionary mapping section names to an ordered
    dictionary of key/value pairs. This manual parser preserves key capitalization.
    Note: Comments and blank lines are ignored.
    """
    data = {}
    current_section = None
    with open(filename, 'r') as f:
        for line in f:
            stripped = line.strip()
            if not stripped or stripped.startswith(';'):
                continue
            if stripped.startswith('[') and stripped.endswith(']'):
                current_section = stripped[1:-1]
                data[current_section] = {}
            elif '=' in line and current_section is not None:
                key, value = line.split('=', 1)
                key = key.strip()  # Preserve original case
                value = value.strip()
                data[current_section][key] = value
    return data

def write_ini_preserve_keys(filename, data):
    """
    Writes the INI data (a dict mapping section names to dicts of key/value pairs)
    back to a file, preserving comments and blank lines.
    """
    with open(filename, 'r') as f:
        lines = f.readlines()

    with open(filename, 'w') as f:
        current_section = None
        for line in lines:
            stripped = line.strip()

            # Preserve comments and blank lines
            if not stripped or stripped.startswith(';'):
                f.write(line)
                continue

            # Handle section headers
            if stripped.startswith('[') and stripped.endswith(']'):
                current_section = stripped[1:-1]
                f.write(line)
                continue

            # Handle key=value pairs
            if '=' in line and current_section:
                key, value = line.split('=', 1)
                key = key.strip()  # Preserve original case
                value = value.strip()

                # Update the value if the key exists in the data
                if key in data[current_section]:
                    f.write(f'{key} = {data[current_section][key]}\n')
                else:
                    f.write(f'{key} = {value}\n')
            else:
                # For other lines, just write them as is
                f.write(line)

class ToolTip:
    """
    A tooltip that appears when the widget is clicked.
    Uses a Text widget to handle long explanations.
    """
    def __init__(self, widget, text):
        self.widget = widget
        self.text = text
        self.widget.bind("<Button-1>", self.showtip)  # Bind to click event (left-click)

    def showtip(self, event=None):
        # Create a Toplevel window for the explanation pop-up
        if self.text:
            tipwindow = tk.Toplevel(self.widget)
            tipwindow.wm_overrideredirect(True)  # Removes the window borders

            # Set the window geometry and position it near the widget
            x = self.widget.winfo_rootx() + 20
            y = self.widget.winfo_rooty() + self.widget.winfo_height() + 10
            tipwindow.wm_geometry(f"+{x}+{y}")

            # Create a Text widget to display the explanation, set word wrapping to true
            text_widget = tk.Text(tipwindow, wrap=tk.WORD, height=10, width=50, font=("tahoma", "8", "normal"))
            text_widget.insert(tk.END, self.text)
            text_widget.config(state=tk.DISABLED)  # Make the Text widget read-only
            text_widget.pack(padx=10, pady=10)

            # Add a close button for the pop-up window
            close_button = ttk.Button(tipwindow, text="Close", command=tipwindow.destroy)
            close_button.pack(pady=5)  # Increase padding around the button

            # Automatically adjust window size to fit the content
            tipwindow.update_idletasks()
            width = text_widget.winfo_width() + 40  # Add padding to width
            height = text_widget.winfo_height() + 60  # Add extra space for the button
            tipwindow.geometry(f"{width}x{height}+{x}+{y}")  # Adjust the window size


class IniEditor:
    '''
    The `IniEditor` class reads the INI file, preserves key capitalization, and displays sections and key-value pairs.
    Users can select sections, edit values, and save changes back to the INI file.
    The class also includes explanations for specific keys, displayed as tooltips when users click on a "?" button next to each key.
    '''
    def __init__(self, master, ini_file):
        self.master = master
        self.ini_file = ini_file
        # Read the INI file using our custom parser to preserve key capitalization
        self.ini_data = read_ini_preserve_keys(self.ini_file)

        # Define explanations for specific keys.
        # Keys here must exactly match the key names in your INI file.
        self.explanations = {
            # STANDALONE
            "PinMAMEPath": "Specifies the directory where the PinMAME emulator is located. PinMAME is an emulator for classic pinball machines.\n"
                "Default is ~/.pinmame: If not specified, the default location for the PinMAME folder is in the home directory.",
            "PinMAMEWindow": "Controls the visibility of the PinMAME DMD (Dot Matrix Display) window.\n"
                "0 - Turns the DMD window off: Disables the PinMAME DMD window, which is often used for displaying the score and game information.\n"
                "1 - Turns the DMD window on: Enables the PinMAME DMD window to display relevant game information. (Default is 1)",
            "PinMAMEWindowX": "Sets the X pixel coordinate of the top-left corner of the PinMAME DMD window on the screen.\n"
                "This controls the horizontal position where the DMD window appears on the screen.",
            "PinMAMEWindowY": "Sets the Y pixel coordinate of the top-left corner of the PinMAME DMD window on the screen.\n"
                "This controls the vertical position where the DMD window appears on the screen.",
            "PinMAMEWindowWidth": "Controls the width of the PinMAME DMD window.\n"
                "You can resize the DMD window by specifying the width in pixels.",
            "PinMAMEWindowHeight": "Controls the height of the PinMAME DMD window.\n"
                "You can resize the DMD window by specifying the height in pixels.",
            "PinMAMEWindowRotation": "Controls the rotation of the PinMAME DMD window.\n"
                "This setting can rotate the DMD window to a specified angle, useful for certain display setups.",
            "B2SHideGrill": "Controls the visibility of the grill in the backbox of the cabinet setup.\n"
                "0 - Show grill (if it exists): Displays the grill, a common component in pinball machines.\n"
                "1 - Hide grill: Hides the grill.",
            "B2SHideB2SDMD": "Controls the visibility of the extra DMD frame in the backbox.\n"
                "0 - Show extra DMD frame (if it exists): Displays the DMD frame around the Dot Matrix Display.\n"
                "1 - Hide extra DMD frame: Hides the DMD frame, leaving just the DMD window.",
            "B2SHideB2SBackglass": "Controls the visibility of the backglass in a cabinet setup.\n"
                "0 - Show backglass: Displays the backglass, which often shows game artwork.\n"
                "1 - Hide backglass: Hides the backglass, potentially saving space in a multi-display setup.",
            "B2SWindows": "Controls the visibility of various B2S windows such as the grill, DMD frame, and backglass.\n"
                "0 - Hide all B2S windows: Hides the grill, DMD frame, and backglass.\n"
                "1 - Show B2S windows according to other settings: Displays the grill, DMD frame, and backglass according to the other settings.",
            "B2SBackglassX": "Sets the X pixel coordinate of the top-left corner of the backglass window.\n"
                "This allows you to position the backglass at a specific horizontal position on the screen.",
            "B2SBackglassY": "Sets the Y pixel coordinate of the top-left corner of the backglass window.\n"
                "This allows you to position the backglass at a specific vertical position on the screen.",
            "B2SBackglassWidth": "Controls the pixel width of the backglass window.\n"
                "This defines how wide the backglass window will appear on the screen.",
            "B2SBackglassHeight": "Controls the pixel height of the backglass window.\n"
                "This defines how tall the backglass window will appear on the screen.",
            "B2SBackglassRotation": "Controls the rotation of the backglass window.\n"
                "Supposed to rotate the backglass, but this feature may not work in certain environments (e.g., macOS).",
            "B2SDMDX": "Sets the X pixel coordinate of the top-left corner of the DMD frame.\n"
                "This controls the horizontal positioning of the DMD frame relative to the screen.",
            "B2SDMDY": "Sets the Y pixel coordinate of the top-left corner of the DMD frame.\n"
                "This controls the vertical positioning of the DMD frame relative to the screen.",
            "B2SDMDWidth": "Controls the pixel width of the DMD frame.\n"
                "This defines how wide the DMD frame will appear on the screen.",
            "B2SDMDHeight": "Controls the pixel height of the DMD frame.\n"
                "This defines how tall the DMD frame will appear on the screen.",
            "B2SDMDRotation": "Controls the rotation of the DMD frame.\n"
                "Supposed to rotate the DMD frame, but this feature may not work in certain environments (e.g., macOS).",
            "B2SDMDFlipY": "Controls whether the DMD frame is flipped vertically.\n"
                "0 - No flip: The DMD frame appears normally.\n"
                "1 - Flip vertically: Flips the DMD frame vertically, which may be useful for specific screen orientations.",
            # PLAYER
            "BGSet": "Controls the display mode for the backglass and the playfield.\n"
                "0 - Desktop (default): The default desktop mode for standard use.\n"
                "1 - Fullscreen: For cabinet use or multi-window on desktop setups.\n"
                "2 - Full Single Screen (FSS): Uses the entire screen for both playfield and backglass. Falls back to desktop view if unavailable.",
            "SyncMode": "Controls the synchronization method for the video display.\n"
                "0 - None: No synchronization, potentially resulting in tearing or stuttering.\n"
                "1 - Vertical Sync: Synchronizes the video output with the display refresh rate, preventing screen tearing at the cost of input latency.\n"
                "2 - Adaptive Sync: Syncs video output to the display refresh rate, except for late frames, which may result in occasional tearing.\n"
                "3 - Frame Pacing (default): Synchronizes the simulation with the video frame rate while keeping input latency low and dynamically adjusting frame rates.",
            "MaxFramerate": "Sets the maximum frame rate for the game.\n"
                "Defaults to the playfield display refresh rate, ensuring a smooth experience.\n"
                "0 - Unbound frame rate: Removes any frame rate cap, allowing the game to run as fast as possible based on hardware.\n"
                "The frame rate will never go below 24fps, ensuring at least minimal smoothness.",
            "FXAA": "Controls the method used for anti-aliasing to smooth out jagged edges in the game visuals.\n"
                "0 - Disabled: No anti-aliasing applied, which may result in jagged edges.\n"
                "1 - Fast FXAA: Fast FXAA (Fast Approximate Anti-Aliasing) method for anti-aliasing, offering a good balance between performance and visual quality.\n"
                "2 - Standard FXAA: Standard FXAA method, offering better anti-aliasing than fast FXAA but with slightly more performance cost.\n"
                "3 - Quality FXAA: High-quality FXAA, providing better image smoothing at the cost of performance.\n"
                "4 - Fast NFAA: Fast NVIDIA Fast Approximate Anti-Aliasing (NFAA), a technique for smoother visuals on NVIDIA hardware.\n"
                "5 - Standard DLLA: Standard Dynamic Line Anti-Aliasing, a method designed for dynamic rendering of lines and edges.\n"
                "6 - Quality SMAA: High-quality Subpixel Morphological Anti-Aliasing (SMAA), a more advanced anti-aliasing method with high visual quality.",
            "Sharpen": "Controls the sharpening effect applied to the game's visuals.\n"
                "0 - Disabled: No sharpening effect applied to the visuals.\n"
                "1 - CAS: Contrast Adaptive Sharpening, a method to enhance image details by adjusting contrast and edges.\n"
                "2 - Bilateral CAS: Bilateral Contrast Adaptive Sharpening, a variant of CAS that reduces noise and artifacts in the image while sharpening.",
            "DisableAO": "Determines whether ambient occlusion (AO) is enabled or disabled. AO simulates realistic shading and lighting by darkening areas that are less exposed to light.\n"
                "0 = Enabled: Ambient occlusion is enabled, providing more realistic shadowing and depth.\n"
                "1 = Disabled: Ambient occlusion is disabled, potentially improving performance but sacrificing visual realism.",
            "DynamicAO": "Controls the type of ambient occlusion used. This setting takes effect only when DisableAO is set to 0.\n"
                "0 = Static: Static ambient occlusion, where shadows are precomputed and don't change in real-time.\n"
                "1 = Dynamic: Dynamic ambient occlusion, where shadows and lighting change in real-time based on the scene's lighting conditions.",
            "SSRefl": "Controls whether screen space reflections (SSR) are enabled or disabled. SSR is a technique for simulating reflections on shiny surfaces.\n"
                "0 - Disable screen space reflections: Disables SSR, resulting in no reflections on reflective surfaces.\n"
                "1 - Enable screen space reflections: Enables SSR, allowing for more realistic reflections on surfaces like water and mirrors.",
            "PFReflection": "Determines the type of reflections on the playfield (game surface). The reflection settings apply to the ball and other game elements that interact with the playfield.\n"
                "0 - Disable reflections (default?): Disables reflections entirely on the playfield.\n"
                "1 - Balls only: Only the ball will reflect on the playfield surface.\n"
                "2 - Static only: Only static objects (such as the table or scenery) will have reflections.\n"
                "3 - Static and balls: Both static objects and the ball will reflect.\n"
                "4 - Static and unsynced dynamics: Static objects and unsynchronized dynamics (like the ball's interaction with moving elements) will have reflections.\n"
                "5 - Dynamic: Dynamic reflections will be applied to all game objects in the scene.",
            "MaxTexDimension": "Sets the maximum dimension for textures. Higher values may improve visual quality but can decrease performance.\n"
                "0 - Unlimited (default): Textures can be any size, limited only by hardware capabilities.",
            "AAFactor": "Controls the level of supersampling applied for anti-aliasing. Supersampling can improve visual quality by rendering the image at a higher resolution and then downsampling it.\n"
                "0.00 to 2.00: Specifies the supersampling factor, with higher values improving quality but requiring more performance.",
            "MSAASamples": "Controls the number of samples for Multi-Sample Anti-Aliasing (MSAA), which helps reduce jagged edges and improves visual quality.\n"
                "0 - None: No MSAA applied, leading to possible jagged edges.\n"
                "4 - 4 samples: Applies 4 samples for MSAA, offering a balance of performance and quality.\n"
                "6 - 6 samples: Applies 6 samples for MSAA, improving image quality further but with a performance hit.\n"
                "8 - 8 samples: Applies 8 samples for the highest quality MSAA, with a significant performance cost.",
            "ForceBloomOff": "Controls whether bloom effects (a lighting effect that creates a glowing halo around bright areas) are enabled or disabled.\n"
                "0 - Enabled: Bloom effects are enabled, enhancing bright areas of the screen for a more dramatic visual effect.\n"
                "1 - Disabled: Bloom effects are disabled, removing the glowing halo effect from bright areas.",
            "ForceMotionBlurOff": "Controls whether motion blur is applied to the ball during gameplay. Motion blur can make fast-moving objects appear smoother but may reduce visual clarity.\n"
                "0 - Enabled: Ball motion blur is enabled, making the ball's movement appear smoother.\n"
                "1 - Disabled: Ball motion blur is disabled, resulting in sharper visuals but potentially making fast-moving objects look more jagged.",
            "ForceAnisotropicFiltering": "Determines whether anisotropic filtering (AF) is enabled or disabled. AF improves the quality of textures viewed at steep angles, making them appear sharper and clearer.\n"
                "0 - Disabled: Anisotropic filtering is disabled, leading to potentially blurry textures at certain angles.\n"
                "1 - Enabled: Anisotropic filtering is enabled, improving the quality of textures, especially at angles.",
        }

        self.entry_widgets = {}
        self.setup_ui()

    def setup_ui(self):
        '''
        Sets up the user interface with section selection, key-value entry fields, and a save button.
        '''
        # Section selection frame
        section_frame = ttk.Frame(self.master)
        section_frame.pack(fill=tk.X, padx=10, pady=5)
        section_label = ttk.Label(section_frame, text="Select Section:")
        section_label.pack(side=tk.LEFT)
        self.section_var = tk.StringVar()
        self.section_combo = ttk.Combobox(section_frame, textvariable=self.section_var, state="readonly")
        self.section_combo['values'] = list(self.ini_data.keys())
        if self.ini_data:
            first_section = list(self.ini_data.keys())[0]
            self.section_combo.current(0)
        self.section_combo.pack(side=tk.LEFT, padx=5)
        self.section_combo.bind("<<ComboboxSelected>>", self.on_section_change)

        # Frame for key/value entries with scrolling
        self.entries_frame = ttk.Frame(self.master)
        self.entries_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)
        self.canvas = tk.Canvas(self.entries_frame)
        self.canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar = ttk.Scrollbar(self.entries_frame, orient="vertical", command=self.canvas.yview)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        self.canvas.configure(yscrollcommand=scrollbar.set)
        self.inner_frame = ttk.Frame(self.canvas)
        self.canvas.create_window((0,0), window=self.inner_frame, anchor="nw")
        self.inner_frame.bind("<Configure>", lambda event: self.canvas.configure(scrollregion=self.canvas.bbox("all")))

        # Create a frame to hold buttons side by side
        button_frame = ttk.Frame(self.master)
        button_frame.pack(pady=10)

        # Save button
        save_button = ttk.Button(button_frame, text="Save", command=self.save_changes)
        save_button.pack(side=tk.LEFT, padx=5)

        # Change INI button
        change_ini_button = ttk.Button(button_frame, text="Change INI", command=self.change_ini)
        change_ini_button.pack(side=tk.LEFT, padx=5)

        # Exit button
        exit_button = ttk.Button(button_frame, text="Exit", command=self.master.quit)
        exit_button.pack(side=tk.LEFT, padx=5)

        if self.ini_data:
            first_section = list(self.ini_data.keys())[0]
            self.load_section(first_section)

    def update_title(self):
        '''
        Updates the window title based on the currently selected INI file.
        '''
        self.master.title(f"INI File Editor - {self.ini_file}")

    def change_ini(self):
        '''
        Opens a file dialog to choose a new INI file and update the UI accordingly.
        '''
        new_ini_file = filedialog.askopenfilename(
            title="Select INI File", filetypes=[("INI Files", "*.ini"), ("All Files", "*.*")]
        )
        if new_ini_file:
            self.ini_file = new_ini_file
            self.ini_data = read_ini_preserve_keys(self.ini_file)
            self.load_section(list(self.ini_data.keys())[0])  # Load the first section
            self.update_title()  # Update the window title after changing the INI file


    def on_section_change(self, event):
        '''
        Event handler for section selection change.
        '''
        section = self.section_var.get()
        self.load_section(section)

    def load_section(self, section):
        '''
        Loads the key-value pairs for the selected section and displays them in the UI.
        '''
        # Clear existing widgets
        for widget in self.inner_frame.winfo_children():
            widget.destroy()
        self.entry_widgets = {}

        if section not in self.ini_data:
            return
        section_data = self.ini_data[section]

        # Repack the canvas window if necessary
        self.canvas.yview_moveto(0)  # Scroll to top after section change

        # Set a standard padding and row height
        row_height = 30  # Adjust row height to control vertical spacing

        for i, (key, value) in enumerate(section_data.items()):
            # Label for key (fixed width to avoid stretching)
            label = ttk.Label(self.inner_frame, text=key, anchor='w')  # Label for key
            label.grid(row=i, column=0, sticky=tk.W, padx=5, pady=2)

            # Add a clickable "?" button if an explanation is provided
            if key in self.explanations:
                tooltip_label = ttk.Label(self.inner_frame, text="?", foreground="blue", cursor="question_arrow")
                tooltip_label.grid(row=i, column=1, padx=5, pady=2)
                ToolTip(tooltip_label, self.explanations[key])  # Bind tooltip to the "?" label

            # Entry field for value (increase width here for longer fields)
            entry = ttk.Entry(self.inner_frame, width=35)  # Increased width to make text field longer
            entry.grid(row=i, column=2, padx=5, pady=2, sticky='ew')  # 'ew' ensures it expands horizontally
            entry.insert(0, value)
            self.entry_widgets[key] = entry

        # Update column weights to ensure proper layout
        self.inner_frame.grid_columnconfigure(0, weight=1, minsize=150)  # Make the key column flexible
        self.inner_frame.grid_columnconfigure(1, weight=0)  # "?" column doesn't need to stretch
        self.inner_frame.grid_columnconfigure(2, weight=2, minsize=300)  # Entry column flexible but fixed min size

        # Ensure the canvas scrolls correctly after updating the section
        self.inner_frame.update_idletasks()  # Update the layout
        self.canvas.config(scrollregion=self.canvas.bbox("all"))  # Set the scroll region

    def save_changes(self):
        '''
        Saves the changes made in the UI back to the INI file.
        '''
        section = self.section_var.get()
        if section not in self.ini_data:
            messagebox.showerror("Error", f"Section '{section}' not found.")
            return
        for key, entry in self.entry_widgets.items():
            self.ini_data[section][key] = entry.get()
        write_ini_preserve_keys(self.ini_file, self.ini_data)
        messagebox.showinfo("Success", f"Changes saved to {self.ini_file}")

def main():
    # Check if the INI file exists
    ini_file = INI_FILE_PATH

    if not os.path.exists(ini_file):
        messagebox.showerror("Error", f"INI file not found: {ini_file}")
        return

    root = tk.Tk()
    root.title(f"INI Editor - {ini_file}")  # Set window title with the ini file path
    root.geometry("700x500")

    app = IniEditor(root, ini_file)
    root.mainloop()

if __name__ == "__main__":
    main()
