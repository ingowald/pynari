# // ======================================================================== //
# // Copyright 2024++ Ingo Wald                                               //
# // Copyright 2024++ Milan Jaros - IT4Innovations, VSB-TUO                   //
# //                                                                          //
# // Licensed under the Apache License, Version 2.0 (the "License");          //
# // you may not use this file except in compliance with the License.         //
# // You may obtain a copy of the License at                                  //
# //                                                                          //
# //     http://www.apache.org/licenses/LICENSE-2.0                           //
# //                                                                          //
# // Unless required by applicable law or agreed to in writing, software      //
# // distributed under the License is distributed on an "AS IS" BASIS,        //
# // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
# // See the License for the specific language governing permissions and      //
# // limitations under the License.                                           //
# // ======================================================================== //

import numpy as np
import dearpygui.dearpygui as dpg

# Initial control points (normalized X in [0,1], Y as opacity in [0,1])
control_points = [
    [0.0, 0.0],
    [0.25, 0.25],
    [0.5, 0.5],
    [0.75, 0.75],
    [1.0, 1.0]
]

RESOLUTION = 256
opacity_tf = np.zeros(RESOLUTION, dtype=np.float32)

# Update curve line and recompute opacity transfer function
def update_transfer_function():
    xs = [dpg.get_value(f"pt_{i}")[0] for i in range(len(control_points))]
    ys = [dpg.get_value(f"pt_{i}")[1] for i in range(len(control_points))]
    dpg.set_value("opacity_curve", [xs, ys])
    interp_x = np.linspace(0.0, 1.0, RESOLUTION)
    interp_y = np.interp(interp_x, xs, ys)
    global opacity_tf
    opacity_tf = np.clip(interp_y, 0.0, 1.0)
    #print("Updated transfer function:", opacity_tf)

# Drag point callback
def on_drag(sender, app_data, user_data):
    update_transfer_function()

def init_show():
    # Setup
    dpg.create_context()

    # GUI
    with dpg.window(label="Transfer Function Editor", width=600, height=300):
        with dpg.plot(label="Opacity Curve", height=-1, width=-1, tag="tf_plot"):

            def lerp_color(color1, color2, t):
                return [
                    int(color1[0] + (color2[0] - color1[0]) * t),
                    int(color1[1] + (color2[1] - color1[1]) * t),
                    int(color1[2] + (color2[2] - color1[2]) * t),
                    int(color1[3] + (color2[3] - color1[3]) * t),
                ]        

            with dpg.draw_layer(parent="tf_plot", tag="gradient_layer", before="x_axis"):
                start_x = 0
                end_x = 1
                y1 = 0
                y2 = 1
                steps = RESOLUTION

                color_start = [0, 0, 255, 50]     # Blue
                color_end = [255, 0, 0, 50]   # Red

                for i in range(steps):
                    t = i / steps
                    x = t #start_x + (end_x - start_x) * t
                    #next_x = start_x + (end_x - start_x) * (i + 1) / steps
                    color = lerp_color(color_start, color_end, t)
                    dpg.draw_line([x, y1], [x, y2], color=color, thickness=2.0/steps, parent="gradient_layer")

            dpg.add_plot_axis(dpg.mvXAxis, label="Scalar", tag="x_axis")
            with dpg.plot_axis(dpg.mvYAxis, label="Opacity", tag="y_axis"):
                dpg.set_axis_limits("x_axis", 0.0, 1.0)
                dpg.set_axis_limits("y_axis", 0.0, 1.0)

                # Curve line
                dpg.add_line_series(
                    x=[p[0] for p in control_points],
                    y=[p[1] for p in control_points],
                    label="Opacity Curve",
                    parent="y_axis",
                    tag="opacity_curve"
                )

            # Draggable control points
            for i, point in enumerate(control_points):
                dpg.add_drag_point(
                    tag=f"pt_{i}",
                    default_value=point,
                    label=f"Pt{i}",
                    color=[255, 0, 0, 255],
                    callback=on_drag,
                    user_data=i,
                    parent="tf_plot"
                )

    # Initialize and show
    update_transfer_function()
    dpg.create_viewport(title="Transfer Function", width=640, height=360)
    dpg.setup_dearpygui()
    dpg.show_viewport()
    dpg.start_dearpygui()
    dpg.destroy_context()
