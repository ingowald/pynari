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

# pip install glfw PyOpenGL PyOpenGL_accelerate numpy
import glfw
from OpenGL.GL import *
from OpenGL.GLU import *
import ctypes
import numpy as np
import threading

"""
ANARI Viewer - Python OpenGL Integration

This module provides an interactive viewer for ANARI-rendered content using OpenGL.
It creates a window that displays the rendered output from ANARI and allows camera
navigation using keyboard and mouse controls.

Navigation Controls:
- Left Mouse Button: Rotate camera
- Middle/Right Mouse Button: Pan camera
- Mouse Wheel: Zoom in/out
- WASD: Move camera forward/backward/left/right
- QE: Move camera up/down

Dependencies:
- glfw: Window creation and input handling
- OpenGL: Rendering interface
- numpy: Array manipulation
- anari_scene: ANARI rendering backend (custom module)
"""

# Window and camera state
w_width, w_height = 800, 600
yaw, pitch = -90.0, 0.0
distance = 5.0
lastX, lastY = 400, 300
left_pressed = False
middle_pressed = False
pan_offset = np.array([0.0, 0.0, 0.0])
move_speed = 0.1
keys = {}

# OpenGL resources
g_textureId = -1
g_bufferId = -1
g_width = w_width
g_height = w_height

def setup_texture(width, height):
    """Initialize OpenGL texture and buffer for rendering"""
    global g_textureId, g_bufferId
    
    # Create texture
    g_textureId = glGenTextures(1)
    glBindTexture(GL_TEXTURE_2D, g_textureId)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, 
                GL_RGBA, GL_UNSIGNED_BYTE, None)
    glBindTexture(GL_TEXTURE_2D, 0)
    
    # Create pixel buffer object (PBO)
    g_bufferId = glGenBuffers(1)
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, g_bufferId)
    glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * 4, None, GL_DYNAMIC_COPY)
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0)


def free_texture():
    """Release OpenGL resources"""
    global g_textureId, g_bufferId
    
    if g_textureId != -1:
        glDeleteTextures(1, [g_textureId])
    
    if g_bufferId != -1:
        glDeleteBuffers(1, [g_bufferId])


def to_ortho(width, height):
    """Set up orthographic projection for texture rendering"""
    glViewport(0, 0, width, height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    glOrtho(0, 1, 0, 1, -1, 1)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()


def draw_texture(anari_scene):
    """Render the ANARI framebuffer as a textured quad"""
    global g_width, g_height, g_textureId, g_bufferId

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)
    to_ortho(g_width, g_height)
    
    # Enable texturing
    glEnable(GL_TEXTURE_2D)
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, g_bufferId)

    # Update texture with new frame data
    if anari_scene.use_cuda_glinterop():
        import pycuda.gl.autoinit
        import pycuda.driver as cuda
        import pycuda.gl as cudagl

        pixels_buf_d = cudagl.RegisteredBuffer(int(g_bufferId), cudagl.graphics_map_flags.NONE)
        mapping = pixels_buf_d.map()
        device_ptr = mapping.device_ptr_and_size()[0]
        
        # Get frame data from ANARI
        # mj:
        #data_d = anari_scene.anari_fb_map()
        #cuda.memcpy_dtod(device_ptr, data_d, g_width * g_height * 4)
        #anari_scene.anari_fb_unmap()
        # iw:
        anari_scene.anari_fb_readGPU(device_ptr)
        
        mapping.unmap()        
        pixels_buf_d.unregister()
    else:
        data = anari_scene.anari_fb_map()
        # Create array view of the data
        pixels = np.ctypeslib.as_array(
            ctypes.cast(data, ctypes.POINTER(ctypes.c_ubyte)),
            shape=(g_height * g_width * 4,)
        )        
        glBufferSubData(GL_PIXEL_UNPACK_BUFFER, 0, g_width * g_height * 4, pixels)
        anari_scene.anari_fb_unmap()

    # Update texture from PBO
    glBindTexture(GL_TEXTURE_2D, g_textureId)
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, g_width, g_height, 
                    GL_RGBA, GL_UNSIGNED_BYTE, None)
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0)

    # Draw textured quad
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE)
    glBegin(GL_QUADS)
    glColor4f(1.0, 1.0, 1.0, 1.0)
    
    glTexCoord2f(0.0, 1.0)
    glVertex2f(0.0, 0.0)
    
    glTexCoord2f(1.0, 1.0)
    glVertex2f(1.0, 0.0)
    
    glTexCoord2f(1.0, 0.0)
    glVertex2f(1.0, 1.0)
    
    glTexCoord2f(0.0, 0.0)
    glVertex2f(0.0, 1.0)
    
    glEnd()

    # Cleanup state
    glDisable(GL_TEXTURE_2D)
    glBindTexture(GL_TEXTURE_2D, 0)


def resize(width, height):
    """Handle window resize by recreating textures"""
    global g_width, g_height, g_textureId
    
    if width == g_width and height == g_height and g_textureId != -1:
        return
    
    if g_textureId != -1:
        free_texture()
    
    g_width = width
    g_height = height
    setup_texture(width, height)


def key_callback(window, key, scancode, action, mods):
    """Track keyboard state"""
    global keys
    
    if action == glfw.PRESS:
        keys[key] = True
    elif action == glfw.RELEASE:
        keys[key] = False


