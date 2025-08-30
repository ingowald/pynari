import matplotlib.pyplot as plt
import numpy as np
import pynari as anari
import random
import time
from matplotlib.animation import FuncAnimation

if anari.has_cuda_capable_gpu():
   print('@pynari: detected cuda-capable GPU; using higher res and sample count')
   fb_size = (1600,800)
   num_paths_per_pixel = 8
else:
   print('@pynari: no CUDA-capable GPU detected, reducing sample count')
   fb_size = (800,400)
   num_paths_per_pixel = 2

look_from_initial = (13., 2., 3.)
look_at = np.array([0., 0., 0.])
look_up = np.array([0.,1.,0.])
fovy = 20.

print('@pynari: -------------------------------------------------------')
print('@pynari: running sample02 - interactive RTOW scene           ')
print('@pynari: DRAG MOUSE to orbit/tilt, SCROLL to zoom.           ')
print('@pynari: -------------------------------------------------------')
random.seed(80577)

# --- Helper functions ---

def add_sphere(pos, radius, material):
    global spheres
    geom = device.newGeometry('sphere')
    array = device.newArray1D(anari.FLOAT32_VEC3, np.array([pos], dtype=np.float32))
    geom.setParameter('vertex.position', anari.ARRAY1D, array)
    geom.setParameter('radius', anari.FLOAT32, radius)
    geom.commitParameters()

    surf = device.newSurface()
    surf.setParameter('geometry', anari.GEOMETRY, geom)
    surf.setParameter('material', anari.MATERIAL, material)
    surf.commitParameters()
    spheres.append(surf)
    return geom

def make_lambertian(r,g,b):
    mat = device.newMaterial('matte')
    mat.setParameter('color',anari.float3,(r,g,b))
    mat.commitParameters()
    return mat

def make_dielectric(ior):
    mat = device.newMaterial('physicallyBased')
    mat.setParameter('baseColor',anari.float3,(1.,1.,1.))
    mat.setParameter('transmission',anari.FLOAT32,1.)
    mat.setParameter('specular',anari.FLOAT32,0.)
    mat.setParameter('metallic',anari.FLOAT32,0.)
    mat.setParameter('ior',anari.FLOAT32,ior)
    mat.commitParameters()
    return mat

# --- NEW: Function for creating tinted, partially transparent glass ---
def make_colored_glass(r, g, b, ior, transmission_amount):
    mat = device.newMaterial('physicallyBased')
    # baseColor will tint the light passing through the object
    mat.setParameter('baseColor', anari.float3, (r, g, b))
    # transmission: 1.0 is fully transparent, 0.0 is opaque
    mat.setParameter('transmission', anari.FLOAT32, transmission_amount)
    mat.setParameter('ior', anari.FLOAT32, ior)
    mat.setParameter('metallic', anari.FLOAT32, 0.0) # Not metallic
    mat.setParameter('specular', anari.FLOAT32, 0.5) # Add some reflections
    mat.setParameter('roughness', anari.FLOAT32, 0.0) # Make it smooth
    mat.commitParameters()
    return mat

def make_metal(albedo,fuzz):
    mat = device.newMaterial('physicallyBased')
    mat.setParameter('baseColor',anari.float3,(albedo[0],albedo[1],albedo[2]))
    mat.setParameter('ior',anari.FLOAT32,1.45)
    mat.setParameter('metallic',anari.FLOAT32,1.)
    mat.setParameter('specular',anari.FLOAT32,0.)
    mat.setParameter('roughness',anari.FLOAT32,fuzz)
    mat.commitParameters()
    return mat

def create_spheres():
    global movable_geoms, sphere_anim_params
    add_sphere((0.,-1000.,-1),1000.,make_lambertian(.5,.5,.5))
    add_sphere((0., 1., 0.), 1., make_dielectric(1.5))

    # --- MODIFIED: Changed the brown sphere to be colored glass ---
    # Old line: add_sphere((-4.,1.,0.),1.,make_lambertian(.4,.2,.1))
    # We use a light base color to tint the glass, a low IOR, and high transmission.
    add_sphere((-4.,1.,0.),1.,make_colored_glass(1.0, 0.95, 0.9, 1.1, 0.9))

    add_sphere((4.,1.,0.),1.,make_metal((.7,.6,.5),0.))
    for a in range(-11,12):
        for b in range(-11,12):
            choose_mat = random.random()
            center = (a + 0.9*random.random(), 0.2, b + 0.9*random.random())
            if np.linalg.norm(np.array(center) - np.array([4,0.2,0])) < 1.2: continue
            if np.linalg.norm(np.array(center) - np.array([-4,0.2,0])) < 1.2: continue
            if np.linalg.norm(np.array(center) - np.array([0,0.2,0])) < 1.2: continue

            if choose_mat < 0.8:
                mat = make_lambertian(random.random()**2, random.random()**2, random.random()**2)
            elif choose_mat < .95:
                mat = make_metal((.5*(1+random.random()),.5*(1+random.random()),.5*(1+random.random())),.2)
            else:
                mat = make_dielectric(1.5)

            geom = add_sphere(center,.2, mat)
            movable_geoms.append(geom)
            anim_props = { "initial_pos": np.array(center), "speed": random.uniform(1.0, 3.0), "phase": random.uniform(0, 2 * np.pi) }
            sphere_anim_params.append(anim_props)

