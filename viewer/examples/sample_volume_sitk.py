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
import SimpleITK as sitk

from .anari_scene_base import AnariSceneBase
from . import anari_tf

class AnariScene(AnariSceneBase):
    def __init__(self):
        super().__init__()

    def use_dearpygui_tf(self):
        return True  # Whether to use DearPyGui
    
    def get_camera_up(self):
        return np.array([0, 0, 1])
    
    def get_color_tf(self):
        color_start = [0.82, 0.70, 0.55, 0.7]
        color_end = [1.0, 1.0, 1.0]

        return color_start, color_end    
    
    def update_world(self, device, world):
        xf = anari_tf.color_tf

        # Fast comparison of transfer functions
        if hasattr(self, 'xf') and np.array_equal(xf, self.xf):
            return  # Skip update if transfer function hasn't changed
                
        # Store current transfer function for future comparisons        
        self.xf = xf
        xf_array = device.newArray1D(anari.float4,xf)        

        self.volume.setParameter('color',anari.ARRAY1D,xf_array)
        self.volume.commitParameters()

    def create_world(self, device):
        """Create and populate the scene with objects."""

        def download_dicom():
            import requests
            import zipfile
            import io
            import os

            # Dir to check
            target_dir = "Class 3 malocclusion"

            # Check if the dir already exists
            if os.path.exists(target_dir):
                print(f"'{target_dir}' already exists. Skipping download.")
            else:
                # URL of the zip file
                url = "https://medimodel.com/wp-content/uploads/2018/08/Class-3-malocclusion.zip"

                # Download the file
                print("Downloading...")
                response = requests.get(url)
                response.raise_for_status()  # Raise an error for bad status

                # Extract the zip file
                print("Extracting...")
                with zipfile.ZipFile(io.BytesIO(response.content)) as zip_ref:
                    zip_ref.extractall(os.getcwd())

                print(f"Done. '{target_dir}' extracted to:", os.getcwd())

            return target_dir + '/DICOM'

        def get_volume_unstructured_hexahedra():
            #-------------------------------------------------------------------------------
            # Define 8 vertices for a single hexahedron (cube-like volume)
            positions = np.array([
                [0, 0, 0],   # 0
                [1, 0, 0],   # 1
                [1, 1, 0],   # 2
                [0, 1, 0],   # 3
                [0, 0, 1],   # 4
                [1, 0, 1],   # 5
                [1, 1, 1],   # 6
                [0, 1, 1]    # 7
            ], dtype=np.float32)            

            # One hexahedral cell with 8 vertex indices (assumes CCW ordering)
            indices = np.array([0, 1, 2, 3, 4, 5, 6, 7], dtype=np.uint32)

            # Scalar value associated with the cell (e.g. density)
            data = np.array([0.5], dtype=np.float32)

            # Cell type array — one entry per cell
            VTK_HEXAHEDRON = 12
            cell_types = np.array([VTK_HEXAHEDRON], dtype=np.uint8)

            # Offset into index array per cell (0 since one cell starts at beginning)
            cell_index = np.array([0], dtype=np.uint32)

            spatial_field = device.newSpatialField('unstructured')

            array_vertex = device.newArray1D(anari.FLOAT32_VEC3,positions)
            spatial_field.setParameter('vertex.position',anari.ARRAY1D,array_vertex)
        
            array_new_cells_np = device.newArray1D(anari.UINT32,indices)        
            spatial_field.setParameter('index',anari.ARRAY1D,array_new_cells_np)
        
            array_cell_temperature = device.newArray1D(anari.FLOAT32,data)        
            spatial_field.setParameter('cell.data',anari.ARRAY1D,array_cell_temperature)

            print('------------------------------------------------------- CELL TYPE')
            array_cell_types_np = device.newArray1D(anari.UINT8,cell_types)
            spatial_field.setParameter('cell.type',anari.ARRAY1D,array_cell_types_np)

            array_cells_index_first_np = device.newArray1D(anari.UINT32,cell_index)
            spatial_field.setParameter('cell.index',anari.ARRAY1D,array_cells_index_first_np)
            spatial_field.commitParameters()
            return spatial_field
        
        
        def get_volume_sitk():
            dicom_dir = download_dicom()

            # Use the directory reader to find all DICOM series
            reader = sitk.ImageSeriesReader()
            
            # Get all DICOM series IDs in the directory
            series_IDs = sitk.ImageSeriesReader.GetGDCMSeriesIDs(dicom_dir)

            # Use the first series found
            series_ID = series_IDs[0]
            print(f"Reading DICOM series: {series_ID}")
            
            # Get the file names for the series
            dicom_names = reader.GetGDCMSeriesFileNames(dicom_dir, series_ID)
            
            # Set the file names and read
            reader.SetFileNames(dicom_names)
            sitk_image3d = reader.Execute()
            
            # Convert to numpy array
            image_data = sitk.GetArrayFromImage(sitk_image3d)
            image_data = image_data.astype(np.float32)
            
            # Print some info about the loaded data
            print(f"Loaded volume with shape: {image_data.shape}")
            print(f"Data range: {image_data.min()} to {image_data.max()}")

            # Min-max normalization to [0,1] range
            min_val = image_data.min()
            max_val = image_data.max()
            image_data = (image_data - min_val) / (max_val - min_val)            
            
            return image_data, image_data.shape

        def get_volume_structured():
            cell_values, volume_dims = get_volume_sitk()
            cell_array = np.array(cell_values,dtype=np.float32).reshape(volume_dims)

            structured_data = device.newArray3D(anari.float,cell_array)

            cellSize = (2.0/(volume_dims[0]-1),2.0/(volume_dims[1]-1),2.0/(volume_dims[2]-1))
            spatial_field = device.newSpatialField('structuredRegular')
            spatial_field.setParameter('origin',anari.float3,(-1,-1,-1))
            spatial_field.setParameter('spacing',anari.float3,cellSize)
            spatial_field.setParameter('data',anari.ARRAY3D,structured_data)
            spatial_field.commitParameters()
            return spatial_field

        spatial_field = get_volume_structured()
        #spatial_field = get_volume_unstructured_hexahedra()
        #####TF####
        # Create transfer function using the DearPyGui library
        self.xf = anari_tf.color_tf
        xf_array = device.newArray1D(anari.float4,self.xf)

        self.volume = device.newVolume('transferFunction1D')
        self.volume.setParameter('color',anari.ARRAY1D,xf_array)
        self.volume.setParameter('value',anari.SPATIAL_FIELD,spatial_field)
        self.volume.setParameter('unitDistance',anari.FLOAT32,0.1)
        self.volume.commitParameters()
                                                            
        world = device.newWorld()
        world.setParameterArray1D('volume', anari.VOLUME, [ self.volume ] )
        light = device.newLight('directional')
        light.setParameter('direction', anari.float3, ( 1., -1., -1. ) )
        light.commitParameters()

        array = device.newArray1D(anari.LIGHT, [light])
        world.setParameter('light', anari.ARRAY1D, array)
        world.commitParameters()

        return world



