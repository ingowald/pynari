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
import pynari as anari
from pathlib import Path
import pyopenvdb as vdb

from .anari_scene_base import AnariSceneBase
from . import anari_tf

class AnariScene(AnariSceneBase):
    def __init__(self):
        super().__init__()

    def use_dearpygui_tf(self):
        return True  # Whether to use DearPyGui
    
    def update_world(self, device, world):
        xf = anari_tf.opacity_tf

        # Fast comparison of transfer functions
        if hasattr(self, 'xf') and np.array_equal(xf, self.xf):
            return  # Skip update if transfer function hasn't changed
                
        # Store current transfer function for future comparisons        
        self.xf = xf
        xf_array = device.newArray(anari.float4,xf)        

        self.volume.setParameter('color',anari.ARRAY,xf_array)
        self.volume.commitParameters()

    def create_world(self, device):
        """Create and populate the scene with objects."""

        def download_vdb():
            import requests
            import zipfile
            import io
            import os

            # File to check
            gridname = "density"
            target_file = "bunny_cloud.vdb"

            # Check if the file already exists
            if os.path.exists(target_file):
                print(f"'{target_file}' already exists. Skipping download.")
            else:
                # URL of the zip file
                url = "https://artifacts.aswf.io/io/aswf/openvdb/models/bunny_cloud.vdb/1.0.0/bunny_cloud.vdb-1.0.0.zip"

                # Download the file
                print("Downloading...")
                response = requests.get(url)
                response.raise_for_status()  # Raise an error for bad status

                # Extract the zip file
                print("Extracting...")
                with zipfile.ZipFile(io.BytesIO(response.content)) as zip_ref:
                    zip_ref.extractall(os.getcwd())

                print(f"Done. '{target_file}' extracted to:", os.getcwd())

            return target_file, gridname
        
        def grid_to_numpy(grid, padding=0):
            # Get the bounding box of the active voxels
            bbox_min, bbox_max = grid.metadata["file_bbox_min"], grid.metadata["file_bbox_max"]
            
            # Add padding if requested
            bbox_min = [x - padding for x in bbox_min]
            bbox_max = [x + padding for x in bbox_max]
            
            # Calculate dimensions of the array needed
            dims = [bbox_max[2] - bbox_min[2] + 1,
                    bbox_max[1] - bbox_min[1] + 1,
                    bbox_max[0] - bbox_min[0] + 1]
            
            # Create empty array of appropriate dimensions and type
            if grid.valueTypeName == 'float':
                array = np.zeros(dims, dtype=np.float32)
            elif grid.valueTypeName == 'double':
                array = np.zeros(dims, dtype=np.float64)
            elif grid.valueTypeName == 'int32':
                array = np.zeros(dims, dtype=np.int32)
            elif grid.valueTypeName == 'int64':
                array = np.zeros(dims, dtype=np.int64)
            elif grid.valueTypeName == 'bool':
                array = np.zeros(dims, dtype=bool)
            else:
                raise ValueError(f"Unsupported grid type: {grid.valueTypeName}")
    
            
            # Copy grid values to the array, starting at the minimum coordinates 
            # of the bounding box
            grid.copyToArray(array, bbox_min)
            
            return array        

            
        def get_volume_vdb():
            target_file, gridname = download_vdb()

            # Read all grids from the file
            grid = vdb.read(target_file, gridname)
            array = grid_to_numpy(grid)

            return array, array.shape

        cell_values, volume_dims = get_volume_vdb()
        cell_array = np.array(cell_values,dtype=np.float32).reshape(volume_dims)

        structured_data = device.newArray(anari.float,cell_array)

        cellSize = (2.0/(volume_dims[0]-1),2.0/(volume_dims[1]-1),2.0/(volume_dims[2]-1))
        spatial_field = device.newSpatialField('structuredRegular')
        spatial_field.setParameter('origin',anari.float3,(-1,-1,-1))
        spatial_field.setParameter('spacing',anari.float3,cellSize)
        spatial_field.setParameter('data',anari.ARRAY3D,structured_data)
        spatial_field.commitParameters()

        self.xf = anari_tf.opacity_tf
        xf_array = device.newArray(anari.float4,self.xf)

        self.volume = device.newVolume('transferFunction1D')
        self.volume.setParameter('color',anari.ARRAY,xf_array)
        self.volume.setParameter('value',anari.SPATIAL_FIELD,spatial_field)
        self.volume.setParameter('unitDistance',anari.FLOAT32,50.)
        self.volume.commitParameters()
                                                            
        world = device.newWorld()
        world.setParameterArray('volume', anari.VOLUME, [ self.volume ] )
        light = device.newLight('directional')
        light.setParameter('direction', anari.float3, ( 1., -1., -1. ) )
        light.commitParameters()

        array = device.newArray(anari.LIGHT, [light])
        world.setParameter('light', anari.ARRAY1D, array)
        world.commitParameters()

        return world