# --- ONE-TIME SETUP ---
device = anari.newDevice('default')
fig, ax = plt.subplots()
spheres, movable_geoms, sphere_anim_params = [], [], []
create_spheres()
world = device.newWorld()
world.setParameterArray1D('surface', anari.SURFACE, spheres )
world.commitParameters()
bg_values = np.array(((.9,.9,.9,1.),(.15,.25,.8,1.)), dtype=np.float32).reshape((2,1,4))
bg_gradient = device.newArray2D(anari.float4, bg_values)
renderer = device.newRenderer('default')
renderer.setParameter('pixelSamples', anari.INT32, num_paths_per_pixel)
renderer.setParameter('background', anari.ARRAY2D, bg_gradient)
renderer.commitParameters()
camera = device.newCamera('perspective')
camera.setParameter('aspect', anari.FLOAT32, fb_size[0]/fb_size[1])
camera.setParameter('fovy', anari.FLOAT32, fovy * np.pi / 180)
frame = device.newFrame()
frame.setParameter('size', anari.uint2, fb_size)
frame.setParameter('channel.color', anari.DATA_TYPE, anari.UFIXED8_RGBA_SRGB)
frame.setParameter('renderer', anari.OBJECT, renderer)
frame.setParameter('world', anari.OBJECT, world)
dummy_frame = np.zeros((fb_size[1], fb_size[0], 4), dtype=np.uint8)
image = ax.imshow(dummy_frame)
plt.gca().invert_yaxis()
plt.axis('off')

# --- INTERACTIVE STATE MANAGEMENT ---
class CameraState:
    def __init__(self, look_from, look_at):
        self.look_at = look_at
        vec = np.array(look_from) - self.look_at
        self.radius = np.linalg.norm(vec)
        self.azimuth = np.arctan2(vec[2], vec[0])
        self.elevation = np.arcsin(vec[1] / self.radius)
        self.is_dragging = False
        self.last_mouse_pos = (0, 0)
cam_state = CameraState(look_from_initial, look_at)
start_time = time.time()

# --- MOUSE EVENT HANDLERS ---
def on_press(event):
    if event.inaxes != ax: return
    cam_state.is_dragging = True
    cam_state.last_mouse_pos = (event.x, event.y)

def on_release(event):
    cam_state.is_dragging = False

def on_motion(event):
    if not cam_state.is_dragging or event.inaxes != ax: return
    dx = event.x - cam_state.last_mouse_pos[0]
    dy = event.y - cam_state.last_mouse_pos[1]
    cam_state.last_mouse_pos = (event.x, event.y)
    
    sensitivity = 0.01
    cam_state.azimuth -= dx * sensitivity
    cam_state.elevation += dy * sensitivity
    cam_state.elevation = np.clip(cam_state.elevation, -np.pi/2.01, np.pi/2.01)

def on_scroll(event):
    if event.inaxes != ax: return
    zoom_speed = 0.9 if event.button == 'up' else 1.1
    cam_state.radius *= zoom_speed
    cam_state.radius = max(2.0, cam_state.radius)

fig.canvas.mpl_connect('button_press_event', on_press)
fig.canvas.mpl_connect('button_release_event', on_release)
fig.canvas.mpl_connect('motion_notify_event', on_motion)
fig.canvas.mpl_connect('scroll_event', on_scroll)

# --- MAIN UPDATE AND RENDER FUNCTION ---
def update(frame_num):
    elapsed_time = time.time() - start_time
    for i, geom in enumerate(movable_geoms):
        props = sphere_anim_params[i]
        new_y = props["initial_pos"][1] + 0.4 * np.sin(props["speed"] * elapsed_time + props["phase"])
        new_pos = (props["initial_pos"][0], new_y, props["initial_pos"][2])
        new_pos_array = device.newArray1D(anari.FLOAT32_VEC3, np.array([new_pos], dtype=np.float32))
        geom.setParameter('vertex.position', anari.ARRAY1D, new_pos_array)
        geom.commitParameters()
    world.commitParameters()

    x = cam_state.radius * np.cos(cam_state.elevation) * np.cos(cam_state.azimuth)
    y = cam_state.radius * np.sin(cam_state.elevation)
    z = cam_state.radius * np.cos(cam_state.elevation) * np.sin(cam_state.azimuth)
    pos = cam_state.look_at + np.array([x, y, z])
    direction = cam_state.look_at - pos

    camera.setParameter('position', anari.FLOAT32_VEC3, pos)
    camera.setParameter('direction', anari.float3, direction)
    camera.setParameter('up', anari.float3, look_up)
    camera.commitParameters()
    frame.setParameter('camera', anari.OBJECT, camera)
    frame.commitParameters()

    print(f"Rendering frame {frame_num}...")
    frame.render()
    fb_color = frame.get('channel.color')
    pixels = np.array(fb_color)
    image.set_data(pixels)
    
    return [image]

# --- START THE APPLICATION ---
ani = FuncAnimation(fig, update, interval=33, blit=False)
plt.show()
