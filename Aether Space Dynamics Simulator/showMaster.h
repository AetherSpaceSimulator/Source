#ifndef _SHOWMASTER_H_
#define _SHOWMASTER_H_

#include "stdafx.h"
#include "win_system.h"
#include "directx11.h"
#include "basic_shader.h"
#include "atmosphere_shader.h"
#include "directx_input.h"
#include "camera.h"
#include "object.h"
#include "model.h"
#include "texture.h"
#include "fps.h"
#include "gui.h"
#include "light.h"
#include "stars.h"
#include "auto.h"
#include "aerodynamics.h"
#include "object_creator.h"
#include "script.h"
#include "moving_parts.h"
#include "instanced_quad.h"

#include "instance_shader.h"

#include "rendertextureclass.h"
#include "depthshaderclass.h"
#include "depthshaderclass_tr.h"

#include "bullet.h"

class showMasterClass
{
public:
	showMasterClass();
	showMasterClass(const showMasterClass&);
	~showMasterClass();

	bool Initialize(bool full_screen_resizeable, int screen_width, int screen_height, bool vsync, bool full_screen);
	bool run_show();
	void start_engine();
	void stop_engine();

private:

	void toggle_engine(bool engine_state_);
	void set_bullet_scenario();
	
	bool render_manager();
	bool load_starfield();
	bool load_main_star();
	bool load_planets();
	bool load_rings();
	bool load_moons();
	bool render_general_objects();
	bool start_synchronized;

	bool BulletPhysics_ObjectManager();

	bool initialize_background_textures();
	bool render_background_textures();
	bool add_object_to_bullet(int n);
	
	bool initialize_models();
	bool initialize_textures();
	bool update_model(int n);
	bool update_texture(int n);
	void transform_object(int objeto);
	void set_special_objects_properties(int n);
	void astrophysics_engine();
	inline bool inside_range(int body_, const float body_count_);
	bool updateInput();
	void set_texture_array(int n, int nn);
	void set_near_and_far_points(int n);
	void update_model_used(int body_);
	bool set_shader_to_use(int object_);
	bool set_shader_parameters(int object_, int material_);
	bool sunlit(vec3 raydir, vec3 rayorig, vec3 pos, float rad);
	bool render_GUI();
	bool calculos_generales();
	void toggle_sync(bool status);
	bool RenderSceneToTexture();
	bool Update_all_objects();
	void fly_by_camera(Vector3 lookat_pos_);
	void shadows_manager();
	bool render_3D_lines();
	bool draw_3D_orbit(int n, Color color_);
	bool draw_constellations(Color color_);
	bool draw_loose_line(Vector3 origin_, Vector3 target_, Color color_);
	bool render_projected_text();
	bool update_dynamic_sphere(int main_iterations, int sub_iterations);
	int hit_facet(int cuerpo, Vector3 origen, Vector3 direccion, int total_facets, FACET* facet);
	bool rayIntersectsTriangle(Vector3 origen, Vector3 direccion, Vector3 v1, Vector3 v2, Vector3 v3);
	bool camera_manager();
	void landing_manager(int n, float dt_);
	void crash_manager(int n);
	bool exclude_background_objects(int n);
	bool render_engine_particles();
	void reset_scene();

	bool in_cockpit, switched_camera;

	void One_time_Initializations_pre();
	void One_time_Initializations_post();

	float screenNear, screenFar, starshine, default_ambient_ilumination;
	float landing_pad_view_altitude;
	int star_object, total_initial_setup_objects;

	int hit_face;
	int total_background_textures;
	FACET hit_single_face, tf[13];

	int SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT;
	float SHADOWMAP_DEPTH, SHADOWMAP_NEAR;
	bool bullet_reload, freeze_aether_physics;

	Mat4d ground_rotation(int n, double heading_degrees);

	float near_, far_, under_clouds;
	bool cut_engine, max_out, inversed_render_, simplified_atmosphere, GUI_visible;
	bool ready_to_render, proceed;
	double minimum_gravity_influence_weight, rot_angle_before, conv_angle_before, calculations_epoch, true_julian, clock_rotation_time, full_date;
	std::vector <std::string> model_filename;
	std::vector <std::string> texture_filename;
	int screenWidth, screenHeight, material_check, models_number, textures_number, default_models, default_textures, total_ventanas;
	std::thread physics_engine;
	bool read_specs_file(int object_count_, std::string string_);

	float light_view_position_to_cast_shadows, camera_in_atmosphere, camera_altitude, bullet_range, control_in_bullet_range;
	bool threadize, sync_;

	win_systemClass* win_system;
	directx11* DirectX;
	directx_input* input;
	Basic_shader_class* Basic_shader;
	Atmosphere_shader_class* Atmosphere_shader;
	CameraClass* camera;
	modelClass* model;
	textureClass* texture;
	std::vector<objectClass> object;
	fpsClass* main_fps;
	fpsClass* physics_engine_fps;
	ID3D11ShaderResourceView* texture_array[3];
	Gui_Class* GUI;
	LightClass* light_;
	color_class* color;
	StarClass* stars;
	AutoClass* auto_;
	AeroDynamicsClass* aero_dynamics;
	BitmapClass* background_texture;
	TextureShaderClass* m_TextureShader;
	ObjectCreatorClass* object_creator;
	ScriptClass* script_;

	InstanceShaderClass* instanced_shader;

	BulletClass* bullet_physics;

	MovingPartsClass* animation;

	RenderTextureClass* m_RenderTexture;
	RenderTextureClass* m_RenderTextureFullScreen;
	RenderTextureClass* m_RenderTextureFullScreen_b;
	DepthShaderClass* m_DepthShader;
	DepthShaderClass_tr* m_DepthShader_tr;

	InstancedModelClass* shock_diamonds_particles;
};

#endif