def process_keyboard_input():
    """Process keyboard input for camera movement"""
    global pan_offset
    
    direction = get_camera_direction()
    right = np.cross(direction, np.array([0, 1, 0]))
    right = right / np.linalg.norm(right)
    up = np.array([0, 1, 0])
    
    if keys.get(glfw.KEY_W, False):
        pan_offset += direction * move_speed
    if keys.get(glfw.KEY_S, False):
        pan_offset -= direction * move_speed
    if keys.get(glfw.KEY_D, False):
        pan_offset -= right * move_speed
    if keys.get(glfw.KEY_A, False):
        pan_offset += right * move_speed
    if keys.get(glfw.KEY_Q, False):
        pan_offset += up * move_speed
    if keys.get(glfw.KEY_E, False):
        pan_offset -= up * move_speed        


def framebuffer_size_callback(window, width, height):
    """Handle window resize"""
    global w_width, w_height
    w_width, w_height = glfw.get_framebuffer_size(window)


def mouse_button_callback(window, button, action, mods):
    """Track mouse button state"""
    global left_pressed, middle_pressed
    if button == glfw.MOUSE_BUTTON_LEFT:
        left_pressed = action == glfw.PRESS
    elif button == glfw.MOUSE_BUTTON_RIGHT or button == glfw.MOUSE_BUTTON_MIDDLE:
        middle_pressed = action == glfw.PRESS


def cursor_position_callback(window, xpos, ypos):
    """Handle mouse movement for camera control"""
    global lastX, lastY, yaw, pitch, pan_offset
    xoffset = xpos - lastX
    yoffset = lastY - ypos
    lastX, lastY = xpos, ypos

    if left_pressed:
        yaw -= xoffset * 0.3
        pitch += yoffset * 0.3
        pitch = np.clip(pitch, -89.0, 89.0)
    elif middle_pressed:
        right = np.cross(get_camera_direction(), np.array([0, 1, 0]))
        up = np.array([0, 1, 0])
        pan_offset += (right * xoffset - up * yoffset) * 0.005


def scroll_callback(window, xoffset, yoffset):
    """Handle mouse wheel for zooming"""
    global distance
    distance -= yoffset * 0.5
    distance = max(1.0, distance)


def get_camera_direction():
    """Calculate camera direction vector from yaw and pitch"""
    front = np.array([
        np.cos(np.radians(yaw)) * np.cos(np.radians(pitch)),
        np.sin(np.radians(pitch)),
        np.sin(np.radians(yaw)) * np.cos(np.radians(pitch))
    ])
    return front / np.linalg.norm(front)


def init_window():
    """Initialize GLFW window and set up callbacks"""
    if not glfw.init():
        return None
    
    window = glfw.create_window(w_width, w_height, "pynari", None, None)
    if not window:
        glfw.terminate()
        return None
    
    glfw.make_context_current(window)
    glfw.set_framebuffer_size_callback(window, framebuffer_size_callback)
    glfw.set_cursor_pos_callback(window, cursor_position_callback)
    glfw.set_mouse_button_callback(window, mouse_button_callback)
    glfw.set_scroll_callback(window, scroll_callback)
    glfw.set_key_callback(window, key_callback)
    framebuffer_size_callback(window, 0, 0)

    return window


def run_glfw(anari_scene):
    """Main application loop"""
    window = init_window()
    if not window:
        return

    anari_scene.anari_init(w_width, w_height)
    glEnable(GL_DEPTH_TEST)

    frame_count = 0
    last_time = glfw.get_time()
    fps = 0    

    while not glfw.window_should_close(window):
        process_keyboard_input()
        resize(w_width, w_height)

        # Calculate camera parameters for ANARI
        dir = get_camera_direction()
        eye = -dir * distance + pan_offset        
        fovy = 60.0
        
        # Render the scene with ANARI
        anari_scene.anari_render(g_width, g_height, eye, dir, fovy)

        # Draw the rendered image
        draw_texture(anari_scene)
        glfw.swap_buffers(window)
        glfw.poll_events()

        # FPS calculation
        frame_count += 1
        current_time = glfw.get_time()
        if current_time - last_time >= 1.0:
            fps = frame_count
            glfw.set_window_title(window, f"pynari - FPS: {fps}")
            frame_count = 0
            last_time = current_time

    glfw.terminate()

###############################    
def run_tf(anari_scene):
    import examples.anari_tf
    examples.anari_tf.init_show()

def run(anari_scene):
    if anari_scene.use_dearpygui_tf():
        # Launch GLFW OpenGL in a separate thread
        glfw_thread = threading.Thread(target=lambda: run_glfw(anari_scene), daemon=True)
        glfw_thread.start()

        run_tf(anari_scene)
    else:
        # Run GLFW in the main thread
        run_glfw(anari_scene)

def main():
    # import examples.sample01  # Import ANARI scene module
    # anari_scene = examples.sample01.AnariScene()
    # run(anari_scene)
    
    # import examples.sample02  # Import ANARI scene module
    # anari_scene = examples.sample02.AnariScene()
    # run(anari_scene)

    # import examples.sample04  # Import ANARI scene module
    # anari_scene = examples.sample04.AnariScene()
    # run(anari_scene)

    # import examples.sample05  # Import ANARI scene module
    # anari_scene = examples.sample05.AnariScene()
    # run(anari_scene)

    # import examples.sample07  # Import ANARI scene module
    # anari_scene = examples.sample07.AnariScene()
    # run(anari_scene)

    # import examples.sample_volume_vdb  # Import ANARI scene module
    # anari_scene = examples.sample_volume_vdb.AnariScene()
    # run(anari_scene)

    import examples.sample_volume_sitk  # Import ANARI scene module
    anari_scene = examples.sample_volume_sitk.AnariScene()
    run(anari_scene)

    pass

if __name__ == "__main__":
    main()
