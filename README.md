# Cosmara

# Brief Description

Cosmara Path Tracer is a physically based CPU Renderer which utilises Monte Carlo global illumination, importance sampling and multithreaded rendering. It supports Bounding Volume Hierarchies for acceleration.

# Example Renders

### Stanford Dragon
<img width="1000" height="1000" alt="Image" src="https://github.com/user-attachments/assets/58f009b8-c982-4373-9449-8eb325b21a9c" />

### Lucy
<img width="1000" height="1000" alt="Image" src="https://github.com/user-attachments/assets/92ab2a05-a68f-433a-b291-65c46232cdb2" />

### Happy Buddha
<img width="1000" height="1000" alt="Image" src="https://github.com/user-attachments/assets/ddce2927-883e-4c0e-9f9d-77bdc1c6cd40" />

The following renders have been passed through a denoiser.

# Build and Run

Download the CosmaraPT folder, and run runrender.py, answer the questions:
1. How many threads do you want to use?
2. How many samples per pixel (spp) do you want to use? Samples per pixels refers to how many rays are shot from the camera to determine the color of each pixel. Higher SPP correlates positively with image quality, but takes more time to render.


# Building a Scene

In this renderer the scene is built through the SceneRender.h file, which has a sceneLayout function, in this function you can add objects to the scene, create materials with or without textures, apply them to primitives and position them. For additional transforms, visit scenetools.h, to edit some parameters such as the transform of the model within the scene. 
This project is focused on the path tracing pipeline/logic and architecture, and there isn't user customisability or scene building without editing the code.


# Features/Highlights

1. Monte Carlo numerical Integration for Global Illumination
2. Bounding Volume Hierarchies for acceleration (Most effective with large scenes containing many primitives).
3. Complex Materials and BRDF/BSDFs such as diffuse (lambertian), dielectrics (glass), glossy (metals with a roughness parameter), textures can be applied to materials.
4. Importance Sampling for Materials.
5. Glossy material uses a standard Cook Torrance BRDF with a Beckmann Distribution function and a Smith-GGX Geometric Attenuation Masking parameter.
6. Multithreaded CPU rendering.

## Future Additions

1. More Specialised diffuse BRDF's than a naive lambertian (e.g. Oren Nayar)
2. Support for more advanced model types such as GLTF.
3. More optimized BVH that uses a binned SAH (Surface Area Heuristic).
