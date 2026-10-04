#include "RegisterScene.h"
#include "SceneRegistry.h"

// All concrete scene header is included here
#include "Welcome.h"
//Getting Started Chapter
#include "GettingStarted/GettingStarted.h"
// Lighting Chapter
#include "Lighting/Colors/Colors.h"
#include "Lighting/BasicLighting/BasicLighting.h"
#include "Lighting/Materials/Materials.h"
#include "Lighting/LightingMaps/LightingMaps.h"
#include "Lighting/LightCasters/LightCasters.h"
#include "Lighting/MultipleLights/MultipleLights.h"

#include "ModelLoading/ModelLoading.h"

#include "AdvancedOpenGL/DepthTesting/DepthTesting.h"
#include "AdvancedOpenGL/StencilTesting/StencilTesting.h"
#include "AdvancedOpenGL/Blending/Blending.h"
#include "AdvancedOpenGL/FaceCulling/FaceCulling.h"
#include "AdvancedOpenGL/Framebuffers/Framebuffers.h"
#include "AdvancedOpenGL/Cubemaps/Cubemaps.h"
#include "AdvancedOpenGL/AdvancedGLSL/AdvancedGLSL.h"
#include "AdvancedOpenGL/Geometry Shader/GeometryShader.h"
#include "AdvancedOpenGL/Instancing/Instancing.h"
#include "AdvancedOpenGL/AntiAliasing/AntiAliasing.h"

#include "AdvancedLighting/AdvancedLighting/AdvancedLighting.h"
#include "AdvancedLighting/GammaCorrection/GammaCorrection.h"
#include "AdvancedLighting/Shadows/ShadowMapping/ShadowMapping.h"
#include "AdvancedLighting/Shadows/PointShadow/PointShadow.h"
#include "AdvancedLighting/NormalMapping/NormalMapping.h"
#include "AdvancedLighting/ParallaxMapping/ParallaxMapping.h"
#include "AdvancedLighting/HDR/HDR.h"

void RegisterScenes(SceneRegistry& Registry)
{
	Registry.RegisterHome<WelcomeScene>();
	Registry.Register<Startup>("Getting Started", "Getting Started", "First steps into OpenGL");
	Registry.Register<Colors>("Lighting", "Colors", "Understanging how to create lights in a scene");
	Registry.Register<BasicLighting>("Lighting", "Basic Lighting", "Understanding basic lighting");
	Registry.Register<Materials>("Lighting", "Materials", "Practice rendering different materials on an object");
	Registry.Register<LightingMaps>("Lighting", "Lighting Maps", "Demonstrait how to add texture and light an object with texture");
	Registry.Register<LightCasters>("Lighting", "Light Casters", "Showcase different light types");
	Registry.Register<MultipleLights>("Lighting", "MultipleLights", "Putting it all together in this final section");
	Registry.Register<ModelLoading>("Model Loading", "Model", "Loading in some models using ASSIMP");
	Registry.Register<DepthTesting>("Advanced OpenGL", "Depth Testing", "testing depth shader");
	Registry.Register<StencilTesting>("Advanced OpenGL", "Stencil Testing", "");
	Registry.Register<Blending>("Advanced OpenGL", "Blending testing", "");
	Registry.Register<FaceCulling>("Advanced OpenGL", "Face Culling", "");
	Registry.Register<Framebuffers>("Advanced OpenGL", "Framebuffers", "");
	Registry.Register<Cubemaps>("Advanced OpenGL", "Cubemaps", "");
	Registry.Register<AdvancedGLSL>("Advanced OpenGL", "Advanced GLSL", "");
	Registry.Register<GeometryShader>("Advanced OpenGL", "Geometry Shader", "");
	Registry.Register<Instancing>("Advanced OpenGL", "Instancing", "");
	Registry.Register<AntiAliasing>("Advanced OpenGL", "AntiAliasing", "");
	Registry.Register<AdvancedLighting>("Advanced Lighting", "Advanced Lighting", "Blinn-Phong lighting");
	Registry.Register<GammaCorrection>("Advanced Lighting", "Gamma Correction", "");
	Registry.Register<ShadowMapping>("Advanced Lighting", "Shadow Mapping", "");
	Registry.Register<PointShadow>("Advanced Lighting", "Point Shadow", "");
	Registry.Register<NormalMapping>("Advanced Lighting", "Normal Mapping", "");
	Registry.Register<ParallaxMapping>("Advanced Lighting", "Parallax Mapping", "");
	Registry.Register<HDR>("Advanced Lighting", "HDR", "");
}
